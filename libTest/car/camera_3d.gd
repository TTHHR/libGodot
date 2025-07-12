extends Camera3D

@export var orbit_speed: float = 30.0 # 每秒旋转角度
@export var look_at_point: Vector3 = Vector3.ZERO  # 观察目标

var initial_offset: Vector3
var angle_deg := 0.0

func _ready() -> void:
	# 相机初始位置相对于观察点的偏移
	initial_offset = global_position - look_at_point
	print("Initial offset: ", initial_offset)

func _process(delta: float) -> void:
	angle_deg += orbit_speed * delta
	var angle_rad = deg_to_rad(angle_deg)

	# 仅绕 Y 轴旋转（绕观察目标旋转 offset）
	var rotated_offset = initial_offset.rotated(Vector3.UP, angle_rad)
	global_position = look_at_point + rotated_offset

	look_at(look_at_point, Vector3.UP)
