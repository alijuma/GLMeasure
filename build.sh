#!/bin/bash

set -e

./clean.sh

function error {
  local exit_status=${1:-$?}
  echo "error:" $1
  exit -1
}


function check_android {
  hash android 2>&-
  HAS_ANDROID=$?

  hash ndk-build 2>&-
  HAS_BUILD=$?

  if [ $HAS_ANDROID != 0 ]
  then
    error "android from sdk not found on path"
  fi

  if [ $HAS_BUILD != 0 ]
  then
    error "ndk-build from ndk-build not found on path"
  fi
}

check_android;

android update project -p . -s --target android-8
ndk-build
ant debug
