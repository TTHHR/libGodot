// embedded_os.h
#pragma once
#include "core/os/os.h"
#include "core/config/project_settings.h"
#include "drivers/unix/os_unix.h"


class EmbeddedOS : public OS_Unix {
public:
    EmbeddedOS();
    ~EmbeddedOS();
    virtual void initialize() override;
    virtual void set_gl_context(void* context) { gl_context = context; }
    virtual void set_host_window(void* window) { host_window = window; }
    
    // 宿主提供的日志接口
    using LogFunc = void (*)(int level, const char* msg);
    void set_logger(LogFunc func) { log_callback = func; }
    
protected:
    void* gl_context = nullptr;
    void* host_window = nullptr;
    LogFunc log_callback = nullptr;
};