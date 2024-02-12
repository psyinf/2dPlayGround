#! /bin/bash
if [ -z "$1" ]
  then
    echo "No argument supplied, please supply a non-root user name as first argument "
fi
USER="$1"

apt -qq update
apt -q install -y cmake ninja-build g++ clang pkgconf gdb
apt -q install -y python3-pip

sudo -Hiu $USER env pip3 install --user conan==1.60.2
sudo -Hiu $USER env conan profile new default --detect
sudo -Hiu $USER env conan profile update settings.compiler.libcxx=libstdc++11 default
sudo -Hiu $USER env conan profile update conf.tools.system.package_manager:mode=install default
sudo -Hiu $USER env conan profile update conf.tools.system.package_manager:sudo=True default

export CPM_SOURCE_CACHE=$HOME/.cache/CPM