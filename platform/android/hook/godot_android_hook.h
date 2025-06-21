#pragma once
#include <android/asset_manager_jni.h>
#include <android/input.h>
#include <android/native_window_jni.h>
#ifdef __ANDROID__
#define EXPORT __attribute__((visibility("default")))
#else
#define EXPORT
#endif
extern "C" {
typedef void (*GODOT_LOGGER_NOTIFY)(const char *format, ...);
EXPORT void initAndroidGodotOs(GODOT_LOGGER_NOTIFY logInterface);
EXPORT bool initGodotOs(const char* execPath, char** cmdLine,int cmdLen);
EXPORT bool godotLibStep(int step);
EXPORT void godotLibChange(int w,int h);
}
