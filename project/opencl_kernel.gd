@tool
extends VisualScriptCustomNode


# The name of the custom node as it appears in the search.
func _get_caption():
	return "kernel_opencl"

func _get_category():
	return "Input"

# The text displayed after the input port / sequence arrow.
func _get_text():
	return "code"

func _get_input_value_port_count():
	return 1

# The types of the inputs per index starting from 0.
func _get_input_value_port_type(idx):
	if idx==0:
		return TYPE_STRING
	return TYPE_OBJECT

func _get_output_value_port_count():
	return 1

# The types of outputs per index starting from 0.
func _get_output_value_port_type(idx):
	return TYPE_STRING

# The text displayed before each output node per index.
#func _get_ouput_value_port_name(idx):
#	return ""

func _has_input_sequence_port():
	return true

# The number of output sequence ports to use
# (has to be at least one if you have an input sequence port).
func _get_output_sequence_port_count():
	return 1

func _step(inputs, outputs, start_mode, working_mem):
	
	# start_mode can be checked to see if it is the first time _step is called.
	# This is useful if you only want to do an operation once.
	
	
	var ocl = OCL.new()
	var rng = RandomNumberGenerator.new()
	
	print(start_mode)

	var v = []
	var src = inputs[0]
	
	rng.randomize()
	
	for i in range(1 << 10):
		v.append(rng.randi_range(1,100))
		

	ocl.set(src)
	ocl.write(v)
	ocl.run()
	ocl.read(v)
	
	if ocl.get_errMsg() != "":
		print(ocl.get_errMsg())
	else:
		print("empty")
		
	print("log: ", ocl.get_log() )
	# working_memory is persistent between _step calls.

	# The inputs array contains the value of the input ports.

	# The outputs array is used to set the data of the output ports.

	# Return the error string if an error occurred, else the id of the next sequence port.
	return 0
