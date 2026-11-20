g++ -c threadTest_03_MazeSolve_inMemory.cpp 
g++ -c cMersenneTwister.cpp
g++ -c cMazeMaker.cpp
g++ -c cHFTimer.cpp
g++ -c cGridMazeExplorer.cpp
g++ -o Thread03.exe threadTest_03_MazeSolve_inMemory.o cMersenneTwister.o cMazeMaker.o cHFTimer.o cGridMazeExplorer.o
@echo "All done"