#include "godot_android_hook.h"
#include "os_android.h"

#include "java_godot_io_wrapper.h"
#include "java_godot_wrapper.h"
#include "core/config/engine.h"
#include "core/config/project_settings.h"
#include "display_server_android.h"
#include "core/input/input.h"
#include "main/main.h"
#include <iostream>
static OS_Android *os_android = nullptr;
static GODOT_LOGGER_NOTIFY logInterface=nullptr;
static GodotJavaWrapper* godotJavaHook=nullptr;
static GodotIOJavaWrapper* godotIoHook=nullptr;
void initAndroidGodotOs(GODOT_LOGGER_NOTIFY logInter){
	godotJavaHook=new GodotJavaWrapper(reinterpret_cast<void*>(logInter));
	godotIoHook=new GodotIOJavaWrapper(reinterpret_cast<void*>(logInter));
    os_android = new OS_Android(godotJavaHook,godotIoHook,true);
	{
		logInterface=logInter;
		logInterface("os init");
	}
}
bool initGodotOs(const char* execPath, char** cmdLine,int cmdLen){
	godotIoHook->setBaseDir(String(execPath));
	Error err = Main::setup(execPath, cmdLen, cmdLine, false);
	logInterface("Main::setup %d",err);
	if (err != OK) {
		return false;
	}
	
	return true;
}
bool godotLibStep(int step)
{
	
	Error err ;
	if(step==1)
	{
		err = Main::setup2(false);
		logInterface("Main::setup2 %d", err);
		return false;
	}
	else if (step==2)
	{
		logInterface("Main::setup_boot_logo ");
		Main::setup_boot_logo();
		return false;
	}
	else if (step==3)
	{
		Main::start();
		logInterface("Main::start ");
		auto main_loop=os_android->get_main_loop();

		main_loop->initialize();
		ProjectSettings* project_settings = ProjectSettings::get_singleton();
		project_settings->set_setting("network/tls/verify_certificates", false);
		project_settings->save();
		return false;
	}

	bool should_swap_buffers = false;
	os_android->main_loop_iterate(&should_swap_buffers);
return should_swap_buffers;
}
void godotLibChange(int w,int h)
{
	auto size=Size2i(w,h);
	if(os_android)
	os_android->set_display_size(size);
	// {
	// DisplayServerAndroid::get_singleton()->reset_window();
	// DisplayServerAndroid::get_singleton()->notify_surface_changed(w, h);
	// }

}