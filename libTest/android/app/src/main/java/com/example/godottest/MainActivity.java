package com.example.godottest;

import static android.opengl.GLES10.GL_COLOR_BUFFER_BIT;
import static android.opengl.GLES10.glClear;
import static android.opengl.GLES10.glClearColor;


import androidx.activity.result.ActivityResult;
import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.splashscreen.SplashScreen;


import android.content.ActivityNotFoundException;
import android.content.Intent;
import android.net.Uri;
import android.opengl.GLSurfaceView;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.provider.DocumentsContract;
import android.provider.Settings;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Toast;


import com.example.godottest.databinding.ActivityMainBinding;



import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;
import java.io.File;

public class MainActivity extends AppCompatActivity implements GLSurfaceView.Renderer{

    // Used to load the 'godottest' library on application startup.
    static {
        System.loadLibrary("godottest");
    }

    private ActivityMainBinding binding;
    private GLSurfaceView           mSurfaceView;
    private ActivityResultLauncher<Intent> allFilesAccessLauncher;
    private ActivityResultLauncher<Intent> projectFolderLauncher;
    private String projectPath;
    private boolean surfaceReady = false;
    private boolean godotInitialized = false;
    private int surfaceWidth = 0;
    private int surfaceHeight = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        SplashScreen.installSplashScreen(this);
        allFilesAccessLauncher = registerForActivityResult(
                new ActivityResultContracts.StartActivityForResult(),
                result -> {
                    if (hasAllFilesAccess()) {
                        openProjectFolderPicker();
                    } else {
                        Toast.makeText(this, "需要所有文件访问权限才能直接读取 Godot 项目目录", Toast.LENGTH_LONG).show();
                    }
                });
        projectFolderLauncher = registerForActivityResult(
                new ActivityResultContracts.StartActivityForResult(),
                this::onProjectFolderSelected);

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

        requestProjectFolder();
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
        if (godotInitialized) {
            nativeShutdown();
            godotInitialized = false;
        }
        super.onDestroy();
    }

    private void requestProjectFolder() {
        if (hasAllFilesAccess()) {
            openProjectFolderPicker();
            return;
        }

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            Intent intent = new Intent(Settings.ACTION_MANAGE_APP_ALL_FILES_ACCESS_PERMISSION);
            intent.setData(Uri.parse("package:" + getPackageName()));
            try {
                allFilesAccessLauncher.launch(intent);
            } catch (ActivityNotFoundException ignored) {
                allFilesAccessLauncher.launch(new Intent(Settings.ACTION_MANAGE_ALL_FILES_ACCESS_PERMISSION));
            }
        } else {
            openProjectFolderPicker();
        }
    }

    private boolean hasAllFilesAccess() {
        return Build.VERSION.SDK_INT < Build.VERSION_CODES.R || Environment.isExternalStorageManager();
    }

    private void openProjectFolderPicker() {
        Intent intent = new Intent(Intent.ACTION_OPEN_DOCUMENT_TREE);
        intent.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION
                | Intent.FLAG_GRANT_WRITE_URI_PERMISSION
                | Intent.FLAG_GRANT_PERSISTABLE_URI_PERMISSION
                | Intent.FLAG_GRANT_PREFIX_URI_PERMISSION);
        projectFolderLauncher.launch(intent);
    }

    private void onProjectFolderSelected(ActivityResult result) {
        if (result.getResultCode() != RESULT_OK || result.getData() == null || result.getData().getData() == null) {
            Toast.makeText(this, "没有选择 Godot 项目目录", Toast.LENGTH_LONG).show();
            return;
        }

        Uri treeUri = result.getData().getData();
        int flags = result.getData().getFlags()
                & (Intent.FLAG_GRANT_READ_URI_PERMISSION | Intent.FLAG_GRANT_WRITE_URI_PERMISSION);
        try {
            getContentResolver().takePersistableUriPermission(treeUri, flags);
        } catch (SecurityException ignored) {
        }

        String selectedPath = pathFromTreeUri(treeUri);
        if (selectedPath == null) {
            Toast.makeText(this, "当前目录不能转换为 native 可访问路径", Toast.LENGTH_LONG).show();
            return;
        }

        projectPath = ensureTrailingSlash(selectedPath);
        Log.d("path", "Godot project path: " + projectPath);
        if (mSurfaceView != null) {
            mSurfaceView.queueEvent(this::tryInitGodot);
        }
    }

    private String pathFromTreeUri(Uri treeUri) {
        String docId = DocumentsContract.getTreeDocumentId(treeUri);
        if (docId == null || docId.isEmpty()) {
            return null;
        }
        if (docId.startsWith("raw:")) {
            return docId.substring(4);
        }

        String[] parts = docId.split(":", 2);
        String volume = parts[0];
        String relativePath = parts.length > 1 ? parts[1] : "";
        File baseDir;
        if ("primary".equalsIgnoreCase(volume)) {
            baseDir = Environment.getExternalStorageDirectory();
        } else if ("home".equalsIgnoreCase(volume)) {
            baseDir = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOCUMENTS);
        } else {
            baseDir = new File("/storage/" + volume);
        }

        if (relativePath.isEmpty()) {
            return baseDir.getAbsolutePath();
        }
        return new File(baseDir, relativePath).getAbsolutePath();
    }

    private String ensureTrailingSlash(String path) {
        if (path.endsWith(File.separator)) {
            return path;
        }
        return path + File.separator;
    }

    private void tryInitGodot() {
        if (!surfaceReady || godotInitialized || projectPath == null) {
            return;
        }

        nativeInitEgl("apk", projectPath);
        godotInitialized = true;
        if (surfaceWidth > 0 && surfaceHeight > 0) {
            nativeSizeChange(surfaceWidth, surfaceHeight);
        }
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


    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        surfaceReady = true;
        tryInitGodot();
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        surfaceWidth = width;
        surfaceHeight = height;
        if (godotInitialized) {
            nativeSizeChange(width,height);
        }
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        if (!godotInitialized) {
            glClearColor(0f, 0f, 0f, 1f);
            glClear(GL_COLOR_BUFFER_BIT);
            return;
        }

        nativeUpdateFrame();
    }
    private native void nativeInitEgl(String execPath,String projPath);
    private native boolean nativeUpdateFrame();
    private native  void nativeSizeChange(int w,int h);
    private native void nativeTouchEvent(int action, float x, float y, int modifiers);
    private native void nativeMouseWheel(float x, float y, float wheelX, float wheelY, int modifiers);
    private native void nativeKeyEvent(int keyCode, int unicode, boolean pressed, boolean echo, int modifiers);
    private native void nativeShutdown();
}
