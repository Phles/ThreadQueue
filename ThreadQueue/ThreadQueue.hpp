/*
 *  Thread Queue Library. Runs functions on threads.
 */

#ifndef THREADQUEUE
#define THREADQUEUE

#include <condition_variable>
#include <thread>
#include <unordered_map>
#include <set>
#include <functional>
#include <any>

 
#ifndef PHFUNCT(...)
//Phles Function header template, prevents duplicate args
#define PHFUNCT(...) (__VA_ARGS__),__VA_ARGS__
#endif // !PHFUNCT




namespace Phles {
	
	/*
	 * Abstract class common to all tasks
	 */
	class Job {
	protected:
		//When to start and end the Job in the queue
		int startPhase;
		int endPhase;
		Job() :startPhase(0), endPhase(0){}
	public:
		virtual void run() = 0;
	};
	
	/*
	 * Task to some function on another thread.
	 */
	template<typename Fn,typename ...Args>
	class Task : Job{
	public:
		//Function to execute
		std::function<Fn> function;
		
		//Arguments to be passed to the function
		std::tuple<Args...> arguments;

		//Constructor
		Task(Fn FunctionAddr = nullptr,Args... args)
			:function(FunctionAddr)
		{
			arguments = std::make_tuple(args...);
		}

		//Run the function
		void run() override{
			std::apply(function, arguments);
		}

	};

	/*
	 * Class that runs functions from a queue in parallel
	 */
	class ThreadQueue {
	protected:
		//List of threads to start
		std::unordered_map<int, std::set<Job*>> startPhase;
		//List of threads to end
		std::unordered_map<int, std::set<Job*>> endPhase;

	public:

	};
	
	
}

#endif