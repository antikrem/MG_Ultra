/**
 * Asyncronous error handling file.
 * Call primeErrorFile() at start, and endlog() at end
 */
#ifndef __ERROR__
#define __ERROR__

#include <string>
#include <vector>

using namespace std;

namespace err {
	// Needs to be run to prepare errorfile
	void primeErrorFile();

	// Flushes output stream and ends output file
	void endLog();

	// Update logging cycle
	void setLoggingCycle(int cycle);

	// Add a message to the error log buffer, which is pushed on occasion
	// Generic when logCycle is zero, else cycle is also noted
	void logMessage(std::string message);

	// Clears the console message buffer and 
	std::vector<std::string> getConsoleMessage();

	// Forces message buffer to be written, freezing thread of execution
	// Freezes calling thread until error thread has ended
	void forceWrite();
}

#endif