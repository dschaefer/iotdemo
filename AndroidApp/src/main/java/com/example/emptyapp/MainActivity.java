package com.example.emptyapp;

import java.util.Timer;
import java.util.TimerTask;

import com.google.gson.JsonObject;
import com.google.gson.JsonParser;
import com.loopj.android.http.AsyncHttpClient;
import com.loopj.android.http.RequestParams;
import com.loopj.android.http.TextHttpResponseHandler;

import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnTouchListener;
import android.widget.ListView;
import cz.msebera.android.httpclient.Header;

public class MainActivity extends AppCompatActivity {

	public static final String TAG = "Doug";
	private static final String URL = "http://10.122.232.191:8080/api/sensors";
	private static AsyncHttpClient client = new AsyncHttpClient();

	private ListView listView;
	private Timer timer = new Timer();

	public static void setState(String sensor, int state) {
		client.put(URL + "?sensor=" + sensor + "&state=" + state, new TextHttpResponseHandler() {
			@Override
			public void onSuccess(int arg0, Header[] arg1, String arg2) {
			}

			@Override
			public void onFailure(int arg0, Header[] arg1, String arg2, Throwable t) {
				Log.e(TAG, t.getLocalizedMessage(), t);
			}
		});
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		listView = (ListView) findViewById(R.id.list);

		final SensorAdapter adapter = new SensorAdapter(this);

		listView.setAdapter(adapter);

		listView.setOnTouchListener(new OnTouchListener() {
			@Override
			public boolean onTouch(View view, MotionEvent event) {
				return false;
			}
		});

		timer.scheduleAtFixedRate(new TimerTask() {
			@Override
			public void run() {
				new Handler(Looper.getMainLooper()).post(new Runnable() {
					@Override
					public void run() {
						client.get(URL, new RequestParams(), new TextHttpResponseHandler() {
							@Override
							public void onSuccess(int statusCode, Header[] headers, String res) {
								final JsonObject sensors = new JsonParser().parse(res).getAsJsonObject();
								new Handler(Looper.getMainLooper()).post(new Runnable() {
									@Override
									public void run() {
										adapter.setSensors(sensors);
									}
								});
							}

							@Override
							public void onFailure(int statusCode, Header[] headers, String res, Throwable t) {
								Log.e(TAG, res, t);
							}
						});
					}
				});
			}
		}, 1000, 1000);
	}

}
