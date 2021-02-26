#!/usr/bin/env python3

from PIL import Image, ImageDraw, ImageChops
from pathlib import Path
import sys
import argparse


class SimulatedPrinter:
  """Simulated Apple ImageWriter II"""

  def __init__(self, output_res):
    self.output_res = output_res

    self.cur_x_step = output_res / 72
    self.cur_y_step = (output_res / 72) * 9
    self.cur_color = (0, 0, 0, 255)
    self.form_height = 11 * output_res

    self.dot_size = 200

  def plot_stripe(self, stripe):
    self.page_dirty = True

    rad = self.output_res / self.dot_size
    x0, y0, x1, y1 = self.cur_head_pos[0] - rad, self.cur_head_pos[1] - rad, self.cur_head_pos[0] + rad, self.cur_head_pos[1] + rad
    ystep = self.output_res / 72

    for pix in range(0, 8):
      if stripe & (1 << pix) != 0:
        for value, channel, eff_color in zip(self.cur_color, (self.page_c, self.page_m, self.page_y, self.page_k), ((255,0,0,0),(0,255,0,0),(0,0,255,0),(0,0,0,255))):
          if value == 0:
            continue
          image_draw = ImageDraw.Draw(channel)
          image_draw.ellipse([x0, y0, x1, y1], fill=eff_color)
      y0, y1 = y0 + ystep, y1 + ystep

  def render_page(self, input_gen):
    self.page_dirty = False
    self.page_c = Image.new('CMYK', (self.output_res * 10, self.output_res * 12), color=(0, 0, 0, 0))
    self.page_m = Image.new('CMYK', (self.output_res * 10, self.output_res * 12), color=(0, 0, 0, 0))
    self.page_y = Image.new('CMYK', (self.output_res * 10, self.output_res * 12), color=(0, 0, 0, 0))
    self.page_k = Image.new('CMYK', (self.output_res * 10, self.output_res * 12), color=(0, 0, 0, 0))

    self.cur_head_pos = (0, 0)

    command = next(input_gen, None)
    while command is not None:
      if command == 0:
        pass
      elif command == 0xD:
        self.cur_head_pos = (0, self.cur_head_pos[1])
      elif command == 0xA:
        self.cur_head_pos = (self.cur_head_pos[0], self.cur_head_pos[1] + self.cur_y_step)
      elif command == 0xC:
        print('form feed')
        self.page_dirty = True
        break
      elif command == 0x1B:
        esc_command = next(input_gen)
        if esc_command == ord('K'):
          color_code = int(chr(next(input_gen)))
          if color_code in range(0, 7):
            self.cur_color = \
            [(0, 0, 0, 255), (0, 0, 255, 0), (0, 255, 0, 0), (255, 0, 0, 0), (0, 255, 255, 0), (255, 0, 255, 0),
             (255, 255, 0, 0)][color_code]
          else:
            raise Exception("K command color number out of bounds")
        elif esc_command == ord('n'):
          self.cur_x_step = self.output_res / 72
        elif esc_command == ord('N'):
          self.cur_x_step = self.output_res / 80
        elif esc_command == ord('E'):
          self.cur_x_step = self.output_res / 96
        elif esc_command == ord('e'):
          self.cur_x_step = self.output_res / 107
        elif esc_command == ord('q'):
          self.cur_x_step = self.output_res / 120
        elif esc_command == ord('Q'):
          self.cur_x_step = self.output_res / 136
        elif esc_command == ord('p'):
          self.cur_x_step = self.output_res / 144
        elif esc_command == ord('P'):
          self.cur_x_step = self.output_res / 160
        elif esc_command == ord('T'):
          newline_height = int(chr(next(input_gen)) + chr(next(input_gen)))
          self.cur_y_step = (self.output_res / 144) * newline_height
        elif esc_command == ord('G') or esc_command == ord('S'):
          num_stripes = int(chr(next(input_gen)) + chr(next(input_gen)) + chr(next(input_gen)) + chr(next(input_gen)))
          for i in range(0, num_stripes):
            self.plot_stripe(next(input_gen))
            self.cur_head_pos = (self.cur_head_pos[0] + self.cur_x_step, self.cur_head_pos[1])
        elif esc_command == ord('V'):
          num_stripes = int(chr(next(input_gen)) + chr(next(input_gen)) + chr(next(input_gen)) + chr(next(input_gen)))
          stripe = next(input_gen)
          for i in range(0, num_stripes):
            self.plot_stripe(stripe)
            self.cur_head_pos = (self.cur_head_pos[0] + self.cur_x_step, self.cur_head_pos[1])
        elif esc_command == ord('H'):
          form_height_dots = int(chr(next(input_gen)) + chr(next(input_gen)) + chr(next(input_gen)) + chr(next(input_gen)))
          self.form_height = self.output_res * form_height_dots // 144
          print('form height =', form_height_dots, 'dots')
        elif esc_command == ord('>'):
          print('left-to-right seek mode')
        elif esc_command == ord('<'):
          print('logical seek mode')
        else:
          print('warning: unparsed escape code {} ({})'.format(esc_command, chr(esc_command)))
      else:
        print('warning: ignored character {} ({})'.format(command, chr(command)))

      if self.cur_head_pos[1] >= self.form_height:
        print('forced line feed because end of form')
        break

      command = next(input_gen, None)

    if not self.page_dirty:
      return None

    composite = ImageChops.add(self.page_y, self.page_c)
    composite = ImageChops.add(composite, self.page_m)
    composite = ImageChops.add(composite, self.page_k)

    composite = composite.crop((0, 0, self.output_res * 10, self.form_height))
    return composite


def byte_generator(bytes):
  for i, n in enumerate(bytes):
    if i % (len(bytes) // 10) == 0 and i != 0:
      print('{}% ({}/{})'.format((i+1) * 100 // len(bytes), i, len(bytes)))
    yield n


def main():
  parser = argparse.ArgumentParser(description='Apple ImageWriter II Printer Simulator')
  parser.add_argument('input_path', type=Path, nargs=1, help='Input printer language file')
  parser.add_argument('--out', '-o', type=str, nargs='?', help='Output TIFF file', default='out.tiff')
  parser.add_argument('--dpi', '-r', type=int, nargs='?', help='Output image DPI', default=144*3)
  args = parser.parse_args()

  input = args.input_path[0].read_bytes()
  input_gen = byte_generator(input)
  prn = SimulatedPrinter(args.dpi)

  pages = []
  next_page = prn.render_page(input_gen)
  while next_page:
    pages += [next_page]
    next_page = prn.render_page(input_gen)
  if len(pages):
    pages[0].save(args.out, compression="tiff_deflate", resolution=prn.output_res, resolution_unit=2, save_all=True, append_images=pages[1:])
  else:
    print('nothing was printed!')


if __name__ == '__main__':
  main()
