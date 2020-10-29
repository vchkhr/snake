#include <GL/glut.h>
#include <string>
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>

#include "Game.h"
#include "Map.h"
#include "Snake.h"
#include "Fruit.h"
#include "Stone.h"

Game game;
Map map;
Snake snake;
Fruit fruit;
Stone stone;

bool searchInVector(std::vector<int> v, int x)
{
	std::sort(v.begin(), v.end());
	if (std::binary_search(v.begin(), v.end(), x))
		return true;
	else
		return false;
}

void Game::start()
{
	game.score = 0;
	stone.positions.clear();
	snake.respawn();
	map.tiles.clear();
	snake.state = false;
	snake.direction = 1;
	fruit.generate();
	game.calcSpeed();

	int stoneNum = map.size / 3 * 2 + 1;
	stone.generate(stoneNum);
}

void Game::saveRead()
{
	std::ifstream gameSave;
	gameSave.open("GameSave.txt");
	gameSave >> game.level >> game.highScore;
	gameSave.close();
}

void Game::saveWrite()
{
	std::ofstream gameSave;
	gameSave.open("GameSave.txt");
	gameSave << game.level << " " << game.highScore;
	gameSave.close();
}

void Game::calcSpeed()
{
	if (game.level == 1)
		game.speed = 300;
	else if (game.level == 2)
		game.speed = 200;
	else if (game.level == 3)
		game.speed = 100;
}

void Map::generate()
{
	for (int i = 0; i < map.size * map.size; i++)
	{
		int type = map.tiles.at(i);
		if (type == 1)
			map.draw(i);
		else
			stone.draw(i); // Stone
	}
}

