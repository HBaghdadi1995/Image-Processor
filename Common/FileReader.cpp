#include "FileReader.h"

std::string FileReader::ReadFile(std::string filename, int &status) {

	std::string s;

	try {
		size_t size;
		char*  str;
		std::fstream f(filename.c_str(), (std::fstream::in | std::fstream::binary));
		
		if (f.is_open())
		{
			size_t fileSize;
			f.seekg(0, std::fstream::end);
			size = fileSize = (size_t)f.tellg();
			f.seekg(0, std::fstream::beg);
			str = new char[size + 1];
			if (!str)
			{
				f.close();
				return "";
			}

			f.read(str, fileSize);
			f.close();
			str[size] = '\0';
			s = str;
			delete[] str;
			return s;
		}
		throw std::runtime_error("Error: failed to open (string): " + filename + "\n");
	}
	catch (const std::runtime_error& e) {
		std::cerr << e.what() << std::endl;
		status = -71;
		return"";
	}
	return s;
}

std::string FileReader::ReadFile(const char* filename, int &status) {

	std::string s;
	
	try {
		size_t size;
		char*  str;
		std::fstream f(filename, (std::fstream::in | std::fstream::binary));
		if (f.is_open())
		{			size_t fileSize;
			f.seekg(0, std::fstream::end);
			size = fileSize = (size_t)f.tellg();
			f.seekg(0, std::fstream::beg);
			str = new char[size + 1];
			if (!str)
			{

				f.close();
				return "";
			}

			f.read(str, fileSize);
			f.close();

			str[size] = '\0';
			s = str;
			delete[] str;
			return s;
		}else
		throw std::runtime_error("Error: failed to open (char *): " + (std::string)filename + "\n");
	}
	catch (const std::runtime_error& e) {
		std::cerr << e.what() << std::endl;
		status = -71;
		return"";
	}
	//return s;
}