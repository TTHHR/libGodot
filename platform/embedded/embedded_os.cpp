// embedded_os.cpp
#include "embedded_os.h"
#include "main/main.h"
#include "displayserver_embedded.h"

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
Size2i EmbeddedOS::get_display_size()
{
    // 返回默认显示大小或自定义大小
    return Size2i(800, 600); // 示例值
}

bool EmbeddedOS::_check_internal_feature_support(const String &p_feature) {
    // 返回 false 或自定义支持
    return false;
}

EmbeddedOS::EmbeddedOS() {
    main_loop = nullptr;
}
EmbeddedOS::~EmbeddedOS() {
    delete_main_loop();
}