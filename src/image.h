#ifndef IMAGE_H_
#define IMAGE_H_

class Image {
private:
	mutable int tex_width, tex_height;
	mutable unsigned int tex;
	mutable bool tex_valid;

public:
	int width, height;
	unsigned char *pixels;

	Image();
	~Image();

	Image(const Image &img);
	Image &operator =(const Image &img);

	bool create(int width, int height, unsigned char *pixels = 0);
	void destroy();

	bool load(const char *fname);

	unsigned int texture() const;
	int texture_width() const;
	int texture_height() const;

	void invalidate_texture();
};

void clear_image(Image *img);
void clear_image(Image *img, float r, float g, float b, float a = 255);
void clear_image_alpha(Image *img, float a);

enum ImgCombine {
	IMG_OP_ADD,
	IMG_OP_SUB,
	IMG_OP_MUL,
	IMG_OP_LERP
};

bool combine_image(Image *dest, const Image *aimg, const Image *bimg, ImgCombine op = IMG_OP_LERP, float t = 0.5);

void convolve_horiz_image(Image *dest, float *kern, int ksz, float scale = 0.0);
void convolve_vert_image(Image *dest, float *kern, int ksz, float scale = 0.0);

#endif	// IMAGE_H_
