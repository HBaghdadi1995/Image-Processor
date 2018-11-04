#pragma once
#include <string>
#include <iostream>
#include <fstream>

class FileReader {
public:
	static std::string ReadFile(std::string filename, int &status);
	static std::string ReadFile(const char* filename, int &status);
};