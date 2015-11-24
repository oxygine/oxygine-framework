package org.oxygine.lib;

import android.content.Intent;
import android.os.Bundle;
import org.libsdl.app.SDLActivity;
import org.oxygine.lib.extension.ActivityObservable;
import org.oxygine.lib.extension.ActivityObserver;

/**
 * OxygineActivity
 */
public class OxygineActivity extends SDLActivity {
    protected static OxygineActivity instance;
    protected ActivityObservable _observable;
    public OxygineActivity() {

        _observable = new ActivityObservable(this);
    }

    public static native void nativeOxygineInit(OxygineActivity activity, Class c);

    public void addObserver(ActivityObserver l) {
        _observable.addObserver(l);
    }

    public ActivityObserver findClass(Class c) {
        return _observable.findClass(c);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        instance = this;
        Utils._context = this;
        nativeOxygineInit(this, getClass());

        _observable.onCreate();
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        _observable.onDestroy();
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        _observable.onActivityResult(requestCode, resultCode, data);
    }

    @Override
    public void onStop() {
        super.onStop();
        _observable.onStop();
    }

    @Override
    public void onStart() {
        super.onStart();
        _observable.onStart();
    }

    @Override
    public void onResume() {
        super.onResume();
        _observable.onResume();
    }

    @Override
    public void onPause() {
        super.onPause();
        _observable.onPause();
    }


    @Override
    protected void onNewIntent(Intent intent) {
        super.onNewIntent(intent);
        _observable.onNewIntent(intent);
    }

    @Override
    public void onBackPressed() {
        if (_observable.onBackPressed())
            super.onBackPressed();
    }
}
