#include "real.hpp"

__kernel void raymarch( global real* density_field,
						unsigned int xcount, unsigned int ycount, unsigned int zcount,
						real xmin, real ymin, real zmin, real xmax, real ymax, real zmax,
						global real* image,	unsigned int width, unsigned int height,
						float4 origin, float4 direction) {

	size_t gidx = get_global_id(0);
	size_t gidy = get_global_id(1);
	size_t globalid = (gidy*width +gidx)*3;
	if( gidx >= width || gidy >= height) return;



	float4 dir = { ( (float) gidx/width*2.0  -1.0),
				   ( (float) gidy/height*2.0 -1.0),
				   1.0, 0.0 };

	dir = normalize(dir);

	float hx = (xmax-xmin) / xcount;
	float hy = (ymax-ymin) / ycount;
	float hz = (zmax-zmin) / zcount;
	float ihx = 1.0 / hx;
	float ihy = 1.0 / hy;
	float ihz = 1.0 / hz;


	float tx1 = (xmin - origin.x) / dir.x;
	float tx2 = (xmax - origin.x) / dir.x;
	float ty1 = (ymin - origin.y) / dir.y;
	float ty2 = (ymax - origin.y) / dir.y;
	float tz1 = (zmin - origin.z) / dir.z;
	float tz2 = (zmax - origin.z) / dir.z;



	float tx;
	float ty;
	float tz;

	if( tx1 < 0) tx1 = 0;
	if( ty1 < 0) ty1 = 0;
	if( tz1 < 0) tz1 = 0;
	if( tx2 < 0) tx2 = 0;
	if( ty2 < 0) ty2 = 0;
	if( tz2 < 0) tz2 = 0;


	if( tx1 < tx2) {
		tx = tx1;
	} else {
		tx = tx2;
	}
	if( ty1 < ty2) {
		ty = ty1;
	} else {
		ty = ty2;
	}
	if( tz1 < tz2) {
		tz = tz1;
	} else {
		tz = tz2;
	}

	float t0 = max(tx, max(tz, ty));


	float cx = origin.x + t0*dir.x;
	float cy = origin.y + t0*dir.y;
	float cz = origin.z + t0*dir.z;

	image[globalid+0] = 0.0;
	image[globalid+1] = 0.0;
	image[globalid+2] = 0.10;



	while(true) {

		int ix = (cx-xmin) *ihx;
		int iy = (cy-ymin) *ihy;
		int iz = (cz-zmin) *ihz;

		if( ix < 0 || ix >= xcount || iy < 0 || iy >= ycount || iz<0 || iz >= zcount) break;

		float density = 0;

		if( ix < xcount-1 && iy < ycount-1 && iz < zcount-1 &&
			ix > 0 && iy >0 && iz >0) {

			float fracx = (cx-xmin-ix*hx)*ihx;
			float fracy = (cy-xmin-iy*hy)*ihy;
			float fracz = (cz-xmin-iz*hz)*ihz;


			unsigned int idx = ix + iy*xcount + iz*xcount*ycount;

			float d000 = density_field[ idx                         ];
			float d001 = density_field[ idx+1                       ];
			float d010 = density_field[ idx   +xcount               ];
			float d011 = density_field[ idx+1 +xcount               ];
			float d100 = density_field[ idx           +ycount*xcount];
			float d101 = density_field[ idx+1         +ycount*xcount];
			float d110 = density_field[ idx   +xcount +ycount*xcount];
			float d111 = density_field[ idx+1 +xcount +ycount*xcount];

			density =
				fracz*
				(( d111*fracx + d110*(1.0-fracx) ) *fracy +
				 ( d101*fracx + d100*(1.0-fracx) ) *(1.0-fracy))+
				(1.0-fracz)*
				(( d011*fracx + d010*(1.0-fracx) ) *fracy +
				 ( d001*fracx + d000*(1.0-fracx) ) *(1.0-fracy));


			if( density > 4.0 ) {
				unsigned int idx = ix + iy*xcount + iz*xcount*ycount;
				float nx = (density_field[idx+1] -
							density_field[idx-1])/2.0;
				float ny = (density_field[idx+xcount] -
							density_field[idx-xcount])/2.0;
				float nz = (density_field[idx+xcount*ycount] -
							density_field[idx-xcount*ycount])/2.0;


				image[globalid+0] = nx*1.0 + ny*-0.5 + nz*1.0;
				image[globalid+1] = nx*0.0 + ny*1.0 + nz*0;;
				image[globalid+2] = nx*-1.0 + ny*0.5 + nz*0;;
				break;
			}
		}

		float tx = (xmin + (ix+sign(dir.x))*hx -cx)/dir.x;
		float ty = (ymin + (iy+sign(dir.y))*hy -cy)/dir.y;
		float tz = (zmin + (iz+sign(dir.z))*hz -cz)/dir.z;


		float t = min(tx, min(ty, tz));

		cx = cx + t*dir.x;
		cy = cy + t*dir.y;
		cz = cz + t*dir.z;

	}



}
