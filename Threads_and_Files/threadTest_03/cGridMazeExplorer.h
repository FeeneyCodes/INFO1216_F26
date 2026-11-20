#ifndef _cGridMazeTraveller_HG_
#define _cGridMazeTraveller_HG_

#include <string>
#include <vector>
#include "cMersenneTwister.h"

// This takes a maze like the one below and does
//	various things with it (finding exits, etc.):
//
// The maze needs to be a square grid with walls
//	and non-walls (corridors).
//
// Top-Left grid location is 0,0
//
//	XXXXXXXXXXXXXXXXXXXXX
//	X.........X.........X
//	X.XXXXXXX.XXX.XXXXXXX
//	X.X.....X...X.X.....X
//	X.X.XXX.XXX.X.X.XXX.X
//	X.....X.X.....X.X.X.X
//	X.XXXXX.X.XXXXX.X.X.X
//	X...X...X.X.....X.X.X
//	XXX.X.XXXXX.XXXXX.X.X
//	X.X.X.....X.....X.X..
//	X.X.XXXXX.XXXXX.X.XXX
//	X.......X...........X
//	XXX.XXXXXXXXXXXXXXXXX
//

class cGridMazeExplorer
{
public:
	cGridMazeExplorer(unsigned long seed = 0);
	~cGridMazeExplorer();
	
	// Takes entire maze like above, as one string with CR-LF at end
	// Assumes anything that's NOT a wall character is a floor (non-wall)
	// Anything not floor or wall is an UNKNOWN_TYPE
	bool SetMaze(std::string mazeString, char wallCharacter = 'X', char floorCharacter = '.');
	
	// Takes maze as a vector of rows (each a string)
	// Assumes anything that's NOT a wall character is a floor (non-wall)
	// Anything not floor or wall is an UNKNOWN_TYPE
	bool SetMaze(std::vector<std::string> vecMazeRows, char wallCharacter = 'X', char floorCharacter = '.');
	
	bool IsMazeValid(void);
	int getNumberOfRows_Length(void);
	int getNumberOfColumns_Width(void);
	

	
	struct sGridLocation
	{
		sGridLocation(int _row, int _col)
			{ this->row = _row; this->col = _col; }
		int row = -1;
		int col = -1;
	};
	

	// Searches around the perimieter and return any exits (entrances)
	// (i.e. non-wall breaks in the outer walls)
	// Returns false if none are found.
	bool FindExits(std::vector<sGridLocation> &vecExits);
	
	enum eDirections
	{
		LEFT = 0,
		UP, 
		RIGHT, 
		DOWN,
		UNKNONW_DIRECTION
	};
	
	// Returns true if it made it from one exit to another
	// Returns false if:
	// - didn't make it to the exit
	// - start or exit is a wall tile
	bool CreateRandomPath(sGridLocation start, sGridLocation end,
	                      std::vector<sGridLocation> &vecPath);
	bool CreateRandomPath(int start_row, int start_col, int end_row, int end_col,
	                      std::vector<sGridLocation> &vecPath);

	// Will keep calling CreatePath until it gets from start to end
	// Passing 0 as maxNumberOfAttempts = run forever
	bool SolveMazeRandomPath(sGridLocation start, sGridLocation end, 
	                         std::vector<sGridLocation> &vecPath,
	                         int &numberOfAttempts, 
	                         int maxNumberOfAttempts = 1000);
				   
	bool SolveMazeRandomPath(int start_row, int start_col, int end_row, int end_col,
	                         std::vector<sGridLocation> &vecPath,
	                         int &numberOfAttemps,
	                         int maxNumberOfAttempts = 1000);

	// If there is a path, this will solve it. 
	// Instead of doinf a random walk, it recursively probes the maze. 
	bool SolveMazeRecursive(sGridLocation start, sGridLocation end,
	                        std::vector<sGridLocation>& vecPath);

	bool SolveMazeRecursive(int start_row, int start_col, int end_row, int end_col,
	                        std::vector<sGridLocation>& vecPath);


	// For printing the maze WITH the path
	std::string getMazeWithPathString(std::vector<sGridLocation> &vecPath, char pathCharacter = '@');

					
	// Converts the direction enums to characters (for printing or whatever)
	void getDirectionCharacters(std::vector<eDirections> vecPath, 
	                            std::vector<char> &vecPathAsChars,
	                            char leftChar = 'L', 
								char rightChar = 'R', 
								char upChar = 'U', 
								char downChar = 'D');

	enum eCellType
	{
		FLOOR = 0,
		WALL = 1,
		UNKNOWN_TYPE = 2,
		INVALID_CELL,		// If a cell is outside of the range
		//
		PATH_CELL			// For printing the maze with the path
	};
	char getCellTypeAsChar(eCellType cell);
	
	eCellType getGridCellType(sGridLocation cell);
	eCellType getGridCellType(unsigned int row, unsigned int col);
	
private:
	std::vector<std::vector<eCellType>> m_vecMaze;
	
	// -1 if invalid
	int m_NumColumns_Width = -1;
	int m_NumRows_Height = -1;
	char m_FloorTileChar = '?';
	char m_WallTileChar = '?';
	
	cMersenneTwister* m_pRandom = NULL;
	
	// Returns false if there's no "next valid cell"
	// Doesn't pick from previous cell (where it came from)
	bool m_PickNextValidCell(std::vector<sGridLocation> &vecPathSoFar);

	// Used by: SolveMazeRecursive()
	bool m_bIsCellInMaze(sGridLocation cellRowCol);
	bool m_bFindValidNextCell(int& row, int& col, std::vector<std::vector<bool>>& vecMazeCellVisited);
};
	
#endif //_cGridMazeTraveller_HG_
