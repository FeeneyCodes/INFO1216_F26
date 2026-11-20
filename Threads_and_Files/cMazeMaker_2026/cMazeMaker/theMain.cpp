#include <iostream>
#include "cMazeMaker.h"
#include <sstream>
#include <fstream>
#include <string>

int main( int argc, char* arv[])
{
	std::cout << "What's your student numbmer? ";
	int SN = 0;
	std::cin >> SN;

	cMazeMaker theMaze(SN);

	theMaze.GenerateMaze(20, 20);

	std::cout << "Your SN is: " << SN << std::endl;
	std::cout << "Your  maze is: " << std::endl;
	std::cout << std::endl;

	std::string sTheMaze = theMaze.GetMazeString();
	std::cout << sTheMaze << std::endl;

	std::stringstream ssFileName;
	ssFileName << "INFO_3135_Maze_" << SN << ".txt";

	std::ofstream mazeFile(ssFileName.str());
	mazeFile << sTheMaze << std::endl;
	

	return 0;
}
