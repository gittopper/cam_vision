package com.github.camvision;

import android.content.res.AssetManager;

public class CamVision {

	static{
		System.loadLibrary("cam_vision");
	}

	public static native void init(int width, int height, AssetManager assetManager);
	public static native void step();
	public static native void onPause();
	public static native void onResume(AssetManager assetManager);
}
