extends Control

@onready var mouse_label: Label = $MouseLabel
@onready var button_label: Label = $ButtonLabel
@onready var wheel_label: Label = $WheelLabel
@onready var key_label: Label = $KeyLabel
@onready var text_label: Label = $TextLabel
@onready var target_label: Label = $TargetLabel
@onready var mouse_marker: ColorRect = $MouseMarker
@onready var click_marker: ColorRect = $ClickMarker
@onready var check_button: CheckButton = $CheckButton
@onready var line_edit: LineEdit = $LineEdit

var mouse_pos := Vector2.ZERO
var last_click_pos := Vector2.ZERO
var pressed_buttons := {}
var motion_count := 0
var click_count := 0
var wheel_x := 0.0
var wheel_y := 0.0
var key_count := 0
var text_buffer := ""
var last_key := "none"

func _ready() -> void:
	line_edit.grab_focus()
	_refresh_labels()

func _process(_delta: float) -> void:
	target_label.text = "CheckButton state: %s" % ("on" if check_button.button_pressed else "off")

func _input(event: InputEvent) -> void:
	if event is InputEventMouseMotion:
		mouse_pos = event.position
		motion_count += 1
		mouse_marker.position = mouse_pos - mouse_marker.size * 0.5
		_refresh_labels()
	elif event is InputEventMouseButton:
		mouse_pos = event.position
		if _is_wheel_button(event.button_index):
			if event.pressed:
				_update_wheel(event.button_index)
		else:
			if event.pressed:
				pressed_buttons[event.button_index] = true
				last_click_pos = event.position
				click_count += 1
				click_marker.position = last_click_pos - click_marker.size * 0.5
			else:
				pressed_buttons.erase(event.button_index)
		_refresh_labels()
	elif event is InputEventKey:
		_update_key(event)
		_refresh_labels()

func _update_key(event: InputEventKey) -> void:
	var name := OS.get_keycode_string(event.keycode)
	if name == "" and event.physical_keycode != 0:
		name = OS.get_keycode_string(event.physical_keycode)
	if name == "" and event.unicode > 0:
		name = "unicode %d" % event.unicode
	if name == "":
		name = "keycode %d" % event.keycode

	last_key = "%s %s%s" % [
		name,
		"down" if event.pressed else "up",
		" repeat" if event.echo else ""
	]

	if event.pressed and not event.echo:
		key_count += 1

	if event.pressed and event.unicode >= 32:
		var ch := char(event.unicode)
		text_buffer += ch
		if text_buffer.length() > 48:
			text_buffer = text_buffer.substr(text_buffer.length() - 48)

func _update_wheel(button_index: int) -> void:
	match button_index:
		MOUSE_BUTTON_WHEEL_UP:
			wheel_y += 1.0
		MOUSE_BUTTON_WHEEL_DOWN:
			wheel_y -= 1.0
		MOUSE_BUTTON_WHEEL_RIGHT:
			wheel_x += 1.0
		MOUSE_BUTTON_WHEEL_LEFT:
			wheel_x -= 1.0

func _is_wheel_button(button_index: int) -> bool:
	return button_index == MOUSE_BUTTON_WHEEL_UP \
		or button_index == MOUSE_BUTTON_WHEEL_DOWN \
		or button_index == MOUSE_BUTTON_WHEEL_LEFT \
		or button_index == MOUSE_BUTTON_WHEEL_RIGHT

func _button_names() -> String:
	var result := ""
	for button in pressed_buttons.keys():
		if result != "":
			result += ", "
		result += _button_name(button)
	return result if result != "" else "none"

func _button_name(button_index: int) -> String:
	match button_index:
		MOUSE_BUTTON_LEFT:
			return "left"
		MOUSE_BUTTON_RIGHT:
			return "right"
		MOUSE_BUTTON_MIDDLE:
			return "middle"
		MOUSE_BUTTON_XBUTTON1:
			return "x1"
		MOUSE_BUTTON_XBUTTON2:
			return "x2"
		_:
			return str(button_index)

func _refresh_labels() -> void:
	mouse_label.text = "Mouse: (%.1f, %.1f)  motion events: %d" % [mouse_pos.x, mouse_pos.y, motion_count]
	button_label.text = "Buttons: %s  clicks: %d  last click: (%.1f, %.1f)" % [_button_names(), click_count, last_click_pos.x, last_click_pos.y]
	wheel_label.text = "Wheel: x %.0f  y %.0f" % [wheel_x, wheel_y]
	key_label.text = "Key: %s  key events: %d" % [last_key, key_count]
	text_label.text = "Text input: %s" % (text_buffer if text_buffer != "" else "none")
