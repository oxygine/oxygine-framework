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
    public String fileName;
    public byte[] postData;
    public long handle;
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

    public static native void nativeHttpRequestResponseSuccess(long handle, byte[] data);

    public static native void nativeHttpRequestResponseProgress(long handle, int loaded, int total);

    public static native void nativeHttpRequestResponseError(long handle);

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
        OutputStream output = null;
        HttpURLConnection connection = null;
        RequestDetails details = details_[0];


        try {
            URL url = new URL(details.url);
            Proxy proxy = detectProxy();
            if (proxy != null)
                connection = (HttpURLConnection) url.openConnection(proxy);
            else
                connection = (HttpURLConnection) url.openConnection();


            if (details.postData != null) {
                connection.setDoOutput(true);
                //connection.setRequestProperty("Content-Type", "application/x-www-form-urlencoded");
                connection.setRequestMethod("POST");
                connection.getOutputStream().write(details.postData);
            }

            //connection.connect();

            // expect HTTP 200 OK, so we don't mistakenly save error report
            // instead of the file
            if (connection.getResponseCode() != HttpURLConnection.HTTP_OK) {
                nativeHttpRequestResponseError(details.handle);
                return "Server returned HTTP " + connection.getResponseCode() + " " + connection.getResponseMessage();
            }

            // this will be useful to display download percentage
            // might be -1: server did not report the length
            int fileLength = connection.getContentLength();

            // download the file
            input = connection.getInputStream();


            ByteArrayOutputStream bt = null;


            if (details.fileName != null)
                output = new FileOutputStream(OxygineActivity.instance.getFilesDir() + "/" + details.fileName);
            else {
                bt = new ByteArrayOutputStream();
                output = bt;
            }

            byte data[] = new byte[4096];
            int total = 0;
            int count;

            nativeHttpRequestResponseProgress(details.handle, 0, fileLength);

            while ((count = input.read(data)) != -1) {
                // allow canceling with back button
                /*
                if (isCancelled()) {
                    input.close();
                    return null;
                }
                */
                total += count;
                // publishing the progress....
                //if (fileLength > 0) // only if total length is known
                //    publishProgress((int) (total * 100 / fileLength));
                output.write(data, 0, count);
                nativeHttpRequestResponseProgress(details.handle, total, fileLength);
            }
            if (bt != null)
                nativeHttpRequestResponseSuccess(details.handle, bt.toByteArray());
            else
                nativeHttpRequestResponseSuccess(details.handle, null);

        } catch (Exception e) {
            nativeHttpRequestResponseError(details.handle);
            Log.v("HttpRequest", "error: " + e.toString());
            return e.toString();
        } finally {
            try {
                if (output != null)
                    output.close();
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
    protected void onProgressUpdate(Integer... progress) {
        super.onProgressUpdate(progress);
        // if we get here, length is known, now set indeterminate to false
        /*
        mProgressDialog.setIndeterminate(false);
        mProgressDialog.setMax(100);
        mProgressDialog.setProgress(progress[0]);
        */
    }

    @Override
    protected void onPostExecute(String result) {
        mWakeLock.release();
        /*
        mProgressDialog.dismiss();
        if (result != null)
            Toast.makeText(context,"Download error: "+result, Toast.LENGTH_LONG).show();
        else
            Toast.makeText(context,"File downloaded", Toast.LENGTH_SHORT).show();
            */
    }
}

class HttpHandler {
    //private RequestQueue _queue;
    public HttpHandler() {
        //_queue = Volley.newRequestQueue(OxygineActivity.instance);
    }

    public HttpRequestHolder createRequest(final RequestDetails details) {

        //HttpRequest r = new HttpRequest(url, fname, post, handle);
        //_queue.add(r);

        HttpRequestHolder downloadTask = new HttpRequestHolder();
        downloadTask.run(details);

        return downloadTask;
    }
}


public class HttpRequests {
    static private HttpHandler _handler;

    static public void init() {
        _handler = new HttpHandler();
    }

    static public void release() {
        _handler = null;
    }

    static public HttpRequestHolder createRequest(String url, String fname, byte[] post, long handle) {

        RequestDetails details = new RequestDetails();
        details.fileName = fname;
        details.url = url;
        details.postData = post;
        details.handle = handle;

        return _handler.createRequest(details);
    }
}