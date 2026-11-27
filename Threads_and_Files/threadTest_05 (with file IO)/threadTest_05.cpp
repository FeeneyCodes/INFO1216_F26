#include <iostream>
#include <string>
#include "cMazeMaker.h"
#include "cGridMazeExplorer.h"
#include <sstream>
#include <fstream>
#include <Windows.h>
// For the "Process Memory Counters" (what task manager sees)
#include "cProcessMemoryCounters.h"
#include "cHFTimer.h"
#include "cVT100TerminalConsole.h"

// Written by: Michael Feeney mfeeney (at) fanshawec (dot) ca 
// November 2026


// What we are passing to the thread as a paramater
struct sTParams
{
	// The number we assign the thread
	int threadNumber = -1;
	unsigned long threadRNGSeed = 0;
	cGridMazeExplorer* pMazeExplorer = NULL;
	// The mazes that this explporer will solve
	unsigned int startingMazeSize = 6;	// Maze maker needs at least 5x5
	std::vector<cMazeMaker*> vec_pMazes;
	std::vector<cGridMazeExplorer::sGridLocation> vecExits;
	// The soluton, if there is one
	std::vector<cGridMazeExplorer::sGridLocation> vecPath;
	// How many tries we'll take (maximum)
	unsigned int maxNumberOfAttemptsToTry = 0;
	// How many tries the solver actually did:
	unsigned int numberOfAttemptsTaken = 0;
	// To indicate that the thread had finished
	bool bThreadIsDone = false;
	bool bFoundAPath = false;
	// This is set to true by main to let the threds know they need to exit
	bool bStopWorkingAndExit = false;
};

