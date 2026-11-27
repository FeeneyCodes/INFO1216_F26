#include "cGridMazeExplorer.h"

#include <sstream>
#include <iostream>
#include <fstream>

#include "cVT100TerminalConsole.h"

cGridMazeExplorer::cGridMazeExplorer(unsigned long seed)
{
	this->m_pRandom = new cMersenneTwister(seed);
	// 
	this->m_UniqueID = this->m_s_NextUniqueID + cGridMazeExplorer::UNIQUE_ID_INCREMENT;	

	this->setLogFileName(this->m_LogFileName);
}

cGridMazeExplorer::~cGridMazeExplorer()
{
	delete this->m_pRandom;
}

unsigned int m_UniqueID = 0;
//static 
unsigned int cGridMazeExplorer::m_s_NextUniqueID = 1;

unsigned int cGridMazeExplorer::getUniqueID(void)
{
	return this->m_UniqueID;
}

std::string cGridMazeExplorer::getLastError(void)
{
	if (this->m_vecErrorStrings.empty())
	{	// s'all good
		return "OK";
	}
	
	std::stringstream ssErrors;
	for (std::string errorString : this->m_vecErrorStrings)
	{
		ssErrors << errorString << std::endl;
	}

	return ssErrors.str();
}

bool cGridMazeExplorer::SetMaze(std::string mazeString, 
                                char wallCharacter, 	// = 'X'
								char floorCharacter)	// = '.'
{
	std::vector<std::string> vecMazeRows;
	
	std::stringstream ssWholeMaze;
	ssWholeMaze << mazeString;
	
	std::string mazeRow = "";
	while ( std::getline(ssWholeMaze, mazeRow) )
	{
		vecMazeRows.push_back(mazeRow);
	}
	
	return this->SetMaze(vecMazeRows, wallCharacter, floorCharacter);
	
	return true;
}

bool cGridMazeExplorer::SetMaze(std::vector<std::string> vecMazeRows, 
                                char wallCharacter,		// = 'X'
								char floorCharacter)	// = '.'
{
	// Empty vector
	if ( vecMazeRows.empty() )
	{
		return false;
	}
	
	// Check to see if every row has the same number of characters
	int numColumns = (int)vecMazeRows[0].length();
	
	for ( int row = 1; row < vecMazeRows.size(); row++ )
	{
		if ( vecMazeRows[row].length() != numColumns )
		{
			return false;
		}
	}

	this->m_NumColumns_Width = numColumns;
	this->m_NumRows_Height = (int)vecMazeRows.size();
	
	this->m_FloorTileChar = floorCharacter;
	this->m_WallTileChar = wallCharacter;


	// Prepare undefined maze
	for ( int row = 0; row < vecMazeRows.size(); row++ )
	{
		std::vector<eCellType> tempRow;
		tempRow.reserve(this->m_NumColumns_Width);
		
		for ( int col = 0; col < this->m_NumColumns_Width; col++ )
		{
			tempRow.push_back(cGridMazeExplorer::UNKNOWN_TYPE);
		}//for ( int col
		
		this->m_vecMaze.push_back(tempRow);
	}//for ( int row


	for ( int row = 0; row < vecMazeRows.size(); row++ )
	{
		for ( int col = 0; col < this->m_NumColumns_Width; col++ )
		{
			if ( (char)vecMazeRows[row][col] == wallCharacter )
			{
				this->m_vecMaze[row][col] = WALL;
			}
			else if ( (char)vecMazeRows[row][col] == floorCharacter )
			{
				this->m_vecMaze[row][col] = FLOOR;
			}
			// Else it's UNKNOWN_TYPE (set above)
		}//for ( int col
	}//for ( int row

	
	return true;
}

cGridMazeExplorer::eCellType cGridMazeExplorer::getGridCellType(unsigned int row, unsigned int col)
{
	if ( row >= (unsigned int)this->m_NumRows_Height )
	{
		return INVALID_CELL;
	}
	if ( col >= (unsigned int)this->m_NumColumns_Width )
	{
		return INVALID_CELL;
	}
	
	return this->m_vecMaze[row][col];	
}

cGridMazeExplorer::eCellType cGridMazeExplorer::getGridCellType(sGridLocation cell)
{
	return this->getGridCellType(cell.row, cell.col);
}

