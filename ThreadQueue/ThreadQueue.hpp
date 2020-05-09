/*
 *  Thread Queue Library. Runs functions on threads.
 */

#ifndef THREADQUEUE
#define THREADQUEUE

#include <condition_variable>
#include <thread>
#include <vector>
#include <functional>
#include <any>
#include <tuple>
namespace Phles {
	/*
	 * Abstract class common to all tasks
	 */
	class Job {
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

	
	
	
}

#endif