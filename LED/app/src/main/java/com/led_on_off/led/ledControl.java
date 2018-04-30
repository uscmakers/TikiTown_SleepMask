package com.led_on_off.led;

import android.database.sqlite.SQLiteDatabase;
import android.support.v7.app.ActionBarActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;

import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.view.View;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.TimePicker;
import android.widget.Toast;
import android.app.ProgressDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.os.AsyncTask;

import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Arrays;
import java.util.Calendar;
import java.util.Date;
import java.util.List;
import java.util.UUID;


public class ledControl extends ActionBarActivity {

   // Button btnOn, btnOff, btnDis;
    ImageButton On, Off, Discnt, Abt;
    String address = null;
    private ProgressDialog progress;
    BluetoothAdapter myBluetooth = null;
    public BluetoothSocket btSocket = null;
    private boolean isBtConnected = false;
    //SPP UUID. Look for it
    static final UUID myUUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");
    SQLiteDatabase db;
    AndroidDatabaseManager dbHandler;
    TimePicker simpleTimePicker;
    Button start;
    Button stop;
    private InputStream mmInStream = null;
    byte[] buffer = new byte[1024];
    int bytes;

//    private short numSamples = 20;
//    private short samplingDelay = 50;
//    private short pulseDelay = 500;
//    private short rampTime = 5000; // ms
//    private char brightness = 255;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        dbHandler = new AndroidDatabaseManager(this);
        db = dbHandler.getReadableDatabase();
        dbHandler.delete(db);

        Intent newint = getIntent();
        address = newint.getStringExtra(DeviceList.EXTRA_ADDRESS); //receive the address of the bluetooth device

        //view of the ledControl
        setContentView(R.layout.activity_led_control);

        simpleTimePicker = (TimePicker)findViewById(R.id.simpleTimePicker);
        start = (Button) findViewById(R.id.start);
        stop = (Button) findViewById(R.id.stop);


        //call the widgets
//        On = (ImageButton)findViewById(R.id.on);
//        Off = (ImageButton)findViewById(R.id.off);
//        Discnt = (ImageButton)findViewById(R.id.discnt);
//        Abt = (ImageButton)findViewById(R.id.abt);

        new ConnectBT().execute(); //Call the class to connect

        BluetoothReceiver task = new BluetoothReceiver();
        task.execute();

        //commands to be sent to bluetooth
//        On.setOnClickListener(new View.OnClickListener()
//        {
//            @Override
//            public void onClick(View v)
//            {
//                turnOnLed();      //method to turn on
//            }
//        });
//
//        Off.setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View v)
//            {
//                turnOffLed();   //method to turn off
//            }
//        });
//
//        Discnt.setOnClickListener(new View.OnClickListener()
//        {
//            @Override
//            public void onClick(View v)
//            {
//                Disconnect(); //close connection
//            }
//        });

//        simpleTimePicker.setIs24HourView(true);

        Time time = new Time(getApplicationContext());
        String hourmin = time.getTime();
        if (hourmin != "") {
            List<String> timing = Arrays.asList(hourmin.split(","));
            int hour = Integer.parseInt(timing.get(0));
            int minute = Integer.parseInt(timing.get(1));
            simpleTimePicker.setCurrentHour(hour);
            simpleTimePicker.setCurrentMinute(minute);
        }


        simpleTimePicker.setOnTimeChangedListener(new TimePicker.OnTimeChangedListener() {
            @Override
            public void onTimeChanged(TimePicker view, int hourOfDay, int minute) {
                Time time = new Time(getApplicationContext());
                time.setTime(hourOfDay, minute);
            }
        });

        start.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                int val = 4;
                byte b = (byte) val;
                try {
                    btSocket.getOutputStream().write(b);
                    Log.d("read", "sending " + b);
                } catch (IOException e) {
                    msg("Error");
                }

            }
        });

        stop.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                int val = 3;
                byte b = (byte) val;
                try {
                    btSocket.getOutputStream().write(b);
                    Log.d("read", "sending " + b);
                } catch (IOException e) {
                    msg("Error");
                }

            }
        });

