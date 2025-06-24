// embedded_export.h
#pragma once
#ifdef __cplusplus
extern "C" {
#endif

// 初始化Godot引擎
void godot_embedded_init(void* gl_context, void* window_handle);

// 处理输入事件
void godot_embedded_handle_input(int type, int key, int state);

// 主循环迭代
void godot_embedded_step();

// 设置日志回调
typedef void (*LogCallback)(int level, const char* message);
void godot_embedded_set_logger(LogCallback callback);

#ifdef __cplusplus
}
#endif