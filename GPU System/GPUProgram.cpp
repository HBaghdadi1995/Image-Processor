#include "GPUProgram.h"

void GPUProgram::CreateFromFile(std::string filename, int& status) {

	strSource = FileReader::ReadFile(filename,status);
}

GPUProgram::~GPUProgram() {
	FreeProgram();
	FreeKernel();
}

void GPUProgram::CreateProgram(int &status) {

	const char* source = strSource.c_str();
	size_t size = strlen(source);

	program = clCreateProgramWithSource(context, 1, &source, &size, &status);
	if (status != CL_SUCCESS)
	{
		std::cerr << "Error: creating Program failed: " + std::to_string(status) << std::endl;
		throw std::runtime_error("Error: creating Program failed: " + std::to_string(status));
	}
	clBuildProgram(program, 1, devices, NULL, NULL, NULL);
}
void GPUProgram::CreateKernel(int &status) {

	kernel = clCreateKernel(program, "function", &status);
	//std::cerr << "SOCIAL MEDIA \n";
	if (status != CL_SUCCESS)
	{
		std::cerr << strSource << std::endl;
		std::cerr << "Error: creating kernel failed: " + std::to_string(status);
		throw std::runtime_error("Error: creating kernel failed: " + std::to_string(status));
	}
	std::cerr << "kernel created \n";

	for (int i = 0; i < buffers.size(); i++) {
		Buffer *buffer = buffers.at(i);
		cl_mem memBuffer = buffer->getBuffer();
		std::cerr << "buffer Copied\n";
		status = clSetKernelArg(kernel, i, sizeof(cl_mem), (void*)&memBuffer);

		if (status != CL_SUCCESS) {
			std::cerr << "Error: failed to set arg " << i << ": " << status << "\n";
		}
		else {
			std::cerr << "arg " << i << " set\n";
		}
	}

	//std::cerr << "kerneled";
}
void GPUProgram::SetBuffers(std::vector<Buffer*> buffers) {
	this->buffers = buffers;
}

void GPUProgram::SetGlobalThreads(size_t x, size_t y, size_t z) {
	globalThreads[0] = x;
	if (y != 0) {
		globalThreads[1] = y;
		dims++;
	}
	if (z != 0) {
		globalThreads[2] = z;
		dims++;
	}

}

void GPUProgram::Execute() {
	cl_int status = clEnqueueNDRangeKernel(queue, kernel, dims, NULL, globalThreads, NULL, 0, NULL, NULL);
	if (status != CL_SUCCESS)
	{
		std::cerr << "Error: Enqueue kernel onto command queue failed!: " << status << "\n";
		
		//throw std::runtime_error("Error: Enqueue kernel onto command queue failed!");
	}

	status = clFinish(queue);
	if (status != CL_SUCCESS)
	{
		std::cerr << "Error: finishing command failed!: "<< status << "\n";
		
		//throw std::runtime_error("Error: failed to finish");
	}
}

void GPUProgram::FreeProgram() {

	clReleaseProgram(program);
}

void GPUProgram::FreeKernel() {
	clReleaseKernel(kernel);
}