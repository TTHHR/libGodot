#include <GLFW/glfw3.h>
#include "embedded_export.h"
#include <cstdio>
#include <cstdlib>
#include <cstdarg> 
#include <GLES2/gl2.h>
#include <chrono> // For time calculations
#include <cstdint>
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"

namespace {

constexpr uint32_t GODOT_KEY_SPECIAL = 1u << 22;
constexpr uint32_t GODOT_KEY_NONE = 0;
constexpr uint32_t GODOT_KEY_ESCAPE = GODOT_KEY_SPECIAL | 0x01;
constexpr uint32_t GODOT_KEY_TAB = GODOT_KEY_SPECIAL | 0x02;
constexpr uint32_t GODOT_KEY_BACKSPACE = GODOT_KEY_SPECIAL | 0x04;
constexpr uint32_t GODOT_KEY_ENTER = GODOT_KEY_SPECIAL | 0x05;
constexpr uint32_t GODOT_KEY_KP_ENTER = GODOT_KEY_SPECIAL | 0x06;
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
constexpr uint32_t GODOT_KEY_CAPSLOCK = GODOT_KEY_SPECIAL | 0x19;
constexpr uint32_t GODOT_KEY_F1 = GODOT_KEY_SPECIAL | 0x1C;
constexpr uint32_t GODOT_KEY_KP_MULTIPLY = GODOT_KEY_SPECIAL | 0x81;
constexpr uint32_t GODOT_KEY_KP_DIVIDE = GODOT_KEY_SPECIAL | 0x82;
constexpr uint32_t GODOT_KEY_KP_SUBTRACT = GODOT_KEY_SPECIAL | 0x83;
constexpr uint32_t GODOT_KEY_KP_PERIOD = GODOT_KEY_SPECIAL | 0x84;
constexpr uint32_t GODOT_KEY_KP_ADD = GODOT_KEY_SPECIAL | 0x85;
constexpr uint32_t GODOT_KEY_KP_0 = GODOT_KEY_SPECIAL | 0x86;
constexpr uint32_t GODOT_KEY_MENU = GODOT_KEY_SPECIAL | 0x42;

uint32_t godotModifiersFromGlfwMods(int mods) {
    uint32_t modifiers = 0;
    if (mods & GLFW_MOD_SHIFT) {
        modifiers |= GODOT_LIB_MOD_SHIFT;
    }
    if (mods & GLFW_MOD_ALT) {
        modifiers |= GODOT_LIB_MOD_ALT;
    }
    if (mods & GLFW_MOD_CONTROL) {
        modifiers |= GODOT_LIB_MOD_CTRL;
    }
    if (mods & GLFW_MOD_SUPER) {
        modifiers |= GODOT_LIB_MOD_META;
    }
    return modifiers;
}

uint32_t godotCurrentModifiers(GLFWwindow *window) {
    uint32_t modifiers = 0;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
        modifiers |= GODOT_LIB_MOD_SHIFT;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS) {
        modifiers |= GODOT_LIB_MOD_ALT;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS) {
        modifiers |= GODOT_LIB_MOD_CTRL;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SUPER) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SUPER) == GLFW_PRESS) {
        modifiers |= GODOT_LIB_MOD_META;
    }
    return modifiers;
}

