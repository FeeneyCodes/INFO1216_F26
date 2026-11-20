all: threadTest_03

cHFTimer.o: cHFTimer.cpp

	g++ -c -o cHFTimer.o cHFTimer.cpp
	
cMersenneTwister.o: cMersenneTwister.cpp

	g++ -c -o cMersenneTwister.o cMersenneTwister.cpp
		
cHFTimer.o: cMazeMaker.cpp

	g++ -c -o cMazeMaker.o cMazeMaker.cpp

cGridMazeExplorer.o: cGridMazeExplorer.cpp

	g++ -c -o cGridMazeExplorer.o cGridMazeExplorer.cpp
	
threadTest_03_MazeSolve_inMemory.o: threadTest_03_MazeSolve_inMemory.cpp

	g++ -c -o threadTest_03_MazeSolve_inMemory.o threadTest_03_MazeSolve_inMemory.cpp

threadTest_03: cHFTimer.o cMazeMaker.o cGridMazeExplorer.o cMersenneTwister.o threadTest_03_MazeSolve_inMemory.o

	g++ cHFTimer.o cMazeMaker.o cGridMazeExplorer.o cMersenneTwister.o threadTest_03_MazeSolve_inMemory.o -o threadTest_03.exe

clean: 
	rm -f *.o threadTest_03
	
	
	
	
	
	