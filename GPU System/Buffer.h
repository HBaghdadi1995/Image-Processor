#pragma once
#include <stdlib.h>

#include "CL/opencl.h"

class Buffer {
public:
	Buffer(cl_context context) : context(context) {}
	~Buffer();

	void SetInputBuffer(size_t, void*);
	void SetOutputBuffer(size_t);

	void ConstructBuffer(int&);

	void FreeBuffer();

	size_t getSize() { return size; }
	cl_mem getBuffer() { return buffer; }
protected:
	size_t size;
	
	cl_mem buffer;

	cl_context context;

	bool isInput;
	bool hasBuffer = false;

	void* inputValues = NULL;

};