#include "embedded_os.h"
#include "main/main.h"
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_X11
#include <GLFW/glfw3native.h>
#include <EGL/egl.h>
void godotLogger(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("\n");
}

int main(int argc,char *argv[])
{
	setlocale(LC_CTYPE, "");

	// We must override main when testing is enabled
	TEST_MAIN_OVERRIDE

	char *cwd = (char *)malloc(PATH_MAX);
	ERR_FAIL_NULL_V(cwd, ERR_OUT_OF_MEMORY);
	char *ret = getcwd(cwd, PATH_MAX);

// 1. 初始化GLFW
    if (!glfwInit()) return -1;

    // 设置OpenGL 3.0 + EGL
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);

    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL + EGL", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    // 2. 获取原生窗口句柄
    EGLNativeWindowType nativeWin;

    nativeWin = glfwGetX11Window(window);
 

    // 3. EGL初始化流程
    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(display, nullptr, nullptr);

    // 配置EGL属性
    const EGLint configAttribs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 24,
        EGL_STENCIL_SIZE, 8,
        EGL_NONE
    };

    EGLConfig config;
    EGLint numConfigs;
    eglChooseConfig(display, configAttribs, &config, 1, &numConfigs);

    // 创建EGL Surface
    EGLSurface surface = eglCreateWindowSurface(display, config, nativeWin, nullptr);

    // 创建EGL Context
    const EGLint contextAttribs[] = {
        EGL_CONTEXT_MAJOR_VERSION, 3,
        EGL_CONTEXT_MINOR_VERSION, 0,
        EGL_NONE
    };
    EGLContext context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs);

    // 绑定上下文
    eglMakeCurrent(display, surface, surface, context);


	EmbeddedOS os;
	os.set_logger(godotLogger);


	Error err = Main::setup(argv[0], argc - 1, &argv[1],false);
	if(os.should_swap_buffers) {
		os.should_swap_buffers=false;
		eglSwapBuffers(display, surface);
	}
	sleep(1);


	err = Main::setup2(false);
if(os.should_swap_buffers) {
		os.should_swap_buffers=false;
		eglSwapBuffers(display, surface);
	}
		sleep(1);

	Main::setup_boot_logo();
if(os.should_swap_buffers) {
		os.should_swap_buffers=false;
		eglSwapBuffers(display, surface);
	}
		sleep(1);
	Main::start();

	auto main_loop=os.get_main_loop();
	main_loop->initialize();
	 while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // glClearColor(0.1f, 0.2f, 0.1f, 1.0f);
        if (Main::iteration()) {
			break;
		}
            // glfwMakeContextCurrent(window);
        
    	if(os.should_swap_buffers) {
			os.should_swap_buffers=false;
			eglSwapBuffers(display, surface);
		}


    }
    glfwTerminate();
    return 0;
}