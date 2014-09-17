#!/bin/bash
adb install -r bin/TutorialResources-debug.apk
adb shell am start -n org.oxygine.TutorialResources/org.oxygine.TutorialResources.MainActivity