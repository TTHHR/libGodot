#include "embedded_os.h"
#include "main/main.h"
#include <GLFW/glfw3.h>
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

// 1. 初始化 GLFW
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    // 2. 创建窗口
    GLFWwindow* window = glfwCreateWindow(800, 600, "Godot + GLFW", nullptr, nullptr);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
glfwSwapInterval(1); // Enable VSync


	EmbeddedOS os;
	os.set_logger(godotLogger);


	Error err = Main::setup(argv[0], argc - 1, &argv[1],false);
	if(os.should_swap_buffers) {
		os.should_swap_buffers=false;
		glfwSwapBuffers(window);
	}
	sleep(1);


	err = Main::setup2(false);
if(os.should_swap_buffers) {
		os.should_swap_buffers=false;
		glfwSwapBuffers(window);
	}
		sleep(1);

	Main::setup_boot_logo();
if(os.should_swap_buffers) {
		os.should_swap_buffers=false;
		glfwSwapBuffers(window);
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
		glfwSwapBuffers(window);
	}


    }
    glfwTerminate();
    return 0;
}