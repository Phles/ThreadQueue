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
	Phles::Task<int PHFUNCT(int,int)> task(someWork, a, 0);
	Phles::Task<int PHFUNCT(int,int)> t(someWork,a,0);
	
	task.run();
	t.run();
	
	//std::cin.get();
	return 0;

	
}