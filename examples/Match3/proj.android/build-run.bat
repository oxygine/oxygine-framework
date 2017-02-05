call ndk-build NDK_MODULE_PATH="../../../../"
call gradlew assembleDebug
call adb install -r build/outputs/apk/proj.android-debug.apk
call adb shell am start -n org.oxygine.Match3/org.oxygine.Match3.MainActivity