extends CheckButton

func _ready():
	text = "Click target: off"

func _pressed():
	text = "Click target: %s" % ("on" if button_pressed else "off")
	print("button click: ", button_pressed)
