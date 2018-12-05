#pragma once

#include <iostream>
#include <queue>

class IdAllocator
{
private:
	int idCount;
	std::priority_queue<int, std::vector<int>, std::greater<int>> removeIds;

public:
	IdAllocator() : idCount(-1) {}
	~IdAllocator() {}

	int AllocateId();
	int DisallocateId(int id);
};
