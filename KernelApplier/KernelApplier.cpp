#include <string>
#include "../GPU System/GPGPUManager.h"
#include <filesystem>

cl_float* GetKernel(const char* kernel) {
	int status;
	std::string kernelStr = FileReader::ReadFile(kernel, status);
	
	cl_float* kernelValues = (cl_float*)malloc(9 * sizeof(cl_float));


	int Index = 0;
	for (int i = 0; i < 9; i++) {

			bool negative = false;
			bool isLhs = true;

			int lhs = 0;
			int rhs = 0;
			int lhsSize = 0;
			int rhsSize = 0;

			do {
				if (kernelStr.at(Index) == '-') {
					negative = true;
				}
				else if (kernelStr.at(Index) >= '0' && kernelStr.at(Index) <= '9') {
					if (isLhs) {
						lhs = lhs * 10 + (kernelStr.at(Index) - '0');
						lhsSize++;
					}
					else {
						rhs = rhs * 10 + (kernelStr.at(Index) - '0');
						rhsSize++;
					}
				}
				else if (kernelStr.at(Index) == '.') {
					isLhs = false;
				}
				Index++;
			} while (kernelStr.at(Index) != ',');

			kernelValues[i] = (cl_float)lhs + ((cl_float)rhs / (cl_float)std::pow(10, rhsSize));
			if (negative) {
				kernelValues[i] = -kernelValues[i];
			}
		
	}
	return kernelValues;
}

/*cl_int4 * GetImg(std::string img, size_t size, int x, int y) {
	int status;
	std::string kernelStr = FileReader::ReadFile(img, status);


	

	int i;
	int j;
	int k;
	int num;
	bool openBraces = false;

	for (int Index = 0; Index < img.size(); Index++) {
		if (kernelStr.at(Index) == '(') {
			openBraces = true;
		}
		else if (kernelStr.at(Index) >= '0' && kernelStr.at(Index) <= '9') {
			num = num * 10 + img.at(Index) - '0';
		}
		else if (kernelStr.at(Index) == ',') {
			if (openBraces) {
				if (k == 0) {
					inputBuffer[(i + 1)* (x + 2) + y + 1].x = num;
				}
				else if (k == 1) {
					inputBuffer[(i + 1)* (x + 2) + y + 1].y = num;
				}
				else if (k == 2) {
					inputBuffer[(i + 1)* (x + 2) + y + 1].z = num;
				}
				k++;
				num = 0;
			}
			else {
				k = 0;
				j++;
			}

		}
		else if (kernelStr.at(Index) == ')') {
			openBraces = false;
		}
		else if (kernelStr.at(Index) == '\n') {
			i++;
		}
	}
	return inputBuffer;
}*/

int * inputBuffer;
int size;
int m_x, m_y;
GPUProgram *program;
std::vector<Buffer*> buffers;

extern "C" __declspec(dllexport) int* InitPointer(int x, int y) {
	size = x*y * sizeof(cl_int3);
	m_x = x;
	m_y = y;
	inputBuffer = (int*)malloc(size);
	return inputBuffer;
}

extern "C" __declspec(dllexport) void ApplyKernel(const char* kernel, const char* kernelApplier) {

	std::cerr << "Getting Kernel... \n";
	cl_float* kernelValues = GetKernel(kernel);
	std::cerr << "Kernel Got... \n";
	
	int status;

	std::cerr << "Initialising Manager... \n";
	GPGPUManager::Instance()->Init();
	std::cerr << "Initialisitation Done... \n";

	std::cerr << "Creating Buffers... \n";
	buffers.push_back(GPGPUManager::Instance()->CreateInputBuffer((void*)inputBuffer, size, status));
	buffers.push_back(GPGPUManager::Instance()->CreateInputBuffer((void*)kernelValues, 9 * sizeof(cl_float), status));
	buffers.push_back(GPGPUManager::Instance()->CreateOutputBuffer(size, status));
	std::cerr << "Buffers Created... \n";
	//std::cerr << std::filesystem::current_path() <<'\n';

	std::cerr << "Generating Program... \n";
	program = GPGPUManager::Instance()->GenerateProgramFromFile(kernelApplier, status);
	std::cerr << "Program Generated... \n";

	std::cerr << "Setting Buffers... \n";
	program->SetBuffers(buffers);
	std::cerr << "Buffers Set... \n";

	std::cerr << "Initilising Program... \n";
	GPGPUManager::Instance()->InitialiseProgram(program);
	std::cerr << "Program Initialised... \n";

	std::cerr << "Setting Global Threads... \n";
	program->SetGlobalThreads(m_x, m_y);
	std::cerr << "Global Threads Set... \n";

	std::cerr << "Executing... \n";
	program->Execute();
	std::cerr << "Executed... \n";

	free(inputBuffer);
	free(kernelValues);

	//inputBuffer = (int*)GPGPUManager::Instance()->GetOutput(status, size, buffers.at(2));

	//program->FreeKernel();
	//program->FreeProgram();

	//GPGPUManager::Instance()->Release();
}

extern "C" __declspec(dllexport) void CleanUp() {
	for (auto it = buffers.begin(); it != buffers.end(); ++it) {
		(*it)->FreeBuffer();
	}
	program->FreeKernel();
	program->FreeProgram();

	//GPGPUManager::Instance()->Release();
}

extern "C" __declspec(dllexport) int* GetValues() {
	int status;
	return (int*)GPGPUManager::Instance()->GetOutput(status, size, buffers.at(2));
}