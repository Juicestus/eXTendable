#!/bin/bash

cp -r bin/xt package/mac/application/

bash package/mac/build-macos-x64.sh xt 1.0.0

mv package/mac/target/pkg/xt-macos-installer-x64-1.0.0.pkg dist/mac-x86_64-installer

rm -rf package/mac/application/xt
rm -rf package/mac/target