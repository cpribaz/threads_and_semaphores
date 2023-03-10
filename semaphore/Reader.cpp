#include <iostream>
#include "SharedObject.h"
#include "Semaphore.h"

using namespace std; 

//create struct with variables to display to user and hold user input 
struct MyShared {
	int threadNumber; //threads ID
	int reportID; //number of times report has been created 
	int passedT; //time passed since write to memory location 
	int delayT; //the entered delay 
};

int main(void)
{
	//create the reader and writer semaphores 
	Semaphore readerSemaphore("reader", 1);
	Semaphore writerSemaphore("writer", 1);

	//display to the terminal that this is the reader 
	std::cout << "I am a reader" << std::endl;

	//create the shared memory 
	Shared<MyShared> sharedMemory("sharedMemory");

	//create a while loop to continuously run and display thread info
	while(true){
		//use the Wait function provided by the semaphore on the reader semaphore 
		readerSemaphore.Wait();
		//display the different values that were entered in the writer terminal and passed through the shared memory 
		cout << "Thread Number = " <<
		sharedMemory->threadNumber << " --- Report Count = " <<
		sharedMemory->reportID << " --- Time Taken = " << 
		sharedMemory->passedT << " --- Delay = " << 
		sharedMemory->delayT << 
		"\n";

		//signal the writer semaphore using the provided Signal function 
		writerSemaphore.Signal();
	}
}
