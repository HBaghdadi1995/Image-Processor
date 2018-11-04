#pragma once
#include "../Common/TSingleton.h"

#include "GPUProgram.h"



class  GPGPUManager: public TSingleton <GPGPUManager>
{
	friend class TSingleton<GPGPUManager>;
public:
	void Init();

	~GPGPUManager();

	GPUProgram* GenerateProgramFromFile(std::string str, int& status);

	void InitialiseProgram(GPUProgram*);

	void ReleaseProgram(GPUProgram*);

	void* GetOutput(int& status, size_t size, Buffer* outputBuffer);

	Buffer* CreateInputBuffer(void*, size_t size, int&);
	Buffer* CreateOutputBuffer(size_t, int&);

protected:

	void PickPhysicalDevice();
	void PickDevice();
	void CreateContext();
	void CreateCommandQueue();

	std::vector<GPUProgram*> programs;

	//OpenCL stuff
	cl_platform_id platform = nullptr;
	cl_uint numDevices = 0;
	cl_device_id *devices;
	cl_context context;
	cl_command_queue commandQueue;

	std::vector<Buffer*> inputBuffers;
	std::vector<Buffer*> outputBuffers;

};