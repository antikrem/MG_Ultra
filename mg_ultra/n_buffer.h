/*Templated class that allows multiple threads to update
A object with only the buffer switch being synchronised
However multiple writes will cause thread clash
Read will always be optimal as hell*/
#ifndef __N_BUFFER__
#define __N_BUFFER__

#include <atomic>

#define SIZE 3

/*read with load(), write with store()
Single write thread*/
template<class T>
class TrippleBuffer {
	T buffer[SIZE];
	atomic<int> pointer = 0;	

public:

	//Loads most recent buffer
	T load() {
		return buffer[pointer.load()];
	}

	//direct get
	T load(int i) {
		return buffer[i];
	}

	//locks multiple reads
	void store(T value) {
		int currentWrite = pointer.load();
		buffer[(currentWrite + 1) % SIZE] = value;
		pointer.store(
			(currentWrite + 1) % SIZE
		);
	}
};

//testing:
/*
void read(TrippleBuffer<int>* in) {
	while (true) {
		cout << in->load() << endl;
	}
}

void write(TrippleBuffer<int>* in) {
	while (true) {
		in->store(rand() % 100);
	}
}

void write1(TrippleBuffer<int>* in, int val) {
	while (true) {
		in->store(val);
	}
}

#include <thread>
//thread conflict
void stress0() {
	TrippleBuffer<int> b;
	auto a0 = thread(&write1, &b, 0);
	auto a1 = thread(&write1, &b, 1);
	auto a2 = thread(&write1, &b, 2);
	auto a3 = thread(&write1, &b, 3);
	auto a4 = thread(&write1, &b, 4);
	auto a5 = thread(&write1, &b, 5);
	auto a6 = thread(&write1, &b, 6);
	auto a7 = thread(&write1, &b, 7);
	auto a8 = thread(&write1, &b, 8);
	auto a9 = thread(&write1, &b, 9);
	auto d = thread(&read, &b);
	d.join();
}

//fast
void stress1() {
	TrippleBuffer<int> b;
	auto a0 = thread(&read, &b);
	auto a1 = thread(&read, &b);
	auto a2 = thread(&read, &b);
	auto a3 = thread(&read, &b);
	auto a4 = thread(&read, &b);
	auto a5 = thread(&read, &b);
	auto a6 = thread(&read, &b);
	auto a7 = thread(&read, &b);
	auto a8 = thread(&read, &b);
	auto a9 = thread(&read, &b);
	auto d = thread(&write, &b);
	d.join();
}

*/

#undef SIZE

#endif