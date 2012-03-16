#!/bin/bash

adb install -r bin/gl_measure-debug-unaligned.apk 1>&2
adb shell am start -W -a org.mozilla.gl_measure  > /dev/null
sleep 1
adb shell cat /data/data/org.mozilla.gl_measure/files/output.txt
echo adb shell cat /data/data/org.mozilla.gl_measure/files/output.txt
