#include "real.hpp"

__kernel void update_cells( const unsigned int N,
							global int* cells, global int* links,
							global real* px, global  real* py, global real* pz,
							const real xmin, const real ymin, const real zmin,
							const real xmax, const real ymax, const real zmax,
							const unsigned int nx,
							const unsigned int ny,
							const unsigned int nz ) {
	const int globalid = get_global_id(0);
	if( globalid >= N ) return;

	const int xindex = (px[globalid]-xmin) / (xmax-xmin) * nx;
	const int yindex = (py[globalid]-ymin) / (ymax-ymin) * ny;
	const int zindex = (pz[globalid]-zmin) / (zmax-zmin) * nz;

	if( xindex < 0 || xindex >= nx ||
		yindex < 0 || yindex >= ny ||
		zindex < 0 || zindex >= nz ) return;


	size_t cellindex = zindex * nx*ny + yindex*nx + xindex;


	links[globalid] = atomic_xchg(cells+cellindex, links[globalid]);

}
