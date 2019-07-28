/*Bunch of useful os stuff*/

#ifndef __OS_KIT__
#define __OS_KIT__

#include <string>
#include <fstream>

using namespace std;

/*All methods are in the str_kit namespace*/
namespace os_kit {

	//checks if file exists
	bool fileExists(const std::string& name);
}

#endif