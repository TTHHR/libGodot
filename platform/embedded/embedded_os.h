// embedded_os.h
#pragma once
#include "core/os/os.h"
#include "core/config/project_settings.h"
#include "drivers/unix/os_unix.h"
#include "embedded_export.h"


class EmbeddedOS : public OS_Unix {
    virtual void delete_main_loop() override;
private:
    MainLoop *main_loop = nullptr;
public:
    EmbeddedOS();
    ~EmbeddedOS();
    static EmbeddedOS *get_singleton();
    virtual void initialize() override;

    virtual void initialize_joypads() override;
    virtual void finalize() override;
    virtual bool _check_internal_feature_support(const String &p_feature) override;
    
    void set_logger(GODOT_LOGGER_NOTIFY func) { log_callback = func; }
    void log(const char* format, ...) {
        if (log_callback) {
            va_list args;
            char formatted_message[1024]; // 假设最大长度为1024
            va_start(args, format);
            sprintf(formatted_message,format, args);
            log_callback("%s", formatted_message);
            va_end(args);
        }
    }

    virtual MainLoop *get_main_loop() const override;
    Size2i get_display_size();
    bool should_swap_buffers;
protected:
    GODOT_LOGGER_NOTIFY log_callback = nullptr;
    virtual void set_main_loop(MainLoop *p_main_loop) override;
};