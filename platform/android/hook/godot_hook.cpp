

#include "java_godot_wrapper.h"
#include "godot_android_hook.h"
static GODOT_LOGGER_NOTIFY logInterface=nullptr;

GodotJavaWrapper::GodotJavaWrapper(void* funptr)
{
	logInterface=reinterpret_cast<GODOT_LOGGER_NOTIFY>(funptr);
}
GodotJavaWrapper::GodotJavaWrapper(JNIEnv *p_env, jobject p_activity, jobject p_godot_instance) {

}

GodotJavaWrapper::~GodotJavaWrapper() {
	
}

jobject GodotJavaWrapper::get_activity() {
    logInterface("%s call",__FUNCTION__);
	return nullptr;
}

GodotJavaViewWrapper *GodotJavaWrapper::get_godot_view() {
    logInterface("%s call",__FUNCTION__);
	return nullptr;
}

void GodotJavaWrapper::on_godot_setup_completed(JNIEnv *p_env) {

}

void GodotJavaWrapper::on_godot_main_loop_started(JNIEnv *p_env) {
	
}

void GodotJavaWrapper::on_godot_terminating(JNIEnv *p_env) {
	
}

void GodotJavaWrapper::restart(JNIEnv *p_env) {
	
}

bool GodotJavaWrapper::force_quit(JNIEnv *p_env, int p_instance_id) {
	
	return false;
}

void GodotJavaWrapper::set_keep_screen_on(bool p_enabled) {
	
}

void GodotJavaWrapper::alert(const String &p_message, const String &p_title) {
	logInterface("%s call %s %s",__FUNCTION__,p_title.utf8().get_data(),p_message.utf8().get_data());
}

bool GodotJavaWrapper::is_dark_mode_supported() {
	
	return false;

}

bool GodotJavaWrapper::is_dark_mode() {
	
		return false;
	
}

// Convert ARGB to RGBA.
static Color _argb_to_rgba(int p_color) {
	int alpha = (p_color >> 24) & 0xFF;
	int red = (p_color >> 16) & 0xFF;
	int green = (p_color >> 8) & 0xFF;
	int blue = p_color & 0xFF;
	return Color(red / 255.0f, green / 255.0f, blue / 255.0f, alpha / 255.0f);
}

Color GodotJavaWrapper::get_accent_color() {
	
	return Color(0, 0, 0, 0);
	
}

Color GodotJavaWrapper::get_base_color() {
	
	return Color(0, 0, 0, 0);
	
}

bool GodotJavaWrapper::has_get_clipboard() {
	return false;
}

String GodotJavaWrapper::get_clipboard() {
	String clipboard="clipboard";
	
	return clipboard;
}

String GodotJavaWrapper::get_input_fallback_mapping() {
	String input_fallback_mapping="input_fallback_mapping";
	
	return input_fallback_mapping;
}

bool GodotJavaWrapper::has_set_clipboard() {
	return false;
}

void GodotJavaWrapper::set_clipboard(const String &p_text) {
	
}

bool GodotJavaWrapper::has_has_clipboard() {
	return false;
}

bool GodotJavaWrapper::has_clipboard() {
	
		return false;
	
}

Error GodotJavaWrapper::show_dialog(const String &p_title, const String &p_description, const Vector<String> &p_buttons) {
	 
		return ERR_UNCONFIGURED;
	
}

Error GodotJavaWrapper::show_input_dialog(const String &p_title, const String &p_message, const String &p_existing_text) {
	
		return ERR_UNCONFIGURED;
	
}

Error GodotJavaWrapper::show_file_picker(const String &p_current_directory, const String &p_filename, int p_mode, const Vector<String> &p_filters) {
	
		return ERR_UNCONFIGURED;
	
}

bool GodotJavaWrapper::request_permission(const String &p_name) {
        logInterface("%s call %s",__FUNCTION__,p_name.utf8().get_data());
		return false;
	
}

bool GodotJavaWrapper::request_permissions() {
	    logInterface("%s call",__FUNCTION__);
		return false;
	
}

Vector<String> GodotJavaWrapper::get_granted_permissions() const {
	Vector<String> permissions_list;
	
	return permissions_list;
}

Vector<String> GodotJavaWrapper::get_gdextension_list_config_file() const {
	Vector<String> config_file_list;
	
	return config_file_list;
}

String GodotJavaWrapper::get_ca_certificates() const {
	String ca_certificates;
	
	return ca_certificates;
}

void GodotJavaWrapper::init_input_devices() {

}

void GodotJavaWrapper::vibrate(int p_duration_ms, float p_amplitude) {
	
}

int GodotJavaWrapper::create_new_godot_instance(const List<String> &args) {
	
		return 0;
	
}

void GodotJavaWrapper::begin_benchmark_measure(const String &p_context, const String &p_label) {
	
}

void GodotJavaWrapper::end_benchmark_measure(const String &p_context, const String &p_label) {
	
}

void GodotJavaWrapper::dump_benchmark(const String &benchmark_file) {
	
}

bool GodotJavaWrapper::has_feature(const String &p_feature) const {
	WARN_PRINT(vformat("has_feature %s",p_feature));
    if (p_feature=="linuxbsd")
        return true;
    if (p_feature=="debug")
        return true;
	if (p_feature=="s3tc")
        return true;
	if (p_feature=="etc2")
        return true;
	return false;
	
}

Error GodotJavaWrapper::sign_apk(const String &p_input_path, const String &p_output_path, const String &p_keystore_path, const String &p_keystore_user, const String &p_keystore_password) {
	
		return ERR_UNCONFIGURED;
	
}

Error GodotJavaWrapper::verify_apk(const String &p_apk_path) {
	
		return ERR_UNCONFIGURED;
	
}

void GodotJavaWrapper::enable_immersive_mode(bool p_enabled) {

}

bool GodotJavaWrapper::is_in_immersive_mode() {
	
		return false;
	
}

void GodotJavaWrapper::on_editor_workspace_selected(const String &p_workspace) {
	
}