bool cGridMazeExplorer::IsMazeValid(void)
{
	if ( this->m_NumRows_Height < 0 || this->m_NumColumns_Width < 0 )
	{
		return false;
	}
	if ( this->m_vecMaze.empty() )
	{
		return false;
	}
	return true;	
}

int cGridMazeExplorer::getNumberOfRows_Length(void)
{
	return this->m_NumRows_Height;
}

int cGridMazeExplorer::getNumberOfColumns_Width(void)
{
	return this->m_NumColumns_Width;
}

char cGridMazeExplorer::getCellTypeAsChar(eCellType cell)
{
	switch (cell)
	{
		case FLOOR:
			return this->m_FloorTileChar;
			break;
		case WALL:
			return this->m_WallTileChar;
			break;
		case UNKNOWN_TYPE:
			return '?';
			break;
		case INVALID_CELL:		// If a cell is outside of the range
			return '^';
			break;
		case PATH_CELL:
			return 'o';
			break;
	}
	return '+';
}


bool cGridMazeExplorer::FindExits(std::vector<sGridLocation> &vecExits)
{
	if ( ! this->IsMazeValid() )
	{
		return false;
	}
	
	vecExits.clear();

	// Go along top and bottom rows
	for ( int col = 0; col < this->m_NumColumns_Width; col++ )
	{
		// Top row
		if (this->m_vecMaze[0][col] == FLOOR )
		{
			// Found one
			sGridLocation anExit(0, col);
			vecExits.push_back(anExit);
		}
		
		// Bottom row
		if (this->m_vecMaze[this->m_NumRows_Height - 1][col] == FLOOR )
		{
			// Found one
			sGridLocation anExit(this->m_NumRows_Height - 1, col);
			vecExits.push_back(anExit);
		}
	}
	
	// Go along left and right columns
	for ( int row = 0; row < this->m_NumRows_Height; row++ )
	{
		// Left column
		if ( this->m_vecMaze[row][0] == FLOOR )
		{
			sGridLocation anExit(row, 0);
			vecExits.push_back(anExit);
		}
		// right column
		if ( this->m_vecMaze[row][this->m_NumColumns_Width - 1] == FLOOR )
		{
			sGridLocation anExit(row, this->m_NumColumns_Width - 1);
			vecExits.push_back(anExit);
		}		
	}
	
	return true;
}


void cGridMazeExplorer::getDirectionCharacters(
	std::vector<eDirections> vecPath, 
	std::vector<char> &vecPathAsChars,
	char leftChar /*='L'*/, char rightChar /*='R'*/, 
	char upChar /*='U'*/, char downChar /*='D'*/)
{
	vecPathAsChars.clear();
	
	for ( eDirections theDir : vecPath )
	{
		switch (theDir)
		{
			case UP:
				vecPathAsChars.push_back(upChar);
				break;
			case DOWN:
				vecPathAsChars.push_back(downChar);
				break;
			case LEFT:
				vecPathAsChars.push_back(leftChar);
				break;
			case RIGHT:
				vecPathAsChars.push_back(rightChar);
				break;
			case UNKNONW_DIRECTION:
				vecPathAsChars.push_back('?');
				break;
			default:
				vecPathAsChars.push_back('*');
				break;
		}
	}
	
	return;
}
		


