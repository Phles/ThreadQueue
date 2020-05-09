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
void someOtherWork(int a, int seconds) {
	
	std::this_thread::sleep_for(std::chrono::seconds(seconds));

	Phles::ThreadQueue::sharedLock.lock();
	std::cout << "Finished Task " << -1*a << " at " << seconds << "s sleep on thread " << std::this_thread::get_id() << "\n";
	Phles::ThreadQueue::sharedLock.unlock();

}



/*
 * Entry point for testing
 */
int main(void) {
	int a = 2;
	
	//Phles::Task<int PHFUNCT(int&,int)> t(someWork,a,0);
	
	Phles::ThreadQueue queue;
	for (int c = 0; c < 50; c++) {
		queue.addJob((c < 25)? 0 : 1, 2, new Phles::Task<int PHFUNCT(int, int)>(someWork,a*c % 5, c));
		queue.addJob((c < 25) ? 1 : 2, 2, new Phles::Task<void PHFUNCT(int, int)>(someOtherWork, c, a * c % 5));
	}
	
	queue.launch(5);
	queue.join();
	
	//std::cin.get();
	return 0;

	
}