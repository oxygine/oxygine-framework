#!/bin/bash
adb install -r bin/HelloWorld-debug.apk
adb shell am start -n org.oxygine.HelloWorld/org.oxygine.HelloWorld.MainActivity