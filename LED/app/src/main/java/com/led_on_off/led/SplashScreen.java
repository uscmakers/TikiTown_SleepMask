package com.led_on_off.led;
import android.app.Activity;
import android.content.Intent;
import android.graphics.Typeface;
import android.os.Bundle;
import android.widget.TextView;

/**
 * Created by PEACE on 3/30/2016.
 */
public class SplashScreen extends Activity {

    TextView tv1;
    Typeface tf1;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        // TODO Auto-generated method stub
        super.onCreate(savedInstanceState);
        setContentView(R.layout.splash);

        tv1 = (TextView) findViewById(R.id.textView3);
        tf1 = Typeface.createFromAsset(getAssets(), "GreatVibes-Regular.otf");
        tv1.setTypeface(tf1);

        Thread timerThread = new Thread(){
            public void run(){
                try{
                    sleep(700);
                }catch(InterruptedException e){
                    e.printStackTrace();
                }finally{
                    Intent intent = new Intent(SplashScreen.this,DeviceList.class);
                    startActivity(intent);
                }
            }
        };
        timerThread.start();
    }

    @Override
    protected void onPause() {
        // TODO Auto-generated method stub
        super.onPause();
        finish();
    }

}
