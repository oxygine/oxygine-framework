#!/usr/bin/env sh
adb install -r bin/GamePart4-debug.apk
adb shell am start -n org.oxygine.GamePart4/org.oxygine.GamePart4.MainActivity
