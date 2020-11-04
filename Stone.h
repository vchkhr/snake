#pragma once
#ifndef STONE_H
#define STONE_H
#include <vector>

class Stone
{
public:
	std::vector<int> positions;

	void generate(int);
	void draw(int);
};

#endif
