package com.github.camvision;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.graphics.BitmapFactory;
import android.graphics.BitmapFactory.Options;
import android.graphics.Bitmap;
import android.graphics.PixelFormat;

import android.opengl.GLES10;
import android.opengl.GLES20;
import com.github.camvision.CamVision;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.view.MotionEvent;
import android.view.View;
import android.content.res.AssetManager;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;

public class GLView extends GLSurfaceView {

	private static AssetManager assetManager;

	public GLView(Context context) {
        super(context);
        assetManager = context.getAssets();
        setEGLContextClientVersion(2);
        super.setEGLConfigChooser(8 , 8, 8, 8, 16, 0);
        setRenderer(new Renderer());
        getHolder().setFormat(PixelFormat.RGBA_8888);
	};

    @Override
    public void onPause() {
        super.onPause();
        CamVision.onPause();
    }

    @Override
    public void onResume() {
        super.onResume();
        CamVision.onResume(assetManager);
    }

    public boolean onTouchEvent(final MotionEvent e) {
        if (e.getPointerCount() == 1){
            int x = (int) e.getX();
            int y = (int) e.getY();
            switch (e.getAction()) {
                case MotionEvent.ACTION_DOWN:
                    CamVision.tap(x, y);
                    break;
            }
        }
        return true;
    }

	private static class Renderer implements GLSurfaceView.Renderer{
        Renderer() {
        }
		public void onSurfaceCreated(GL10 gl, EGLConfig config) {
	    }
	    public void onDrawFrame(GL10 unused) {
            CamVision.step();
	    }

	    public void onSurfaceChanged(GL10 unused, int width, int height) {
            CamVision.init(width, height, assetManager);
	    }
	}
}
