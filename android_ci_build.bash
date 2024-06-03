#!/bin/bash
# set -e

export BUILD_ANDROID=true

cmake_build () {
  ANDROID_ABI=$1
  mkdir -p build-$ANDROID_ABI
  mkdir -p lib/$ANDROID_ABI
  cd build-$ANDROID_ABI
  cmake $GITHUB_WORKSPACE -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DANDROID_PLATFORM=24 -DANDROID_ABI=$ANDROID_ABI -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK_LATEST_HOME/build/cmake/android.toolchain.cmake
  cmake --build . --clean-first
  # | echo "Build exit code: $?"
  # --verbose
  cd ..
  cp build-$ANDROID_ABI/libopenal.so lib/$ANDROID_ABI/
}

mkdir -p lib

cmake_build arm64-v8a
