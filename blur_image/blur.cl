__kernel void blur_image(
	__read_only image2d_t img1,
	// image2d_t img1,
	__write_only image2d_t img2
) {
	const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;
	int2 coord = (int2)(get_global_id(0), get_global_id(1));
	uint4 val = read_imageui(img1, smp, coord);

	// val.y = 255 - val.y;
	// val.z = 255 - val.z;
	// val.x = 255 - val.x;
	//val.y = val.z = val.x =  0.36 * val.x + 0.53 * val.y + 0.11 * val.z;
	// write_imageui(img2, coord, val);

	const int ksize = 64;
	const int blur_radius = ksize / 2;

	uint4 sum = {0, 0, 0, 0};
	for (int i = -blur_radius; i < blur_radius; ++i)
		for (int j = -blur_radius; j < blur_radius; ++j) {
			int2 c = {coord.x + i, coord.y + j};
			sum += read_imageui(img1, smp, c);
		}
	sum /= ksize * ksize;
	write_imageui(img2, coord, sum);

	// uint4 clr; /// BGRA
	// clr.x = val.x;
	// clr.y = 0;
	// clr.z = 0;
	// write_imageui(img2, coord, clr);
}
    