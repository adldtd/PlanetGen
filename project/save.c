#include <stdio.h>
#include <stdlib.h>
#include <direct.h>
#include <png.h>
#include "save.h"



int save(unsigned char **colors, const char *seed, unsigned int lengthX, unsigned int lengthY, const char *fp, int fp_len)
{

	FILE *img;
	fopen_s(&img, fp, "wb");
	if (!img)
	{
		printf("An error occured while constructing the file.\n");
		return 0;
	}

	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr)
	{
		return 0;
	}

	png_infop png_info = png_create_info_struct(png_ptr);
	if (!png_info)
	{
		png_destroy_write_struct(&png_ptr, &png_info);
		printf("An error occurred while constructing PNG information.\n");
		return 0;
	}

	png_init_io(png_ptr, img);
	//png_set_compression_level(png_ptr, );
	
	png_set_IHDR(png_ptr, png_info,
				 lengthX, lengthY,
				 8, PNG_COLOR_TYPE_RGBA,
				 PNG_INTERLACE_NONE,
				 PNG_COMPRESSION_TYPE_DEFAULT,
				 PNG_FILTER_TYPE_DEFAULT);

	//png_set_invert_alpha(png_ptr);
	png_write_info(png_ptr, png_info);
	png_write_image(png_ptr, colors);
	
	png_write_end(png_ptr, png_info);
	png_destroy_write_struct(&png_ptr, &png_info);

	fclose(img);

	return 1;
}