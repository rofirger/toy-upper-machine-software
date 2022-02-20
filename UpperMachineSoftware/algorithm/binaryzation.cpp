#include "binaryzation.h"
unsigned char** BinaryzationProcess(unsigned char** pixel_mat, int rows, int cols, unsigned int threshold_value)
{
	for (int i = 0; i < rows; ++i)
	{
		for (int j = 0; j < cols; ++j)
		{
			pixel_mat[i][j] = pixel_mat[i][j] > threshold_value ? 255 : 0;
		}
	}
	return pixel_mat;
}