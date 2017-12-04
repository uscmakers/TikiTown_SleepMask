package com.led_on_off.led;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.os.AsyncTask;
import android.util.Log;
import android.widget.TextView;

import java.io.IOException;
import java.io.InputStream;
import java.lang.ref.WeakReference;
import java.net.Socket;

/**
 * Created by zsurani on 12/3/17.
 */

public class BluetoothReceiver extends AsyncTask<Void, Integer, Void> {

    private BluetoothSocket btSocket;

        public BluetoothReceiver(BluetoothSocket bluetooth) {
            Log.d("here", "here");
            this.btSocket = bluetooth;
        }

    @Override
    protected Void doInBackground(Void... voids) {
        try {

            InputStream is = btSocket.getInputStream();
            byte[] buffer = new byte[25];
            while (true) {
                int read = is.read(buffer);
                while(read != -1){
                    publishProgress(read);
                    read = is.read(buffer);
                }

                ledControl led = new ledControl();
                if (led.isTime()) {
                    int val = 2;
                    byte b = (byte) val;
                    try {
                        btSocket.getOutputStream().write(b);
                        Log.d("read", "sending " + b);
                    } catch (IOException e) {
                    }
                }

            }

        } catch (IOException e) {
            e.printStackTrace();
        }

        return null;
    }

}