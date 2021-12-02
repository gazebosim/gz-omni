#!/bin/bash
set -e

here=$(dirname "$0")
cd "$here"

if [[ -d third_party ]]; then
  echo 'already bootstrapped, please delete `third_party` dir to re-bootstrap'
  exit 0
fi

mkdir -p "$here/third_party"
cd "$here/third_party"

echo 'Downloading dependencies...'

curl -O https://d4i3qtqj3r0z5.cloudfront.net/nv-usd@20.08.1818.f41ff452-linux64_py37-centos_release-main.7z
if ! echo '2c0eb3b636714ab7fb9c80c056582d20  nv-usd@20.08.1818.f41ff452-linux64_py37-centos_release-main.7z' | md5sum -c; then
  echo 'ERROR: checksum mismatch'
  exit 1
fi
mkdir -p nv-usd
pushd nv-usd
p7zip -d ../nv-usd@20.08.1818.f41ff452-linux64_py37-centos_release-main.7z
popd

curl -O https://d4i3qtqj3r0z5.cloudfront.net/omni_client_library.py37.linux-x86_64@1.13.19-main.2174%2Btc.b44c68ac.7z
if ! echo '76f02ee07b4434eb3dee108992809ac1  omni_client_library.py37.linux-x86_64@1.13.19-main.2174%2Btc.b44c68ac.7z' | md5sum -c; then
  echo 'ERROR: checksum mismatch'
  exit 1
fi
mkdir -p omni_client_library
pushd omni_client_library
p7zip -d ../omni_client_library.py37.linux-x86_64@1.13.19-main.2174%2Btc.b44c68ac.7z
popd

curl -O https://d4i3qtqj3r0z5.cloudfront.net/python@3.7.9-173.e9ee4ea0-linux-x86_64.7z
if ! echo '4dc0d8b501047a6a6616d1afaf421d4e  python@3.7.9-173.e9ee4ea0-linux-x86_64.7z' | md5sum -c; then
  echo 'ERROR: checksum mismatch'
  exit 1
fi
mkdir -p python
pushd python
p7zip -d ../python@3.7.9-173.e9ee4ea0-linux-x86_64.7z
popd
