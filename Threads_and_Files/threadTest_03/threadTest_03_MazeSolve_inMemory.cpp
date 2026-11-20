#include <iostream>
#include <string>
#include "cMazeMaker.h"
#include "cGridMazeExplorer.h"
#include <sstream>
#include <fstream>
#include <Windows.h>
#include "cHFTimer.h"
#include "cVT100TerminalConsole.h"



// We can pass anything to the thread, but it's common 
//	to package what we are passing in a struct or class.
// Like we could have just passed this string, but we
//	will add other things that we want to give to the 
//	thread function later.  
struct sTParams
{
	std::string Message;
};

// This is the function that the thread will run
// To be more general, it is passed a type "void*"
//	called a "pointer to void" meaning it can be any type
//	or that it is just a memory location, but with no specific
//	type. 
// The "catch" is that you have to be very careful casting it
//	to something else because it will blindly trust you. 
// 
// Otherwise, this is just a regular function.
DWORD WINAPI ThreadProc( LPVOID lpParams)
{
	sTParams* pTheParams = (sTParams*)lpParams;
	
	std::cout << "Thread was passed " << pTheParams->Message << std::endl;
	
	pTheParams->Message = "Hello from the thread!";
	
	return 0;
}


// 8 25 20 myMaze 8 -> doesn't solve with recursive serach

