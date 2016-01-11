#!/usr/bin/env sh
adb install -r bin/${PROJECT}-debug.apk
adb shell am start -n org.oxygine.${PROJECT}/org.oxygine.${PROJECT}.MainActivity
