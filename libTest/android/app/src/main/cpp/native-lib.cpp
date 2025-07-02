#include <jni.h>
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
// 顶点着色器源码
const char* vertexShaderSource =
        "#version 100\n"
        "attribute vec4 in_Position;       \n"
        "void main() {                   \n"
        "   gl_Position = in_Position;     \n"
        "}                               \n";

// 片段着色器源码
const char* fragmentShaderSource =
        "#version 100\n"
        "precision mediump float;        \n"
        "void main() {                   \n"
        "   gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0); \n"  // 红色
        "}                               \n";

GLuint createProgram() {
    // 创建顶点着色器
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    GLint vShaderCompiled = 0;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vShaderCompiled);
    if (!vShaderCompiled) {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        printf("Vertex shader compilation failed: %s\n", infoLog);
        glDeleteShader(vertexShader);
        return -1;
    }


    // 创建片段着色器
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    GLint fShaderCompiled = 0;
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fShaderCompiled);
    if (!fShaderCompiled) {
        char infoLog[512];
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        printf("Fragment shader compilation failed: %s\n", infoLog);
        glDeleteShader(fragmentShader);
        return -2;
    }

    // 创建着色器程序
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    // 清理临时资源
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}
void renderMyFrame() {
    static GLuint program = -1;
    if(program==-1)
    {
        program=createProgram();
        printf("program %d\n",program);
    }

    glUseProgram(program);

    // 定义三角形顶点数据（NDC坐标系）
    GLfloat vertices[] = {
            0.0f,  0.5f, 0.0f,  // 顶部顶点
            -0.5f, -0.5f, 0.0f,  // 左下顶点
            0.5f, -0.5f, 0.0f   // 右下顶点
    };

    GLint positionAttrib = glGetAttribLocation(program, "in_Position");
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vertices);

    // 绘制三角形
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // 禁用顶点属性数组
    glDisableVertexAttribArray(positionAttrib);
    glUseProgram(0);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_godottest_MainActivity_nativeInitEgl(JNIEnv *env, jobject thiz,jstring execPath,jstring projPath) {
    initGodotOs(logger);
// 1. 转换 jstring 为 const char*
    const char* nativeExecPath = env->GetStringUTFChars(execPath, 0);

    // 2. 构建命令参数列表
    std::vector<std::string> cmds = {
            // 添加你的命令参数，例如：
            "--path",
            env->GetStringUTFChars(projPath, 0),
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
    // 7. 释放JNI字符串资源
//    env->ReleaseStringUTFChars(execPath, nativeExecPath);
//    for (auto& cmd : cmds) {
//        env->ReleaseStringUTFChars(projPath, cmd.c_str());
//    }

}
extern "C"
JNIEXPORT jboolean JNICALL
Java_com_example_godottest_MainActivity_nativeUpdateFrame(JNIEnv *env, jobject thiz) {
    static GODOT_LIB_STEP_TYPE step = GODOT_LIB_SETUP2;
    godotLibStep(step);
    if(step == GODOT_LIB_SETUP2)
    {
        step = GODOT_LIB_SET_BOOT_LOGO;
    }
    else if(step == GODOT_LIB_SET_BOOT_LOGO)
    {
        step = GODOT_LIB_INIT;
    }
    else if(step == GODOT_LIB_INIT)
    {
        step = GODOT_LIB_RUN;
    }
    renderMyFrame();
    return true;
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_godottest_MainActivity_nativeSizeChange(JNIEnv *env, jobject thiz, jint w,
                                                         jint h) {

}