// embedded_export.h
#pragma once

#include <stdint.h>

#define EXPORT __attribute__((visibility("default")))

extern "C" {
    typedef enum {
        GODOT_LIB_SETUP2 = 0,
        GODOT_LIB_SET_BOOT_LOGO,
        GODOT_LIB_INIT,
        GODOT_LIB_RUN,
    } GODOT_LIB_STEP_TYPE;
    typedef enum {
        TYPE_INT,
        TYPE_BOOL,
        TYPE_FLOAT,
        TYPE_STRING,
    }GD_TYPE;
    typedef enum {
        GODOT_LIB_MOD_SHIFT = 1 << 0,
        GODOT_LIB_MOD_ALT = 1 << 1,
        GODOT_LIB_MOD_CTRL = 1 << 2,
        GODOT_LIB_MOD_META = 1 << 3,
    } GODOT_LIB_MODIFIER_MASK;
typedef void (*GODOT_LOGGER_NOTIFY)(const char *format, ...);
EXPORT void initGodotOs(GODOT_LOGGER_NOTIFY logInterface);
EXPORT bool godotLibSetup(const char* execPath, char** cmdLine,int cmdLen);
EXPORT bool godotLibStep(GODOT_LIB_STEP_TYPE step);
EXPORT bool godotLibShouldQuit();
EXPORT void godotLibShutdown();
EXPORT void godotLibWindowChange(int w,int h);
EXPORT void godotLibMouseMove(float x, float y, uint32_t modifiers);
EXPORT void godotLibMouseButton(int button, bool pressed, float x, float y, uint32_t modifiers);
EXPORT void godotLibMouseWheel(float x, float y, float wheel_x, float wheel_y, uint32_t modifiers);
EXPORT void godotLibKeyEvent(uint32_t keycode, uint32_t physical_keycode, bool pressed, bool echo, uint32_t modifiers);
EXPORT void godotLibTextInput(uint32_t unicode);
EXPORT void godotLibSetGDValue(GD_TYPE p_name,char *paramName,void *val);
EXPORT void godotLibGetGDValue(GD_TYPE p_name,char *paramName,void *val);
}
