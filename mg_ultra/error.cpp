#include "error.h"
#include "constants.h"

#include <windows.h>
#include <fstream>
#include <iostream>

//remove
#include <stdlib.h>

#include <ctime>

#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <algorithm>

#include "atomic_queue.h"

using namespace std;

namespace err {
	// Temporary representation of a error message
	struct ErrorMessage {
		int cycle = 0;
		string message;
	};

	// Queue of error messages
	AtomicQueue<ErrorMessage> errors;
	
	//console message buffer
	mutex consoleLock;
	vector<string> consoleMirror;

	// Set to true when logging thread is over
	atomic<bool> safeToEnd = false;
	atomic<bool> logActive = false;

	atomic<int> loggingCycle = 0;

	string currentTime() {
		time_t t = time(0);
		tm now;
		localtime_s(&now, &t);
		char* buf = (char*)malloc(30);
		asctime_s(buf, 30, &now);
		string value(buf);
		free(buf);
		return value;
	}

	void setLoggingCycle(int cycle) {
		loggingCycle.store(cycle);
	}

	std::ofstream* openErrorFile() {
		std::ofstream* outfile = new std::ofstream();
		outfile->open("log.txt", std::ios_base::app);
		return outfile;
	}

	void closeErrorFile(std::ofstream* outfile) {
		outfile->close();
	}

	void appendToErrorFile(std::ofstream* outfile, std::string message) {
		*outfile << message << std::endl;
		unique_lock<mutex>(consoleLock);
		consoleMirror.push_back(message);
	}

	void appendToErrorFile(std::ofstream* outfile, int cycle, std::string message) {
		*outfile << cycle << ": " << message << std::endl;
		unique_lock<mutex>(consoleLock);
		consoleMirror.push_back(to_string(cycle) + ": " + message);
	}

	bool getSize() {
		return errors.size();
	}

	void loggingDuty() {
		ErrorMessage buffer;

		while (errors.get(&buffer)) {
			auto outfile = openErrorFile();

			if (buffer.cycle == 0) {
				appendToErrorFile(outfile, buffer.message);
			}
			else {
				appendToErrorFile(outfile, buffer.cycle, buffer.message);
			}
			closeErrorFile(outfile);
		}
		safeToEnd = true;

	}

	void primeErrorFile() {
		std::ofstream outfile;
		outfile.open("log.txt", std::ofstream::out | std::ofstream::trunc);
		outfile << "LOG FILE FOR mgULTRA-INDEV INSTANCE RAN ON: " << currentTime() << std::endl;
		outfile.close();

		std::thread logThread(&err::loggingDuty);
		logThread.detach();
	}

	void endLog() {
		logMessage("Ending Log...");

		// Stop receiving errors
		errors.stop();

		int count = 0;

		loggingCycle.store(0);

		while (!safeToEnd) {
			count++;
		};

		std::ofstream outfile;
		outfile.open("log.txt", std::ios_base::app);
		outfile << "SAFELY EXITED AT: " << currentTime() << std::endl;
		outfile << "Cycles waited:" << count << std::endl;
		outfile.close();
	}

	void logMessage(std::string message) {
		errors.add(ErrorMessage{ loggingCycle.load(), message });
	}

	vector<string> getConsoleMessage() {
		unique_lock<mutex> lck(consoleLock);
		auto temp = consoleMirror;
		consoleMirror.clear();
		return temp;
	}

	void forceWrite() {
		//lock
		//unique_lock<mutex> lck(lock);
		//todo implement
		cout << "147 error.cpp" << endl;

	}
}