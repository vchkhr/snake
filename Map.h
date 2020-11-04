#pragma once
#ifndef MAP_H
#define MAP_H
#include <vector>

class Map {
public:
	std::vector<int> tiles;
	int size = 20;
	float rotate[2] = {0, 0};
	float position[3] = {100, -50, 300};
	float speed = 2.0f;

	void generate();
	void draw(int);
};

#endif
