void __kernel add( __global double* A, __global double* B, __global double * C) {
	size_t globalx = get_global_id(0);
	size_t globaly = get_global_id(1);

	if( globalx >= 1023 || globalx == 0 ||
		globaly >= 1023 || globaly == 0) {
		return;
	}

	size_t idx = globaly*1024 + globalx;

	C[idx] = ( A[idx - 1024] + A[idx + 1024] +  A[idx+1] + A[idx-1]) *0.25;

}
