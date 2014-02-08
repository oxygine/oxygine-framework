package org.oxygine.lib;

import android.app.*;
import android.content.*;
import android.view.*;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.Uri;

import java.util.Locale;

public class Utils
{
    static public Activity _context;
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

    public static void exit()
    {
        System.exit(0);
    }

    public static void moveTaskToBack()
    {
        _context.moveTaskToBack(true);
    }

    public static void browse(String url)
    {
        Intent browseIntent = new Intent(Intent.ACTION_VIEW, Uri.parse(url));
        _context.startActivity(browseIntent);
    }
}