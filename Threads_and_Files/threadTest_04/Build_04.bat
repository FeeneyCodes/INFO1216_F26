g++ -c threadTest_04_MazeSolve_inMemory.cpp 
g++ -c cProcessMemoryCounters.cpp
g++ -c cMersenneTwister.cpp
g++ -c cMazeMaker.cpp
g++ -c cHFTimer.cpp
g++ -c cVT100TerminalConsole.cpp
g++ -c cGridMazeExplorer.cpp
g++ -o Thread04.exe threadTest_04_MazeSolve_inMemory.o cProcessMemoryCounters.o cMersenneTwister.o cMazeMaker.o cHFTimer.o cVT100TerminalConsole.o cGridMazeExplorer.o 
@echo "All done"