#include "real.hpp"

__kernel void density_field( const unsigned particleCount,
							 global real* px, global real* py, global real* pz,
							 global real* density_field,
							 unsigned int xcount, unsigned int ycount, unsigned int zcount,
							 real xmin, real ymin, real zmin,
							 real xmax, real ymax, real zmax ) {

	size_t gidx = get_global_id(0);
	size_t gidy = get_global_id(1);
	size_t gidz = get_global_id(2);

	if( gidx >= xcount || gidy >= ycount || gidz >= zcount) return;
	size_t globalid = (gidz*xcount*ycount + gidy*xcount +gidx);


	float cellx = xmin + (float) gidx/xcount * (xmax-xmin);
	float celly = ymin + (float) gidy/ycount * (ymax-ymin);
	float cellz = zmin + (float) gidz/zcount * (zmax-zmin);


	density_field[globalid] = 0;
	for( unsigned int i = 0; i < particleCount; i++) {
		float dx = px[i] - cellx;
		float dy = py[i] - celly;
		float dz = pz[i] - cellz;
		density_field[globalid] += 1.0 / (dx*dx+dy*dy+dz*dz);
	}

	


}
