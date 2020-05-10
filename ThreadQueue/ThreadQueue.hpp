/*
 *  Thread Queue Library. 
 *	Runs a set of functions on threads, specifying when to start each task and when each task must end.
 *  This library, while as standard C++ as possible, has some limitations to its function.
 *
 *  License: zlib/png
 */

//Include Guard
#ifndef THREADQUEUE
#define THREADQUEUE
//Threading headers
#include <thread>
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <chrono>

//Storage headers
#include <vector>
#include <map>
#include <set>
#include <functional>

 
#ifndef PHFUNCT
//Phles Function header template, prevents duplicate args
#define PHFUNCT(...) (__VA_ARGS__),__VA_ARGS__
#endif // !PHFUNCT




namespace Phles {
	//Allow literals in this namespace
	using namespace std::chrono_literals;
	/*
	 * Abstract class common to all tasks
	 */
	class Job {
	protected:
		//When to start and end the Job in the queue
		int startPhase;
		int endPhase;
		
	public:

		Job() :startPhase(0), endPhase(0), isRunning(false), isComplete(false) {}
		
		//Whether the job is currently running
		std::atomic<bool> isRunning;
		//Whether a job has finished
		bool isComplete;

		virtual void run() = 0;
	};
	
	/*
	 * Task to some function on another thread.
	 */
	template<typename Fn,typename ...Args>
	class Task : public Job{
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
			isRunning = true;
			//Run the function
			std::apply(function, arguments);
			//Mark as done and stopped.
			isComplete = true;
			isRunning = false;
		}

	};

	/*
	 * Class that runs functions from a queue in parallel
	 */
	class ThreadQueue {
	protected:
		//List of threads to start
		std::map<int, std::set<Job*>> startPhase;
		//List of threads to end
		std::map<int, std::set<Job*>> endPhase;
		
		//Current phase of the thread queue. Determines which threads start and which end
		int phase;
		
		//Mutex to watch over threads
		std::mutex mutexLock;
		//Flag to move to the next phase.
		std::condition_variable changePhase;

		//List of active threads
		std::vector<std::thread*> workers;
		
		/* Internal Functions for use by the Thread Loop */
		
		
		/*
		 * Runs an individual phase, which has the following sequence:
		 * All jobs in startPhase of the current phase begin execution
		 * All jobs in endPhase of the current phase end execution
		 */
		void runPhase(int phase,std::unique_lock<std::mutex>&lock){
			
			//Ensure the phase has new jobs starting
			if (startPhase.count(phase) > 0) {
				//Start a job that hasn't been started
				for (auto& job : startPhase[phase]) {
					//Ensure the job wasn't already done
					if (!job->isComplete && !job->isRunning) {
						job->run();
						//A job has finished, so wake the other threads to check if more work is available
						changePhase.notify_all();
					}
					
				}
			}


			//Loop for ending the phase
			while (endPhase[phase].size() > 0) {
				//Locked section, tries removing a completed job
				if (lock.try_lock()) {
					auto job = endPhase[phase].begin();
					//Job must exist
					if (job != endPhase[phase].end()) {
						//Job has finished, can be removed.
						if ((*job)->isComplete && !(*job)->isRunning) {
							//Delete object pointed to by iterator
							delete (*job);
							endPhase[phase].erase(*job);
						}
						//Job hasn't finished, so it cannot be removed. Wait for 1 second
						else {
							if (changePhase.wait_for(lock, 20s) == std::cv_status::timeout) {
								lock.lock();
							}
						}
					}
					
					lock.unlock();
					

				}

			}
		}

	public:
		//Shared mutex for child threads
		static std::mutex sharedLock;
		//Constructor
		ThreadQueue():phase(0){}

		//Function that runs jobs from the thread queue. 
		void ThreadLoop(bool isMain=false) {
			std::unique_lock<std::mutex> lock(mutexLock, std::defer_lock);
			
			//Last phase in the queue
			int last = (endPhase.rbegin())->first;
			
			while (last >= phase) {
				
				//Run the phase
				runPhase(phase,lock);
				//The phase has ended, so clean up and ensure the phase moves when the main phase moves
				if (isMain) {
					lock.lock();
					phase++;
					lock.unlock();
				}

				
			}
		}

		/*
		 * Adds a job to the thread queue
		 */
		void addJob(int start,int end, Job* task) {


			//Ensure each phase exists.
			if (startPhase.count(start) == 0)
				startPhase[start] = std::set<Job*>();
			if (endPhase.count(end) == 0)
				endPhase[end] = std::set<Job*>();
			
			//Add task to start and end phase list
			startPhase[start].insert(task);
			endPhase[end].insert(task);

		}

		/*
		 * Start the thread Queue
		 * Args:
		 * workerCt: number of threads to spawn(excluding calling thread)
		 * includeCaller: if true, the calling thread runs a threadloop and acts like a worker
		 */
		void launch(size_t workerCt=2,bool includeCaller=false) {

			for (size_t c = 0; c <= workerCt;c++) {
				if(c == 0)
					workers.emplace_back(new std::thread(&Phles::ThreadQueue::ThreadLoop, std::ref(*this), true));
				else
					workers.emplace_back(new std::thread(&Phles::ThreadQueue::ThreadLoop, std::ref(*this), false));
			}
			
		}
		//Join all threads called in the thread queue
		void join(){
			for (auto& worker : workers) {
				worker->join();
				delete worker;
			}
			workers.clear();
		}

		//Detach all threads called in the thread queue
		void detach(){
			for (auto& worker : workers) {
				worker->detach();
				delete worker;
			}
			workers.clear();
		}

	};
	
	//Define Static var
	std::mutex ThreadQueue::sharedLock;
	
}

#endif