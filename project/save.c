#include <stdio.h>
#include <stdlib.h>
#include <direct.h>
#include <png.h>
#include "save.h"



int save(unsigned char** colors, const char* seed, unsigned int lengthX, unsigned int lengthY)
{
	char file_path[100] = "";
	char t[20] = ""; sprintf_s(t, 20, "%x", (unsigned int)time(NULL));
	strcat(file_path, "map_"); strcat(file_path, t); strcat(file_path, ".png"); //Careful that buffer overflow does not occur here


	png_byte** row_pointers = (png_byte**)malloc(sizeof(png_byte*) * lengthY); //Allocate memory for rows
	if (!row_pointers)
	{
		printf("The program failed to allocate memory for the image.\n");
		return 0;
	}
	
	for (unsigned int i = 0; i < lengthY; i++)
	{
		row_pointers[i] = (png_byte*)malloc(sizeof(png_byte) * lengthX * 4);
		if (!row_pointers[i])
		{
			printf("The program failed to allocate memory for row %i of the image.\n", i);
			for (unsigned int r = 0; r < i; r++)
				free(row_pointers[r]);
			free(row_pointers);
			return 0;
		}

		for (unsigned int j = 0; j < (lengthX * 4); j += 4) //************************************* WATCH OUT FOR POSSIBLE OVERFLOW HERE
		{
			row_pointers[i][j] = colors[i][j];
			row_pointers[i][j + 1] = colors[i][j + 1];
			row_pointers[i][j + 2] = colors[i][j + 2];
			row_pointers[i][j + 3] = colors[i][j + 3];
			//printf("%d %d %d %d\n", colors[i][j], colors[i][j + 1], colors[i][j + 2], colors[i][j + 3]);
		}
	}



	FILE* img;
	fopen_s(&img, file_path, "wb");
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
	png_write_image(png_ptr, row_pointers);
	
	png_write_end(png_ptr, png_info);
	png_destroy_write_struct(&png_ptr, &png_info);

	char cwd[_MAX_PATH + 1] = "";
	if (!_getcwd(cwd, _MAX_PATH + 1)) //********************************************* _MAX_PATH IS NOT AN ABSOLUTE FILE PATH LIMIT
		printf("Map saved as %s in the current working directory\n", file_path);
	else
		printf("Map saved as %s\\%s\n", cwd, file_path);

	for (unsigned int r = 0; r < lengthY; r++)
		free(row_pointers[r]);
	free(row_pointers);
	fclose(img);

	return 1;
}