#include <string.h>
#include <alloca.h>
#include "opengl.h"
#include "image.h"
#include "imago2.h"


static unsigned int next_pow2(unsigned int x);

Image::Image()
{
	width = height = tex_width = tex_height = 0;
	pixels = 0;
	tex = 0;
	tex_valid = false;
}

Image::~Image()
{
	destroy();
}

Image::Image(const Image &img)
{
	pixels = 0;
	create(img.width, img.height, img.pixels);
}

Image &Image::operator =(const Image &img)
{
	if(&img != this) {
		create(img.width, img.height, img.pixels);
	}
	return *this;
}

bool Image::create(int width, int height, unsigned char *pixels)
{
	destroy();

	try {
		unsigned char *tmp = new unsigned char[width * height * 4];
		this->pixels = tmp;
		this->width = width;
		this->height = height;
	}
	catch(...) {
		return false;
	}

	if(pixels) {
		memcpy(this->pixels, pixels, width * height * 4);
	}
	return true;
}

void Image::destroy()
{
	delete [] pixels;
	pixels = 0;
	width = height = 0;

	if(tex) {
		glDeleteTextures(1, &tex);
		tex = 0;
		tex_valid = false;
	}
}

bool Image::load(const char *fname)
{
	int xsz, ysz;
	unsigned char *pix = (unsigned char*)img_load_pixels(fname, &xsz, &ysz, IMG_FMT_RGBA32);
	if(!pix) {
		return false;
	}
	return create(xsz, ysz, pix);
}

unsigned int Image::texture() const
{
	if(!pixels) {
		return 0;
	}
	if(!tex) {
		glGenTextures(1, &tex);
	}

	if(!tex_valid) {
		tex_width = next_pow2(width);
		tex_height = next_pow2(height);

		glBindTexture(GL_TEXTURE_2D, tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		if(GLEW_SGIS_generate_mipmap) {
			glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, 1);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
					width == tex_width && height == tex_height ? pixels : 0);
			if(width != tex_width || height != tex_height) {
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
			}
		} else {
			gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, tex_width, tex_height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
		}

		if(GLEW_EXT_texture_filter_anisotropic) {
			static float max_aniso = -1.0;

			if(max_aniso < 0.0) {
				glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_aniso);
				printf("using anisotropic filtering: x%g\n", max_aniso);
			}

			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_aniso);
		}
		tex_valid = true;
	}
	return tex;
}

int Image::texture_width() const
{
	return tex_width;
}

int Image::texture_height() const
{
	return tex_height;
}

void Image::invalidate_texture()
{
	tex_valid = false;
}


void clear_image(Image *img)
{
	clear_image(img, 0, 0, 0, 255);
}

void clear_image(Image *img, float r, float g, float b, float a)
{
	if(!img->pixels) {
		return;
	}

	unsigned char col[4];
	unsigned char *ptr = img->pixels;
	int npix = img->width * img->height;

	col[0] = (int)(r * 255.0);
	col[1] = (int)(g * 255.0);
	col[2] = (int)(b * 255.0);
	col[3] = (int)(a * 255.0);

	for(int i=0; i<npix; i++) {
		for(int j=0; j<4; j++) {
			ptr[j] = col[j];
		}
		ptr += 4;
	}
}

void clear_image_alpha(Image *img, float a)
{
	if(!img->pixels) {
		return;
	}

	unsigned char alpha = (int)(a * 255.0);
	unsigned char *ptr = img->pixels;
	int npix = img->width * img->height;

	for(int i=0; i<npix; i++) {
		ptr[3] = alpha;
		ptr += 4;
	}
}

bool combine_image(Image *dest, const Image *aimg, const Image *bimg, ImgCombine op, float t)
{
	int xsz = dest->width;
	int ysz = dest->height;
	int npixels = xsz * ysz;
	int nbytes = npixels * 4;
	int tint = (int)(t * 255);

	if(aimg->width != xsz || bimg->width != xsz || aimg->height != ysz || bimg->height != ysz) {
		return false;
	}

	unsigned char *dptr = dest->pixels;
	const unsigned char *aptr = aimg->pixels;
	const unsigned char *bptr = bimg->pixels;

	switch(op) {
	case IMG_OP_ADD:
		for(int i=0; i<nbytes; i++) {
			unsigned int x = *aptr++ + *bptr++;
			*dptr++ = x > 255 ? 255 : x;
		}
		break;

	case IMG_OP_SUB:
		for(int i=0; i<nbytes; i++) {
			int x = (int)*aptr++ - (int)*bptr++;
			*dptr++ = x < 0 ? 0 : x;
		}
		break;

	case IMG_OP_MUL:
		for(int i=0; i<nbytes; i++) {
			unsigned int x = ((unsigned int)*aptr++ * (unsigned int)*bptr++) >> 8;
			*dptr++ = x > 255 ? 255 : x;
		}
		break;

	case IMG_OP_LERP:
		for(int i=0; i<nbytes; i++) {
			int x = (int)*aptr + ((((int)*bptr - (int)*aptr) * tint) >> 8);
			*dptr++ = x > 255 ? 255 : (x < 0 ? 0 : x);
		}
		break;

	default:
		break;
	}

	dest->invalidate_texture();
	return true;
}

