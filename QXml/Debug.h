#pragma once

#define DEBUG Debug::file

#include <iostream>
#include <fstream>

class Debug
{
private:
	Debug() {};
	~Debug() {};

public:
	static int debug;
	static std::ofstream file;

	static bool Initialize(const std::string& path);
	static bool Finalize();

	enum DEBUG_MODES
	{
		DEBUG_MODE,
		DEBUG_NONE
	};
};
