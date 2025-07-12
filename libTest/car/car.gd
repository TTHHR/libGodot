extends Node3D

# 使用铰链位置字典（转换为Vector3）
var hinge_positions = {
	"front_left": Vector3(-8.88495, 10.87088, -9.24071),
	"front_right": Vector3(8.88495, 10.87088, -9.24071),
	# 可以添加后门位置
}
@export var door_state_fl: bool = false #门开关状态
@export var door_state_fr: bool = false #门开关状态

@onready var door_fl = $door_fl
@onready var door_fr = $door_fr

var is_door_fl_open = false
var is_door_fr_open = false
var changeDelay = 0

# 存储原始位置和旋转
var original_positions = {}
var original_rotations = {}

func _ready():
	# 存储初始状态
	original_positions["front_left"] = door_fl.position
	original_positions["front_right"] = door_fr.position
	original_rotations["front_left"] = door_fl.rotation_degrees
	original_rotations["front_right"] = door_fr.rotation_degrees

func _process(delta):
	changeDelay += 1
	if changeDelay > 100:
		changeDelay = 0
		
		if door_state_fl!=is_door_fl_open :
			is_door_fl_open=door_state_fl
			if is_door_fl_open:
				open_door("front_left", -70)  # 左门开角度为负
			else:
				close_door("front_left")
		if door_state_fr!=is_door_fr_open :
			is_door_fr_open=door_state_fr
			if is_door_fr_open:
				open_door("front_right", 70)  # 右门开角度为正
			else:
				close_door("front_right")

# 使用指定铰链点开门
func open_door(door_type, angle):
	var door = door_fl if door_type == "front_left" else door_fr
	var hinge_position = hinge_positions[door_type]
	
	# 创建旋转矩阵
	var transform = Transform3D()
	transform = transform.rotated(Vector3(0, 1, 0), deg_to_rad(angle))
	
	# 计算相对于铰链的偏移量
	var local_offset = door.global_position - hinge_position
	var rotated_offset = transform.basis * local_offset
	
	# 创建补间动画
	var tween = get_tree().create_tween()
	tween.set_parallel()
	
	# 动画位置和旋转
	tween.tween_property(door, "global_position", hinge_position + rotated_offset, 0.5)
	tween.tween_property(door, "rotation_degrees:y", 
						 original_rotations[door_type].y + angle, 0.5)

# 使用原始状态关门
func close_door(door_type):
	var door = door_fl if door_type == "front_left" else door_fr
	
	# 创建补间动画
	var tween = get_tree().create_tween()
	tween.set_parallel()
	
	# 动画恢复到原始位置和旋转
	tween.tween_property(door, "position", original_positions[door_type], 0.5)
	tween.tween_property(door, "rotation_degrees", 
						 original_rotations[door_type], 0.5)