//        try {
//            mmInStream = btSocket.getInputStream();
//        } catch (IOException e) {
//        }
//        while (true) {
//            try {
//                int bytes = mmInStream.read(buffer); //read bytes from input buffer
//                String readMessage = new String(buffer, 0, bytes);
//                if (readMessage.length() > 0 && isTime()) {
//                    int val = 2;
//                    byte b = (byte) val;
//                    try {
//                        btSocket.getOutputStream().write(b);
//                    } catch (IOException e) {
//                        msg("Error");
//                    }
//                }
//
//            } catch (IOException e) {
//                break;
//            }
//        }


    }

    public Boolean isTime() {
        Time time = new Time(getApplicationContext());
        String hourmin = time.getTime();
        List<String> timing = Arrays.asList(hourmin.split(","));
        int hour = Integer.parseInt(timing.get(0));
        int minute = Integer.parseInt(timing.get(1));

        Date currentTime = Calendar.getInstance().getTime();
        Log.d("elapsed", currentTime.toString());

        SimpleDateFormat sdf = new SimpleDateFormat("hh:mm:ss");
        String formattedTime = sdf.format(currentTime);

        Date date1 = null;
        Date date2 = null;

        SimpleDateFormat simpleDateFormat = new SimpleDateFormat("hh:mm:ss");
        try {
            date1 = simpleDateFormat.parse(hour + ":" + minute + ":00");
            date2 = simpleDateFormat.parse(formattedTime);
        } catch (ParseException e) {

        }

        long different = date1.getTime() - date2.getTime();
        long secondsInMilli = 1000;
        long minutesInMilli = secondsInMilli * 60;
        long hoursInMilli = minutesInMilli * 60;
        long daysInMilli = hoursInMilli * 24;

        long elapsedDays = different / daysInMilli;
        different = different % daysInMilli;

        long elapsedHours = different / hoursInMilli;
        different = different % hoursInMilli;

        long elapsedMinutes = different / minutesInMilli;
        different = different % minutesInMilli;

        long elapsedSeconds = different / secondsInMilli;

        Log.d("elapsed hours", Long.toString(elapsedHours));
        Log.d("elapsed hours", Long.toString(elapsedMinutes));
        Log.d("elapsed hours", Long.toString(elapsedSeconds));
        Log.d("elapsed hours", date1.toString());
        Log.d("elapsed hours", date2.toString());



        if (elapsedHours == 0 && elapsedMinutes <= 0 && elapsedSeconds < 30) {
            return true;
        }

        return false;
    }

    private void Disconnect()
    {
        if (btSocket!=null) //If the btSocket is busy
        {
            try
            {
                btSocket.close(); //close connection
            }
            catch (IOException e)
            { msg("Error");}
        }
        finish(); //return to the first layout

    }

    private void turnOffLed()
    {
        if (btSocket!=null)
        {
            try
            {
                int val = 0;
                byte b = (byte) val;
                btSocket.getOutputStream().write(b);
                Log.d("read", "sending " + b);
            }
            catch (IOException e)
            {
                msg("Error");
            }
        }
    }

    private void turnOnLed()
    {
        if (btSocket!=null)
        {
            try
            {
                int val = 30;
                byte b = (byte) val;
                btSocket.getOutputStream().write(b);
                Log.d("read", "sending " + b);
            }
            catch (IOException e)
            {
                msg("Error");
            }
        }
    }

    // 1 - start
    // 2 - wake
    // 3 - stop


    // gets 1 (not a character) - light sleep
    // light sleep and within frame of time = send 2

//    private void sendStartPacket() {
//        ByteBuffer pump_on_buf =
//        pump_on_buf.putShort(numSamples);
//        pump_on_buf.putShort(samplingDelay);
//        pump_on_buf.putInt(pulseDelay);
//
//        private short numSamples = 20;
//        private short samplingDelay = 50;
//        private short pulseDelay = 500;
//    }

    // fast way to call Toast
    private void msg(String s)
    {
        Toast.makeText(getApplicationContext(),s,Toast.LENGTH_LONG).show();
    }

//    public  void about(View v)
//    {
//        if(v.getId() == R.id.abt)
//        {
//            Intent i = new Intent(this, AboutActivity.class);
//            startActivity(i);
//        }
//    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_led_control, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }


    private class ConnectBT extends AsyncTask<Void, Void, Void>  // UI thread
    {
        private boolean ConnectSuccess = true; //if it's here, it's almost connected

        @Override
        protected void onPreExecute()
        {
            progress = ProgressDialog.show(ledControl.this, "Connecting...", "Please wait!!!");  //show a progress dialog
        }

        @Override
        protected Void doInBackground(Void... devices) //while the progress dialog is shown, the connection is done in background
        {
            try
            {
                if (btSocket == null || !isBtConnected)
                {
                 myBluetooth = BluetoothAdapter.getDefaultAdapter();//get the mobile bluetooth device
                 BluetoothDevice dispositivo = myBluetooth.getRemoteDevice(address);//connects to the device's address and checks if it's available
                 btSocket = dispositivo.createInsecureRfcommSocketToServiceRecord(myUUID);//create a RFCOMM (SPP) connection
                 BluetoothAdapter.getDefaultAdapter().cancelDiscovery();
                 btSocket.connect();//start connection
                }
            }
            catch (IOException e)
            {
                ConnectSuccess = false;//if the try failed, you can check the exception here
            }
            return null;
        }
        @Override
        protected void onPostExecute(Void result) //after the doInBackground, it checks if everything went fine
        {
            super.onPostExecute(result);

            if (!ConnectSuccess)
            {
                msg("Connection Failed. Is it a SPP Bluetooth? Try again.");
                finish();
            }
            else
            {
                msg("Connected.");
                isBtConnected = true;
            }
            progress.dismiss();
        }
    }

    public class BluetoothReceiver extends AsyncTask<Void, Integer, Void> {

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
                        Log.d("read", Integer.toString(read));
                        if (read == 1) {
                            if (isTime()) {
                                int val = 5;
                                byte b = (byte) val;
                                try {
                                    btSocket.getOutputStream().write(b);
                                    Log.d("read", "sending " + b);
                                } catch (IOException e) {
                                }
                                read = 50;
                            }
                        }
                    }

                }

            } catch (IOException e) {
                e.printStackTrace();
            }

            return null;
        }

    }
}