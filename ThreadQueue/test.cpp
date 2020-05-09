#include <chrono>
#include <iostream>
#include "ThreadQueue.hpp"


/*
 *	Function that happens first
 */
int someWork(int seconds,int a) {
	
	std::this_thread::sleep_for(std::chrono::seconds(seconds));
	Phles::ThreadQueue::sharedLock.lock();
	std::cout << "Finished "<< a <<" at " << seconds << "s sleep on thread " << std::this_thread::get_id() << "\n";
	Phles::ThreadQueue::sharedLock.unlock();
	return a + seconds;
}



/*
 * Entry point for testing
 */
int main(void) {
	int a = 2;
	
	Phles::Task<int PHFUNCT(int,int)> t(someWork,a,0);
	
	Phles::ThreadQueue queue;
	queue.addJob(0, 0, new Phles::Task<int PHFUNCT(int,int)>(someWork,a,0));
	queue.addJob(0, 0, new Phles::Task<int PHFUNCT(int,int)>(someWork,a,1));
	queue.addJob(0, 0, new Phles::Task<int PHFUNCT(int,int)>(someWork,a,2));
	
	
	std::thread th(&Phles::ThreadQueue::ThreadLoop, std::ref(queue),true);
	std::thread th1(&Phles::ThreadQueue::ThreadLoop, std::ref(queue),false);
	
	th1.join();
	th.join();
	
	//std::cin.get();
	return 0;

	
}