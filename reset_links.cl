__kernel void reset_links(  const unsigned int N,
							global int* links) {
	const int globalid = get_global_id(0);
	if( globalid >= N ) return;

	links[globalid] = globalid;
}
