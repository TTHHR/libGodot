
#include "displayserver_embedded.h"
#include "core/error/error_macros.h"
#include "core/config/project_settings.h"
#include "core/input/input.h"
#include "core/input/input_event.h"
#include "core/io/image.h"
#include "embedded_os.h"
#include "drivers/gles3/rasterizer_gles3.h"
#include <EGL/egl.h>  // 或者使用宿主提供的GL接口

template <typename T>
static void embedded_set_modifiers(const Ref<T> &p_event, uint32_t p_modifiers) {
    p_event->set_shift_pressed((p_modifiers & GODOT_LIB_MOD_SHIFT) != 0);
    p_event->set_alt_pressed((p_modifiers & GODOT_LIB_MOD_ALT) != 0);
    p_event->set_ctrl_pressed((p_modifiers & GODOT_LIB_MOD_CTRL) != 0);
    p_event->set_meta_pressed((p_modifiers & GODOT_LIB_MOD_META) != 0);
}

static MouseButton embedded_mouse_button_from_int(int p_button) {
    switch (p_button) {
        case (int)MouseButton::LEFT:
            return MouseButton::LEFT;
        case (int)MouseButton::RIGHT:
            return MouseButton::RIGHT;
        case (int)MouseButton::MIDDLE:
            return MouseButton::MIDDLE;
        case (int)MouseButton::MB_XBUTTON1:
            return MouseButton::MB_XBUTTON1;
        case (int)MouseButton::MB_XBUTTON2:
            return MouseButton::MB_XBUTTON2;
        default:
            return MouseButton::NONE;
    }
}

static void embedded_dispatch_input_event(const Ref<InputEvent> &p_event) {
    if (Input::get_singleton()) {
        Input::get_singleton()->parse_input_event(p_event);
    }
}

void DisplayServerEmbedded::_dispatch_input_events(const Ref<InputEvent> &p_event) {
    DisplayServerEmbedded *ds = static_cast<DisplayServerEmbedded *>(get_singleton());
    if (ds) {
        ds->_dispatch_input_event(p_event);
    }
}

void DisplayServerEmbedded::_dispatch_input_event(const Ref<InputEvent> &p_event) {
    if (input_event_callback.is_valid()) {
        input_event_callback.call(p_event);
    }
}

DisplayServer *DisplayServerEmbedded::create_func(const String &p_rendering_driver, WindowMode p_mode, DisplayServer::VSyncMode p_vsync_mode, uint32_t p_flags, const Vector2i *p_position, const Vector2i &p_resolution, int p_screen, Context p_context, int64_t p_parent_window, Error &r_error) {
    return memnew(DisplayServerEmbedded(p_rendering_driver, p_mode, p_flags, p_resolution, r_error));
}

