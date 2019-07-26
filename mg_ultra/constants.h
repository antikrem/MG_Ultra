#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

using namespace std;

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

#define SUCCESSFUL(a) (a == EXIT_SUCCESS)
#define FAILURE(a) (a != EXIT_SUCCESS)

#define pass true

#define PI (float)3.14159265359

#define E (float)2.71828182845

#define not(a) (!(a))

#define DEG2RAD(a) a*(PI/180)
#define RAD2DEG(a) a*(180/PI)

//unsigned long long max
const unsigned long long ULLINT_MAX = (unsigned long long int) -1;

#endif