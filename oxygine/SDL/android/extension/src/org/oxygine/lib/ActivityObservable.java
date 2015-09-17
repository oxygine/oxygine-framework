package org.oxygine.lib;

import android.app.Activity;
import android.content.Intent;
import android.util.Log;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;


public class ActivityObservable {
    public static Comparator<ActivityObserver> listenerComparator = new Comparator<ActivityObserver>() {

        public int compare(ActivityObserver a1, ActivityObserver a2) {
            return a2.getPriority() - a1.getPriority();
        }
    };
    private List<ActivityObserver> _observers;
    private Activity _activity;

    public ActivityObservable(Activity activity) {
        _activity = activity;
        _observers = new ArrayList<ActivityObserver>();
    }

    public Activity getActivity() {
        return _activity;
    }

    public void addObserver(ActivityObserver al) {
        for (ActivityObserver l : _observers) {
            if (al.getClass().getName() == l.getClass().getName()) {
                Log.i("ActivitySubscribe", "adding duplicate listener");
                return;
            }
        }

        _observers.add(al);
        al.added(_activity);
        Collections.sort(_observers, listenerComparator);
    }

    public ActivityObserver findClass(Class c) {
        for (ActivityObserver l : _observers) {
            Class lc = l.getClass();
            boolean found = c.isAssignableFrom(lc);
            if (found)
                return l;
        }
        return null;
    }

    public boolean onBackPressed()//true if need to call super
    {
        for (ActivityObserver l : _observers)
            l.onBackPressed();

        return true;
    }

    public void onNewIntent(Intent intent) {
        for (ActivityObserver l : _observers)
            l.onNewIntent(intent);
    }

    public void onPause() {
        for (ActivityObserver l : _observers)
            l.onPause();
    }

    public void onResume() {
        for (ActivityObserver l : _observers)
            l.onResume();
    }

    void dump() {
        Log.i("ActivitySubscribe ", "------ dump --------");
        for (ActivityObserver l : _observers)
            Log.i("ActivitySubscribe ", l.getClass().getName() + " p = " + String.valueOf(l.getPriority()));
    }

    public void onStart() {
        for (ActivityObserver l : _observers)
            l.onStart();
    }

    public void onCreate() {
        for (ActivityObserver l : _observers)
            l.onCreate();
    }

    public void onStop() {
        for (ActivityObserver l : _observers)
            l.onStop();
    }

    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        for (ActivityObserver l : _observers)
            l.onActivityResult(requestCode, resultCode, data);
    }

    public void onDestroy() {
        for (ActivityObserver l : _observers)
            l.onDestroy();
    }

}