extends Node3D

var rng = RandomNumberGenerator.new()
# Called when the node enters the scene tree for the first time.
func _ready():
	
	var v = []
	
	rng.randomize()
	
	var src = "kernel void func(ulong n, global const int * data, global int * res){
	size_t i = get_global_id(0);
	res[i] = data[i] * data[i];
	}"
	
	for i in range(1 << 10):
		v.append(rng.randi_range(1,100))
		
	#print(v)
	
	var ocl = OCL.new()
	
	ocl.set(src)
	ocl.write(v)
	ocl.run()
	ocl.read(v)
	
	if ocl.get_errMsg() != "":
		print(ocl.get_errMsg())
	else:
		print("empty")
#
	
	print("log: ", ocl.get_log() )
	pass # Replace with function body.



# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass
