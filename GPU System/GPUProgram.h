#pragma once
#include <vector>

#include "CL/cl.h"

#include "Buffer.h"

#include "../Common/FileReader.h"

class GPUProgram{
public:
	GPUProgram(cl_context context, cl_command_queue queue, cl_device_id* devices) : context(context), queue(queue), devices(devices) {}
	~GPUProgram();

	void CreateFromFile(std::string, int&);


	void CreateProgram(int &status);
	void CreateKernel(int &status);

	void Execute();

	void FreeProgram();
	void FreeKernel();

	void SetBuffers(std::vector<Buffer*>);

	void SetGlobalThreads(size_t, size_t y = 0, size_t z = 0);

protected:
	
	cl_program program;
	cl_kernel kernel;

	cl_context context;
	cl_command_queue queue;
	cl_device_id *devices;

	int dims = 1;

	std::vector<Buffer*> buffers;

	std::string strSource;

	size_t globalThreads[3];
};