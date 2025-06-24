#include "core/config/engine.h"
#include "embedded_os.h"
#include "embedded_export.h"

static EmbeddedOS* embedded_os = nullptr;


// 初始化Godot引擎
void godot_embedded_init(void* gl_context, void* window_handle)
{

}

// 处理输入事件
void godot_embedded_handle_input(int type, int key, int state)
{

}

// 主循环迭代
void godot_embedded_step()
{

}

// 设置日志回调
void godot_embedded_set_logger(LogCallback callback)
{
    
}
