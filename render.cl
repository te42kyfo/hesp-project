#include "real.hpp"

__kernel void render( const unsigned particleCount,
					  global real* px, global real* py, global real* pz,
					  global real* image,
					  unsigned int width, unsigned int height,
					  float4 origin, float4 direction) {

	size_t gidx = get_global_id(0);
	size_t gidy = get_global_id(1);
	size_t globalid = (gidy*width +gidx)*3;
	if( gidx >= width || gidy >= height) return;



	float4 dir = { (float) gidx/width*2.0  -1.0,
				   (float) gidy/height*2.0 -1.0,
				   -1.0, 0.0 };
	dir = normalize(dir);


	image[globalid+0] = 0;
	image[globalid+1] = 0;
	image[globalid+2] = 0;

	for( unsigned int i = 0; i < 32; i++) {
		float t =
			(px[i]-origin.x) * dir.x +
			(py[i]-origin.y) * dir.y +
			(pz[i]-origin.z) * dir.z ;


		float tx = origin.x + t*dir.x;
		float ty = origin.y + t*dir.y;
		float tz = origin.z + t*dir.z;


		float sum = 0;
		for( unsigned int n = i; n < 32; n++) {
			float dx = px[n]-tx;
			float dy = py[n]-ty;
			float dz = pz[n]-tz;

			float r2 = ( dx*dx + dy*dy + dz*dz);
			sum += 1.0/r2;

			if( sum > 100 ) {
				image[globalid+0] = t/2.0;
				image[globalid+1] = t/4.0;
				image[globalid+2] = t/8.0;
				break;
			}
		}
	}

}