// Returns false if there's no "next valid cell"
// Doesn't pick from previous cell (where it came from)
bool cGridMazeExplorer::m_PickNextValidCell(std::vector<cGridMazeExplorer::sGridLocation> &vecPathSoFar)
{
	// There should at least be a starting cell...
	if ( vecPathSoFar.size() == 0 )
	{
		return false;
	}
	
	// Where are we? (the last cell)
	sGridLocation curCell = vecPathSoFar[vecPathSoFar.size() - 1];
	
	// Find the previous cell.
	// We start with the current (in case this is the only one)
	sGridLocation previousCell = curCell;
	bool bThereIsAPreviousCell = false;
	if ( vecPathSoFar.size() > 1 )
	{
		previousCell = vecPathSoFar[vecPathSoFar.size() - 2];
		bThereIsAPreviousCell = true;
	}
	
	// Add the four (untested) possible future locations
	std::vector<sGridLocation> vecAdjacent;
	
	vecAdjacent.push_back( sGridLocation( curCell.row - 1, curCell.col ) );	// Up one
	vecAdjacent.push_back( sGridLocation( curCell.row + 1, curCell.col ) );	// Down one
	vecAdjacent.push_back( sGridLocation( curCell.row, curCell.col - 1 ) ); 	// Left one
	vecAdjacent.push_back( sGridLocation( curCell.row, curCell.col + 1 ) ); 	// Rigth one
	
	// Go through these possible ones and add them if they are OK
	// (i.e. on the grid and not the previous location)
	std::vector<sGridLocation> vecAdjacentValid;
	
	for ( sGridLocation cell : vecAdjacent )
	{
		if ( this->getGridCellType(cell) == FLOOR )
		{
			// Was it the cell we just came from
			if (bThereIsAPreviousCell)
			{
				if ( cell.row != previousCell.row || 
				     cell.col != previousCell.col )
				{
					// No, it's not the previous cell
					vecAdjacentValid.push_back(cell);
				}
			}
			else
			{
				// Edge case. Any floor cell is OK
				vecAdjacentValid.push_back(cell);
			}
		}
	}
	
//	std::cout << "Found " << vecAdjacentValid.size() << " valid cells" << std::endl;
//	for ( sGridLocation cell : vecAdjacentValid )
//	{
//		std::cout << cell.row << ", " << cell.col << std::endl;
//	}
	
	// Did we find any valid cells to move to
	if ( vecAdjacentValid.size() == 0 )
	{
		// Nope.
		return false;
	}
	
	// Yes, so pick from these
	unsigned long randIndex = this->m_pRandom->getRandBetween(0, (unsigned int)vecAdjacentValid.size() );

	vecPathSoFar.push_back( vecAdjacentValid[randIndex] );
	
	
	return true;
}	

// Smae as above, but is passed the maze
bool cGridMazeExplorer::m_PickNextValidCell(std::vector<std::vector<eCellType>> &vecTempMaze, std::vector<sGridLocation>& vecPathSoFar)
{
	// There should at least be a starting cell...
	if (vecPathSoFar.size() == 0)
	{
		return false;
	}

	// Where are we? (the last cell)
	sGridLocation curCell = vecPathSoFar[vecPathSoFar.size() - 1];

	// Find the previous cell.
	// We start with the current (in case this is the only one)
	sGridLocation previousCell = curCell;
	bool bThereIsAPreviousCell = false;
	if (vecPathSoFar.size() > 1)
	{
		previousCell = vecPathSoFar[vecPathSoFar.size() - 2];
		bThereIsAPreviousCell = true;
	}

	// Add the four (untested) possible future locations
	std::vector<sGridLocation> vecAdjacent;

	vecAdjacent.push_back(sGridLocation(curCell.row - 1, curCell.col));	// Up one
	vecAdjacent.push_back(sGridLocation(curCell.row + 1, curCell.col));	// Down one
	vecAdjacent.push_back(sGridLocation(curCell.row, curCell.col - 1)); 	// Left one
	vecAdjacent.push_back(sGridLocation(curCell.row, curCell.col + 1)); 	// Rigth one

	// Go through these possible ones and add them if they are OK
	// (i.e. on the grid and not the previous location)
	std::vector<sGridLocation> vecAdjacentValid;

	for (sGridLocation cell : vecAdjacent)
	{
		if (cell.row < 0 || cell.row >= vecTempMaze.size())
		{
			// Not in maze
			continue;
		}
		if (cell.col < 0 || cell.col >= vecTempMaze[cell.row].size())
		{
			// Not in maze
			continue;
		}

		// Cell IS in maze. Is it a floor (a possible move location)?
		if (vecTempMaze[cell.row][cell.col] == FLOOR)
		{
			// Was it the cell we just came from
			if (bThereIsAPreviousCell)
			{
				if (cell.row != previousCell.row ||
					cell.col != previousCell.col)
				{
					// No, it's not the previous cell
					vecAdjacentValid.push_back(cell);
				}
			}
			else
			{
				// Edge case. Any floor cell is OK
				vecAdjacentValid.push_back(cell);
			}
		}
	}

		// Did we find any valid cells to move to
	if (vecAdjacentValid.size() == 0)
	{
		// Nope.
		return false;
	}

	// Yes, so pick from these
	unsigned long randIndex = this->m_pRandom->getRandBetween(0, (unsigned int)vecAdjacentValid.size());

	vecPathSoFar.push_back(vecAdjacentValid[randIndex]);


	return true;
}

bool cGridMazeExplorer::CreateRandomPath(
	sGridLocation start, sGridLocation end,
	std::vector<sGridLocation>& vecPath)
{
	return this->CreateRandomPath(start.row, start.col,
		end.row, end.col,
		vecPath);
}

bool cGridMazeExplorer::CreateRandomPath(
	int start_row, int start_col, 
	int end_row, int end_col, 
	std::vector<sGridLocation> &vecPath)
{
	// Are start and end valid?
	if ( this->getGridCellType( start_row, start_col ) != FLOOR ||
	     this->getGridCellType( end_row, end_col ) != FLOOR )
	{
		return false;
	}
	
	vecPath.clear();
	vecPath.push_back(sGridLocation(start_row, start_col));
	
	this->m_PickNextValidCell(vecPath);
	
	while ( this->m_PickNextValidCell(vecPath) )
	{
		// Keep picking
	}	
	
	// Did we end up at the exit?
	sGridLocation lastCell = vecPath[vecPath.size() - 1];
	if ( lastCell.row == end_row && lastCell.col == end_col )
	{
		// Yes we did 
		return true;
	}
	
	// Didn't find a path
	return false;
}

// For printing the maze WITH the path
std::string cGridMazeExplorer::getMazeWithPathString(
	std::vector<sGridLocation>& vecPath,
	char wallCharacter /*='X'*/,
	char floorCharacter /*=' '*/,
	char pathCharacter /*='@'*/)
{
	// Make a copy of the maze
	std::vector<std::vector<char>> vecMazeWithPath;
	
	vecMazeWithPath.reserve(this->m_NumRows_Height);
	
	for ( int row = 0; row < this->m_NumRows_Height; row++ )
	{
		// Add empty row
		vecMazeWithPath.push_back( std::vector<char>() );
		vecMazeWithPath[row].reserve(this->m_NumColumns_Width);
		for ( int col = 0; col < this->m_NumColumns_Width; col++ )
		{
			if ( this->m_vecMaze[row][col] == FLOOR )
			{
				vecMazeWithPath[row].push_back(floorCharacter);
			}
			else
			{
				// Assume if it's not a floor, it's a wall
				vecMazeWithPath[row].push_back(wallCharacter);
			}
		}
	}
	
	// Add the path characters
	for ( sGridLocation cell : vecPath )
	{
		vecMazeWithPath[cell.row][cell.col] = pathCharacter;
	}
	
	// Mark the 1st and last ('S' for start, 'E' for end)
	vecMazeWithPath[vecPath[0].row][vecPath[0].col] = 'S';
	vecMazeWithPath[vecPath[vecPath.size() - 1].row][vecPath[vecPath.size() - 1].col] = 'E';
	
	std::stringstream ssMazeWithPath;
	
	for ( int row = 0; row < this->m_NumRows_Height; row++ )
	{
		for ( int col = 0; col < this->m_NumColumns_Width; col++ )
		{
			ssMazeWithPath << vecMazeWithPath[row][col];
		}
		ssMazeWithPath << std::endl;
	}
	
	return ssMazeWithPath.str();
}


// Will keep calling CreatePath until it gets from start to end
bool cGridMazeExplorer::SolveMazeRandomPath(
	sGridLocation start, sGridLocation end, 
	std::vector<sGridLocation> &vecPath,
	unsigned int &numberOfAttempts, 
	unsigned int maxNumberOfAttempts /*=1000*/)
{	
	return this->SolveMazeRandomPath(start.row, start.col,
	                       end.row, end.col, 
						   vecPath,
						   numberOfAttempts,
						   maxNumberOfAttempts);
}
				   
bool cGridMazeExplorer::SolveMazeRandomPath(
	int start_row, int start_col, 
	int end_row, int end_col, 
	std::vector<sGridLocation> &vecPath,
	unsigned int &numberOfAttempts, 
	unsigned int maxNumberOfAttempts /*=1000*/)
{
	// Are start and end valid?
	if ( this->getGridCellType( start_row, start_col ) != FLOOR ||
	     this->getGridCellType( end_row, end_col ) != FLOOR )
	{
		return false;
	}
	
	numberOfAttempts = 0;
	bool bKeepSearching = true;
	
	while ( bKeepSearching )
	{
		numberOfAttempts++;
		vecPath.clear();
		if ( this->CreateRandomPath(start_row, start_col, end_row, end_col, vecPath ) )
		{
			// We solved the maze! 
			return true;
		}
		// Exit at a certain num attempts
		if (maxNumberOfAttempts != 0)
		{
			if (numberOfAttempts >= maxNumberOfAttempts)
			{
				bKeepSearching = false;
			}
		}
	}//while ( bKeepSearching )

	// DIDN'T solve the maze in the number of attempts
	return false;
}

