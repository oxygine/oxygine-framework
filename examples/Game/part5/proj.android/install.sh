#!/usr/bin/env sh
adb install -r bin/GamePart5-debug.apk
adb shell am start -n org.oxygine.GamePart5/org.oxygine.GamePart5.MainActivity
