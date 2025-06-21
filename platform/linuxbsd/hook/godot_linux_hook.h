#pragma once


#define EXPORT __attribute__((visibility("default")))

extern "C" {
typedef void (*GODOT_LOGGER_NOTIFY)(const char *format, ...);
EXPORT void initGodotOs(GODOT_LOGGER_NOTIFY logInterface);
EXPORT bool godotLibSetup(const char* execPath, char** cmdLine,int cmdLen);
EXPORT bool godotLibStep(int step);
EXPORT void godotLibWindowChange(int w,int h);
}
