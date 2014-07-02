#include "real.hpp"

__kernel void update_forces( const unsigned int N,
							 const real dt, const real ks, const real kd,
							 const real gx, const real gy, const real gz,
							 int reflect_x, int reflect_y, int reflect_z,
							 global real * m, global real * radius,
							 global real * px, global real * py, global real * pz,
							 global real * vx, global real * vy, global real * vz,
							 global real * fx, global real * fy, global real * fz,
							 global real * ofx, global real * ofy, global real * ofz,
							 global int* cells, global int* links,
							 real x1, real y1, real z1,
							 real x2, real y2, real z2,
							 int nx, int ny, int nz) {


	const int globalid = get_global_id(0);
	if( globalid >= N ) return;



	real new_force_x = 0;
	real new_force_y = 0;
	real new_force_z = 0;

	int force_count = 0;
	int cell_count = 0;
	if( !isinf(m[globalid]) ) {
		new_force_x = gx*m[globalid];
		new_force_y = gy*m[globalid];
		new_force_z = gz*m[globalid];
	}

	const int xindex = (px[globalid]-x1) / (x2-x1) * nx;
	const int yindex = (py[globalid]-y1) / (y2-y1) * ny;
	const int zindex = (pz[globalid]-z1) / (z2-z1) * nz;


	for( int iz = -1; iz <= 1; iz++) {
		for( int iy = -1; iy <= 1; iy++) {
			for( int ix = -1; ix <= 1; ix++) {

				cell_count++;

				int xlocal = xindex+ix;
				int ylocal = yindex+iy;
				int zlocal = zindex+iz;

				xlocal += 2*nx;
				ylocal += 2*ny;
				zlocal += 2*nz;

				xlocal %= nx;
				ylocal %= ny;
				zlocal %= nz;



				const int cellindex = zlocal * nx*ny + ylocal *nx + xlocal;
				int index = cells[cellindex];


				int inner_cell_count = 0;

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

						real length = sqrt(dx*dx+dy*dy+dz*dz);

						dx /= length;
						dy /= length;
						dz /= length;

						real p = radius[globalid] + radius[index] - length;
						if( p > 0 ) {
							real dvx = vx[globalid] - vx[index];
							real dvy = vy[globalid] - vy[index];
							real dvz = vz[globalid] - vz[index];

							real dp = - (dx*dvx + dy*dvy + dz*dvz);


							new_force_x += dx * (ks*p+kd*dp);
							new_force_y += dy * (ks*p+kd*dp);
							new_force_z += dz * (ks*p+kd*dp);
						}
						force_count++;
						inner_cell_count++;
					}
					index = links[index];

				}


			}
		}
	}


	if( reflect_x != 0) {
		real p = radius[globalid] - (px[globalid] - x1);
		if( p > 0 ) {
			real dp = - vx[globalid];
			new_force_x += (ks*p + kd*dp);
		}
		p = radius[globalid] - (x2 - px[globalid]);
		if( p > 0 ) {
			real dp = vx[globalid];
			new_force_x += - (ks*p+kd*dp);
		}
	}

	if( reflect_y != 0) {
		real p = radius[globalid] - (py[globalid] - y1);
		if( p > 0 ) {
			real dp = -vy[globalid];
			new_force_y += (ks*p+kd*dp);
		}
		p = radius[globalid] - (y2 - py[globalid]);
		if( p > 0 ) {
			real dp =  vy[globalid];
			new_force_y += - (ks*p+kd*dp);
		}
	}

	if( reflect_z != 0) {
		real p = radius[globalid] - (pz[globalid] - z1);
		if( p > 0 ) {
			real dp = - vz[globalid];
			new_force_z += (ks*p+kd*dp);
		}
		p = radius[globalid] - (z2 - pz[globalid]);
		if( p > 0 ) {
			real dp = vz[globalid];
			new_force_z += - (ks*p+kd*dp);
		}
	}


	ofx[globalid] = fx[globalid];
	ofy[globalid] = fy[globalid];
	ofz[globalid] = fz[globalid];

	fx[globalid] = new_force_x;
	fy[globalid] = new_force_y;
	fz[globalid] = new_force_z;
}