bool cGridMazeExplorer::SolveMazeDeadEndFilling(
	sGridLocation start, sGridLocation end,
	unsigned int& numberOfDeadEndsFilled,
	std::vector<sGridLocation>& vecPath,
	bool bLogProgressToFile /*=false*/)
{
	return this->SolveMazeDeadEndFilling(start.row, start.col, end.row, end.col, numberOfDeadEndsFilled, vecPath, bLogProgressToFile);
}


bool cGridMazeExplorer::SolveMazeDeadEndFilling(
	int start_row, int start_col, int end_row, int end_col,
	unsigned int& numberOfDeadEndsFilled,
	std::vector<sGridLocation>& vecPath,
	bool bLogProgressToFile /*=false*/)
{
	// Are start and end valid?
	if (this->getGridCellType(start_row, start_col) != FLOOR ||
		this->getGridCellType(end_row, end_col) != FLOOR)
	{
		return false;
	}

	std::ofstream theLogFile;

	// Are we logging to a file?
	if (bLogProgressToFile)
	{
		// Open log file FOR APPENDING onto any existing one
		theLogFile.open(this->getLogFileName().c_str(), std::ios::app);
		if ( ! theLogFile.is_open() )
		{
			// Can't open the log file
			// Don't return false, but set error
			this->m_vecErrorStrings.push_back("Can't open progress log file: " + this->getLogFileName());
			bLogProgressToFile = false;
		}
	}

	if (bLogProgressToFile)
	{
		theLogFile << "Making copy of maze...";
	}

	// Make a copy of the maze so we know what's a wall and what's not
	std::vector<std::vector<eCellType>> vecMazeAreWalls_COPY;
	vecMazeAreWalls_COPY.reserve(this->m_NumRows_Height);

	for (int rowIndex = 0; rowIndex != this->m_NumRows_Height; rowIndex++)
	{
		// Add an empty row
		vecMazeAreWalls_COPY.push_back(std::vector<eCellType>());
		vecMazeAreWalls_COPY[rowIndex].reserve(this->m_NumColumns_Width);

		for (int colIndex = 0; colIndex != this->m_NumColumns_Width; colIndex++)
		{
			if (this->m_vecMaze[rowIndex][colIndex] == WALL)
			{
				vecMazeAreWalls_COPY[rowIndex].push_back(this->m_vecMaze[rowIndex][colIndex]);
			}
			else
			{
				vecMazeAreWalls_COPY[rowIndex].push_back(this->m_vecMaze[rowIndex][colIndex]);
			}
		}
	}//for (int rowIndex

	if (bLogProgressToFile)
	{
		theLogFile << "done." << std::endl;
	}


	// Keep scaning through the maze looking for dead ends. 
	// NOTE: Since this scans the maze over and over again, 
	//       it takes exponentially more time with maze size.
	// 
	// If you wanted this to take linear time, you could 
	//  make a list of all the 'floor' files in the maze, 
	//  then scan through just this list (of floor tiles)
	//  removing them as you decide they are dead ends.
	//


	bool bKeepScanningForDeadEnds = true;
	numberOfDeadEndsFilled = 0;
	while (bKeepScanningForDeadEnds)
	{
		// Assume we've found all the dead ends (and filled them)
		bKeepScanningForDeadEnds = false;

		// These are cells that have only one exit
		for (int rowIndex = 0; rowIndex != this->m_NumRows_Height; rowIndex++)
		{
			for (int colIndex = 0; colIndex != this->m_NumColumns_Width; colIndex++)
			{
				// Check if this is a start or end location
				// (those should be ignored as they are technically "dead ends")
				if ((rowIndex == start_row) && (colIndex == start_col))
				{
					// It's the start cell
					continue;
				}
				if ((rowIndex == end_row) && (colIndex == end_col))
				{
					// It's the end cell
					continue;
				}

				if (vecMazeAreWalls_COPY[rowIndex][colIndex] == FLOOR)
				{
					// It's a floor. Is it a dead end? 

					std::vector<sGridLocation> vecAdjacent;

					vecAdjacent.push_back(sGridLocation(rowIndex - 1, colIndex));	// Up one
					vecAdjacent.push_back(sGridLocation(rowIndex + 1, colIndex));	// Down one
					vecAdjacent.push_back(sGridLocation(rowIndex, colIndex - 1)); // Left one
					vecAdjacent.push_back(sGridLocation(rowIndex, colIndex + 1)); // Rigth one

					// Check to see if these are inside the maze
					std::vector<sGridLocation> vecAdjacentValid;
					for (sGridLocation cell : vecAdjacent)
					{
						if (this->m_bIsCellInMaze(cell))
						{
							// Is this adjacent cell still a floor (hasn't been filled in)?
							if (vecMazeAreWalls_COPY[cell.row][cell.col] == FLOOR)
							{
								vecAdjacentValid.push_back(cell);
							}
						}
					}

					// If there is only one adjacent cell, then it's a dead end. 
					if (vecAdjacentValid.size() == 1)
					{
						// It IS a dead end, so fill it (i,e. make it a wall)
						vecMazeAreWalls_COPY[rowIndex][colIndex] = WALL;
						bKeepScanningForDeadEnds = true;
						numberOfDeadEndsFilled++;

						if (bLogProgressToFile)
						{
							theLogFile << "(" << rowIndex << ", " << colIndex << ") is a dead end." << std::endl;
						}
						//						// ***************************************************
						//
						//						cVT100TerminalConsole* pVT = new cVT100TerminalConsole();
						//						pVT->enableVirtualTerminalMode();
						//
						//						for (int rowIndex = 0; rowIndex < this->m_NumRows_Height; rowIndex++)
						//						{
						//							for (int colIndex = 0; colIndex < this->m_NumColumns_Width; colIndex++)
						//							{
						//								if (!vecMazeAreWalls[rowIndex][colIndex])
						//								{
						//									pVT->moveCursorToXY(colIndex, rowIndex);
						//									pVT->setTextForeground(cVT100TerminalConsole::GREEN, false);
						//									pVT->printBlackHappyFace();
						//									pVT->setTextForeground_Default();
						//								}
						//								else
						//								{
						//									pVT->moveCursorToXY(colIndex, rowIndex);
						//									pVT->setTextForeground(cVT100TerminalConsole::CYAN, false);
						//									pVT->printDECLineSymbol(cVT100TerminalConsole::CHECKERBOARD_STIPPLE_SHADE);
						//									pVT->setTextForeground_Default();
						//								}
						//							}
						//						}//for (int rowIndex
						//
						//						pVT->moveCursorToXY(colIndex, rowIndex);
						//						pVT->setTextForeground(cVT100TerminalConsole::RED, true);
						//						pVT->printBlackHappyFace();
						//						pVT->setTextForeground_Default();
						//
						//
						//						pVT->restoreTerminalMode();
						//						delete pVT;
						//
						//						std::wstringstream ssCount;
						//						ssCount << numberOfDeadEndsFilled;
						//						MessageBox(NULL, ssCount.str().c_str(), L"OK", MB_OK);
						//
						//						// ***************************************************
					}

				}//if (!vecMazeAreWalls[rowIndex][colIndex])
			}//for (int colIndex
		}//for (int rowIndex
	}//while (bKeepScanningForDeadEnds)

	if (bLogProgressToFile)
	{
		theLogFile << "All dead end cells have been found." << std::endl;
	}

	// There are no no "dead ends", so either there is at least one 
	//	solution or there isn't one.
	vecPath.clear();

	// Add starting point
	vecPath.push_back(sGridLocation(start_row, start_col));

	// Keep picking if there's a valid next cell to move to...
	while (this->m_PickNextValidCell(vecMazeAreWalls_COPY, vecPath))
	{
		// continue
	};

	// If the last item on the vector is the exit, we found a way out
	sGridLocation lastCell = vecPath[vecPath.size() - 1];

	if (lastCell.row == end_row && lastCell.col == end_col)
	{
		// Found a way out

		if (bLogProgressToFile)
		{
			theLogFile << "There is a path out with " << vecPath.size() << " steps:" << std::endl;
			for (unsigned int index = 0; index < (unsigned int)vecPath.size(); index++ )
			{
				theLogFile << "\t" << index << ": " << "(" << vecPath[index].row << ", " << vecPath[index].col << ")" << std::endl;
			}
			theLogFile << "And we're out!" << std::endl;

			theLogFile.close();
		}

		return true;
	}

	// else there wasn't a way thorugh
	if (bLogProgressToFile)
	{
		theLogFile << "There wasn't a way through the maze." << std::endl;
		theLogFile.close();
	}
	return false;
}

