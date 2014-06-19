#include "dtime.hpp"
#include <sys/time.h>

double dtime() {
	double tseconds = 0;
	struct timeval t;
	gettimeofday( &t, nullptr);
	tseconds = (double) t.tv_sec + (double) t.tv_usec*1.0e-6;
	return tseconds;
}
