package org.oxygine.lib;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.Uri;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.Locale;

public class Utils {
    static public Activity _context;

    public static long getTimeUTCMS() {
        return System.currentTimeMillis();
    }

    public static String getLanguage() {
        return Locale.getDefault().getLanguage();
    }

    public static boolean isNetworkAvailable() {
        if (_context == null)
            return false;
        ConnectivityManager connectivityManager = (ConnectivityManager) _context.getSystemService(Context.CONNECTIVITY_SERVICE);
        NetworkInfo activeNetworkInfo = connectivityManager.getActiveNetworkInfo();
        return activeNetworkInfo != null;
    }

    public static void exit() {
        System.exit(0);
    }

    public static void moveTaskToBack() {
        _context.moveTaskToBack(true);
    }

    public static void browse(String url) {
        Intent browseIntent = new Intent(Intent.ACTION_VIEW, Uri.parse(url));
        _context.startActivity(browseIntent);
    }

    public static void writeBuffer2InternalStorage(String path, byte[] data) {
        File file = new File(_context.getFilesDir() + File.separator + path);

        try {
            FileOutputStream outstream;

            if (!file.exists()) {
                file.createNewFile();
            }

            outstream = new FileOutputStream(file);
            outstream.write(data);
            outstream.close();

        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public static native void runOnGameThread(Runnable task);
}