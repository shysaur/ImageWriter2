#!/bin/sh
#
# Script to install a debug build of the sample raster driver.
#
# Uses xcodebuild.
#

# Install into /...
sudo xcodebuild -workspace ImageWriterII.xcworkspace -scheme ImageWriterIIBundle -configuration Debug clean
sudo xcodebuild -workspace ImageWriterII.xcworkspace -scheme ImageWriterIIBundle -configuration Debug install DSTROOT=/

sudo launchctl stop org.cups.cupsd
sudo launchctl start org.cups.cupsd
