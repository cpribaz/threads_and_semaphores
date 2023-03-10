#include <iostream>
#include "thread.h"
#include "SharedObject.h"
#include "Blockable.h"
#include <time.h>
#include <stack>
#include <thread>
#include <unistd.h>
#include "Semaphore.h"

using namespace std;


struct MyShared{
	//create variables to hold values that will be displayed to user
	int threadNumber; //threads ID
	int reportID; //number of times report has been created 
	int passedT; //time passed since write to memory location 
	int delayT; //the entered delay 
};

//create a reader and writer semaphore 
Semaphore readerSemaphore("reader", 1, true);
Semaphore writerSemaphore("writer", 1, true);

////////////////////////////////////////////////////////////////////////
// This is a possible starting point for using threads and shared memory. 
// You do not have to start with this
////////////////////////////////////////////////////////////////////////
class WriterThread : public Thread{
	public:
		int  threadNum, delay;
		bool flag = false; //this is set to false to ensure the loop does not break
		int reportID = 0;
		
		WriterThread(int in, int delayTime):Thread(delayTime*1000){
			this->threadNum = in; 
			this -> delay = delayTime;
		}

		virtual long ThreadMain(void) override{
			//declare shared memory var so this thread can access it
			Shared<MyShared> sharedMemory ("sharedMemory");
			while(true)
			{
				//write to shared memory
				//reportID incremented each loop
				this -> reportID++;
				time_t firstTime = time(0);
				writerSemaphore.Wait();
				sleep(delay);
				time_t lastTime = time(0);

				//set the shared memory values 
				int timeElapsed = lastTime - firstTime;
				sharedMemory -> threadNumber = threadNum;
				sharedMemory -> passedT = timeElapsed;
				sharedMemory -> reportID = reportID;
				sharedMemory -> delayT = delay;
				readerSemaphore.Signal();

				if(flag){
					//Exit loop to end the thread
					break;
				}
			}
			return 1;
		}
};


int main(void)
{
	std::cout << "I am a Writer" << std::endl;
	
	////////////////////////////////////////////////////////////////////////
	// This is a possible starting point for using threads and shared memory. 
	// You do not have to start with this
	////////////////////////////////////////////////////////////////////////
	
	//Create the owner of sharedMamory
	Shared<MyShared> shared("sharedMemory", true); 
	//set thread number to 0 and declare user input and delay variables
	int threadNum = 0;
	string delayTime;
	string userInput;

	//create a writer thread 
	WriterThread * thread1;
	stack<WriterThread*> threadStack;

	//create loop for user input 
	while(true){
		//ask user if they would like to create a new thread 
		cout<< "Would you like to create a new thread? (y or n)\n";
		getline(cin, userInput);
		//if user enters "n" break the loop 
		if(userInput == "n"){
			break;
		}
		//if user enters "y" or other values continue the loop
		else{
			//ask user to enter a delay value 
			cout << "Enter a delay for this thread: \n";
			//increment the thread number (aka thread ID)
			threadNum++;
			getline(cin, delayTime);
			//convert the entered delay time to a integer and set value for delayInt
			int delayInt = stoi(delayTime);
			//initialize new threads values with user input and thread number/ID
			thread1 = new WriterThread(threadNum, delayInt); 
			//put the new thread into the thread stack 
			threadStack.push(thread1);
		}
		
		
	}
	//create a loop for deleting the thread stack when we exit the loop
	for (auto j = 0; j < threadStack.size(); j++){
		thread1 = threadStack.top();
		//set the flag from earlier to true to allow loop to break
		thread1->flag= true;
		//delete the thread 
		delete thread1;
		//pop the thread from the stack
		threadStack.pop();
	}
}
