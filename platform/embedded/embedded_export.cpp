#include "embedded_os.h"
#include "embedded_export.h"

#include "core/config/engine.h"
#include "core/config/project_settings.h"
#include "core/input/input.h"
#include "main/main.h"
#include <iostream>

static EmbeddedOS* embedded_os = nullptr;


void initGodotOs(GODOT_LOGGER_NOTIFY logInterface)
{
    embedded_os = new EmbeddedOS();
    embedded_os->set_logger(logInterface);
	embedded_os->log("init godot lib");
}
bool godotLibSetup(const char* execPath, char** cmdLine,int cmdLen){
	
	Error err = Main::setup(execPath, cmdLen, cmdLine, false);
	embedded_os->log("Main::setup %d",static_cast<int>(err));
	if (err != OK) {
		return false;
	}
	
	return true;
}
bool godotLibStep(GODOT_LIB_STEP_TYPE step)
{
	
	Error err ;
	if(step==GODOT_LIB_SETUP2)
	{
		err = Main::setup2(false);
		embedded_os->log("Main::setup2 %d", static_cast<int>(err));
		return false;
	}
	else if (step==GODOT_LIB_SET_BOOT_LOGO)
	{
		embedded_os->log("Main::setup_boot_logo ");
		Main::setup_boot_logo();
		return true;
	}
	else if (step==GODOT_LIB_INIT)
	{
		Main::start();
		embedded_os->log("Main::start ");
        auto main_loop=embedded_os->get_main_loop();
        main_loop->initialize();
		ProjectSettings* project_settings = ProjectSettings::get_singleton();
		project_settings->set_setting("network/tls/verify_certificates", false);
		project_settings->save();
		return false;
	}
	else if (step==GODOT_LIB_RUN)
	{
		embedded_os->should_swap_buffers=false;
		bool exit = Main::iteration();
		if (exit) {
			embedded_os->log("!!!!!some err!!!!");
			return false;
		}
		return embedded_os->should_swap_buffers;
	}
	embedded_os->log("step err %d",step);
return false;
}
void godotLibWindowChange(int w,int h)
{


}