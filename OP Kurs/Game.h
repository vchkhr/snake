#pragma once

class Game
{
public:
	bool state = true; // true - play, false - stop
	float light[4] = { 0, -250, 0, 1 };

	int level = 1;
	int speed;

	int score = 0;
	int highScore = 1000;

	bool save = false;

	void start();
	void saveRead();
	void saveWrite();
	void calcSpeed();
};