void Map::draw(int i)
{
	glColor3f(0.0f, 0.0f, 0.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glColor3f(0.0, 1.0, 0.0);

	glBegin(GL_QUADS);
	int x = (i % map.size) * 10;
	int y = (i / map.size) * 10;
	int z = 0;
	glTexCoord2f(0, 0);
	glNormal3f(0, 0, 1);
	glVertex3i(x, y, z);
	glTexCoord2f(0, 1);
	glNormal3f(0, 0, 1);
	glVertex3i(x, y + 10, z);
	glTexCoord2f(1, 1);
	glNormal3f(0, 0, 1);
	glVertex3i(x + 10, y + 10, z);
	glTexCoord2f(1, 0);
	glNormal3f(0, 0, 1);
	glVertex3i(x + 10, y, z);
	glEnd();
}

void Snake::move(int direction)
{
	int indexHead = snake.positions.at(0);
	int xHead = indexHead % map.size;
	int yHead = indexHead / map.size;
	int xNext, yNext, indexNext;
	if (direction == 1) // right
	{
		xNext = xHead == map.size - 1 ? 0 : xHead + 1;
		yNext = yHead;
	}
	else if (direction == 2) // down
	{
		xNext = xHead;
		yNext = yHead == 0 ? map.size - 1 : yHead - 1;
	}
	else if (direction == 3) // left
	{
		xNext = xHead == 0 ? map.size - 1 : xHead - 1;
		yNext = yHead;
	}
	else // up
	{
		xNext = xHead;
		yNext = yHead == map.size - 1 ? 0 : yHead + 1;
	}
	indexNext = yNext * map.size + xNext;

	if (indexNext == fruit.position)
	{
		// Fruit is next block
		game.score++;
		if (game.score > game.highScore)
			game.highScore = game.score;
		snake.positions.insert(snake.positions.begin(), indexNext);
		fruit.generate();
	}
	else if (searchInVector(stone.positions, indexNext) || searchInVector(snake.positions, indexNext))
	{
		// Stone or Snake is next block
		Sleep(game.speed);
		if (game.save)
			game.saveWrite();
		game.state = false;
	}
	else
	{
		Sleep(game.speed);
		snake.positions.insert(snake.positions.begin(), indexNext);
		snake.positions.pop_back();
	}
}

void Snake::draw()
{
	glMatrixMode(GL_MODELVIEW);

	glColor3f(1.0, 1.0, 0.0);

	if (snake.state)
		snake.move(snake.direction);

	for (int i = 0; i < snake.positions.size(); i++)
	{
		int index = snake.positions.at(i);
		int x = 10 / 2 + index % map.size * 10;
		int y = 10 / 2 + index / map.size * 10;
		glPushMatrix();
		glTranslatef(x, y, 10 / 2);
		glutSolidSphere(10 / 2, 50, 50);
		glPopMatrix();
	}
	glDisable(GL_BLEND);
	glutPostRedisplay();
}

void Snake::respawn()
{
	snake.positions.clear();
	if (snake.spawn == 1)
	{
		snake.positions.push_back(0);
		snake.direction = 1;
	}
	else if (snake.spawn == 2)
	{
		snake.positions.push_back(map.size * (map.size - 1));
		snake.direction = 1;
	}
	else if (snake.spawn == 3)
	{
		snake.positions.push_back(map.size * map.size - 1);
		snake.direction = 3;
	}
	else if (snake.spawn == 4)
	{
		snake.positions.push_back(map.size - 1);
		snake.direction = 3;
	}
}


void Fruit::generate()
{
	srand(time(NULL));
	int randomPos = rand() % (map.size * map.size);
	while (true)
	{
		if (searchInVector(stone.positions, randomPos) || searchInVector(snake.positions, randomPos))
			randomPos = rand() % (map.size * map.size);
		else
			break;
	}
	this->position = randomPos;
}

void Fruit::draw()
{
	glMatrixMode(GL_MODELVIEW);

	glColor3f(1, 0, 0);
	int x = 10 / 2 + fruit.position % map.size * 10;
	int y = 10 / 2 + fruit.position / map.size * 10;
	glPushMatrix();
	glTranslatef(x, y, 10 / 2);
	glutSolidSphere(10 / 2, 50, 50);
	glPopMatrix();

	glutPostRedisplay();
}

void Stone::generate(int numStones)
{
	for (int i = 0; i < map.size * map.size; i++)
		map.tiles.push_back(1);

	srand(time(NULL));
	for (int i = 0; i < numStones; i++)
	{
		int randomPos = rand() % (map.size * map.size);
		while (true)
		{
			if (randomPos == fruit.position || randomPos == 0 || randomPos == 1 || randomPos == 2 ||
				randomPos == map.size + 0 || randomPos == map.size + 1 || randomPos == map.size + 2 ||
				randomPos == map.size * 2 + 0 || randomPos == map.size * 2 + 1 || randomPos == map.size * 2 + 2 ||
				searchInVector(stone.positions, randomPos))
				randomPos = rand() % (map.size * map.size);
			else
				break;
		}
		map.tiles.at(randomPos) = 0;
		stone.positions.push_back(randomPos);
	}
}

void Stone::draw(int i)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glColor3f(0.3, 0.3, 0.3);
	glBegin(GL_QUADS);
	int x = (i % map.size) * 10;
	int y = (i / map.size) * 10;

	// Top
	glTexCoord2f(0, 0);
	glNormal3f(0, 0, 1);
	glVertex3i(x, y, 10);
	glTexCoord2f(0, 1);
	glNormal3f(0, 0, 1);
	glVertex3i(x, y + 10, 10);
	glTexCoord2f(1, 1);
	glNormal3f(0, 0, 1);
	glVertex3i(x + 10, y + 10, 10);
	glTexCoord2f(1, 0);
	glNormal3f(0, 0, 1);
	glVertex3i(x + 10, y, 10);

	// Front
	glTexCoord2f(0, 0);
	glNormal3f(0, -1, 0);
	glVertex3i(x, y, 0);
	glTexCoord2f(0, 1);
	glNormal3f(0, -1, 0);
	glVertex3i(x, y, 10);
	glTexCoord2f(1, 1);
	glNormal3f(0, -1, 0);
	glVertex3i(x + 10, y, 10);
	glTexCoord2f(1, 0);
	glNormal3f(0, -1, 0);
	glVertex3i(x + 10, y, 0);

	// Left
	glTexCoord2f(0, 0);
	glNormal3f(-1, 0, 0);
	glVertex3i(x, y, 0);
	glTexCoord2f(0, 1);
	glNormal3f(-1, 0, 0);
	glVertex3i(x, y + 10, 0);
	glTexCoord2f(1, 1);
	glNormal3f(-1, 0, 0);
	glVertex3i(x, y + 10, 10);
	glTexCoord2f(1, 0);
	glNormal3f(-1, 0, 0);
	glVertex3i(x, y, 10);

	// Right
	glTexCoord2f(0, 0);
	glNormal3f(1, 0, 0);
	glVertex3i(x + 10, y, 0);
	glTexCoord2f(0, 1);
	glNormal3f(1, 0, 0);
	glVertex3i(x + 10, y + 10, 0);
	glTexCoord2f(1, 1);
	glNormal3f(1, 0, 0);
	glVertex3i(x + 10, y + 10, 10);
	glTexCoord2f(1, 0);
	glNormal3f(1, 0, 0);
	glVertex3i(x + 10, y, 10);

	// Back
	glTexCoord2f(0, 0);
	glNormal3f(0, 1, 0);
	glVertex3i(x, y + 10, 0);
	glTexCoord2f(0, 1);
	glNormal3f(0, 1, 0);
	glVertex3i(x, y + 10, 10);
	glTexCoord2f(1, 1);
	glNormal3f(0, 1, 0);
	glVertex3i(x + 10, y + 10, 10);
	glTexCoord2f(1, 0);
	glNormal3f(0, 1, 0);
	glVertex3i(x + 10, y + 10, 0);

	glEnd();
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(map.position[0], map.position[1], map.position[2], 10 * map.size / 2, 10 * map.size / 2, 0, 0, 1, 0);

	if (game.state)
	{
		glPushMatrix();

		// Map rotation
		glTranslatef(10 * map.size / 2, 10 * map.size / 2, 0);
		glRotatef(map.rotate[0], 1, 0, 0);
		glTranslatef(-10 * map.size / 2, -10 * map.size / 2, 0);

		glTranslatef(10 * map.size / 2, 10 * map.size / 2, 0);
		glRotatef(map.rotate[1], 0, 1, 0);
		glTranslatef(-10 * map.size / 2, -10 * map.size / 2, 0);

		// Generate map
		glEnable(GL_LIGHTING);

		map.generate();

		glBindTexture(GL_TEXTURE_2D, 0);
		fruit.draw();
		snake.draw();
		glPopMatrix();

		// Text
		glDisable(GL_LIGHTING);

		std::string scoreDescription = "";
		if (game.save)
		{
			glRasterPos2i(40, 240);
			scoreDescription = "LEVEL: " + std::to_string(game.level) + "\t\t\tSCORE: " + std::to_string(game.score) + "\t\t\tHIGH: " + std::to_string(game.highScore);
		}
		else
		{
			glRasterPos2i(65, 240);
			scoreDescription = "LEVEL: " + std::to_string(game.level) + "\t\t\tSCORE: " + std::to_string(game.score);
		}

		for (std::string::iterator i = scoreDescription.begin(); i != scoreDescription.end(); ++i)
		{
			char c = *i;
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
		}
	}
	else
	{
		glPushMatrix();
		glRasterPos2i(65, 110);
		std::string scoreDescription2 = "";
		if (game.score == game.highScore)
			scoreDescription2.append("New record!");
		else
			scoreDescription2.append("Game over!");
		scoreDescription2.append(" Score: " + std::to_string(game.score));
		for (std::string::iterator i = scoreDescription2.begin(); i != scoreDescription2.end(); ++i)
		{
			char c = *i;
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
		}
		glPopMatrix();
	}
	glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'q':
	case 'Q':
	case 27: // Esc
		if (game.save)
			game.saveWrite();
		exit(0);
		break;
	case 'd':
	case 'D':
		if ((snake.direction == 2 || snake.direction == 4) && snake.state)
			snake.direction = 1;
		break;
	case 's':
	case 'S':
		if ((snake.direction == 1 || snake.direction == 3) && snake.state)
			snake.direction = 2;
		break;
	case 'a':
	case 'A':
		if ((snake.direction == 2 || snake.direction == 4) && snake.state)
			snake.direction = 3;
		break;
	case 'w':
	case 'W':
		if ((snake.direction == 1 || snake.direction == 3) && snake.state)
			snake.direction = 4;
		break;
	case 'e':
	case 'E':
		if (!snake.state) {
			snake.spawn++;
			if (snake.spawn == 5)
				snake.spawn = 1;

			snake.respawn();
		}
		break;
	case 'r':
	case 'R':
		game.start();
		break;
	case 'l':
	case 'L':
		if (!snake.state && game.score == 0)
		{
			game.level++;
			if (game.level == 4)
				game.level = 1;

			if (game.save)
				game.saveWrite();

			game.calcSpeed();
		}
		break;
	case 'f':
	case 'F':
		if (!snake.state && game.score == 0)
		{
			game.save = !game.save;

			if (game.save)
				game.saveRead();
			else
			{
				game.level = 1;
				game.highScore = 1000;
			}

			game.calcSpeed();
		}
		break;
	case ' ':
		if (!game.state)
		{
			game.state = true;
			glutPostRedisplay();
			game.start();
		}
		else
			snake.state = !snake.state;
		break;
	}
}

void special(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:
		map.rotate[0] -= map.speed;
		break;
	case GLUT_KEY_DOWN:
		map.rotate[0] += map.speed;
		break;
	case GLUT_KEY_LEFT:
		map.rotate[1] -= map.speed;
		break;
	case GLUT_KEY_RIGHT:
		map.rotate[1] += map.speed;
		break;
	}
	glutPostRedisplay();
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Snake");

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);

	glEnable(GL_DEPTH_TEST);

	glClearColor(0, 0, 0, 0);
	glColor3f(1, 1, 1);
	glMatrixMode(GL_PROJECTION);
	gluPerspective(45, 1, 1, 1000);

	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT0, GL_POSITION, game.light);

	game.start();

	glutMainLoop();
	return (0);
}
