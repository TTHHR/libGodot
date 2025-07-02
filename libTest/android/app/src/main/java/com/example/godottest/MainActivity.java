package com.example.godottest;

import static android.opengl.GLES10.GL_COLOR_BUFFER_BIT;
import static android.opengl.GLES10.glClear;
import static android.opengl.GLES10.glClearColor;


import androidx.activity.result.ActivityResultLauncher;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.content.ContextCompat;
import androidx.core.splashscreen.SplashScreen;


import android.opengl.EGL14;
import android.opengl.EGLDisplay;
import android.opengl.EGLSurface;
import android.opengl.GLSurfaceView;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;


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
    private ActivityResultLauncher<String[]> requestPermissionLauncher;
    boolean hasPermission=false;
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
        mSurfaceView.onResume();

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
}