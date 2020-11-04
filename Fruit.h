#pragma once
#ifndef FRUIT_H
#define FRUIT_H

class Map;

class Fruit {
public:
	int position = 0;

	void generate();
	void draw();
};

#endif