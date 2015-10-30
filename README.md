ImageWriter II Printer Driver
=============================

This is an OS X driver for the Apple ImageWriter II dot matrix printer.


Features
--------

This driver supports all of the output resolutions provided by the ImageWriter
II. Also, it uses the repetition commands to compress the output, which allows
the printer to reach its maximum speed.

Plus, if you want to speed up printing at the expense of some minor artifacts,
you can enable bidirectional mode on any resolution setting.

This driver does not require ghostscript or foomatic-rip. This is the main
advantage it has over the foomatic filter which uses the Ghostscript device,
since these two packages are not included in OS X.

Since I have a Sheetfeeder attachment for the ImageWriter II, this driver
works with it.

### How to connect your ImageWriter to your Mac

First you need a cable which converts the Mini-DIN port on the back of the
printer to a standard IBM PC RS-232 DB-9 connector. You can buy this cable,
or you can build one. Either way, the cable MUST support the handshake
signals (some cables do not). 

Then, you can connect the printer to the Mac by connecting it to an old PC and
sharing it from there. You'll have to set the serial port transmission rate
and handshake mode in the PC you are sharing the printer from.

### Non-features

Since I'm not in possession of an original ImageWriter, an Apple DMP or
an ImageWriter LQ, I have not tested this driver on these printers. It looks
like it should be compatible, except for some resolution settings.

If you really want a driver that supports the ImageWriter LQ's high resolution
mode, Ghostscript's Apple DMP device supports it.


Copyright & License
-------------------

Copyright (c) 2013-15 Daniele Cattaneo

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

ImageWriter is a trademark of Apple Inc. This software is not provided and/or
endorsed in any way by Apple Inc.


