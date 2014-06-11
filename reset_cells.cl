__kernel void reset_cells(  const unsigned int N,
							global int* cells) {
	const int globalid = get_global_id(0);
	if( globalid >= N ) return;

	cells[globalid] = -1;
}
