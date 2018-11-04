#include "GPGPUManager.h"

void GPGPUManager::Init() {
	PickPhysicalDevice();
	PickDevice();
	CreateContext();
	CreateCommandQueue();
}

void GPGPUManager::PickPhysicalDevice() {
	cl_uint numPlatforms;

	//Find Platform

	if (clGetPlatformIDs(0, NULL, &numPlatforms) != CL_SUCCESS) {
		throw std::runtime_error("Error while getting platforms");
	}
	if (numPlatforms > 0) {
		cl_platform_id* platforms = (cl_platform_id*)malloc(numPlatforms * sizeof(cl_platform_id));
		clGetPlatformIDs(numPlatforms, platforms, NULL);
		platform = platforms[0];
		free(platforms);
	}

	if (platform == NULL) {
		throw std::runtime_error("Error: Can't find suitable Platform");
	}
}

void GPGPUManager::CreateCommandQueue() {

	int status;
	commandQueue = clCreateCommandQueueWithProperties(context, devices[0], 0, &status);
	if (status != CL_SUCCESS)
	{
		throw std::runtime_error("Error: Creating command queue failed!");
	}
}

void GPGPUManager::PickDevice() {
	if (clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 0, NULL, &numDevices) != CL_SUCCESS) {
		throw std::runtime_error("Error while getting context");
	}
	devices = (cl_device_id*)malloc(numDevices * sizeof(cl_device_id));
	if (numDevices == 0) {
		throw std::runtime_error("Error: No Suitable Devices Available");
	}
	clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, numDevices, devices, NULL);
}

void GPGPUManager::CreateContext() {
	cl_int status;
	context = clCreateContext(NULL, numDevices, devices, NULL, NULL, &status);
	if (status != CL_SUCCESS) {
		throw std::runtime_error("Error: couldn't create context");
	}
}

GPGPUManager::~GPGPUManager() {
	for (auto it = programs.begin(); it != programs.end(); it++) {
		delete *it;
		//ReleaseProgram(*it);
	}

	for (auto it = inputBuffers.begin(); it != inputBuffers.end(); it++) {
		delete *it;
	}
	for (auto it = outputBuffers.begin(); it != outputBuffers.end(); it++) {
		delete *it;
	}

	clReleaseContext(context);

	if (devices != NULL)
	{
		free(devices);
		devices = NULL;
	}
	
}

void* GPGPUManager::GetOutput(int& status, size_t size, Buffer* outputBuffer) {

	void* output = malloc(size);

	status = clEnqueueReadBuffer(commandQueue, outputBuffer->getBuffer(), CL_TRUE, 0, size, output, NULL, NULL, NULL);

	return output;
}

Buffer* GPGPUManager::CreateInputBuffer(void* input, size_t size, int& status) {

	Buffer* buffer = new Buffer(context);
	buffer->SetInputBuffer(size, input);
	buffer->ConstructBuffer(status);
	if (status == CL_SUCCESS) {
		std::cerr << "created buffer";
	}
	inputBuffers.push_back(buffer);

	return buffer;
}

Buffer* GPGPUManager::CreateOutputBuffer(size_t size, int& status) {

	Buffer* buffer = new Buffer(context);
	buffer->SetOutputBuffer(size);
	buffer->ConstructBuffer(status);
	outputBuffers.push_back(buffer);

	return buffer;
}

GPUProgram* GPGPUManager::GenerateProgramFromFile(std::string str, int& status) {
	GPUProgram *pro = new GPUProgram(context, commandQueue, devices);
	pro->CreateFromFile(str, status);
	programs.push_back(pro);
	return pro;
}

void GPGPUManager::InitialiseProgram(GPUProgram* program) {
	int status;
	program->CreateProgram(status);

	program->CreateKernel(status);

	std::cerr << "kerneled";
}

void GPGPUManager::ReleaseProgram(GPUProgram* program) {
	programs.erase(std::find(programs.begin(),programs.end(),program));
}