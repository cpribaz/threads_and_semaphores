#include <iostream>
#include "thread.h"
#include "SharedObject.h"
#include "Blockable.h"
#include <time.h>
#include <stack>
#include <thread>
#include <unistd.h>

using namespace std;

struct MyShared{
	int threadNumber, reportID, passedT, totalT, delayT;
};

////////////////////////////////////////////////////////////////////////
// This is a possible starting point for using threads and shared memory. 
// You do not have to start with this
////////////////////////////////////////////////////////////////////////
class WriterThread : public Thread{
	public:
		int threadNum, delay;
		//set flag to false so that the loop does not break
		bool flag = false;
		int reportID = 0;

		WriterThread(int in, int delayTime):Thread(8*1000){
			this->threadNum = in; 
			this->delay = delayTime;
		}

		virtual long ThreadMain(void) override{
			//declare shared memory var so this thread can access it
			Shared<MyShared> sharedMemory ("sharedMemory");
			time_t firstTime = time(0);

			while(true)
			{
				//write to shared memory
				//incremenet the report ID counter every cycle/loop
				this->reportID++;

				sleep(delay); //sleep this thread for the time specified in the Thread parameter
				time_t lastTime = time(0);

				int timeElapsed = lastTime - firstTime;
				sharedMemory-> threadNumber = threadNum; //set the shared mem location to the current threadID
				sharedMemory-> passedT = timeElapsed;
				sharedMemory-> reportID = reportID; //update everything for this shared mem locaton 
				sharedMemory-> totalT = timeElapsed*reportID;
				sharedMemory-> delayT = delay;

				if(flag){//Exit loop to end the thread
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
	Shared<MyShared> shared("sharedMemory", true); //This is the owner of sharedMamory

	int threadNum = 0;
	string delayTime, userInput;

	WriterThread * thread1;
	stack<WriterThread*> threadStack;

	while(true){
		//create thread using user input
		cout << "Would you like to create a new thread: (y/n)\n";
		getline(cin, userInput);

		if(userInput == "n"){
			break;
		}
		else{
		cout << "What delay time do you want on this thread: \n";
		threadNum++; 
		getline(cin, delayTime);
		int delayInt = stoi(delayTime);
		thread1 = new WriterThread(threadNum, delayInt); //add arguments as needed
		threadStack.push(thread1);
		}
	}
	for(auto i = 0; i < threadStack.size(); i++){
		thread1 = threadStack.top();
		thread1->flag= true;
		delete thread1;
		threadStack.pop();
	}
}



