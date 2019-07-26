/*Allows a workload to be multithreaded
Workload is in the form
omega[length]: array of type T
f(T) : a function that only takes pointer to one element of T
*/
#ifndef __THREAD_CRUNCHER__
#define __THREAD_CRUNCHER__

#include <atomic>
#include <condition_variable>


template<class T>
class ThreadSingle {
	condition_variable cv;
	mutex mtx;

	thread* masterThread = nullptr;

	atomic<bool> active = true;
	atomic<bool> computing = false;
	atomic<bool> newInput = false;

	T* omega = nullptr;
	atomic<int> start = 0;
	atomic<int> end = 0;

	void(*f)(T*) = nullptr;

public:
	ThreadSingle(void(*f)(T*)) {
		this->f = f;
		masterThread = new thread(&ThreadSingle<T>::threadProcess, this);
	}

	~ThreadSingle() {
		{
			unique_lock<mutex> lck(this->mtx);
			active = false;
			newInput = true;
		}
		cv.notify_one();
		masterThread->join();
	}
	
	void threadProcess() {
		while (this->active) {
			{
				unique_lock<mutex> lck(this->mtx);
				this->cv.wait(lck, [this] {return this->newInput.load(); });

				if (active) {
					for (int i = this->start; i < this->end; i++) {
						this->f(this->omega + i);
					}
				}
				this->computing = false;
				this->newInput = false;
			}
			cv.notify_one();
		}
	}

	void process(T* omega, int start, int end) {
		{
			unique_lock<mutex> lck(this->mtx);
			cv.wait(lck, [this] {return !(this->computing); });
			this->omega = omega;
			this->start = start;
			this->end = end;
			newInput = true;
			computing = true;
		}
		cv.notify_one();
	}

	void wait() {
		while (computing) {

		}
	}
};

template<class T>
class ThreadCruncher {
	int n = 0;
	void(*f)(T*) = nullptr;

	//vector of thread singles
	vector < ThreadSingle<T>* > threadSingles;

public:
	/*n is the number of threads
	f is function to call
	*/
	ThreadCruncher(int n, void(*f)(T*)) {
		this->n = n;
		this->f = f;

		for (int i = 0; i < n; i++) {
			threadSingles.push_back(new ThreadSingle<T>(f));
		}
	}

	//T omega[length] is valid
	void crunch(T* omega, int length) {
		int size = (length / n);
		if (length % n)
			size++;
		
		int start;
		int end;
		
		for (int i = 0; i < n; i++) {
			start = i * size;
			end = min(start + size, length);
			threadSingles[i]->process(omega, start, end);
			//if end is length, then leave
			if (end == length)
				break;
		}

	}
};


#endif