// The thread function
DWORD WINAPI ThreadProc( LPVOID lpParams)
{
	sTParams* pTheParams = (sTParams*)lpParams;
	
	// Find and save the exits
	std::vector<cGridMazeExplorer::sGridLocation> vecExits;
	pTheParams->pMazeExplorer->FindExits(vecExits);

	// Keep searching until we get through the maze
	pTheParams->numberOfAttemptsTaken = 0;
	pTheParams->bFoundAPath = false;
	for (unsigned int count = 0; count < pTheParams->maxNumberOfAttemptsToTry; count++)
	{
		if (pTheParams->pMazeExplorer->SolveMazeDeadEndFilling(vecExits[0], vecExits[1], pTheParams->numberOfAttemptsTaken, pTheParams->vecPath, true))
		{
			// Found an exit
			pTheParams->bFoundAPath = true;
			break;
		}
	}//for (int count...

	// All done my work
	pTheParams->bThreadIsDone = true;

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
		mazeFileName.clear();
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
	

	// This "explorer" is just for the main, to get info about the maze
	// (Each thread will be given its own 'explorer' later)
	cGridMazeExplorer* pMazeExplorer = new cGridMazeExplorer(RNGseed);
	
	pMazeExplorer->SetMaze(mazeAsString, 'X', '.');

	
	cVT100TerminalConsole* pVT = new cVT100TerminalConsole();
	pVT->enableVirtualTerminalMode();

	pVT->printBox(0, 120, 0, 40);



	// Create the number of threads to try to solve this maze
	// This looks strange, but it is creating an array on the heap
	//	instead of the stack.
	//
	// These are used just like "regular" arrays
	//	pTParams[3] is the 4th element, etc.
	// (Like arrays, they start at 0 up to numThreads-1)
	// 
	sTParams* pTParams = new sTParams[numThreads];
	DWORD* pThreadIDs = new DWORD[numThreads];
	HANDLE* pTHandles = new HANDLE[numThreads];

	pVT->moveCursorToXY(1, 1);
	std::cout << "Creating " << numThreads << " threads...";

	// The random number generator is set up so that if we give it the same 
	//	seed, they will all pick the same sequence of numbers. 
	// Here, we are going to creat a RNG with the passed seed, then 
	//	pick seeds for each Maze Explorer based on that common RNG
	cMersenneTwister* pMainRNG = new cMersenneTwister(RNGseed);

	// Make a maze explorer for each of our threads
	for (unsigned int index = 0; index < numThreads; index++)
	{
		pTParams[index].threadRNGSeed = pMainRNG->getNextRand();

		pTParams[index].threadNumber = index;

		cGridMazeExplorer* pMazeExplorer = new cGridMazeExplorer(pTParams[index].threadRNGSeed);
		pMazeExplorer->SetMaze(mazeAsString, 'X', '.');

		pTParams[index].maxNumberOfAttemptsToTry = 10000;

		// Pass it to the parameters we are giving to the thread
		pTParams[index].pMazeExplorer = pMazeExplorer;

		pTParams[index].startingMazeSize = 6;

		// Create a suspended thread
		pTHandles[index] = CreateThread(NULL,	// Security attribs (user or kernel mode, etc.)
		                                0, 		// Stack size (0 = default stack size)
		                                ThreadProc,					// Function thread runs
		                                (void*)&(pTParams[index]),	// Pointer to the stuff we pass
		                                CREATE_SUSPENDED,			// Creation params
		                                &(pThreadIDs[index]));		// Pointer to threadID variable
	}
	std::cout << "done." << std::endl;
	
	std::cout << "Starting the " << numThreads << " threads." << std::endl;
	std::cout.flush();

	// Set the start time...
	deltaTime = pTimer->getElapsedTimeMilliSeconds();

	for (unsigned int index = 0; index < numThreads; index++)
	{
		ResumeThread(pTHandles[index]);
	}

	// Now we check through the params we passed seeing if any thread has solved it
	bool bPathFound = false;
	bool bThreadsStillRunning = true;
	//  Run if:
	// - Threads are still running
	//   AND
	// - One thread hasn't found a path
	while (bThreadsStillRunning && !bPathFound)
	{
		// Assume they are all done
		bThreadsStillRunning = false;

		for (unsigned int index = 0; index < numThreads; index++)
		{
			if (pTParams[index].bFoundAPath)
			{
				// At least one thread has found a path
				bPathFound = true;
			}
			if (!pTParams[index].bThreadIsDone)
			{
				// At least one thread is still running
				bThreadsStillRunning = true;
			}
			// This releases control over this main thread
			// (You could also call Sleep(1) to wait approx 1 ms)
			Sleep(0);
		}
	}

	// At this point, either:
	// - At least one thread found a path 
	// OR
	// - ALL the threads exited NOT finding a path

	deltaTime = pTimer->getElapsedTimeMicroSeconds();
	std::string elapsedTimeString = pTimer->getElapsedTimeAppropriateDurationString(deltaTime);

	// Anyone find a path?
	int threadIDThatFoundAPath = -1;
	for (unsigned int index = 0; index < numThreads; index++)
	{
		if (pTParams[index].bFoundAPath)
		{
			// Yup
			threadIDThatFoundAPath = index;
			break;
		}
	}

	if (threadIDThatFoundAPath < 0)
	{
		// Nope. 
		std::cout << "ALl the threads exited and none found a path." << std::endl;
		return -1;
	}

	// At this point, threadIDThatFoundAPath thread found a path through the maze
	std::cout << "Thread # " << threadIDThatFoundAPath << " found a path";
	std::cout << " in " << elapsedTimeString << " (" << deltaTime << " us)";
	std::cout << " taking " << pTParams[threadIDThatFoundAPath].numberOfAttemptsTaken << " tries." << std::endl;


	std::cout << "Do you want to see the solution? (Y/N)" << std::endl;
	bool bShowMazeAndSolution = false;
	std::string answer;
	std::cin >> answer;
	if (answer.length() > 0)
	{
		if (answer[0] == 'Y' || answer[0] == 'y')
		{
			bShowMazeAndSolution = true;
		}
	}

	if (!bShowMazeAndSolution)
	{
		return 0;
	}

	// Show the maze and solution
	// **********************************************************************
	// Draw maze...
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
	}//for (int row...
	// **********************************************************************
	// Draw path through maze...
	for (cGridMazeExplorer::sGridLocation pathCell : pTParams[threadIDThatFoundAPath].vecPath)
	{
		pVT->moveCursorToXY(pathCell.col, pathCell.row);
		pVT->setTextForeground(cVT100TerminalConsole::GREEN, false);
		pVT->printBlackHappyFace();
		pVT->setTextForeground_Default();
	}
	// put terminal cursor at bottom of maze (so later couts don't write over maze)
	pVT->moveCursorToXY(0, pMazeExplorer->getNumberOfRows_Length() + 2);

	// **********************************************************************

	cProcessMemoryCounters* pPMC = new cProcessMemoryCounters();
	std::ofstream PMCInfoFile("PMCInfo.txt");
	PMCInfoFile << pPMC->getCounterSummaryString() << std::endl;
	PMCInfoFile.close();

	// TODO: Delete the things created by the threads
	delete pMazeExplorer;
	delete pTheMaze;

	return 0;
}
