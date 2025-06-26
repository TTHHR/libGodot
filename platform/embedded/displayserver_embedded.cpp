
#include "displayserver_embedded.h"
#include "core/error/error_macros.h"
#include "core/config/project_settings.h"
#include "core/io/image.h"
#include "embedded_os.h"
#include "drivers/gles3/rasterizer_gles3.h"
#include <EGL/egl.h>  // 或者使用宿主提供的GL接口

DisplayServer *DisplayServerEmbedded::create_func(const String &p_rendering_driver, WindowMode p_mode, DisplayServer::VSyncMode p_vsync_mode, uint32_t p_flags, const Vector2i *p_position, const Vector2i &p_resolution, int p_screen, Context p_context, int64_t p_parent_window, Error &r_error) {
    return memnew(DisplayServerEmbedded(p_rendering_driver, p_mode, p_flags, p_resolution, r_error));
}

DisplayServerEmbedded::DisplayServerEmbedded(const String &p_rendering_driver, WindowMode p_mode, 
                                             uint32_t p_flags, const Size2i &p_resolution, Error &r_error) {
    // 设置主窗口参数
    main_window.size = EmbeddedOS::get_singleton()->get_display_size();
	EmbeddedOS::get_singleton()->log("DisplayServerEmbedded: Initializing with driver %s" , p_rendering_driver.utf8().get_data());
    RasterizerGLES3::make_current(true);
    // 初始化成功
    r_error = OK;
}

DisplayServerEmbedded::~DisplayServerEmbedded() {
    // 清理资源
}

Vector<DisplayServer::WindowID> DisplayServerEmbedded::get_window_list() const {
	Vector<WindowID> ret;
	ret.push_back(MAIN_WINDOW_ID);
	return ret;
}
bool DisplayServerEmbedded::has_feature(Feature p_feature) const {
    return p_feature == FEATURE_SWAP_BUFFERS;
}

String DisplayServerEmbedded::get_name() const {
    return "embedded";
}
void DisplayServerEmbedded::window_set_mode(DisplayServer::WindowMode p_mode, DisplayServer::WindowID p_window) {

}
int DisplayServerEmbedded::window_get_current_screen(DisplayServer::WindowID p_window) const {

	return 0;
}
void DisplayServerEmbedded::process_events() {

}
void DisplayServerEmbedded::window_set_title(const String &p_title, DisplayServer::WindowID p_window) {
	// Not supported on Android.
}
void DisplayServerEmbedded::swap_buffers() {
	EmbeddedOS::get_singleton()->log("swap buff");
    EmbeddedOS::get_singleton()->should_swap_buffers=true;
}
Size2i DisplayServerEmbedded::window_get_size(WindowID p_window) const {
    ERR_FAIL_COND_V(p_window != MAIN_WINDOW_ID, Size2i());
    return main_window.size;
}
bool DisplayServerEmbedded::window_get_flag(DisplayServer::WindowFlags p_flag, DisplayServer::WindowID p_window) const {

	return false;
	
}
void DisplayServerEmbedded::window_set_current_screen(int p_screen, DisplayServer::WindowID p_window) {
	// Not supported on Android.
}

Point2i DisplayServerEmbedded::window_get_position(DisplayServer::WindowID p_window) const {
	return Point2i();
}

Point2i DisplayServerEmbedded::window_get_position_with_decorations(DisplayServer::WindowID p_window) const {
	return Point2i();
}
Size2i DisplayServerEmbedded::window_get_size_with_decorations(DisplayServer::WindowID p_window) const {
	return EmbeddedOS::get_singleton()->get_display_size();
}
void DisplayServerEmbedded::window_set_position(const Point2i &p_position, DisplayServer::WindowID p_window) {
	// Not supported on Android.
}

void DisplayServerEmbedded::window_set_transient(DisplayServer::WindowID p_window, DisplayServer::WindowID p_parent) {
	// Not supported on Android.
}

void DisplayServerEmbedded::window_set_max_size(const Size2i p_size, DisplayServer::WindowID p_window) {
	// Not supported on Android.
}

Size2i DisplayServerEmbedded::window_get_max_size(DisplayServer::WindowID p_window) const {
	return Size2i();
}

