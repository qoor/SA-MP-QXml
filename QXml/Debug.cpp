#include "Debug.h"

int Debug::debug = Debug::DEBUG_NONE;
std::ofstream Debug::file;

bool Debug::Initialize(const std::string& path)
{
	if (debug == Debug::DEBUG_NONE || file.is_open())
		return false;
	
	file.open(path, std::ios::trunc);

	return true;
}

bool Debug::Finalize()
{
	if (!file.is_open())
		return false;

	file.close();

	return true;
}