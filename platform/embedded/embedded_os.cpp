// embedded_os.cpp
#include "embedded_os.h"

#include "displayserver_embedded.h"

#include "main/main.h"
#include "scene/main/scene_tree.h"
#include "scene/main/window.h"
#include "scene/property_list_helper.h"
#include "scene/register_scene_types.h"
#include "scene/resources/packed_scene.h"
#include "scene/theme/theme_db.h"

void EmbeddedOS::initialize() {
	DisplayServerEmbedded::register_display_driver();
	OS_Unix::initialize_core();
}
EmbeddedOS *EmbeddedOS::get_singleton() {
	return static_cast<EmbeddedOS *>(OS::get_singleton());
}
MainLoop *EmbeddedOS::get_main_loop() const {
	return main_loop;
}
void EmbeddedOS::delete_main_loop() {
	if (main_loop) {
		memdelete(main_loop);
	}
	main_loop = nullptr;
}

void EmbeddedOS::set_main_loop(MainLoop *p_main_loop) {
	main_loop = p_main_loop;
}
void EmbeddedOS::initialize_joypads() {
	// 如果没有手柄功能，可留空
}

void EmbeddedOS::finalize() {
	delete_main_loop();
}
Size2i EmbeddedOS::get_display_size() const {
	return display_size;
}

void EmbeddedOS::set_display_size(const Size2i &p_size) {
	if (p_size.width <= 0 || p_size.height <= 0) {
		return;
	}
	display_size = p_size;
}

bool EmbeddedOS::_check_internal_feature_support(const String &p_feature) {
	// 返回 false 或自定义支持
	return false;
}

void EmbeddedOS::push_input_event(const EmbeddedInputEvent &p_event) {
	std::lock_guard<std::mutex> lock(input_mutex);
	input_events.push_back(p_event);
}

bool EmbeddedOS::pop_input_event(EmbeddedInputEvent &r_event) {
	std::lock_guard<std::mutex> lock(input_mutex);
	if (input_events.empty()) {
		return false;
	}
	r_event = input_events.front();
	input_events.pop_front();
	return true;
}

void EmbeddedOS::setGDValue(GD_TYPE p_name, char *paramName, void *value) {
	Variant val;
	switch (p_name) {
		case TYPE_FLOAT:
			val = *static_cast<float *>(value);
			break;
		case TYPE_BOOL:
			val = *static_cast<bool *>(value);
			break;
		//todo add more
		default:
			log("not support type %d", static_cast<int>(p_name));
			return;
	}
	SceneTree *scene_tree = SceneTree::get_singleton();
	int cc = scene_tree->get_current_scene()->get_child_count();
	for (size_t i = 0; i < cc; i++) {
		auto child = scene_tree->get_current_scene()->get_child(i);
		if (child != nullptr && child->get_script_instance() != nullptr) {
			child->get_script_instance()->set(paramName, val);
		}
	}
}
void EmbeddedOS::getGDValue(GD_TYPE p_name, char *paramName, void *value) {
	SceneTree *scene_tree = SceneTree::get_singleton();
	int cc = scene_tree->get_current_scene()->get_child_count();
	for (size_t i = 0; i < cc; i++) {
		auto child = scene_tree->get_current_scene()->get_child(i);
		if (child != nullptr && child->get_script_instance() != nullptr) {
			Variant val;
			bool ret = child->get_script_instance()->get(paramName, val);
			if (ret) {
				switch (p_name) {
					case TYPE_FLOAT:
						if (val.get_type() == Variant::FLOAT) {
							*static_cast<float *>(value) = val.operator float();
						}
						break;
					//todo add more
					default:
						break;
				}
			}
		}
	}
}

EmbeddedOS::EmbeddedOS() {
	main_loop = nullptr;
}
EmbeddedOS::~EmbeddedOS() {
	delete_main_loop();
}
