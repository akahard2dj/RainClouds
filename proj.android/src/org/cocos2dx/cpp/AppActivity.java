/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2014 Chukong Technologies Inc.
 
http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
package org.cocos2dx.cpp;

import org.cocos2dx.lib.Cocos2dxActivity;

import com.google.android.gms.ads.AdRequest;
import com.google.android.gms.ads.AdSize;
import com.google.android.gms.ads.AdView;

import android.annotation.SuppressLint;

import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.View;
import android.view.WindowManager;
import android.widget.LinearLayout;


@SuppressLint("HandlerLeak")
public class AppActivity extends Cocos2dxActivity {
	
	static public Handler handler;
	
	/* Your ad unit id. Replace with your actual ad unit id. */
	private static final String AD_UNIT_ID = "ca-app-pub-4871252957066311/3047485788";
	
	private AdView admobView;
	Cocos2dxActivity mActivity;
	LinearLayout layout;
	LinearLayout mainLayout;
	boolean isAdmobInited = false;
	
	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		mActivity = this;
		// Create an ad.
		admobView = new AdView(this);
		admobView.setAdSize(AdSize.BANNER);
		admobView.setAdUnitId(AD_UNIT_ID);
		//showAdmob();
		if (isAdmobInited || admobView == null) {
			return;
		}
		mainLayout = new LinearLayout(mActivity);
		mainLayout.post(new Runnable() {	
			@Override
			public void run() {
				AdRequest adRequest = new AdRequest.Builder().addTestDevice(AdRequest.DEVICE_ID_EMULATOR)
					    .addTestDevice("INSERT_YOUR_HASHED_DEVICE_ID_HERE")
					    .build();

				WindowManager wm = (WindowManager) mActivity
				.getSystemService("window");
				WindowManager.LayoutParams mLayoutParams = new WindowManager.LayoutParams();
				mLayoutParams.type = WindowManager.LayoutParams.TYPE_APPLICATION_PANEL;
				mLayoutParams.width = WindowManager.LayoutParams.WRAP_CONTENT;
				mLayoutParams.height = WindowManager.LayoutParams.WRAP_CONTENT;
				mLayoutParams.flags |= WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE;
				mLayoutParams.gravity = Gravity.BOTTOM;
				wm.addView(admobView, mLayoutParams);
				// Enable this if your are testing AdMob, otherwise you'll risk
				// to be banned!
				//adRequest.isTestDevice (AdRequest.DEVICE_ID_EMULATOR);
				admobView.loadAd(adRequest);
				isAdmobInited = true;
				
				handler = new Handler() {
					public void handleMessage(Message msg) {
						if (msg.what == 0) {
							admobView.setVisibility(View.VISIBLE);
						}
						
						else if (msg.what == 1) {
							admobView.setVisibility(View.INVISIBLE);
						}
					}
				};
				
			}
		});
		admobView.setVisibility(View.INVISIBLE);
	}
	
	public void hideAdmob() {		
		//admobView.setVisibility(View.INVISIBLE);
	}
	
	public void showAdmob() {
		//admobView.setVisibility(View.VISIBLE);
	}
	
	public void JniTest()
	{
		Log.d("Cocos2d-x","Function call from C++");
	}
	@Override
	public void onResume() {
		super.onResume();
		if (admobView != null) {
			admobView.resume();
		}
	}
	
	@Override
	public void onPause() {
		if (admobView != null) {
			admobView.pause();
		}		
		super.onPause();
	}
	
	/** Called before the activity is destroyed. */
	@Override
	public void onDestroy() {
		// Destroy the AdView.
		if (admobView != null) {
			admobView.destroy();
		}
		super.onDestroy();
	}
	
	@Override
	public boolean onKeyUp(int keyCode, KeyEvent event) {
		switch (keyCode) {
		case KeyEvent.KEYCODE_BACK:
			finish();
			break;
		}
		return super.onKeyUp(keyCode, event);
	}	

	public static void ShowAdPopup()
    {
        handler.sendEmptyMessage( 0 );
    }

    public static void HideAdPopup()
    {
        handler.sendEmptyMessage( 1 );
    }
}