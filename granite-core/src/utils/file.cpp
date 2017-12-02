#include "file.h"

std::string granite::read_file(const char* path)
{
	std::string result("");
	std::ifstream file(path);
    std::string str; 
    while (std::getline(file, str))
    {
		result += str;
		result.push_back('\n');
    }

	if (result.size() == 0)
		throw std::runtime_error("File not found " + std::string(path));
	return result;
}
