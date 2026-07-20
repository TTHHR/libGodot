// embedded_os.h
#pragma once
#include "embedded_export.h"

#include "core/config/project_settings.h"
#include "core/os/os.h"
#include "drivers/unix/os_unix.h"

#include <stdint.h>

#include <cstdarg>
#include <cstdio>
#include <deque>
#include <mutex>
#include <string>

class EmbeddedOS : public OS_Unix {
	virtual void delete_main_loop() override;

public:
	struct EmbeddedInputEvent {
		enum Type {
			MOUSE_MOVE,
			MOUSE_BUTTON,
			MOUSE_WHEEL,
			KEY,
			TEXT,
		};

		Type type = MOUSE_MOVE;
		float x = 0;
		float y = 0;
		float wheel_x = 0;
		float wheel_y = 0;
		int button = 0;
		uint32_t keycode = 0;
		uint32_t physical_keycode = 0;
		uint32_t unicode = 0;
		uint32_t modifiers = 0;
		bool pressed = false;
		bool echo = false;
	};

private:
	MainLoop *main_loop = nullptr;
	Size2i display_size = Size2i(800, 600);
	std::mutex input_mutex;
	std::deque<EmbeddedInputEvent> input_events;

public:
	EmbeddedOS();
	~EmbeddedOS();
	static EmbeddedOS *get_singleton();
	virtual void initialize() override;

	virtual void initialize_joypads() override;
	virtual void finalize() override;
	virtual bool _check_internal_feature_support(const String &p_feature) override;

	void set_logger(GODOT_LOGGER_NOTIFY func) { log_callback = func; }
	void log(const char *fmt, ...) {
		va_list args;
		va_start(args, fmt);
		int len = vsnprintf(nullptr, 0, fmt, args);
		va_end(args);

		std::string buf(len + 1, '\0');
		va_start(args, fmt);
		vsnprintf(&buf[0], buf.size(), fmt, args);
		va_end(args);

		if (log_callback) {
			log_callback(buf.c_str());
		}
	}

	virtual MainLoop *get_main_loop() const override;
	Size2i get_display_size() const;
	void set_display_size(const Size2i &p_size);
	bool should_swap_buffers;
	void push_input_event(const EmbeddedInputEvent &p_event);
	bool pop_input_event(EmbeddedInputEvent &r_event);
	void setGDValue(GD_TYPE p_name, char *paramName, void *value);
	void getGDValue(GD_TYPE p_name, char *paramName, void *value);

protected:
	GODOT_LOGGER_NOTIFY log_callback = nullptr;
	virtual void set_main_loop(MainLoop *p_main_loop) override;
};
