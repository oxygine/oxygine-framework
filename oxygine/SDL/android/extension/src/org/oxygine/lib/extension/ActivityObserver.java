package org.oxygine.lib.extension;

import android.app.Activity;
import android.content.Intent;


public class ActivityObserver {
    protected Activity _activity = null;
    private int _priority = 0;

    public ActivityObserver() {
    }

    public int getPriority() {
        return _priority;
    }

    public void added(Activity act) {
        _activity = act;
    }

    public boolean onBackPressed()//true if need to call super
    {
        return true;
    }

    public void onNewIntent(Intent intent) {
    }

    public void onPause() {
    }

    public void onResume() {
    }

    public void onCreate() {
    }

    public void onStart() {
    }

    public void onStop() {
    }

    public void onActivityResult(int requestCode, int resultCode, Intent data) {
    }

    public void onDestroy() {
    }
}