// (Used by: SolveMazeDeadEndFilling)
bool cGridMazeExplorer::m_bIsCellADeadEnd(sGridLocation cellRowCol)
{
	if (!this->m_bIsCellInMaze(cellRowCol))
	{
		return false;
	}
	// Is this a wall cell
	if (this->getGridCellType(cellRowCol) == WALL)
	{
		return false;
	}

	// Add the four (untested) possible future locations
	std::vector<sGridLocation> vecAdjacent;

	vecAdjacent.push_back(sGridLocation(cellRowCol.row - 1, cellRowCol.col));	// Up one
	vecAdjacent.push_back(sGridLocation(cellRowCol.row + 1, cellRowCol.col));	// Down one
	vecAdjacent.push_back(sGridLocation(cellRowCol.row, cellRowCol.col - 1)); // Left one
	vecAdjacent.push_back(sGridLocation(cellRowCol.row, cellRowCol.col + 1)); // Rigth one

	// Check to see if these are inside the maze
	std::vector<sGridLocation> vecAdjacentValid;
	for (sGridLocation cell : vecAdjacent)
	{
		if (this->m_bIsCellInMaze(cell))
		{
			vecAdjacentValid.push_back(cell);
		}
	}

	// If there is only one adjacent cell, then it's a dead end. 
	if (vecAdjacentValid.size() == 1)
	{
		return true;
	}
	// It's not a dead end
	return false;
}