void DisplayServerEmbedded::window_set_min_size(const Size2i p_size, DisplayServer::WindowID p_window) {
	// Not supported on Android.
}

Size2i DisplayServerEmbedded::window_get_min_size(DisplayServer::WindowID p_window) const {
	return Size2i();
}

void DisplayServerEmbedded::window_set_size(const Size2i p_size, DisplayServer::WindowID p_window) {
	// Not supported on Android.
}
int DisplayServerEmbedded::get_screen_count() const {
	return 1;
}

int DisplayServerEmbedded::get_primary_screen() const {
	return 0;
}
Point2i DisplayServerEmbedded::screen_get_position(int p_screen) const {
	return Point2i(0, 0);
}
Size2i DisplayServerEmbedded::screen_get_size(int p_screen) const {
	return Size2i();
}
Rect2i DisplayServerEmbedded::screen_get_usable_rect(int p_screen) const {

	Size2i display_size = EmbeddedOS::get_singleton()->get_display_size();
	return Rect2i(0, 0, display_size.width, display_size.height);
}
int DisplayServerEmbedded::screen_get_dpi(int p_screen) const {
	
	return 160;
}
void DisplayServerEmbedded::window_request_attention(DisplayServer::WindowID p_window) {
	// Not supported on Android.
}

void DisplayServerEmbedded::window_move_to_foreground(DisplayServer::WindowID p_window) {
	// Not supported on Android.
}

bool DisplayServerEmbedded::window_is_focused(WindowID p_window) const {
	return true;
}

bool DisplayServerEmbedded::window_can_draw(DisplayServer::WindowID p_window) const {
	return true;
}
float DisplayServerEmbedded::screen_get_refresh_rate(int p_screen) const {
	
	return 60.0f; // 假设刷新率为60Hz
}
void DisplayServerEmbedded::window_set_rect_changed_callback(const Callable &p_callable, DisplayServer::WindowID p_window) {
	rect_changed_callback = p_callable;
}
void DisplayServerEmbedded::window_set_window_event_callback(const Callable &p_callable, DisplayServer::WindowID p_window) {
	window_event_callback = p_callable;
}

void DisplayServerEmbedded::window_set_input_event_callback(const Callable &p_callable, DisplayServer::WindowID p_window) {
	input_event_callback = p_callable;
}

void DisplayServerEmbedded::window_set_input_text_callback(const Callable &p_callable, DisplayServer::WindowID p_window) {
	input_text_callback = p_callable;
}


void DisplayServerEmbedded::window_set_drop_files_callback(const Callable &p_callable, DisplayServer::WindowID p_window) {
	// Not supported on Android.
}

DisplayServer::WindowID DisplayServerEmbedded::get_window_at_screen_position(const Point2i &p_position) const {
	return MAIN_WINDOW_ID;
}
void DisplayServerEmbedded::window_attach_instance_id(ObjectID p_instance, DisplayServer::WindowID p_window) {
	window_attached_instance_id = p_instance;
}
ObjectID DisplayServerEmbedded::window_get_attached_instance_id(DisplayServer::WindowID p_window) const {
	return window_attached_instance_id;
}
bool DisplayServerEmbedded::can_any_window_draw() const {
	return true;
}
DisplayServer::WindowMode DisplayServerEmbedded::window_get_mode(DisplayServer::WindowID p_window) const {
		return WINDOW_MODE_FULLSCREEN;
}

bool DisplayServerEmbedded::window_is_maximize_allowed(DisplayServer::WindowID p_window) const {
	return false;
}

void DisplayServerEmbedded::window_set_flag(DisplayServer::WindowFlags p_flag, bool p_enabled, DisplayServer::WindowID p_window) {
	// Not supported on Android.
}
Vector<String> DisplayServerEmbedded::get_rendering_drivers_func() {
	Vector<String> drivers;
	drivers.push_back("opengl3");
	drivers.push_back("opengl3_es");
	drivers.push_back("dummy");

	return drivers;
}
void DisplayServerEmbedded::register_display_driver() {
	register_create_function("x11", create_func, get_rendering_drivers_func);
}