package org.oxygine.lib;

import android.os.Bundle;
import org.libsdl.app.SDLActivity;
/**
    OxygineActivity
*/
public class OxygineActivity extends SDLActivity 
{
    protected static OxygineActivity instance;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        instance = this;
        Utils._context = this;
        nativeOxygineInit(this);
    }

    public static native void nativeOxygineInit(OxygineActivity activity);
}
