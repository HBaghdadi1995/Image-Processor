#include "Buffer.h"

Buffer::~Buffer() {
	
	FreeBuffer();
	
	if (inputValues != NULL) {
		free(inputValues);
	}
}

void Buffer::SetInputBuffer(size_t size, void* inputValues) {
	this->size = size;
	this->inputValues = inputValues;
	isInput = true;
}

void Buffer::SetOutputBuffer(size_t size) {
	this->size = size;
	isInput = false;
}

void Buffer::ConstructBuffer(int& status) {
	hasBuffer = true;
	if(isInput)
		buffer = clCreateBuffer(
			context,
			CL_MEM_READ_ONLY |
			CL_MEM_USE_HOST_PTR,
			size,
			(void *)inputValues,
			&status);
	else
		buffer = clCreateBuffer(
			context,
			CL_MEM_WRITE_ONLY,
			size,
			NULL,
			&status);
}

void Buffer::FreeBuffer() {
	if (hasBuffer) {
		clReleaseMemObject(buffer);
		hasBuffer = false;
	}
}