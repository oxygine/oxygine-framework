call adb logcat -c
cls
rem call adb logcat -v threadtime
call adb logcat -v threadtime System.out:S EDMNativeHelper:S TimaService:S BatteryService:S PowerManagerService:S NotificationService:S  KeyguardUpdateMonitor:S SELinux:S