// (Used by: SolveMazeRecursive and SolveMazeDeadEndFilling)
bool cGridMazeExplorer::m_bIsCellInMaze(sGridLocation cellRowCol)
{
	if (cellRowCol.row < 0 || cellRowCol.col < 0)
	{
		return false;
	}
	if (cellRowCol.row >= this->m_NumRows_Height || cellRowCol.col >= this->m_NumColumns_Width)
	{
		return false;
	}
	// Is inside the maze
	return true;
}

// (Used by: SolveMazeRecursive)
// Given a row and col, will check the next adjacent UNvisited cell, based on the 
//	m_vecMazeVisitState vector (a copy of the maze with bool = true if visited
bool cGridMazeExplorer::m_bFindValidNextCell(int& row, int& col, std::vector<std::vector<bool>> &vecMazeCellVisited)
{
	// Try Up, Right, Down, then Left and return 1st one that's valid and not visited yet

	// Add the four (untested) possible future locations
	std::vector<sGridLocation> vecAdjacent;

	vecAdjacent.push_back(sGridLocation(row - 1, col));	// Up one
	vecAdjacent.push_back(sGridLocation(row + 1, col));	// Down one
	vecAdjacent.push_back(sGridLocation(row, col - 1)); // Left one
	vecAdjacent.push_back(sGridLocation(row, col + 1)); // Rigth one

	// Check to see if these are inside the maze
	std::vector<sGridLocation> vecAdjacentValid;
	for (sGridLocation cell : vecAdjacent)
	{
		if (this->m_bIsCellInMaze(cell))
		{
			// Have we visited this cell already?
			if ( ! vecMazeCellVisited[cell.row][cell.col] )
			{
				vecAdjacentValid.push_back(cell);
			}
		}
	}

	// At this point, vecAdjacentValid has all the cells we COULD move to.

	if (vecAdjacentValid.empty())
	{
		// No valid 'next' cell to move to
		return false;
	}

	// Pick the first one in the vector
	row = vecAdjacentValid[0].row;
	col = vecAdjacentValid[0].col;
	
	return true;
}

void cGridMazeExplorer::setLogFileName(std::string fileName)
{
	std::stringstream ssLogFileName;
	ssLogFileName << fileName << this->getUniqueID() << ".log";

	this->m_LogFileName = ssLogFileName.str();
	return;
}

std::string cGridMazeExplorer::getLogFileName(void)
{
	return this->m_LogFileName;
}

