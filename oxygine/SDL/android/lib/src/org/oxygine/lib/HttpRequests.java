package org.oxygine.lib;

import android.annotation.TargetApi;
import android.content.Context;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.os.AsyncTask;
import android.os.Build;
import android.os.PowerManager;
import android.util.Log;

import java.io.*;
import java.net.HttpURLConnection;
import java.net.InetSocketAddress;
import java.net.Proxy;
import java.net.URL;

/**
 * Created by Denis on 31.12.2014.
 */

class RequestDetails {
    public String url;
    public byte[] postData;
    public long handle;
    public String[] headerKeys;
    public String[] headerValues;
};


class HttpRequestHolder {

    private HttpRequest task;

    public HttpRequestHolder() {
    }


    @TargetApi(Build.VERSION_CODES.HONEYCOMB) // API 11
    public static <T> void executeAsyncTask(AsyncTask<T, ?, ?> asyncTask, T... params) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.HONEYCOMB)
            asyncTask.executeOnExecutor(AsyncTask.THREAD_POOL_EXECUTOR, params);
        else
            asyncTask.execute(params);
    }


    public void run(final RequestDetails details) {
        OxygineActivity.instance.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                HttpRequest task = new HttpRequest();
                //task.execute(details);
                executeAsyncTask(task, details);
            }
        });
    }
}

// usually, subclasses of AsyncTask are declared inside the activity class.
// that way, you can easily modify the UI thread from here
class HttpRequest extends AsyncTask<RequestDetails, Integer, String> {

    private PowerManager.WakeLock mWakeLock;

    public HttpRequest() {
    }

    public static native void nativeHttpRequestSuccess(long handle);
    public static native void nativeHttpRequestError(long handle);
    public static native void nativeHttpRequestGotHeader(long handle, int code, int contentLength);
    public static native void nativeHttpRequestWrite(long handle, byte[] data, int size);


    private Proxy detectProxy() {
        try {
            ConnectivityManager cm = (ConnectivityManager) OxygineActivity.instance.getSystemService(Context.CONNECTIVITY_SERVICE);
            NetworkInfo ni = cm.getActiveNetworkInfo();
            if (ni != null && ni.isAvailable() && ni.getType() == ConnectivityManager.TYPE_MOBILE) {
                String proxyHost = android.net.Proxy.getDefaultHost();
                int port = android.net.Proxy.getDefaultPort();
                if (proxyHost != null) {
                    final InetSocketAddress sa = new InetSocketAddress(proxyHost, port);
                    return new Proxy(Proxy.Type.HTTP, sa);
                }
            }
        } catch (SecurityException ex) {
        }
        return null;
    }

    @Override
    protected String doInBackground(RequestDetails... details_) {
        InputStream input = null;

        HttpURLConnection connection = null;
        RequestDetails details = details_[0];


        try {
            URL url = new URL(details.url);

            Proxy proxy = detectProxy();
            if (proxy != null)
                connection = (HttpURLConnection) url.openConnection(proxy);
            else
                connection = (HttpURLConnection) url.openConnection();

            connection.setInstanceFollowRedirects(true);

            for (int i = 0; i < details.headerKeys.length; ++i)
            {
                String key = details.headerKeys[i];
                String value = details.headerValues[i];

                connection.setRequestProperty(key, value);
            }

            if (details.postData != null) {
                connection.setDoOutput(true);
                connection.setRequestMethod("POST");
                connection.getOutputStream().write(details.postData);
            }



            int respCode = connection.getResponseCode();
            nativeHttpRequestGotHeader(details.handle, respCode, connection.getContentLength());


            if (respCode >= 400 && respCode <= 599)
                input = connection.getErrorStream();
            else
                input = connection.getInputStream();


            byte data[] = new byte[4096];
            int count;

            while ((count = input.read(data)) != -1)
                nativeHttpRequestWrite(details.handle, data, count);


            nativeHttpRequestSuccess(details.handle);

        } catch (Exception e) {
            nativeHttpRequestError(details.handle);
            Log.v("HttpRequest", "error: " + e.toString());
            return e.toString();
        } finally {
            try {
                if (input != null)
                    input.close();
            } catch (IOException ignored) {
            }

            if (connection != null)
                connection.disconnect();
        }
        return null;
    }

    @Override
    protected void onPreExecute() {
        super.onPreExecute();
        // take CPU lock to prevent CPU from going off if the user
        // presses the power button during download
        PowerManager pm = (PowerManager) OxygineActivity.instance.getSystemService(Context.POWER_SERVICE);
        mWakeLock = pm.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK,
                getClass().getName());
        mWakeLock.acquire();
        //mProgressDialog.show();
    }


    @Override
    protected void onPostExecute(String result) {
        mWakeLock.release();
    }
}


public class HttpRequests {

    static public void init() {

    }

    static public void release() {

    }

    static public HttpRequestHolder createRequest(String url, String[] headerKeys, String[] headerValues, byte[] post, long handle) {

        RequestDetails details = new RequestDetails();
        details.url = url;
        details.postData = post;
        details.handle = handle;
        details.headerKeys = headerKeys;
        details.headerValues = headerValues;

        HttpRequestHolder downloadTask = new HttpRequestHolder();
        downloadTask.run(details);

        return downloadTask;
    }
}