#include <string.h>
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
