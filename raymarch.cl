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


	float tx0 = min( max( 0.0f, (xmin - origin.x) / dir.x),
					max( 0.0f, (xmax - origin.x) / dir.x) );
	float ty0 = min( max( 0.0f, (ymin - origin.y) / dir.y),
					max( 0.0f, (ymax - origin.y) / dir.y) );
	float tz0 = min( max( 0.0f, (zmin - origin.z) / dir.z),
					max( 0.0f, (zmax - origin.z) / dir.z));
	float t0 = max(tx0, max(tz0, ty0));


	float cx = origin.x + t0*dir.x;
	float cy = origin.y + t0*dir.y;
	float cz = origin.z + t0*dir.z;


	float step_x = hx/fabs( dir.x );
	float step_y = hy/fabs( dir.y );
	float step_z = hz/fabs( dir.z );

	int ix = (cx-xmin) *ihx;
	int iy = (cy-ymin) *ihy;
	int iz = (cz-zmin) *ihz;

	image[globalid+0] = 0.0;
	image[globalid+1] = 0.0;
	image[globalid+2] = 0.10;

	float t = t0;

	float tx = (xmin + (ix+sign(dir.x))*hx -cx)/dir.x;
	float ty = (ymin + (iy+sign(dir.y))*hy -cy)/dir.y;
	float tz = (zmin + (iz+sign(dir.z))*hz -cz)/dir.z;

	if( isnan(tx) ) tx = INFINITY;
	if( isnan(ty) ) ty = INFINITY;
	if( isnan(tz) ) tz = INFINITY;

	while(true) {



		if( ix < 0 || ix >= xcount || iy < 0 || iy >= ycount || iz<0 || iz >= zcount) break;

		float density = 0;

		if( ix < xcount-1 && iy < ycount-1 && iz < zcount-1 &&
			ix > 0 && iy >0 && iz >0) {

			float fracx = fmod((cx - xmin)*ihx, 1.0f);
			float fracy = fmod((cy - ymin)*ihy, 1.0f);
			float fracz = fmod((cz - zmin)*ihz, 1.0f);

			unsigned int idx = ix + iy*xcount + iz*xcount*ycount;

			float d000 = density_field[ idx                         ];
			float d001 = density_field[ idx+1                       ];
			float d010 = density_field[ idx   +xcount               ];
			float d011 = density_field[ idx+1 +xcount               ];
			float d100 = density_field[ idx           +ycount*xcount];
			float d101 = density_field[ idx+1         +ycount*xcount];
			float d110 = density_field[ idx   +xcount +ycount*xcount];
			float d111 = density_field[ idx+1 +xcount +ycount*xcount];

			density = mix( mix ( mix(d000, d001, fracx), mix(d010, d011, fracx), fracy),
						   mix ( mix(d100, d101, fracx), mix(d110, d111, fracx), fracy), fracz);


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


		float temp_tx = tx;
		float temp_ty = ty;
		float temp_tz = tz;

		float next_step;
		if( tx < ty && tx < tz) {
			next_step = tx;
			ix += sign(dir.x);
			tx += step_x;
		} else if( ty < tz ) {
			next_step = ty;
			iy += sign(dir.y);
			ty += step_y;
		} else {
			next_step = tz;
			iz += sign(dir.z);
			tz += step_z;
		}


		t += next_step;
		tx -= next_step;
		ty -= next_step;
		tz -= next_step;

		//	printf( "%f, %f, %f | %f | %f, %f, %f\n", temp_tx, temp_ty, temp_tz, next_step,
		//		tx, ty, tz);


		cx += next_step*dir.x;
		cy += next_step*dir.y;
		cz += next_step*dir.z;

	}
	


}
