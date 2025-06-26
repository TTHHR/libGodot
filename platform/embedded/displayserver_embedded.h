#pragma once
#include "servers/display_server.h"
#include "servers/rendering/renderer_compositor.h"
#include "servers/rendering_server.h"
class DisplayServerEmbedded : public DisplayServer {
    GDSOFTCLASS(DisplayServerEmbedded, DisplayServer);

private:
    struct WindowData {
        Size2i size;
    };
    
    WindowData main_window;
    ObjectID window_attached_instance_id;

    Callable window_event_callback;
	Callable input_event_callback;
	Callable input_text_callback;
	Callable rect_changed_callback;

	Callable system_theme_changed;
	Callable hardware_keyboard_connection_changed;

	Callable dialog_callback;
	Callable input_dialog_callback;

	Callable file_picker_callback;
    
public:
    // 构造/析构函数
    static DisplayServer *create_func(const String &p_rendering_driver, WindowMode p_mode, DisplayServer::VSyncMode p_vsync_mode, uint32_t p_flags, const Vector2i *p_position, const Vector2i &p_resolution, int p_screen, Context p_context, int64_t p_parent_window, Error &r_error);
    DisplayServerEmbedded(const String &p_rendering_driver, WindowMode p_mode, uint32_t p_flags, const Size2i &p_resolution, Error &r_error);
    ~DisplayServerEmbedded();
    

    // 显示服务器必须实现的方法
    virtual bool has_feature(Feature p_feature) const override;
    virtual String get_name() const override;
    virtual void swap_buffers() override;
    
    // 窗口尺寸管理
    virtual void window_set_size(const Size2i p_size, WindowID p_window = MAIN_WINDOW_ID) override;
    virtual Size2i window_get_size(WindowID p_window = MAIN_WINDOW_ID) const override;
    virtual Size2i window_get_size_with_decorations(WindowID p_window = MAIN_WINDOW_ID) const override;
    virtual Point2i window_get_position(WindowID p_window = MAIN_WINDOW_ID) const override;
    virtual bool window_get_flag(DisplayServer::WindowFlags p_flag, DisplayServer::WindowID p_window) const  override;
    virtual void window_set_position(const Point2i &p_position, WindowID p_window = MAIN_WINDOW_ID) override;
    virtual Point2i window_get_position_with_decorations(WindowID p_window = MAIN_WINDOW_ID) const override;
    virtual void window_set_current_screen(int p_screen, WindowID p_window = MAIN_WINDOW_ID) override ;
    virtual int window_get_current_screen(WindowID p_window = MAIN_WINDOW_ID) const override ;
    virtual void window_set_max_size(const Size2i p_size, WindowID p_window = MAIN_WINDOW_ID) override ;
    virtual Size2i window_get_max_size(WindowID p_window = MAIN_WINDOW_ID) const override ;
    virtual void window_set_min_size(const Size2i p_size, WindowID p_window = MAIN_WINDOW_ID) override;
    virtual Size2i window_get_min_size(WindowID p_window = MAIN_WINDOW_ID) const override ;
    virtual void window_set_title(const String &p_title, WindowID p_window = MAIN_WINDOW_ID) override ;
    virtual void window_set_transient(WindowID p_window, WindowID p_parent) override;
    virtual void window_set_mode(WindowMode p_mode, WindowID p_window = MAIN_WINDOW_ID) override ;
    virtual void process_events() override ;
    virtual Point2i screen_get_position(int p_screen) const override;
    virtual int get_screen_count() const override;
    virtual Size2i screen_get_size(int p_screen) const override;
    virtual int get_primary_screen() const override;
    virtual Rect2i screen_get_usable_rect(int p_screen) const  override;
    virtual int screen_get_dpi(int p_screen) const override;
    virtual void window_request_attention(DisplayServer::WindowID p_window) override;
virtual float screen_get_refresh_rate(int p_screen) const override;
virtual void window_set_window_event_callback(const Callable &p_callable, WindowID p_window = MAIN_WINDOW_ID) override;
	virtual void window_set_input_event_callback(const Callable &p_callable, WindowID p_window = MAIN_WINDOW_ID) override;
	virtual void window_set_input_text_callback(const Callable &p_callable, WindowID p_window = MAIN_WINDOW_ID) override;
	virtual void window_set_rect_changed_callback(const Callable &p_callable, WindowID p_window = MAIN_WINDOW_ID) override;
	virtual void window_set_drop_files_callback(const Callable &p_callable, WindowID p_window = MAIN_WINDOW_ID) override;

virtual ObjectID window_get_attached_instance_id(WindowID p_window = MAIN_WINDOW_ID) const override;
virtual DisplayServer::WindowID get_window_at_screen_position(const Point2i &p_position) const override;
virtual void window_attach_instance_id(ObjectID p_instance, DisplayServer::WindowID p_window) override;
    virtual void window_move_to_foreground(DisplayServer::WindowID p_window)override;

    virtual bool window_is_focused(WindowID p_window) const override;

    virtual bool window_can_draw(DisplayServer::WindowID p_window) const override;

    virtual bool can_any_window_draw() const override;
    virtual DisplayServer::WindowMode window_get_mode(DisplayServer::WindowID p_window) const  override;

virtual bool window_is_maximize_allowed(DisplayServer::WindowID p_window) const  override;

virtual void window_set_flag(DisplayServer::WindowFlags p_flag, bool p_enabled, DisplayServer::WindowID p_window)  override;
    // 空实现或占位实现
    virtual Vector<DisplayServer::WindowID> get_window_list() const override ;
    
    static Vector<String> get_rendering_drivers_func();
    static void register_display_driver();
};