#!/usr/bin/env sh
adb install -r bin/GamePart1-debug.apk
adb shell am start -n org.oxygine.GamePart1/org.oxygine.GamePart1.MainActivity
