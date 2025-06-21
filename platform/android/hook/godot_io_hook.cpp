

#include "java_godot_io_wrapper.h"

#include "core/math/rect2.h"
#include "core/variant/variant.h"
#include "godot_android_hook.h"
static GODOT_LOGGER_NOTIFY logInterface=nullptr;
GodotIOJavaWrapper::GodotIOJavaWrapper(void* funptr)
{
	logInterface=reinterpret_cast<GODOT_LOGGER_NOTIFY>(funptr);
}
GodotIOJavaWrapper::GodotIOJavaWrapper(JNIEnv *p_env, jobject p_godot_io_instance) {

}
void GodotIOJavaWrapper::setBaseDir(String base)
{
	baseDir=base;
}
GodotIOJavaWrapper::~GodotIOJavaWrapper() {
	
}

jobject GodotIOJavaWrapper::get_instance() {
    logInterface("%s call",__FUNCTION__);
	return nullptr;
}

Error GodotIOJavaWrapper::open_uri(const String &p_uri) {
	logInterface("%s call",__FUNCTION__);
		return ERR_UNAVAILABLE;
	
}

String GodotIOJavaWrapper::get_cache_dir() {
        logInterface("%s call",__FUNCTION__);
		return String("./cache");
	
}

String GodotIOJavaWrapper::get_temp_dir() {
	logInterface("%s call",__FUNCTION__);
	return String("./temp");
}

String GodotIOJavaWrapper::get_user_data_dir(const String &p_user_dir) {
	logInterface("%s call",__FUNCTION__);
		return String(baseDir+"/user");
	
}

String GodotIOJavaWrapper::get_locale() {
	logInterface("%s call",__FUNCTION__);
		return String(baseDir+"/local");
	
}

String GodotIOJavaWrapper::get_model() {
	logInterface("%s call",__FUNCTION__);
	return String("./model");
}

int GodotIOJavaWrapper::get_screen_dpi() {

		return 160;
	
}

float GodotIOJavaWrapper::get_scaled_density() {

		return 1.0f;
	
}

float GodotIOJavaWrapper::get_screen_refresh_rate(float fallback) {
	return fallback;
}

TypedArray<Rect2> GodotIOJavaWrapper::get_display_cutouts() {
	TypedArray<Rect2> result;
	
	return result;
}

Rect2i GodotIOJavaWrapper::get_display_safe_area() {
	Rect2i result;
	
	return result;
}

String GodotIOJavaWrapper::get_unique_id() {
	logInterface("%s call",__FUNCTION__);
		return String("unique_id");
	
}

bool GodotIOJavaWrapper::has_vk() {
	return false;
}

bool GodotIOJavaWrapper::has_hardware_keyboard() {

		return false;
	
}

void GodotIOJavaWrapper::show_vk(const String &p_existing, int p_type, int p_max_input_length, int p_cursor_start, int p_cursor_end) {

}

void GodotIOJavaWrapper::hide_vk() {
	
}

void GodotIOJavaWrapper::set_screen_orientation(int p_orient) {
	
}

int GodotIOJavaWrapper::get_screen_orientation() {
	
		return 0;
	
}

String GodotIOJavaWrapper::get_system_dir(int p_dir, bool p_shared_storage) {
	logInterface("%s call",__FUNCTION__);
		return String("./system");
	
}

// SafeNumeric because it can be changed from non-main thread and we need to
// ensure the change is immediately visible to other threads.
static SafeNumeric<int> virtual_keyboard_height;

int GodotIOJavaWrapper::get_vk_height() {
	return 100;
}

void GodotIOJavaWrapper::set_vk_height(int p_height) {

}
