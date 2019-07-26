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

using namespace std;

namespace err {
	//Standard lock
	mutex lock;
	condition_variable cv;

	//A lock used when forcing a write
	mutex forceLock;

	struct ErrorMessage {
		int cycle = 0;
		string message;
	};

	//console message buffer
	mutex consoleLock;
	vector<string> consoleMirror;

	vector<ErrorMessage> messageBuffer;
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

	void appendToErrorFile(std::string message) {
		std::ofstream outfile;
		outfile.open("log.txt", std::ios_base::app);
		outfile << message << std::endl;
		outfile.close();
	}

	void appendToErrorFile(int cycle, std::string message) {
		std::ofstream outfile;

		outfile.open("log.txt", std::ios_base::app);
		outfile << cycle << ": " << message << std::endl;
		outfile.close();
	}

	bool checkSize() {
		return messageBuffer.size();
	}

	void loggingDuty() {
		vector<ErrorMessage> localBuffer;
		vector<string> consoleMessageBuffer;

		while (logActive) {
			//will not conflict in most cases
			unique_lock<mutex> greaterLock(forceLock);
			{
				unique_lock<mutex> lck(lock);
				cv.wait(lck, &checkSize);
				//If safe to write and there is a message in the buffer
				localBuffer = messageBuffer;
				messageBuffer.clear();
			}

			for (ErrorMessage i : localBuffer) {
				if (i.cycle == 0) {
					appendToErrorFile(i.message);
					consoleMessageBuffer.push_back(i.message);
				}
				else {
					consoleMessageBuffer.push_back(to_string(i.cycle) + ": " + i.message);
					appendToErrorFile(i.cycle, i.message);
				}
			}
			localBuffer.clear();

			//copy console message buffer to consoleMirror
			{
				unique_lock<mutex> lck(consoleLock);
				consoleMirror.insert(consoleMirror.end(), consoleMessageBuffer.begin(), consoleMessageBuffer.end());
			}
			consoleMessageBuffer.clear();
		}

		safeToEnd = true;
	}

	void primeErrorFile() {
		std::ofstream outfile;
		outfile.open("log.txt", std::ofstream::out | std::ofstream::trunc);
		outfile << "LOG FILE FOR mgULTRA-INDEV INSTANCE RAN ON: " << currentTime() << std::endl;
		outfile.close();

		logActive = true;
		std::thread logThread(&err::loggingDuty);
		logThread.detach();
	}

	void endLog() {
		logActive = false;
		int count = 0;

		loggingCycle.store(0);
		logMessage("Ending Log...");
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
		unique_lock<mutex> lck(lock);
		ErrorMessage toPush = { loggingCycle.load(), message };
		messageBuffer.push_back(toPush);
		cv.notify_one();
	}

	vector<string> getConsoleMessage() {
		unique_lock<mutex> lck(consoleLock);
		auto temp = consoleMirror;
		consoleMirror.clear();
		return temp;
	}

	void forceWrite() {
		//lock
		unique_lock<mutex> lck(lock);
		//todo implement
		cout << "147 error.cpp" << endl;

	}
}