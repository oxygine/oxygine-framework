package org.oxygine.lib;

import android.content.Context;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;

import java.util.Locale;

public class Utils
{
    static public Context _context;
	public static long		getTimeUTCMS()
	{
        return System.currentTimeMillis();
	}

	public static String	getLanguage()
	{
		return Locale.getDefault().getLanguage();
	}

    public static boolean isNetworkAvailable()
    {
        if (_context == null)
            return  false;
        ConnectivityManager connectivityManager = (ConnectivityManager) _context.getSystemService(Context.CONNECTIVITY_SERVICE);
        NetworkInfo activeNetworkInfo = connectivityManager.getActiveNetworkInfo();
        return activeNetworkInfo != null;
    }
}