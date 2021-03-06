#!/bin/bash
#
# Script to make an installable package of the driver.
#
# Uses xcodebuild, pkgbuild and productbuild.
#

# Create a clean install directory...
if test -d build/Package; then
	sudo chmod -R u+w build/Package
	sudo rm -rf build/Package
fi
mkdir -p build/Package/Root
mkdir -p build/Package/Resources

# Install into this directory...
xcodebuild -workspace "$PWD/ImageWriterII.xcworkspace" \
           -scheme ImageWriterIIBundle \
           -configuration Release \
           install \
           DSTROOT="$PWD/build/Package/Root"

# Extract the version number from the project...
ver=$(/usr/libexec/PlistBuddy -c "Print:CFBundleShortVersionString" "DriverBundle/imagewriterii-Info.plist")

# Make the package with pkgbuild and the product distribution with productbuild...
echo pkgbuild...
pkgbuild --identifier       com.danielecattaneo.printers.imagewriterii   \
         --version          "$ver"   \
         --install-location /   \
         --root             build/Package/Root   \
         "./DriverPackage.pkg"   
productbuild --distribution     ./PackageResources/Distribution.xml   \
             --resources        ./PackageResources/Resources \
             --package-path     ./ \
             "./AppleImageWriterII-$ver.pkg"
rm ./DriverPackage.pkg
