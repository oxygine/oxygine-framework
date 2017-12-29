#!/usr/bin/env sh

#ndk-build NDK_MODULE_PATH="../../../../"
gradlew assembleDebug
adb install -r build/outputs/apk/debug/proj.android-debug.apk
adb shell am start -n org.oxygine.HelloWorld/org.oxygine.HelloWorld.MainActivity