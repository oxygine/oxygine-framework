#!/bin/bash
adb install -r bin/Demo-debug.apk
adb shell am start -n org.oxygine.Demo/org.oxygine.Demo.MainActivity