#include <jni.h>
#include <cstdarg>
#include <cstdint>
#include <cstring>
#include <memory>
#include <string>
#include <vector>
#include <unistd.h>
#include <GLES2/gl2.h>

#include "embedded_export.h"
#include "android/log.h"
extern "C" JNIEXPORT jstring JNICALL
Java_com_example_godottest_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}
void logger( const char *format,
                        ...)
{
    char buffer[1024];
    va_list args;

    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    __android_log_print(ANDROID_LOG_DEBUG,"godot","%s",buffer);

}

namespace {

constexpr uint32_t GODOT_KEY_SPECIAL = 1u << 22;
constexpr uint32_t GODOT_KEY_NONE = 0;
constexpr uint32_t GODOT_KEY_ESCAPE = GODOT_KEY_SPECIAL | 0x01;
constexpr uint32_t GODOT_KEY_TAB = GODOT_KEY_SPECIAL | 0x02;
constexpr uint32_t GODOT_KEY_BACKSPACE = GODOT_KEY_SPECIAL | 0x04;
constexpr uint32_t GODOT_KEY_ENTER = GODOT_KEY_SPECIAL | 0x05;
constexpr uint32_t GODOT_KEY_INSERT = GODOT_KEY_SPECIAL | 0x07;
constexpr uint32_t GODOT_KEY_DELETE = GODOT_KEY_SPECIAL | 0x08;
constexpr uint32_t GODOT_KEY_HOME = GODOT_KEY_SPECIAL | 0x0D;
constexpr uint32_t GODOT_KEY_END = GODOT_KEY_SPECIAL | 0x0E;
constexpr uint32_t GODOT_KEY_LEFT = GODOT_KEY_SPECIAL | 0x0F;
constexpr uint32_t GODOT_KEY_UP = GODOT_KEY_SPECIAL | 0x10;
constexpr uint32_t GODOT_KEY_RIGHT = GODOT_KEY_SPECIAL | 0x11;
constexpr uint32_t GODOT_KEY_DOWN = GODOT_KEY_SPECIAL | 0x12;
constexpr uint32_t GODOT_KEY_PAGEUP = GODOT_KEY_SPECIAL | 0x13;
constexpr uint32_t GODOT_KEY_PAGEDOWN = GODOT_KEY_SPECIAL | 0x14;
constexpr uint32_t GODOT_KEY_SHIFT = GODOT_KEY_SPECIAL | 0x15;
constexpr uint32_t GODOT_KEY_CTRL = GODOT_KEY_SPECIAL | 0x16;
constexpr uint32_t GODOT_KEY_META = GODOT_KEY_SPECIAL | 0x17;
constexpr uint32_t GODOT_KEY_ALT = GODOT_KEY_SPECIAL | 0x18;
constexpr uint32_t GODOT_KEY_F1 = GODOT_KEY_SPECIAL | 0x1C;
constexpr uint32_t GODOT_KEY_BACK = GODOT_KEY_SPECIAL | 0x48;
constexpr uint32_t GODOT_KEY_MENU = GODOT_KEY_SPECIAL | 0x42;

GODOT_LIB_STEP_TYPE godot_step = GODOT_LIB_SETUP2;

uint32_t godotKeyFromAndroid(int keycode, int unicode) {
    if (unicode >= 32 && unicode <= 126) {
        if (unicode >= 'a' && unicode <= 'z') {
            return (uint32_t)('A' + (unicode - 'a'));
        }
        return (uint32_t)unicode;
    }

    if (keycode >= 131 && keycode <= 142) {
        return GODOT_KEY_F1 + (uint32_t)(keycode - 131);
    }

    switch (keycode) {
        case 4:
            return GODOT_KEY_BACK;
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
            return (uint32_t)('0' + (keycode - 7));
        case 21:
            return GODOT_KEY_LEFT;
        case 19:
            return GODOT_KEY_UP;
        case 22:
            return GODOT_KEY_RIGHT;
        case 20:
            return GODOT_KEY_DOWN;
        case 29:
        case 30:
        case 31:
        case 32:
        case 33:
        case 34:
        case 35:
        case 36:
        case 37:
        case 38:
        case 39:
        case 40:
        case 41:
        case 42:
        case 43:
        case 44:
        case 45:
        case 46:
        case 47:
        case 48:
        case 49:
        case 50:
        case 51:
        case 52:
        case 53:
        case 54:
            return (uint32_t)('A' + (keycode - 29));
        case 57:
        case 58:
            return GODOT_KEY_ALT;
        case 59:
        case 60:
            return GODOT_KEY_SHIFT;
        case 61:
            return GODOT_KEY_TAB;
        case 62:
            return ' ';
        case 66:
            return GODOT_KEY_ENTER;
        case 67:
            return GODOT_KEY_BACKSPACE;
        case 82:
            return GODOT_KEY_MENU;
        case 92:
            return GODOT_KEY_PAGEUP;
        case 93:
            return GODOT_KEY_PAGEDOWN;
        case 111:
            return GODOT_KEY_ESCAPE;
        case 112:
            return GODOT_KEY_DELETE;
        case 113:
        case 114:
            return GODOT_KEY_CTRL;
        case 117:
        case 118:
            return GODOT_KEY_META;
        case 122:
            return GODOT_KEY_HOME;
        case 123:
            return GODOT_KEY_END;
        case 124:
            return GODOT_KEY_INSERT;
        default:
            return GODOT_KEY_NONE;
    }
}

}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_godottest_MainActivity_nativeInitEgl(JNIEnv *env, jobject thiz,jstring execPath,jstring projPath) {
    initGodotOs(logger);
    godot_step = GODOT_LIB_SETUP2;
// 1. 转换 jstring 为 const char*
    const char* nativeExecPath = env->GetStringUTFChars(execPath, 0);
    const char* nativeProjectPath = env->GetStringUTFChars(projPath, 0);

    // 2. 构建命令参数列表
    std::vector<std::string> cmds = {
            // 添加你的命令参数，例如：
            "--path",
            nativeProjectPath,
            "--print-fps"
    };

    // 3. 自动管理命令参数内存 (无需手动delete)
    auto deleter = [](char** ptr) {
        for (size_t i = 0; ptr[i]; i++) delete[] ptr[i];
        delete[] ptr;
    };

    // 4. 创建参数数组
    std::unique_ptr<char*[], decltype(deleter)> args(new char*[cmds.size() + 1], deleter);

    // 5. 填充参数数组
    for (size_t i = 0; i < cmds.size(); i++) {
        args[i] = new char[cmds[i].size() + 1];
        strcpy(args[i], cmds[i].c_str());
    }
    args[cmds.size()] = nullptr; // 添加null终止符

    // 6. 调用Godot设置函数
    bool ret = godotLibSetup(nativeExecPath, args.get(), cmds.size());
    logger("setup %d",ret);
    env->ReleaseStringUTFChars(execPath, nativeExecPath);
    env->ReleaseStringUTFChars(projPath, nativeProjectPath);

}
extern "C"
JNIEXPORT jboolean JNICALL
Java_com_example_godottest_MainActivity_nativeUpdateFrame(JNIEnv *env, jobject thiz) {
    if (godotLibShouldQuit()) {
        return false;
    }

    bool swap = godotLibStep(godot_step);
    if(godot_step == GODOT_LIB_SETUP2)
    {
        godot_step = GODOT_LIB_SET_BOOT_LOGO;
    }
    else if(godot_step == GODOT_LIB_SET_BOOT_LOGO)
    {
        godot_step = GODOT_LIB_INIT;
    }
    else if(godot_step == GODOT_LIB_INIT)
    {
        godot_step = GODOT_LIB_RUN;
    }
    return swap;
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_godottest_MainActivity_nativeSizeChange(JNIEnv *env, jobject thiz, jint w,
                                                         jint h) {
    glViewport(0, 0, w, h);
    godotLibWindowChange(w, h);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_godottest_MainActivity_nativeTouchEvent(JNIEnv *env, jobject thiz, jint action,
                                                         jfloat x, jfloat y, jint modifiers) {
    switch (action) {
        case 0:
            godotLibMouseMove(x, y, (uint32_t)modifiers);
            godotLibMouseButton(1, true, x, y, (uint32_t)modifiers);
            break;
        case 1:
        case 3:
            godotLibMouseMove(x, y, (uint32_t)modifiers);
            godotLibMouseButton(1, false, x, y, (uint32_t)modifiers);
            break;
        case 2:
            godotLibMouseMove(x, y, (uint32_t)modifiers);
            break;
        default:
            break;
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_godottest_MainActivity_nativeMouseWheel(JNIEnv *env, jobject thiz, jfloat x,
                                                         jfloat y, jfloat wheel_x,
                                                         jfloat wheel_y, jint modifiers) {
    godotLibMouseWheel(x, y, wheel_x, wheel_y, (uint32_t)modifiers);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_godottest_MainActivity_nativeKeyEvent(JNIEnv *env, jobject thiz, jint keycode,
                                                       jint unicode, jboolean pressed,
                                                       jboolean echo, jint modifiers) {
    uint32_t godot_key = godotKeyFromAndroid((int)keycode, (int)unicode);
    if (godot_key != GODOT_KEY_NONE) {
        godotLibKeyEvent(godot_key, godot_key, pressed == JNI_TRUE, echo == JNI_TRUE, (uint32_t)modifiers);
    }
    if (pressed == JNI_TRUE && unicode >= 32) {
        godotLibTextInput((uint32_t)unicode);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_godottest_MainActivity_nativeShutdown(JNIEnv *env, jobject thiz) {
    godotLibShutdown();
    godot_step = GODOT_LIB_SETUP2;
}
