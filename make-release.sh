#!/bin/sh

# Creates a tarball for releasing
# Don't forget to set target=Release in CMakeLists.txt beforehand

if [ "$#" -lt 1 ]; then
    exit 1
fi

echo "Creating tarball file"

rm -rf "/tmp/wRssSyn-$1"
mkdir "/tmp/wRssSyn-$1"

cp -a CMakeLists.txt LICENSE README cmake/ crawler/ viewer/ wRssSyn.conf "/tmp/wRssSyn-$1"

cd "/tmp"

tar zcvf "wRssSyn-$1.tar.gz" "wRssSyn-$1"

rm -rf "wRssSyn-$1"

cd "$OLDPWD"
mv "/tmp/wRssSyn-$1.tar.gz" ./

echo "Uploading to server"
scp "wRssSyn-$1.tar.gz" ostrova:~/www-default/files/

echo "Generating checksums for FreeBSD port file"
cd freebsd-port
su root -c "rm -f \"/usr/ports/distfiles/wRssSyn-$1.tar.gz\" && make makesum"