void convolve_horiz_image(Image *dest, float *kern, int ksz, float scale)
{
	if((ksz & 1) == 0) {
		fprintf(stderr, "%s: kernel size (%d) must be odd, skipping last value\n", __FUNCTION__, ksz);
		--ksz;
	}
	if(scale == 0.0) {
		// calculate scale factor
		float sum = 0.0;
		for(int i=0; i<ksz; i++) {
			sum += kern[i];
		}
		scale = 1.0 / sum;
	}
	int krad = ksz / 2;
	float *buf = (float*)alloca(dest->width * 4 * sizeof *buf);
	unsigned char *sptr = dest->pixels;

	for(int i=0; i<dest->height; i++) {
		float *bptr = buf;
		for(int j=0; j<dest->width * 4; j++) {
			*bptr++ = (float)(sptr[j] / 255.0);
		}

		for(int j=0; j<dest->width; j++) {
			float col[] = {0, 0, 0, 0};

			for(int k=0; k<ksz; k++) {
				int idx = j + k - krad;
				if(idx < 0) idx = 0;
				if(idx >= dest->width) idx = dest->width - 1;

				col[0] += buf[idx * 4] * kern[k];
				col[1] += buf[idx * 4 + 1] * kern[k];
				col[2] += buf[idx * 4 + 2] * kern[k];
				col[3] += buf[idx * 4 + 3] * kern[k];
			}

			int ri = (int)(col[0] * scale * 255.0);
			int gi = (int)(col[1] * scale * 255.0);
			int bi = (int)(col[2] * scale * 255.0);
			int ai = (int)(col[3] * scale * 255.0);

			sptr[0] = ri < 0 ? 0 : (ri > 255 ? 255 : ri);
			sptr[1] = gi < 0 ? 0 : (gi > 255 ? 255 : gi);
			sptr[2] = bi < 0 ? 0 : (bi > 255 ? 255 : bi);
			sptr[3] = ai < 0 ? 0 : (ai > 255 ? 255 : ai);
			sptr += 4;
		}
	}

	dest->invalidate_texture();
}

void convolve_vert_image(Image *dest, float *kern, int ksz, float scale)
{
	if((ksz & 1) == 0) {
		fprintf(stderr, "%s: kernel size (%d) must be odd, skipping last value\n", __FUNCTION__, ksz);
		--ksz;
	}
	if(scale == 0.0) {
		// calculate scale factor
		float sum = 0.0;
		for(int i=0; i<ksz; i++) {
			sum += kern[i];
		}
		scale = 1.0 / sum;
	}
	int krad = ksz / 2;
	float *buf = (float*)alloca(dest->height * 4 * sizeof *buf);
	unsigned char *sptr = dest->pixels;

	for(int i=0; i<dest->width; i++) {
		float *bptr = buf;
		sptr = dest->pixels + i * 4;

		for(int j=0; j<dest->height; j++) {
			for(int k=0; k<4; k++) {
				*bptr++ = (float)(sptr[k] / 255.0);
			}
			sptr += dest->width * 4;
		}

		sptr = dest->pixels + i * 4;

		for(int j=0; j<dest->height; j++) {
			float col[] = {0, 0, 0, 0};

			for(int k=0; k<ksz; k++) {
				int idx = j + k - krad;
				if(idx < 0) idx = 0;
				if(idx >= dest->height) idx = dest->height - 1;

				col[0] += buf[idx * 4] * kern[k];
				col[1] += buf[idx * 4 + 1] * kern[k];
				col[2] += buf[idx * 4 + 2] * kern[k];
				col[3] += buf[idx * 4 + 3] * kern[k];
			}

			int ri = (int)(col[0] * scale * 255.0);
			int gi = (int)(col[1] * scale * 255.0);
			int bi = (int)(col[2] * scale * 255.0);
			int ai = (int)(col[3] * scale * 255.0);

			sptr[0] = ri < 0 ? 0 : (ri > 255 ? 255 : ri);
			sptr[1] = gi < 0 ? 0 : (gi > 255 ? 255 : gi);
			sptr[2] = bi < 0 ? 0 : (bi > 255 ? 255 : bi);
			sptr[3] = ai < 0 ? 0 : (ai > 255 ? 255 : ai);
			sptr += dest->width * 4;
		}
	}
}

static unsigned int next_pow2(unsigned int x)
{
	x--;
	x = (x >> 1) | x;
	x = (x >> 2) | x;
	x = (x >> 4) | x;
	x = (x >> 8) | x;
	x = (x >> 16) | x;
	return x + 1;
}
