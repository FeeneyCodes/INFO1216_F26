#include <iostream>
#include <string>
#include <Windows.h>

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


int main(int argc, char* argv[])
{
	printf("Hello world\n");
	
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
	HANDLE tHandle = CreateThread( NULL, 
	                               0, 				// Stack size
								   ThreadProc,		// Function thread runs
								   (void*)&tParams,	// Pointer to the stuff we pass
								   CREATE_SUSPENDED,// Creation params
								   &threadId);		// Pointer to threadID variable

	std::cout << "Thread created (threadID = " << threadId << ")" << std::endl;
	std::cout << "Starting thread then sleeping for 1 second..." << std::endl;

	ResumeThread(tHandle);
	// Suspend this main() thread for 1000 ms (or 1 second)
	// 	to give the thread time to start and run.
	// The thread runs then exits so will return long before
	//	this 1 second has elapsed.
	Sleep(1000);
	
	std::cout << "Thread returned " << tParams.Message << std::endl;
	
	
	return 0;
}
