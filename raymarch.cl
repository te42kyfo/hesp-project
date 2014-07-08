#include "real.hpp"

float sample( global real* image, unsigned int xcount, unsigned int ycount, unsigned int zcount,
	      float x, float y, float z) {

	int ix = x;
	int iy = y;
	int iz = z;

	if( ix == 0 || iy == 0 || iz == 0 ||
	    ix >= xcount-2 || iy >= ycount-2 || iz >= zcount-2 ) {
	  return 0.0;
	}


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

float3 shade( float3 normal, float3 incoming) {

	float3 light1 = { 0.0, -1.0, 0.0 };
	float3 light2 = { -1.0, 0.0, 1.0 };

	light1 = normalize( light1 );
	light2 = normalize( light2 );
	incoming = normalize( incoming );


	float3 reflection =  -(incoming - 2.0f *normal* dot( incoming, normal ));

	float lambert1 = dot( light1, normal );
	float specular1 = dot( light1, reflection );
	float lambert2 = dot( light2, normal );
	float specular2 = dot( light2, reflection );

	lambert1 = max(lambert1, 0.0f);
	specular1 = max(specular1, 0.0f);
	lambert2 = max(lambert2, 0.0f);
	specular2 = max(specular2, 0.0f);


	specular1 = pow( specular1, 60);
	specular2 = pow( specular2, 60);

	float3 color1 = { specular1, specular1, specular1 };
	float3 color2 = { specular2, specular2, specular2 };

    float3 blue1   = { 0.3f * lambert1, 0.3 * lambert1, 0.8f * lambert1  };
    float3 blue2   = { 0.1f * lambert2, 0.1 * lambert2, 0.4f * lambert2  };
	return color1 + color2 + blue1 + blue2;
}

float min4( float4 v) {
	return min( v.x, min(v.y, v.z));
}

__kernel void raymarch( global real* density_field,
			uint4 cell_count,
			float4 lo_bound, float4 hi_bound,
			global real* image,	unsigned int width, unsigned int height,
			float4 origin, float4 direction) {

	size_t gidx = get_global_id(0);
	size_t gidy = get_global_id(1);
	size_t globalid = (gidy*width +gidx)*3;
	if( gidx >= width || gidy >= height) return;

	float4 fcell_count = (float4)(cell_count.x, cell_count.y, cell_count.z, 1.0);

	float4 dir = { ( (float) gidx/width*2.0f  -1.0f),
		       ( (float) gidy/height*2.0f -1.0f),
		       1.0f, 0.0f };

	dir.y = dir.y*0.8 + dir.z*-0.2;
	dir.z = dir.y*0.2 + dir.z*0.8;


	float4 h = (hi_bound-lo_bound) / fcell_count;
	float4 ih = native_recip(h);

	//AABB advance
	float4 txyz0 = min( max( 0.0f, (lo_bound + 3.0f*h - origin) / dir),
			    max( 0.0f, (hi_bound - 3.0f*h - origin) / dir) );

	float t0 = max(txyz0.x, max(txyz0.y, txyz0.z));

	float4 current = origin + t0*dir;
	float stepsize = min4( h / fabs(dir) );

	float t = t0;

	image[globalid+0] = 0.f;
	image[globalid+1] = 0.f;
	image[globalid+2] = 0.f;

	float last_density = 0;

	bool entry = true;

	while( true ) {

	  if( any( (current <= lo_bound+h).xyz) || 
	      any( (current >= hi_bound-h).xyz) ) {
	    break;
	  }

	  float density = sample( density_field, cell_count.x, cell_count.y, cell_count.z,
				  (current.x-lo_bound.x)*ih.x,
				  (current.y-lo_bound.y)*ih.y, (current.z-lo_bound.z)*ih.z);
	  
	  
	  if( density > 0.1f ) {
	    float fine_t = ( density-0.1f) / (density-last_density);
	    
	    float4 fine_position = current - fine_t*stepsize*dir;

	    float3 normal = (float3) ( 0.0, 0.0, 1.0);
	    if(!entry) {
	      normal = calculateNormal( density_field,
					cell_count.x, cell_count.y, cell_count.z,
					fine_position.x, fine_position.y, fine_position.z,
					lo_bound.x, lo_bound.y, lo_bound.z,
					h.x, h.y, h.z, ih.x, ih.y, ih.z );
	    }
	    
	    float3 color = shade( normalize( normal), dir.xyz );
	    image[globalid+0] = color.x; //ny*1.0f;//  + ny*-0.5f + nz*1.0f;
	    image[globalid+1] = color.y; //ny*1.0f;//  + ny*-0.5f + nz*1.0f;
	    image[globalid+2] = color.z; //ny*1.0f;//  + ny*-0.5f + nz*1.0f;
	    break;
	  }
	  
	  last_density = density;
	  //}
	  
	  entry = false;
	  t += stepsize;
	  
	  
	  current += stepsize* dir;
	}

}
