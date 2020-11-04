#pragma once
#include <vector>

class Snake
{
public:
	std::vector<int> positions;
	int direction = 1; // 1 - right, 2 - down, 3 - left, 4 - up
	int spawn = 1; // 1 - left bottom, 2 - left top, 3 - right top, 4 - right bottom
	bool state = false; // true - in move, false - stop

	void move(int direction);
	void draw();
	void respawn();
};
