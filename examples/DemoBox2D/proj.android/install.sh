#!/bin/bash
adb install -r bin/DemoBox2D-debug.apk
adb shell am start -n org.oxygine.DemoBox2D/org.oxygine.DemoBox2D.MainActivity