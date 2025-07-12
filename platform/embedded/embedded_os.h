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
    double x, y;//mouse
    unsigned int mouse_button_state = 0;
public:
    EmbeddedOS();
    ~EmbeddedOS();
    static EmbeddedOS *get_singleton();
    virtual void initialize() override;

    virtual void initialize_joypads() override;
    virtual void finalize() override;
    virtual bool _check_internal_feature_support(const String &p_feature) override;
    
    void set_logger(GODOT_LOGGER_NOTIFY func) { log_callback = func; }
    void log(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        int len = vsnprintf(nullptr, 0, fmt, args);
        va_end(args);
        
        std::string buf(len + 1, '\0');
        va_start(args, fmt);
        vsnprintf(&buf[0], buf.size(), fmt, args);
        va_end(args);
    
        if (log_callback) log_callback(buf.c_str());
    }

    virtual MainLoop *get_main_loop() const override;
    Size2i get_display_size();
    bool should_swap_buffers;
    virtual Point2i get_mouse_position() const ;
    virtual unsigned int get_mouse_button_state() const;
    void setMouseState(double newX, double newY, unsigned int newButtonState) {
        x = newX;
        y = newY;
        mouse_button_state = newButtonState;
    }
    void setGDValue(GD_TYPE p_name,char *paramName,void *value);
    void getGDValue(GD_TYPE p_name,char *paramName,void *value);
protected:
    GODOT_LOGGER_NOTIFY log_callback = nullptr;
    virtual void set_main_loop(MainLoop *p_main_loop) override;
};