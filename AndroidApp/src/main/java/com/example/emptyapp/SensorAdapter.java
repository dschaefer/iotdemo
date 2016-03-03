package com.example.emptyapp;

import com.google.gson.JsonElement;
import com.google.gson.JsonObject;

import android.content.Context;
import android.graphics.Color;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnTouchListener;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;

public class SensorAdapter extends BaseAdapter {

	private final LayoutInflater inflater;
	private JsonObject sensors;

	public SensorAdapter(Context context) {
		inflater = LayoutInflater.from(context);
	}

	public void setSensors(JsonObject sensors) {
		try {
			this.sensors = sensors;
			notifyDataSetChanged();
		} catch (Throwable e) {
			Log.e(MainActivity.TAG, "setSensors", e);
		}
	}

	@Override
	public int getCount() {
		return 20;
	}

	@Override
	public Object getItem(int position) {
		return position + 1;
	}

	@Override
	public long getItemId(int position) {
		return position;
	}

	@Override
	public View getView(final int position, View convertView, ViewGroup parent) {
		try {
			View view = convertView;
			if (view == null) {
				view = inflater.inflate(android.R.layout.simple_list_item_1, parent, false);
			}

			final String sensorId = "sensor" + getItem(position);
			int state = 0;
			if (sensors != null) {
				JsonElement stateObj = sensors.get(sensorId);
				if (stateObj != null) {
					state = stateObj.getAsJsonObject().get("state").getAsInt();
				}
			}
			int color;
			switch (state) {
			case 1:
				color = Color.YELLOW;
				break;
			case 2:
				color = Color.RED;
				break;
			default:
				color = Color.GREEN;
			}
			view.setBackgroundColor(color);

			TextView textView = (TextView) view.findViewById(android.R.id.text1);
			textView.setText("Sensor " + getItem(position));

			view.setOnTouchListener(new OnTouchListener() {
				@Override
				public boolean onTouch(View view, MotionEvent event) {
					int state;
					switch (event.getActionMasked()) {
					case MotionEvent.ACTION_DOWN:
						state = 1;
						break;
					case MotionEvent.ACTION_UP:
					case MotionEvent.ACTION_CANCEL:
						state = 0;
						break;
					default:
						return false;
					}

					MainActivity.setState(sensorId, state);
					return true;
				}
			});
			return view;
		} catch (Throwable e) {
			Log.e(MainActivity.TAG, "getView", e);
			return null;
		}
	}

}
