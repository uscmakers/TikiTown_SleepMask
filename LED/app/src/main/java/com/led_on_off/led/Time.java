package com.led_on_off.led;

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;

/**
 * Created by zsurani on 12/3/17.
 */

public class Time {

    public static final String TABLE = "Timing";
    public static final String KEY_ID = "id";
    public static final String KEY_TIME = "selected_time";
    public static final String KEY_HOUR = "hour";
    public static final String KEY_MINUTE = "minute";
    private AndroidDatabaseManager dbHelper;

    private Context context;

    public Time(Context context) {
        dbHelper = new AndroidDatabaseManager(context);
        this.context = context;
    }

    public String getTime() {
        SQLiteDatabase db = dbHelper.getReadableDatabase();
        String selectQuery =  "SELECT  * FROM " + Time.TABLE + " WHERE " + Time.KEY_ID + "=" + 1;// It's a good practice to use parameter ?, instead of concatenate string

        Cursor cursor = db.rawQuery(selectQuery, null);
        String time = "";

        if (cursor.moveToFirst()) {
            do {
                time += cursor.getInt(cursor.getColumnIndex(Time.KEY_HOUR));
                time += ",";
                time += cursor.getInt(cursor.getColumnIndex(Time.KEY_MINUTE));
            } while (cursor.moveToNext());
        }

        cursor.close();
        db.close();
        return time;
    }

    public void setTime(Integer hour, Integer minute) {
        SQLiteDatabase db = dbHelper.getReadableDatabase();

        String selectQuery = "SELECT * FROM " +
                Time.TABLE + " WHERE " + Time.KEY_ID + "=" + 1;

        Cursor c = db.rawQuery(selectQuery, null);
        boolean found = false;
        if (c.moveToFirst()) found = true;

        db = dbHelper.getWritableDatabase();
        ContentValues values = new ContentValues();

        values.put(Time.KEY_HOUR, hour);
        values.put(Time.KEY_MINUTE, minute);
        // Inserting Row
        if (!found) db.insert(Time.TABLE, null, values);
        else db.update(Time.TABLE, values, Time.KEY_ID+"="+1, null);
//        db.close();
//        c.close();
    }

}
