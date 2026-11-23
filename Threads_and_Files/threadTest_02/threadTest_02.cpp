#include <iostream>
#include <string>
#include <Windows.h>
#include <sstream>	// like string builder

// We can pass anything to the thread, but it's common 
//	to package what we are passing in a struct or class.
// Like we could have just passed this string, but we
//	will add other things that we want to give to the 
//	thread function later.  
struct sTParams
{
	std::string Message;
	// A DWORD is "unsigned long" or an unsigned 32 or 64 bit integer 
	// (likely 64 on a 64 bit system)
	DWORD millisecondsToWait = 0;
	// When the thread is done, it sets this to true
	// We check this in the main to see if all the threads have finished
	bool bThreadDone = false;
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
	
	std::stringstream ssMessage;
	ssMessage << "Thread wiating for " << pTheParams->millisecondsToWait << " ms.";
	pTheParams->Message = ssMessage.str();
	std::cout << ssMessage.str() << std::endl;
	std::cout.flush();

	Sleep(pTheParams->millisecondsToWait);

	// Set flag to indicate we are done and exiting
	pTheParams->bThreadDone = true;

	return 0;
}


int main(int argc, char* argv[])
{
	// Optional: seed number generator
	//std::srand(std::time(0));

	
	// Here we are making multiple threads, so 
	//	the things we passed to each thread are
	//	duplicated. 

	const unsigned int NUM_THREADS = 8;

	// Create one of the things to pass to the thread
	sTParams tParams[NUM_THREADS];	
	// 
	DWORD threadID[NUM_THREADS];
	HANDLE tHandle[NUM_THREADS];

	tParams[0].Message = "You are thread A";
	tParams[1].Message = "You are thread B";
	tParams[2].Message = "You are thread C";
	tParams[4].Message = "You are thread D";
	tParams[5].Message = "You are thread E";
	tParams[6].Message = "You are thread F";
	tParams[7].Message = "You are thread G";


	// Create all the thread, but "suspended" so they don't start right away
	for (unsigned int index = 0; index < NUM_THREADS; index++)
	{
		// Pick a random time for each thread to wait
		// (a number between 2 and 4 seconds (2000 to 4000 ms))
		//
		// NOTE: rand() isn't 'thread safe', so if we do this in the thread
		//       they will almost certainly get the same "random" values
		// (this goes for the newer C++ 11 random - not thread safe)
		//
		tParams[index].millisecondsToWait = (std::rand() % 4000) + 2000;

		tHandle[index] = CreateThread( NULL, 
									   0, 				// Stack size
									   ThreadProc,		// Function thread runs
									   (void*)&(tParams[index]),	// Pointer to the stuff we pass
									   CREATE_SUSPENDED,// Creation params
									   &(threadID[index]) );		// Pointer to threadID variable
		std::cout << "Thread created (threadID = " << threadID[index] << ") "
			<< " wait time = " << tParams[index].millisecondsToWait << " (ms)" << std::endl;
	}
	
	// Now we start them all 
	std::cout << "Resuming all " << NUM_THREADS << " threads." << std::endl;
	std::cout.flush();

	for (unsigned int index = 0; index < NUM_THREADS; index++)
	{
		ResumeThread(tHandle[index]);
	}

	std::cout << "Now we wait for all threads to say they are done..." << std::endl;

	bool bKeepWaiting = true;
	while (bKeepWaiting)
	{
		// Assume they are all done
		bKeepWaiting = false;
		// Go through the array of bools to see if they have
		//	ALL been set to true
		for (unsigned int index = 0; index < NUM_THREADS; index++)
		{
			if (!tParams[index].bThreadDone)
			{
				// This thread is still running
				bKeepWaiting = true;
			}
			// This releases control over this main thread
			// (You could also call Sleep(1) to wait approx 1 ms)
			Sleep(0);
		}
	}

	std::cout << "All threads have completed their work:" << std::endl;

	for (unsigned int index = 0; index < NUM_THREADS; index++)
	{
		std::cout << "Thread " << index << " says: " << tParams[index].Message << std::endl;
	}

	std::cout << "All done." << std::endl;
	
	return 0;
}
