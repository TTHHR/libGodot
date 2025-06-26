// embedded_export.h
#pragma once

#define EXPORT __attribute__((visibility("default")))

extern "C" {
    typedef enum {
        GODOT_LIB_SETUP2 = 0,
        GODOT_LIB_SET_BOOT_LOGO,
        GODOT_LIB_INIT,
        GODOT_LIB_RUN,
    } GODOT_LIB_STEP_TYPE;
typedef void (*GODOT_LOGGER_NOTIFY)(const char *format, ...);
EXPORT void initGodotOs(GODOT_LOGGER_NOTIFY logInterface,void* gl_context, void* window_handle);
EXPORT bool godotLibSetup(const char* execPath, char** cmdLine,int cmdLen);
EXPORT bool godotLibStep(GODOT_LIB_STEP_TYPE step);
EXPORT void godotLibWindowChange(int w,int h);
}
