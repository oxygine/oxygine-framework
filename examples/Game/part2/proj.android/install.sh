#!/usr/bin/env sh
adb install -r bin/GamePart2-debug.apk
adb shell am start -n org.oxygine.GamePart2/org.oxygine.GamePart2.MainActivity
