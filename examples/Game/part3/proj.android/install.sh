#!/usr/bin/env sh
adb install -r bin/GamePart3-debug.apk
adb shell am start -n org.oxygine.GamePart3/org.oxygine.GamePart3.MainActivity
