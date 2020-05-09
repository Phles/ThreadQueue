#include <chrono>
#include <iostream>
#include "ThreadQueue.hpp"
/*
 *	Function that happens first
 */
int someWork(int seconds,int a) {
	
	std::this_thread::sleep_for(std::chrono::seconds(seconds));

	std::cout << "Finished "<< a <<" at " << seconds << "s sleep on thread " << std::this_thread::get_id() << "\n";
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
	
	std::thread th(&Phles::ThreadQueue::ThreadLoop, std::ref(queue));
	
	th.join();
	
	//std::cin.get();
	return 0;

	
}