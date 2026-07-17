#include "embedded_os.h"
#include "embedded_export.h"

#include "core/config/engine.h"
#include "core/config/project_settings.h"
#include "core/input/input.h"
#include "main/main.h"
#include "servers/display_server.h"
#include <iostream>

static EmbeddedOS* embedded_os = nullptr;
static bool godot_started = false;
static bool godot_shutdown = false;
static bool godot_should_quit = false;
static bool godot_main_loop_initialized = false;


void initGodotOs(GODOT_LOGGER_NOTIFY logInterface)
{
    embedded_os = new EmbeddedOS();
    embedded_os->set_logger(logInterface);
    godot_started = false;
    godot_shutdown = false;
    godot_should_quit = false;
    godot_main_loop_initialized = false;
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
        godot_started = true;
		embedded_os->log("Main::start ");
        auto main_loop=embedded_os->get_main_loop();
        if (main_loop) {
            main_loop->initialize();
            godot_main_loop_initialized = true;
        }
		ProjectSettings* project_settings = ProjectSettings::get_singleton();
		project_settings->set_setting("network/tls/verify_certificates", false);
		project_settings->save();
		return false;
	}
	else if (step==GODOT_LIB_RUN)
	{
		embedded_os->should_swap_buffers=false;
        if (DisplayServer::get_singleton()) {
            DisplayServer::get_singleton()->process_events();
        }
		bool exit = Main::iteration();
		if (exit) {
            godot_should_quit = true;
			embedded_os->log("!!!!!some err!!!!");
			return false;
		}
		return embedded_os->should_swap_buffers;
	}
	embedded_os->log("step err %d",step);
return false;
}

bool godotLibShouldQuit()
{
    return godot_should_quit;
}

void godotLibShutdown()
{
    if (!embedded_os || godot_shutdown) {
        return;
    }

    if (godot_started) {
        if (godot_main_loop_initialized) {
            MainLoop *main_loop = embedded_os->get_main_loop();
            if (main_loop) {
                embedded_os->log("MainLoop::finalize");
                main_loop->finalize();
            }
            godot_main_loop_initialized = false;
        }
        embedded_os->log("Main::cleanup");
        Main::cleanup();
    }

    delete embedded_os;
    embedded_os = nullptr;
    godot_started = false;
    godot_shutdown = true;
    godot_should_quit = false;
    godot_main_loop_initialized = false;
}

void godotLibWindowChange(int w,int h)
{
    if (!embedded_os || w <= 0 || h <= 0) {
        return;
    }

    Size2i size(w, h);
    embedded_os->set_display_size(size);

    if (DisplayServer::get_singleton()) {
        DisplayServer::get_singleton()->window_set_size(size, DisplayServer::MAIN_WINDOW_ID);
    }
}

void godotLibMouseMove(float x, float y, uint32_t modifiers)
{
    if (!embedded_os) {
        return;
    }
    EmbeddedOS::EmbeddedInputEvent event;
    event.type = EmbeddedOS::EmbeddedInputEvent::MOUSE_MOVE;
    event.x = x;
    event.y = y;
    event.modifiers = modifiers;
    embedded_os->push_input_event(event);
}

void godotLibMouseButton(int button, bool pressed, float x, float y, uint32_t modifiers)
{
    if (!embedded_os) {
        return;
    }
    EmbeddedOS::EmbeddedInputEvent event;
    event.type = EmbeddedOS::EmbeddedInputEvent::MOUSE_BUTTON;
    event.button = button;
    event.pressed = pressed;
    event.x = x;
    event.y = y;
    event.modifiers = modifiers;
    embedded_os->push_input_event(event);
}

void godotLibMouseWheel(float x, float y, float wheel_x, float wheel_y, uint32_t modifiers)
{
    if (!embedded_os) {
        return;
    }
    EmbeddedOS::EmbeddedInputEvent event;
    event.type = EmbeddedOS::EmbeddedInputEvent::MOUSE_WHEEL;
    event.x = x;
    event.y = y;
    event.wheel_x = wheel_x;
    event.wheel_y = wheel_y;
    event.modifiers = modifiers;
    embedded_os->push_input_event(event);
}

void godotLibKeyEvent(uint32_t keycode, uint32_t physical_keycode, bool pressed, bool echo, uint32_t modifiers)
{
    if (!embedded_os) {
        return;
    }
    EmbeddedOS::EmbeddedInputEvent event;
    event.type = EmbeddedOS::EmbeddedInputEvent::KEY;
    event.keycode = keycode;
    event.physical_keycode = physical_keycode;
    event.pressed = pressed;
    event.echo = echo;
    event.modifiers = modifiers;
    embedded_os->push_input_event(event);
}

void godotLibTextInput(uint32_t unicode)
{
    if (!embedded_os) {
        return;
    }
    EmbeddedOS::EmbeddedInputEvent event;
    event.type = EmbeddedOS::EmbeddedInputEvent::TEXT;
    event.unicode = unicode;
    embedded_os->push_input_event(event);
}
void godotLibSetGDValue(GD_TYPE p_name,char *paramName,void *val)
{
    embedded_os->setGDValue(p_name, paramName, val);
}
void godotLibGetGDValue(GD_TYPE p_name,char *paramName,void *val)
{
    embedded_os->getGDValue(p_name, paramName, val);
}