int main(int argc, char* argv[])
{
	// Usage: exe #threads mazeRows mazeColumns filename seed
	// example: exe 8 100 50 mazefile 543
	// (8 threads, 100x50 maze, 
	//  "mazefile.txt" has original maze
	//  "mazefile_path.txt has maze with path,
	//  RNG seed = 543)
	
	unsigned int numThreads = 1;
	std::string mazeFileName = "mazeFile";
	unsigned int mazeColumns_Width = 20;
	unsigned int mazeRows_Length = 20;
	unsigned long RNGseed = 0;
	
	
	if ( argc == 6 )
	{
		// //exe 8 100 50 mazefile 543
		numThreads = (unsigned long)atol(argv[1]);
		mazeColumns_Width = (unsigned long)atol(argv[2]);
		mazeRows_Length = (unsigned long)atol(argv[3]);
		mazeFileName.append(argv[4]);
		RNGseed = (unsigned long)atol(argv[5]);
	}
	else
	{
		// Didn't enter any command line params
		std::cout << "You can enter parameters on the command line:" << std::endl;
		std::cout << "\tUsage: exe #threads mazesize filename seed" << std::endl;
		std::cout << "\texample: exe 8 100 50 mazefile 543" << std::endl;
		std::cout << "\t( 8 threads, 100x50 maze," << std::endl;
		std::cout << "\t 'mazefile.txt' has original maze," << std::endl;
		std::cout << "\t 'mazefile_path.txt' has maze with path," << std::endl;
		std::cout << "\t  RNG seed = 543)" << std::endl;
		std::cout << std::endl;
		std::cout << "How wide is the maze (num of columns) in cells? ";
		std::cin >> mazeColumns_Width;
		std::cout << "How long is the maze (num of rows) in cells? ";
		std::cin >> mazeRows_Length;
		std::cout << "How many threads to solve the maze? ";
		std::cin >> numThreads;
		std::cout << "What random seed would you like (0 = program picks one)?";
		std::cin >> RNGseed;
		std::cout << "Maze file name will be '" << mazeFileName << ".txt'" << std::endl;
	}

	
	// If seed = 0, then pick on based on time
	if (RNGseed == 0)
	{
		// Seed with time, processID, and clock time
		RNGseed = (unsigned int)(time(NULL) ^ GetCurrentProcessId() ^ clock());
		std::cout << "Picked RNG seed of " << RNGseed << "." << std::endl;
	}
	
	cHFTimer* pTimer = new cHFTimer();
	
	std::cout << "Generating " << mazeColumns_Width << "x" << mazeRows_Length
		<< " maze with seed of " << RNGseed << "...";
	cMazeMaker* pTheMaze = new cMazeMaker(RNGseed);
	
	pTheMaze->GenerateMaze(mazeColumns_Width, mazeRows_Length);
	std::cout << "done ";
	
	double deltaTime = pTimer->getElapsedTimeMilliSeconds();
	std::cout << "(took " << deltaTime << " ms)." << std::endl;
	
	
	std::string mazeAsString = pTheMaze->GetMazeString('X', '.');
		
	std::ofstream mazeFile(mazeFileName + ".txt");
	mazeFile << mazeAsString << std::endl;
	mazeFile.close();
	
	bool bMazeFitsOnScreen = true;
	if ( mazeColumns_Width > 120 )	{	bMazeFitsOnScreen = false; }
//	if ( mazeRows_Length > 40 )	{ 	bMazeFitsOnScreen = false; }
	if ( ! bMazeFitsOnScreen )
	{
		std::cout << "The maze is too big to fit nicely on the screen." << std::endl;
		std::cout << "Do you want to see it, anyways? (Y/N)" << std::endl;
		std::string answer;
		std::cin >> answer;
		if ( answer.length() > 0 )
		{
			if ( answer[0] == 'Y' || answer[0] == 'y' )
			{
				bMazeFitsOnScreen = true;
			}
		}
	}
	
//	if ( bMazeFitsOnScreen )
//	{
//		std::cout << mazeAsString << std::endl;
//	}
	
	cGridMazeExplorer* pMazeExplorer = new cGridMazeExplorer(RNGseed);
	
	pMazeExplorer->SetMaze(mazeAsString, 'X', '.');
	
//	std::cout << "Rows: " << pMazeExplorer->getNumberOfRows_Length() << std::endl;
//	std::cout << "Cols: " << pMazeExplorer->getNumberOfColumns_Width() << std::endl;
	

//	 std::cout << "Maze from Maze eplorer:" << std::endl;
//	 for ( int row = 0; row < pMazeExplorer->getNumberOfRows_Length(); row++ )
//	 {
//		 for ( int col = 0; col < pMazeExplorer->getNumberOfColumns_Width(); col++ )
//		 {
//			 switch (pMazeExplorer->getGridCellType(row, col))
//			 {
//				 case cGridMazeExplorer::FLOOR:
//					 std::cout << '.';
//					 break;
//				 case cGridMazeExplorer::WALL:
//					 std::cout << 'X';
//					 break;
//				 case cGridMazeExplorer::UNKNOWN_TYPE:
//					 std::cout << '?';
//					 break;
//				 case cGridMazeExplorer::INVALID_CELL:
//					 std::cout << '^';
//					 break;
//				 default:
//					 std::cout << 'V';
//					 break;
//			 }
//		 }
//		 std::cout << std::endl;
//	 }
	
	std::vector<cGridMazeExplorer::sGridLocation> vecExits;
	if ( pMazeExplorer->FindExits(vecExits) )
	{
		std::cout << vecExits.size() << " exits:" << std::endl;
		for ( cGridMazeExplorer::sGridLocation cell : vecExits )
		{
			std::cout << "at (" << cell.row << ", " << cell.col << ")" << std::endl;
		}
	}
	

	// **********************************************************************
	cVT100TerminalConsole* pVT = new cVT100TerminalConsole();
//	std::cout << "enabling virtual terminal" << std::endl;
	if (pVT->enableVirtualTerminalMode())
	{
//		std::cout << "virtual terminal enabled." << std::endl;
	}

	int lineOffset = 3;
	int colOffset = 14;


	for (int row = 0; row < pMazeExplorer->getNumberOfRows_Length(); row++)
	{
		for (int col = 0; col < pMazeExplorer->getNumberOfColumns_Width(); col++)
		{
			pVT->moveCursorToXY(col, row);
			cGridMazeExplorer::eCellType cellType = pMazeExplorer->getGridCellType(row, col);
			//char cellChar = pMazeExplorer->getCellTypeAsChar(cellType);
			//pVT->printCharAtXY(cellChar, col, row);
			if (cellType == cGridMazeExplorer::WALL)
			{
				pVT->printDECLineSymbol(cVT100TerminalConsole::CHECKERBOARD_STIPPLE_SHADE);
			}
			else
			{
				printf(" ");
			}
		}
	}

	pVT->restoreTerminalMode();
	// **********************************************************************

	return 0;
	
	std::vector<cGridMazeExplorer::sGridLocation> vecPath;
	//bool bSolvedMaze = pMazeExplorer->CreatePath( vecExits[0], vecExits[1], vecPath );

	int numberOfAttempsTaken = 0;
	//const int MAX_NUMBER_OF_ATTEMPTS = 10000000;
	const int MAX_NUMBER_OF_ATTEMPTS = 0;
	
	// Set the start time...
	deltaTime = pTimer->getElapsedTimeMilliSeconds();
	
	std::cout << "Trying to solve the maze. Max attempts = " << MAX_NUMBER_OF_ATTEMPTS << std::endl;
//	bool bSolvedMaze = pMazeExplorer->SolveMazeRandomPath( vecExits[0], vecExits[1],
//	                                                       vecPath, numberOfAttempsTaken,
//	                                                       MAX_NUMBER_OF_ATTEMPTS);

	bool bSolvedMaze = pMazeExplorer->SolveMazeRecursive(vecExits[0], vecExits[1], vecPath);

	if ( bSolvedMaze )
	{
		std::cout << "This path exits the maze!" << std::endl;
		std::cout << "(took " << numberOfAttempsTaken << " attempts " << std::endl;
		
		deltaTime = pTimer->getElapsedTimeMilliSeconds();
		std::cout << " and " << deltaTime << " ms to solve)" << std::endl;

		for ( cGridMazeExplorer::sGridLocation cell : vecPath )
		{
//			std::cout << "(" << cell.row << ", " << cell.col << ") ";
		}
		
		std::cout << std::endl;
	
		std::cout << "Maze with path:" << std::endl;
		std::cout << pMazeExplorer->getMazeWithPathString(vecPath) << std::endl;


	}
	else
	{
		std::cout << "Made " << numberOfAttempsTaken << " attempts but didn't find an exit." << std::endl;
//		std::cout << "This path hit a dead end." << std::endl;
	}
	
	



//	MessageBox(NULL, "Hi", "Testing", MB_OK);
	
	// Create one of the things to pass to the thread
	sTParams tParams;	
	tParams.Message = "Hey from main()!";
	
	// This is an optional "ID" for the thread. 
	// 	we can get information about the thread from this.
	// (or we can ignore it)
	// A "DWORD" is a "double word" or a 32 bit number 
	// (A "word" = 2 bytes, or 16 bits)
	DWORD threadId = 0;
	
	// The "handle" is a unique number for this thread "object".
	// It's what we can talk to the operating system about when 
	//	referring to this thread. 
	// It doubles as an error return value. 
	//	If it returns NULL (0), then there was a problem.
//	HANDLE tHandle = CreateThread( NULL, 
//	                               0, 				// Stack size
//								   ThreadProc,		// Function thread runs
//								   (void*)&tParams,	// Pointer to the stuff we pass
//								   CREATE_SUSPENDED,// Creation params
//								   &threadId);		// Pointer to threadID variable
//
//	std::cout << "Thread created (threadID = " << threadId << ")" << std::endl;
//	std::cout << "Starting thread then sleeping for 1 second..." << std::endl;
//
//	ResumeThread(tHandle);
	// Suspend this main() thread for 1000 ms (or 1 second)
	// 	to give the thread time to start and run.
	// The thread runs then exits so will return long before
	//	this 1 second has elapsed.
//	Sleep(1000);
//	
//	std::cout << "Thread returned " << tParams.Message << std::endl;



	delete pMazeExplorer;
	delete pTheMaze;

	return 0;
}
