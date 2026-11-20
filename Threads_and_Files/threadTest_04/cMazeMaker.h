#ifndef _cMazeMaker_HG_
#define _cMazeMaker_HG_

// This code is taken from Jaden Peterson's posting on codereview:
// https://codereview.stackexchange.com/questions/135443/c-maze-generator

// I just put it into a class for you. That's it. He did all the real work!

#include <cctype>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>
#include "cMersenneTwister.h"


class cMazeMaker
{
public:
	cMazeMaker(int seed = 0);
	~cMazeMaker();
	bool GenerateMaze(unsigned int width, unsigned int height);
	void PrintMaze(void);
	void PrintMaze(char wallCharacter, char hallwayCharacter);
	void PrintMazeToFile(std::string fileName);
	void PrintMazeToFile(std::string fileName, char wallCharacter, char hallwayCharacter);

	void PrintMazeToFile_INFO3111(int SN, std::string fileName, char wallCharacter, char hallwayCharacter);

	std::string GetMazeString(void);
	std::string GetMazeString(char wallCharacter, char hallwayCharacter);

	// This is taken from the original code from Jaden Peterson's code.
	// The 1st 2 vectors are the 2D array.
	// The "vector of bools" has something to do with the generation of the maze,
	//	but you only have to look at the ***1st*** element of the vector of bools
	// this->maze[a][b][0] to see if it's a wall or an empty space (hallway)
	// == 0 --> a wall
	// == 1 (true) --> NOT a wall
	std::vector< std::vector< std::vector< bool > > > maze;

	bool getValue(unsigned int row, unsigned int column);

private:
	int m_maze_size[2];

	int m_start_axis;
	int m_start_side;

	static const unsigned int UP = 0;
	static const unsigned int DOWN = 1;
	static const unsigned int LEFT = 2;
	static const unsigned int RIGHT = 3;

	std::vector< std::vector< int > > m_dfs_path;

	bool m_randomMove(bool first_move);
	bool m_validInteger(char* cstr);
	void m_initializeMaze(void);
	void m_randomPoint(bool part);

	void m_ClearArea(unsigned int wStart, unsigned int wEnd, unsigned int hStart, unsigned int hEnd);

	cMersenneTwister* m_pRandom = NULL;

};

#endif
