#include <GLFW/glfw3.h>
#include "embedded_export.h"
#include <cstdio>
#include <cstdlib>
#include <cstdarg> 
#include <GLES2/gl2.h>
#include <chrono> // For time calculations

void renderMyFrame();

// Godot 日志回调（转发到控制台）
void godotLogger(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("\n");
}

int main(int argc, char *argv[]) {
    // 1. 初始化 GLFW
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }
    // 设置OpenGL 3.0 + EGL
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
    // 2. 创建窗口
    GLFWwindow* window = glfwCreateWindow(800, 600, "Godot + GLFW", nullptr, nullptr);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

   
    // 5. 初始化 Godot
    initGodotOs(godotLogger);

    char* cmdLine[] = { (char*)"--path", (char*)"../../testProj" }; // 命令行参数
    if (!godotLibSetup(argv[0], cmdLine,2)) {
        fprintf(stderr, "Godot setup failed\n");
        glfwTerminate();
        return -1;
    }

    // 6. 分步执行 Godot 初始化
    
GODOT_LIB_STEP_TYPE step = GODOT_LIB_SETUP2;
int fps = 0;
auto start_time = std::chrono::high_resolution_clock::now();
    // 7. 主循环
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.1f, 0.2f, 0.1f, 1.0f);
        if (godotLibStep(step)) {
            //glfwSwapBuffers(window);
        }

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
        else
        {
            renderMyFrame();  // 渲染自定义内容
            glfwSwapBuffers(window);
        }
        fps++;
        auto draw_end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> draw_duration = draw_end_time - start_time;
        if(draw_duration.count() > 1.0)
        {
            printf("FPS:%d\n",fps);
            fps=0;
            start_time=draw_end_time;
        }

    }
    glfwTerminate();
    return 0;
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