DisplayServerEmbedded::DisplayServerEmbedded(const String &p_rendering_driver, WindowMode p_mode, 
                                             uint32_t p_flags, const Size2i &p_resolution, Error &r_error) {
    // 设置主窗口参数
    main_window.size = EmbeddedOS::get_singleton()->get_display_size();
	EmbeddedOS::get_singleton()->log("DisplayServerEmbedded: Initializing with driver %s" , p_rendering_driver.utf8().get_data());
	OS::get_singleton()->set_current_rendering_method("gl_compatibility");
	OS::get_singleton()->set_current_rendering_driver_name(p_rendering_driver);
    Input::get_singleton()->set_event_dispatch_function(_dispatch_input_events);
    RasterizerGLES3::make_current(false);
	 // 初始化鼠标状态
    mouse_position = Point2i(0, 0);
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
    return p_feature == FEATURE_SWAP_BUFFERS||p_feature == FEATURE_MOUSE;
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
    EmbeddedOS *os = EmbeddedOS::get_singleton();
    EmbeddedOS::EmbeddedInputEvent input_event;

    while (os->pop_input_event(input_event)) {
        switch (input_event.type) {
            case EmbeddedOS::EmbeddedInputEvent::MOUSE_MOVE: {
                Point2i new_position((int)input_event.x, (int)input_event.y);
                if (mouse_position == new_position) {
                    break;
                }

                Ref<InputEventMouseMotion> motion_event;
                motion_event.instantiate();
                motion_event->set_window_id(MAIN_WINDOW_ID);
                motion_event->set_position(new_position);
                motion_event->set_global_position(new_position);
                motion_event->set_relative(new_position - mouse_position);
                motion_event->set_relative_screen_position(new_position - mouse_position);
                motion_event->set_button_mask(mouse_button_state);
                embedded_set_modifiers(motion_event, input_event.modifiers);

                embedded_dispatch_input_event(motion_event);

                mouse_position = new_position;
            } break;
            case EmbeddedOS::EmbeddedInputEvent::MOUSE_BUTTON: {
                MouseButton button = embedded_mouse_button_from_int(input_event.button);
                if (button == MouseButton::NONE) {
                    break;
                }

                Point2i new_position((int)input_event.x, (int)input_event.y);
                mouse_position = new_position;

                MouseButtonMask button_mask = mouse_button_to_mask(button);
                if (input_event.pressed) {
                    mouse_button_state.set_flag(button_mask);
                } else {
                    mouse_button_state.clear_flag(button_mask);
                }

                Ref<InputEventMouseButton> button_event;
                button_event.instantiate();
                button_event->set_window_id(MAIN_WINDOW_ID);
                button_event->set_position(new_position);
                button_event->set_global_position(new_position);
                button_event->set_button_mask(mouse_button_state);
                button_event->set_button_index(button);
                button_event->set_pressed(input_event.pressed);
                embedded_set_modifiers(button_event, input_event.modifiers);

                embedded_dispatch_input_event(button_event);
            } break;
            case EmbeddedOS::EmbeddedInputEvent::MOUSE_WHEEL: {
                Point2i new_position((int)input_event.x, (int)input_event.y);
                mouse_position = new_position;

                const float wheel_values[4] = {
                    input_event.wheel_y,
                    -input_event.wheel_y,
                    input_event.wheel_x,
                    -input_event.wheel_x,
                };
                const MouseButton wheel_buttons[4] = {
                    MouseButton::WHEEL_UP,
                    MouseButton::WHEEL_DOWN,
                    MouseButton::WHEEL_RIGHT,
                    MouseButton::WHEEL_LEFT,
                };

                for (int i = 0; i < 4; i++) {
                    if (wheel_values[i] <= 0) {
                        continue;
                    }

                    BitField<MouseButtonMask> wheel_mask = mouse_button_state;
                    wheel_mask.set_flag(mouse_button_to_mask(wheel_buttons[i]));

                    Ref<InputEventMouseButton> button_event;
                    button_event.instantiate();
                    button_event->set_window_id(MAIN_WINDOW_ID);
                    button_event->set_position(new_position);
                    button_event->set_global_position(new_position);
                    button_event->set_button_mask(wheel_mask);
                    button_event->set_button_index(wheel_buttons[i]);
                    button_event->set_pressed(true);
                    button_event->set_factor(wheel_values[i]);
                    embedded_set_modifiers(button_event, input_event.modifiers);

                    embedded_dispatch_input_event(button_event);

                    Ref<InputEventMouseButton> release_event;
                    release_event.instantiate();
                    release_event->set_window_id(MAIN_WINDOW_ID);
                    release_event->set_position(new_position);
                    release_event->set_global_position(new_position);
                    release_event->set_button_index(wheel_buttons[i]);
                    release_event->set_button_mask(mouse_button_state);
                    release_event->set_pressed(false);
                    release_event->set_factor(wheel_values[i]);
                    embedded_set_modifiers(release_event, input_event.modifiers);

                    embedded_dispatch_input_event(release_event);
                }
            } break;
            case EmbeddedOS::EmbeddedInputEvent::KEY: {
                Ref<InputEventKey> key_event;
                key_event.instantiate();
                key_event->set_window_id(MAIN_WINDOW_ID);
                key_event->set_pressed(input_event.pressed);
                key_event->set_echo(input_event.echo);
                key_event->set_keycode((Key)input_event.keycode);
                key_event->set_physical_keycode((Key)input_event.physical_keycode);
                key_event->set_key_label((Key)input_event.keycode);
                embedded_set_modifiers(key_event, input_event.modifiers);

                embedded_dispatch_input_event(key_event);
            } break;
            case EmbeddedOS::EmbeddedInputEvent::TEXT: {
                Ref<InputEventKey> key_event;
                key_event.instantiate();
                key_event->set_window_id(MAIN_WINDOW_ID);
                key_event->set_pressed(true);
                key_event->set_echo(false);
                key_event->set_keycode(Key::NONE);
                key_event->set_physical_keycode(Key::NONE);
                key_event->set_key_label(Key::NONE);
                key_event->set_unicode((char32_t)input_event.unicode);

                embedded_dispatch_input_event(key_event);
            } break;
        }
    }

    if (Input::get_singleton()) {
        Input::get_singleton()->flush_buffered_events();
    }
}
void DisplayServerEmbedded::window_set_title(const String &p_title, DisplayServer::WindowID p_window) {
	// Not supported on Android.
}
void DisplayServerEmbedded::swap_buffers() {
    EmbeddedOS::get_singleton()->should_swap_buffers=true;
}
Size2i DisplayServerEmbedded::window_get_size(WindowID p_window) const {
    ERR_FAIL_COND_V(p_window != MAIN_WINDOW_ID, Size2i());
    return main_window.size;
}
bool DisplayServerEmbedded::window_get_flag(DisplayServer::WindowFlags p_flag, DisplayServer::WindowID p_window) const {

	return false;
	
}
DisplayServer::VSyncMode DisplayServerEmbedded::window_get_vsync_mode(WindowID p_window) const {
	return DisplayServer::VSYNC_ENABLED;
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
// 实现鼠标相关函数
Point2i DisplayServerEmbedded::mouse_get_position() const {
    return mouse_position;
}

BitField<MouseButtonMask>  DisplayServerEmbedded::mouse_get_button_state() const {
   
   return mouse_button_state;
}
Vector<String> DisplayServerEmbedded::get_rendering_drivers_func() {
	Vector<String> drivers;
	drivers.push_back("opengl3_es");
	drivers.push_back("dummy");
	return drivers;
}
void DisplayServerEmbedded::register_display_driver() {
	register_create_function("x11", create_func, get_rendering_drivers_func);
}
