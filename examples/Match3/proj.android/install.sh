#!/bin/bash
adb install -r bin/Match3-debug.apk
adb shell am start -n org.oxygine.Match3/org.oxygine.Match3.MainActivity