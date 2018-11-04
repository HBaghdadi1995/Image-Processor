
__kernel void function(__global int3* inputbuffer, __global float *imgkrnl, __global int3 *outputbuffer) {

	int width = get_global_size(0);
	int height = get_global_size(1);

	int ID = get_global_id(1) * width + get_global_id(0);

	//uint globalID = get_global_id(1) * get_global_size(0) + get_global_id(1);

	uint left = clamp((int)get_global_id(0) - 1, 0, 1);
	uint right = clamp(width - (int)(get_global_id(0) + 1), 0, 1);
	uint up = clamp((int)get_global_id(1) - 1, 0, 1);
	uint down = clamp(height - ((int)get_global_id(1) + 1), 0, 1);

	outputbuffer[ID] =
		clamp(
		(int3)(imgkrnl[0]) * inputbuffer[ID - 1 * left - width * up] +
			(int3)(imgkrnl[1]) * inputbuffer[ID - 1 * left] +
			(int3)(imgkrnl[2]) * inputbuffer[ID - 1 * left + width * down] +
			(int3)(imgkrnl[3]) * inputbuffer[ID - width * up] +
			(int3)(imgkrnl[4]) * inputbuffer[ID] +
			(int3)(imgkrnl[5]) * inputbuffer[ID + width * down] +
			(int3)(imgkrnl[6]) * inputbuffer[ID + 1 * right - width * up] +
			(int3)(imgkrnl[7]) * inputbuffer[ID + 1 * right] +
			(int3)(imgkrnl[8]) * inputbuffer[ID + 1 * right + width * down],

			(int3)(0),
			(int3)(255)
		);

	//int o = 1;

	//outputbuffer[ID] = height;
}