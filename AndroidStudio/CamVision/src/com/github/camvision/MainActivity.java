package com.github.camvision;

import android.app.ActionBar;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.PixelFormat;
import android.os.Bundle;
import android.app.Activity;
import android.view.KeyEvent;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.RelativeLayout;

import java.io.IOException;
import java.io.InputStream;

public class MainActivity extends Activity {

	GLView gl_view;

    @Override
    public void onCreate(Bundle savedInstanceState) {
       super.onCreate(savedInstanceState);
        gl_view = new GLView(getApplication());
        setContentView(R.layout.activity_main);
        RelativeLayout layout = (RelativeLayout) findViewById(R.id.root);
        RelativeLayout.LayoutParams engine_view = new RelativeLayout.LayoutParams(
                RelativeLayout.LayoutParams.MATCH_PARENT,
                RelativeLayout.LayoutParams.MATCH_PARENT);
        layout.addView(gl_view, 0, engine_view);
    }

    @Override
    protected void onPause() {
        super.onPause();
        gl_view.onPause();
    }
    

    @Override
    protected void onResume() {
        super.onResume();
        gl_view.onResume();
    }
    
    @Override
    protected void onStop() {
        super.onStop();
        finish();
    }
    
    @Override
    protected void onDestroy(){	
    	super.onDestroy();
    }

    public boolean onKeyDown(int keyCode, KeyEvent event) {
	        if (keyCode == KeyEvent.KEYCODE_BACK) {
	        		finish();
	                return true;
	        }
	    return super.onKeyDown(keyCode, event);
	}
}
