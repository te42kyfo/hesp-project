#include "real.hpp"

__kernel void update_velocities( const unsigned int N,
								 const real dt, const real ks, const real kd,
								 const real gx, const real gy, const real gz,
								 int reflect_x, int reflect_y, int reflect_z,
								 global real * m, global real * radius,
								 global real * px, global real * py, global real * pz,
								 global real * vx, global real * vy, global real * vz,
								 global real * fx, global real * fy, global real * fz,
								 global int* cells, global int* links,
								 real x1, real y1, real z1,
								 real x2, real y2, real z2,
								 unsigned int nx, unsigned int ny, unsigned int nz) {


	const int globalid = get_global_id(0);
	if( globalid >= N ) return;



	real new_force_x;
	real new_force_y;
	real new_force_z;

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

						real length = sqrt(dx*dx+dy*dy+dz*dz);

						real p = radius[globalid] + radius[index] - length;
						if( p > 0 ) {
							real dvx = vx[globalid] - vx[index];
							real dvy = vy[globalid] - vy[index];
							real dvz = vz[globalid] - vz[index];

							real dp = - (dx*dvx + dy*dvy + dz*dvz) / length;

							new_force_x += dx/length * (ks*p+kd*dp);
							new_force_y += dy/length * (ks*p+kd*dp);
							new_force_z += dz/length * (ks*p+kd*dp);
						}





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


	vx[globalid] += (fx[globalid] + new_force_x) * dt * 0.5 / m[globalid];
	vy[globalid] += (fy[globalid] + new_force_y) * dt * 0.5 / m[globalid];
	vz[globalid] += (fz[globalid] + new_force_z) * dt * 0.5 / m[globalid];



	fx[globalid] = new_force_x;
	fy[globalid] = new_force_y;
	fz[globalid] = new_force_z;
}