uint32_t godotKeyFromGlfw(int key) {
    if (key >= GLFW_KEY_A && key <= GLFW_KEY_Z) {
        return 'A' + (key - GLFW_KEY_A);
    }
    if (key >= GLFW_KEY_0 && key <= GLFW_KEY_9) {
        return '0' + (key - GLFW_KEY_0);
    }
    if (key >= GLFW_KEY_F1 && key <= GLFW_KEY_F25) {
        return GODOT_KEY_F1 + (key - GLFW_KEY_F1);
    }
    if (key >= GLFW_KEY_KP_0 && key <= GLFW_KEY_KP_9) {
        return GODOT_KEY_KP_0 + (key - GLFW_KEY_KP_0);
    }

    switch (key) {
        case GLFW_KEY_SPACE:
            return ' ';
        case GLFW_KEY_APOSTROPHE:
            return 0x27;
        case GLFW_KEY_COMMA:
            return ',';
        case GLFW_KEY_MINUS:
            return '-';
        case GLFW_KEY_PERIOD:
            return '.';
        case GLFW_KEY_SLASH:
            return '/';
        case GLFW_KEY_SEMICOLON:
            return ';';
        case GLFW_KEY_EQUAL:
            return '=';
        case GLFW_KEY_LEFT_BRACKET:
            return '[';
        case GLFW_KEY_BACKSLASH:
            return '\\';
        case GLFW_KEY_RIGHT_BRACKET:
            return ']';
        case GLFW_KEY_GRAVE_ACCENT:
            return '`';
        case GLFW_KEY_ESCAPE:
            return GODOT_KEY_ESCAPE;
        case GLFW_KEY_ENTER:
            return GODOT_KEY_ENTER;
        case GLFW_KEY_TAB:
            return GODOT_KEY_TAB;
        case GLFW_KEY_BACKSPACE:
            return GODOT_KEY_BACKSPACE;
        case GLFW_KEY_INSERT:
            return GODOT_KEY_INSERT;
        case GLFW_KEY_DELETE:
            return GODOT_KEY_DELETE;
        case GLFW_KEY_RIGHT:
            return GODOT_KEY_RIGHT;
        case GLFW_KEY_LEFT:
            return GODOT_KEY_LEFT;
        case GLFW_KEY_DOWN:
            return GODOT_KEY_DOWN;
        case GLFW_KEY_UP:
            return GODOT_KEY_UP;
        case GLFW_KEY_PAGE_UP:
            return GODOT_KEY_PAGEUP;
        case GLFW_KEY_PAGE_DOWN:
            return GODOT_KEY_PAGEDOWN;
        case GLFW_KEY_HOME:
            return GODOT_KEY_HOME;
        case GLFW_KEY_END:
            return GODOT_KEY_END;
        case GLFW_KEY_CAPS_LOCK:
            return GODOT_KEY_CAPSLOCK;
        case GLFW_KEY_KP_DECIMAL:
            return GODOT_KEY_KP_PERIOD;
        case GLFW_KEY_KP_DIVIDE:
            return GODOT_KEY_KP_DIVIDE;
        case GLFW_KEY_KP_MULTIPLY:
            return GODOT_KEY_KP_MULTIPLY;
        case GLFW_KEY_KP_SUBTRACT:
            return GODOT_KEY_KP_SUBTRACT;
        case GLFW_KEY_KP_ADD:
            return GODOT_KEY_KP_ADD;
        case GLFW_KEY_KP_ENTER:
            return GODOT_KEY_KP_ENTER;
        case GLFW_KEY_LEFT_SHIFT:
        case GLFW_KEY_RIGHT_SHIFT:
            return GODOT_KEY_SHIFT;
        case GLFW_KEY_LEFT_CONTROL:
        case GLFW_KEY_RIGHT_CONTROL:
            return GODOT_KEY_CTRL;
        case GLFW_KEY_LEFT_ALT:
        case GLFW_KEY_RIGHT_ALT:
            return GODOT_KEY_ALT;
        case GLFW_KEY_LEFT_SUPER:
        case GLFW_KEY_RIGHT_SUPER:
            return GODOT_KEY_META;
        case GLFW_KEY_MENU:
            return GODOT_KEY_MENU;
        default:
            return GODOT_KEY_NONE;
    }
}

int godotMouseButtonFromGlfw(int button) {
    switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            return 1;
        case GLFW_MOUSE_BUTTON_RIGHT:
            return 2;
        case GLFW_MOUSE_BUTTON_MIDDLE:
            return 3;
        case GLFW_MOUSE_BUTTON_4:
            return 8;
        case GLFW_MOUSE_BUTTON_5:
            return 9;
        default:
            return 0;
    }
}

void godotCursorPosCallback(GLFWwindow *window, double x, double y) {
    ImGui_ImplGlfw_CursorPosCallback(window, x, y);
    godotLibMouseMove((float)x, (float)y, godotCurrentModifiers(window));
}

void godotMouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
    int godot_button = godotMouseButtonFromGlfw(button);
    if (godot_button == 0 || action == GLFW_REPEAT) {
        return;
    }

    double x = 0;
    double y = 0;
    glfwGetCursorPos(window, &x, &y);
    godotLibMouseButton(godot_button, action == GLFW_PRESS, (float)x, (float)y, godotModifiersFromGlfwMods(mods));
}

void godotScrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
    double x = 0;
    double y = 0;
    glfwGetCursorPos(window, &x, &y);
    godotLibMouseWheel((float)x, (float)y, (float)xoffset, (float)yoffset, godotCurrentModifiers(window));
}

void godotKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
    if (action != GLFW_PRESS && action != GLFW_RELEASE && action != GLFW_REPEAT) {
        return;
    }

    uint32_t godot_key = godotKeyFromGlfw(key);
    if (godot_key == GODOT_KEY_NONE) {
        return;
    }

    bool pressed = action != GLFW_RELEASE;
    bool echo = action == GLFW_REPEAT;
    godotLibKeyEvent(godot_key, godot_key, pressed, echo, godotModifiersFromGlfwMods(mods));
}

void godotCharCallback(GLFWwindow *window, unsigned int codepoint) {
    ImGui_ImplGlfw_CharCallback(window, codepoint);
    if (codepoint != 0) {
        godotLibTextInput((uint32_t)codepoint);
    }
}

void godotWindowSizeCallback(GLFWwindow *window, int width, int height) {
    (void)window;
    godotLibWindowChange(width, height);
}

void godotFramebufferSizeCallback(GLFWwindow *window, int width, int height) {
    (void)window;
    if (width > 0 && height > 0) {
        glViewport(0, 0, width, height);
    }
}

void installGodotInputCallbacks(GLFWwindow *window) {
    glfwSetCursorPosCallback(window, godotCursorPosCallback);
    glfwSetMouseButtonCallback(window, godotMouseButtonCallback);
    glfwSetScrollCallback(window, godotScrollCallback);
    glfwSetKeyCallback(window, godotKeyCallback);
    glfwSetCharCallback(window, godotCharCallback);
    glfwSetWindowSizeCallback(window, godotWindowSizeCallback);
    glfwSetFramebufferSizeCallback(window, godotFramebufferSizeCallback);
}

}

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
    // 2设置OpenGL 3.2 + EGL
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
    // 3. 创建窗口
    GLFWwindow* window = glfwCreateWindow(800, 600, "Godot + GLFW", nullptr, nullptr);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);


    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.DisplaySize = ImVec2(800, 600);
    io.IniFilename = nullptr;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    bool b = ImGui_ImplOpenGL3_Init("#version 300 es"); // 根据ES版本调整
    if (!b)
    {
        printf("imgui init fail\n ");
    }
    installGodotInputCallbacks(window);
    int framebuffer_width = 0;
    int framebuffer_height = 0;
    glfwGetFramebufferSize(window, &framebuffer_width, &framebuffer_height);
    godotFramebufferSizeCallback(window, framebuffer_width, framebuffer_height);
    // 4. 初始化 Godot
    initGodotOs(godotLogger);
    int window_width = 0;
    int window_height = 0;
    glfwGetWindowSize(window, &window_width, &window_height);
    godotLibWindowChange(window_width, window_height);

    const char *project_path = argc > 1 ? argv[1] : "../../testProj";
    printf("Godot project path: %s\n", project_path);
    char* cmdLine[] = { (char*)"--path", (char*)project_path }; // 命令行参数
    if (!godotLibSetup(argv[0], cmdLine,2)) {
        fprintf(stderr, "Godot setup failed\n");
        glfwTerminate();
        return -1;
    }

    // 5. 分步执行 Godot 初始化
    
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
        if (godotLibShouldQuit()) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
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
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            ImGui::Begin("Hello, Godot + GLFW!"); // 创建一个窗口
            ImGui::Text("This is a simple example of using Godot with GLFW and OpenGL ES 3.0.");
            ImGui::End();
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    godotLibShutdown();
    glfwTerminate();
    return 0;
}
