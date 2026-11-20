#include "cGridMazeExplorer.h"

#include <sstream>
#include <iostream>

cGridMazeExplorer::cGridMazeExplorer()
{
	this->m_pRandom = new cMersenneTwister((unsigned long)time(NULL));
}

cGridMazeExplorer::~cGridMazeExplorer()
{
	delete this->m_pRandom;
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

	std::cout << "Top row" << std::endl;
	
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
		
		
bool cGridMazeExplorer::CreatePath(
	sGridLocation start, sGridLocation end, 
	std::vector<sGridLocation> &vecPath)
{
	return this->CreatePath(start.row, start.col, 
	                        end.row, end.col, 
							vecPath);
}

//bool CanIMoveThere(sGridLocation curLocation, cGridMazeExplorer::sGridLocation testLocation)
//{
//	// Is textLocation off the grid?
//	
//	return true;
//}

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

bool cGridMazeExplorer::CreatePath(
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
	std::vector<sGridLocation> &vecPath, 
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
				vecMazeWithPath[row].push_back( '.' );
			}
			else
			{
				// Assume if it's not a floor, it's a wall
				vecMazeWithPath[row].push_back( 'X' );
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
bool cGridMazeExplorer::SolveMaze(
	sGridLocation start, sGridLocation end, 
	std::vector<sGridLocation> &vecPath,
	int &numberOfAttempts, 
	int maxNumberOfAttempts /*=1000*/)
{	
	return this->SolveMaze(start.row, start.col, 
	                       end.row, end.col, 
						   vecPath,
						   numberOfAttempts,
						   maxNumberOfAttempts);
}
				   
bool cGridMazeExplorer::SolveMaze(
	int start_row, int start_col, 
	int end_row, int end_col, 
	std::vector<sGridLocation> &vecPath,
	int &numberOfAttempts, 
	int maxNumberOfAttempts /*=1000*/)
{
	// Are start and end valid?
	if ( this->getGridCellType( start_row, start_col ) != FLOOR ||
	     this->getGridCellType( end_row, end_col ) != FLOOR )
	{
		return false;
	}
	
	numberOfAttempts = 0;
	
	while ( numberOfAttempts < maxNumberOfAttempts )
	{
		numberOfAttempts++;
		vecPath.clear();
		if ( this->CreatePath(start_row, start_col, end_row, end_col, vecPath ) )
		{
			// We solved the maze! 
			return true;
		}
	}

	// DIDN'T solve the maze in the number of attempts
	return false;
}
