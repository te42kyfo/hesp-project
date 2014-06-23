#include "real.hpp"

float sample( global real* image, unsigned int xcount, unsigned int ycount, unsigned int zcount,
			  float x, float y, float z) {

	int ix = x;
	int iy = y;
	int iz = z;


	float fracx = fmod(x, 1.0f);
	float fracy = fmod(y, 1.0f);
	float fracz = fmod(z, 1.0f);

	unsigned int idx = (int)x + iy*xcount + iz*xcount*ycount;

	float d000 = image[ idx                         ];
	float d001 = image[ idx+1                       ];
	float d010 = image[ idx   +xcount               ];
	float d011 = image[ idx+1 +xcount               ];
	float d100 = image[ idx           +ycount*xcount];
	float d101 = image[ idx+1         +ycount*xcount];
	float d110 = image[ idx   +xcount +ycount*xcount];
	float d111 = image[ idx+1 +xcount +ycount*xcount];

	return mix( mix ( mix(d000, d001, fracx), mix(d010, d011, fracx), fracy),
				mix ( mix(d100, d101, fracx), mix(d110, d111, fracx), fracy), fracz);
}

float3 calculateNormal( global real* density_field,
						unsigned int xcount, unsigned int ycount, unsigned int zcount,
						float cx, float cy, float cz,
						float xmin, float ymin, float zmin,
						float hx, float hy, float hz,
						float ihx, float ihy, float ihz) {
	float3 normal;
	normal.x = ( sample( density_field, xcount, ycount, zcount,
						 (cx+hx-xmin)*ihx,
						 (cy-ymin)*ihy,
						 (cz-zmin)*ihz) -
				 sample( density_field, xcount, ycount, zcount,
						 (cx-hx-xmin)*ihx,
						 (cy-ymin)*ihy,
						 (cz-zmin)*ihz))*0.5 ;
	normal.y = ( sample( density_field, xcount, ycount, zcount,
						 (cx-xmin)*ihx,
						 (cy+hy-ymin)*ihy,
						 (cz-zmin)*ihz) -
				 sample( density_field, xcount, ycount, zcount,
						 (cx-xmin)*ihx,
						 (cy-hy-ymin)*ihy,
						 (cz-zmin)*ihz))*0.5 ;
	normal.z = ( sample( density_field, xcount, ycount, zcount,
						 (cx-xmin)*ihx,
						 (cy-ymin)*ihy,
						 (cz+hz-zmin)*ihz) -
				 sample( density_field, xcount, ycount, zcount,
						 (cx-xmin)*ihx,
						 (cy-ymin)*ihy,
						 (cz-hz-zmin)*ihz))*0.5 ;
	return normal;

}


__kernel void raymarch( global real* density_field,
						unsigned int xcount, unsigned int ycount, unsigned int zcount,
						real xmin, real ymin, real zmin, real xmax, real ymax, real zmax,
						global real* image,	unsigned int width, unsigned int height,
						float4 origin, float4 direction) {

	size_t gidx = get_global_id(0);
	size_t gidy = get_global_id(1);
	size_t globalid = (gidy*width +gidx)*3;
	if( gidx >= width || gidy >= height) return;




	float4 dir = { ( (float) gidx/width*2.0f  -1.0f),
				   ( (float) gidy/height*2.0f -1.0f),
				   1.0f, 0.0f };




	//dir = normalize(dir);


	float hx = (xmax-xmin) / xcount;
	float hy = (ymax-ymin) / ycount;
	float hz = (zmax-zmin) / zcount;
	float ihx = 1.0f / hx;
	float ihy = 1.0f / hy;
	float ihz = 1.0f / hz;



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


	int ix = (cx-xmin)*ihx;
	int iy = (cy-ymin)*ihy;
	int iz = (cz-zmin)*ihz;



	float t = t0;

	float tx = (xmin + (ix+sign(dir.x))*hx -cx)/dir.x;
	float ty = (ymin + (iy+sign(dir.y))*hy -cy)/dir.y;
	float tz = (zmin + (iz+sign(dir.z))*hz -cz)/dir.z;

	float stepsize = min( step_x, min(step_y, step_z));


	image[globalid+0] = 0.f;
	image[globalid+1] = 0.f;
	image[globalid+2] = 0.f;

	float last_density = 0;

	while( ix >= 0 && ix < xcount && iy >= 0 && iy < ycount && iz >= 0 && iz < zcount ) {

		if( ix < xcount-1 && iy < ycount-1 && iz < zcount-1 &&
			ix > 0 && iy >0 && iz >0) {

			float density = sample( density_field, xcount, ycount, zcount,
									(cx-xmin)*ihx, (cy-ymin)*ihy, (cz-zmin)*ihz);


			if( density > 60.0f ) {
				float fine_t = ( density-60.0f) / (density-last_density);

				float fine_cx = cx - fine_t*stepsize*dir.x;
				float fine_cy = cy - fine_t*stepsize*dir.y;
				float fine_cz = cz - fine_t*stepsize*dir.z;

				float3 normal = calculateNormal( density_field, xcount, ycount, zcount,
												 fine_cx, fine_cy, fine_cz,
												 xmin, ymin, zmin, hx, hy, hz, ihx, ihy, ihz );

				normal = normalize( normal);
				image[globalid+0] = (normal.x+1.0)*0.5; //ny*1.0f;//  + ny*-0.5f + nz*1.0f;
				image[globalid+1] = (normal.y+1.0)*0.5; //ny*1.0f;//  + ny*-0.5f + nz*1.0f;
				image[globalid+2] = (normal.z+1.0)*0.5; //ny*1.0f;//  + ny*-0.5f + nz*1.0f;
				break;
			}

			last_density = density;
		}


		t += stepsize;


		cx += stepsize* dir.x;
		cy += stepsize* dir.y;
		cz += stepsize* dir.z;

		ix = (cx-xmin)*ihx;
		iy = (cy-ymin)*ihy;
		iz = (cz-zmin)*ihz;


	}

}
