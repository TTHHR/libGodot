package com.example.godottest;

import static android.opengl.GLES10.GL_COLOR_BUFFER_BIT;
import static android.opengl.GLES10.glClear;
import static android.opengl.GLES10.glClearColor;


import androidx.appcompat.app.AppCompatActivity;
import androidx.core.splashscreen.SplashScreen;


import android.opengl.EGL14;
import android.opengl.EGLDisplay;
import android.opengl.EGLSurface;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;


import com.example.godottest.databinding.ActivityMainBinding;



import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class MainActivity extends AppCompatActivity implements GLSurfaceView.Renderer{

    // Used to load the 'godottest' library on application startup.
    static {
        System.loadLibrary("godottest");
    }

    private ActivityMainBinding binding;
    private GLSurfaceView           mSurfaceView;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        SplashScreen.installSplashScreen(this);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());
        mSurfaceView=binding.surfaceView;
        mSurfaceView.setEGLConfigChooser((egl10, eglDisplay) -> {
            int[] attribList = new int[]{
                    EGL10.EGL_RED_SIZE, 8,
                    EGL10.EGL_GREEN_SIZE, 8,
                    EGL10.EGL_BLUE_SIZE, 8,
                    EGL10.EGL_ALPHA_SIZE, 8,
                    EGL10.EGL_DEPTH_SIZE, 16,
                    EGL10.EGL_STENCIL_SIZE, 8,
                    EGL10.EGL_RENDERABLE_TYPE, 4,
                    EGL10.EGL_SAMPLE_BUFFERS, 1, // 启用多采样
                    EGL10.EGL_SAMPLES, 4, // 指定采样数为4
                    EGL10.EGL_NONE // 属性列表结束
            };

            EGLConfig[] configs = new EGLConfig[1];
            int[] numConfig = new int[1];
            // 选择EGL配置
            if (!egl10.eglChooseConfig(eglDisplay, attribList, configs, 1, numConfig)) {
                throw new IllegalArgumentException("eglChooseConfig failed");
            }
            return configs[0]; // 返回选择的EGL配置
        });
        mSurfaceView.setEGLContextClientVersion(2);
        mSurfaceView.setRenderer(this);
        mSurfaceView.setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);
        mSurfaceView.setFocusable(true);
        mSurfaceView.setFocusableInTouchMode(true);
        mSurfaceView.requestFocus();
        mSurfaceView.setOnTouchListener(this::onGodotTouch);
        mSurfaceView.setOnKeyListener(this::onGodotKey);
        mSurfaceView.setOnGenericMotionListener(this::onGodotGenericMotion);

    }

    @Override
    protected void onPause() {
        super.onPause();
        mSurfaceView.onPause();
    }

    @Override
    protected void onResume() {
        super.onResume();
        mSurfaceView.onResume();
    }

    @Override
    protected void onDestroy() {
        nativeShutdown();
        super.onDestroy();
    }

    private boolean onGodotTouch(View view, MotionEvent event) {
        int actionMasked = event.getActionMasked();
        int pointerIndex = actionMasked == MotionEvent.ACTION_MOVE ? 0 : event.getActionIndex();
        if (pointerIndex < 0 || pointerIndex >= event.getPointerCount()) {
            return true;
        }

        switch (actionMasked) {
            case MotionEvent.ACTION_DOWN:
            case MotionEvent.ACTION_POINTER_DOWN:
                nativeTouchEvent(0, event.getX(pointerIndex), event.getY(pointerIndex), 0);
                break;
            case MotionEvent.ACTION_UP:
            case MotionEvent.ACTION_POINTER_UP:
                nativeTouchEvent(1, event.getX(pointerIndex), event.getY(pointerIndex), 0);
                break;
            case MotionEvent.ACTION_MOVE:
                nativeTouchEvent(2, event.getX(pointerIndex), event.getY(pointerIndex), 0);
                break;
            case MotionEvent.ACTION_CANCEL:
                nativeTouchEvent(3, event.getX(pointerIndex), event.getY(pointerIndex), 0);
                break;
            default:
                break;
        }

        view.requestFocus();
        return true;
    }

    private boolean onGodotKey(View view, int keyCode, KeyEvent event) {
        int action = event.getAction();
        if (action != KeyEvent.ACTION_DOWN && action != KeyEvent.ACTION_UP) {
            return false;
        }

        boolean pressed = action == KeyEvent.ACTION_DOWN;
        boolean echo = pressed && event.getRepeatCount() > 0;
        nativeKeyEvent(keyCode, event.getUnicodeChar(), pressed, echo, modifiersFromKeyEvent(event));
        return true;
    }

    private boolean onGodotGenericMotion(View view, MotionEvent event) {
        if (event.getActionMasked() == MotionEvent.ACTION_SCROLL) {
            nativeMouseWheel(
                    event.getX(),
                    event.getY(),
                    event.getAxisValue(MotionEvent.AXIS_HSCROLL),
                    event.getAxisValue(MotionEvent.AXIS_VSCROLL),
                    0);
            return true;
        }
        if (event.getActionMasked() == MotionEvent.ACTION_HOVER_MOVE) {
            nativeTouchEvent(2, event.getX(), event.getY(), 0);
            return true;
        }
        return false;
    }

    private int modifiersFromKeyEvent(KeyEvent event) {
        int modifiers = 0;
        if (event.isShiftPressed()) {
            modifiers |= 1;
        }
        if (event.isAltPressed()) {
            modifiers |= 2;
        }
        if (event.isCtrlPressed()) {
            modifiers |= 4;
        }
        if (event.isMetaPressed()) {
            modifiers |= 8;
        }
        return modifiers;
    }


    private EGLDisplay eglDisplay;
    private EGLSurface eglSurface;
    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        eglDisplay = EGL14.eglGetCurrentDisplay();
        eglSurface = EGL14.eglGetCurrentSurface(EGL14.EGL_DRAW);
        Log.d("path",getExternalFilesDir(null)+"/car/");
        nativeInitEgl("apk",getExternalFilesDir(null)+"/car/");
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        nativeSizeChange(width,height);
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        {

            //while(true)
            {
                glClearColor(0f,1.0f,0,1);
                glClear(GL_COLOR_BUFFER_BIT);
                boolean swap=nativeUpdateFrame();
                // 手动交换缓冲区
             if(swap)
                    EGL14.eglSwapBuffers(eglDisplay, eglSurface);
            }
        }


    }
    private native void nativeInitEgl(String execPath,String projPath);
    private native boolean nativeUpdateFrame();
    private native  void nativeSizeChange(int w,int h);
    private native void nativeTouchEvent(int action, float x, float y, int modifiers);
    private native void nativeMouseWheel(float x, float y, float wheelX, float wheelY, int modifiers);
    private native void nativeKeyEvent(int keyCode, int unicode, boolean pressed, boolean echo, int modifiers);
    private native void nativeShutdown();
}
