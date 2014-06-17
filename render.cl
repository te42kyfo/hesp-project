#include "real.hpp"

__kernel void render( const unsigned particleCount,
					  global real* px, global real* py, global real* pz,
					  global real* image,
					  unsigned int width, unsigned int height,
					  float3 origin, float3 direction) {

	size_t gidx = get_global_id(0);
	size_t gidy = get_global_id(1);
	size_t globalid = gidy*width +gidx;
	if( gidx > width || gidy > height) return;

	image[globalid*3] = gidx/width;
	image[globalid*3+1] = gidy/height;
	image[globalid*3+2] = 0.3;


}
