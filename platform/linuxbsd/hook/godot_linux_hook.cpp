#include "godot_linux_hook.h"
#include "os_linuxbsd.h"


#include "core/config/engine.h"
#include "core/config/project_settings.h"
#include "x11/display_server_x11.h"
#include "core/input/input.h"
#include "main/main.h"
#include <iostream>
static OS_LinuxBSD *os_linuxbsd = nullptr;
static GODOT_LOGGER_NOTIFY logInterface=nullptr;

void initGodotOs(GODOT_LOGGER_NOTIFY logInter){
	
    os_linuxbsd = new OS_LinuxBSD();
	{
		logInterface=logInter;
		logInterface("os init");
	}
}
bool godotLibSetup(const char* execPath, char** cmdLine,int cmdLen){
	
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
        auto main_loop=os_linuxbsd->get_main_loop();
        main_loop->initialize();
		ProjectSettings* project_settings = ProjectSettings::get_singleton();
		project_settings->set_setting("network/tls/verify_certificates", false);
		project_settings->save();
		return false;
	}

	bool should_swap_buffers = true;
	DisplayServer::get_singleton()->process_events(); 
	Main::iteration();
return should_swap_buffers;
}
void godotLibWindowChange(int w,int h)
{


}