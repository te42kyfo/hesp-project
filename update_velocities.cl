#include "real.hpp"

__kernel void update_velocities( const unsigned int N,
								 const real dt, const real epsilon, const real sigma,
								 global real * m,
								 global real * px, global real * py, global real * pz,
								 global real * vx, global real * vy, global real * vz,
								 global real * fx, global real * fy, global real * fz,
								 global int* cells, global int* links,
								 real x1, real y1, real z1,
								 real x2, real y2, real z2,
								 unsigned int nx, unsigned int ny, unsigned int nz,
								 const real r_cut ) {


	const int globalid = get_global_id(0);
	if( globalid >= N ) return;

	real new_force_x = 0;
	real new_force_y = 0;
	real new_force_z = 0;

	const int xindex = (px[globalid]-x1) / (x2-x1) * nx;
	const int yindex = (py[globalid]-y1) / (y2-y1) * ny;
	const int zindex = (pz[globalid]-z1) / (z2-z1) * nz;


	for( int iz = -1; iz <= 1; iz++) {
		for( int iy = -1; iy <= 1; iy++) {
			for( int ix = -1; ix <= 1; ix++) {

				int xlocal = xindex+ix;
				int ylocal = yindex+iy;
				int zlocal = zindex+iz;

				xlocal %= nx;
				ylocal %= ny;
				zlocal %= nz;

				const int cellindex = zlocal * nx*ny + ylocal *nx + xlocal;
				int index = cells[cellindex];


				while( index != -1 ) {
					if( index != globalid ) {

						real dx = px[globalid] - px[index];
						real dy = py[globalid] - py[index];
						real dz = pz[globalid] - pz[index];

						if( dx > (x2-x1)*0.5 ) dx -= (x2-x1);
						if( dy > (y2-y1)*0.5 ) dy -= (y2-y1);
						if( dz > (z2-z1)*0.5 ) dz -= (z2-z1);

						if( dx < -(x2-x1)*0.5 ) dx += (x2-x1);
						if( dy < -(y2-y1)*0.5 ) dy += (y2-y1);
						if( dz < -(z2-z1)*0.5 ) dz += (z2-z1);


						real sqlength = (dx*dx + dy*dy + dz*dz);
						if( sqlength <= r_cut*r_cut) {

							real sigma6 = sigma*sigma*sigma*sigma*sigma*sigma;

							real factor =
								24.0*epsilon/sqlength *
								sigma6 / (sqlength*sqlength*sqlength)*
								(2.0*sigma6 / (sqlength*sqlength*sqlength)-1.0);

							new_force_x += factor*dx;
							new_force_y += factor*dy;
							new_force_z += factor*dz;
						}


					}
					index = links[index];
				}

			}
		}
	}



	vx[globalid] += (fx[globalid] + new_force_x) * dt * 0.5 / m[globalid];
	vy[globalid] += (fy[globalid] + new_force_y) * dt * 0.5 / m[globalid];
	vz[globalid] += (fz[globalid] + new_force_z) * dt * 0.5 / m[globalid];



	fx[globalid] = new_force_x;
	fy[globalid] = new_force_y;
	fz[globalid] = new_force_z;
}
