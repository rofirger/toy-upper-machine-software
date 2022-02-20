#include "perspective_transform.h"
unsigned char** dst_pixel_mat = NULL;
unsigned char** PerspectiveTransformProcess(unsigned char** src_pixel_mat, size_t src_rows, size_t src_cols, size_t dst_rows, size_t dst_cols, const double* const* perspective_transform_mat)
{
	dst_pixel_mat = new unsigned char* [dst_rows];
	for (size_t i = 0; i < dst_rows; ++i)
	{
		dst_pixel_mat[i] = new unsigned char[dst_cols];
	}
	for (size_t i = 0; i < dst_rows; ++i)
	{
		for (size_t j = 0; j < dst_cols; ++j)
		{
			dst_pixel_mat[i][j] = 0;
		}
	}
	for (size_t i = 0; i < src_cols; ++i)
	{
		for (size_t j = 0; j < src_rows; ++j)
		{
			double x, y, w;
			x = perspective_transform_mat[0][0] * i + perspective_transform_mat[0][1] * j + perspective_transform_mat[0][2];
			y = perspective_transform_mat[1][0] * i + perspective_transform_mat[1][1] * j + perspective_transform_mat[1][2];
			w = perspective_transform_mat[2][0] * i + perspective_transform_mat[2][1] * j + perspective_transform_mat[2][2];
			int dst_col = (int)round(x / w);
			int dst_row = (int)round(y / w);
			if (dst_col >= 0 && dst_col < dst_cols
				&& dst_row >= 0 && dst_row < dst_rows)
			{
				dst_pixel_mat[dst_row][dst_col] = src_pixel_mat[j][i];
			}
		}
	}
	return dst_pixel_mat;
}
void DeleteDstMat(size_t dst_rows)
{
	if (NULL != dst_pixel_mat)
	{
		for (size_t i = 0; i < dst_rows; ++i)
		{
			delete[]dst_pixel_mat[i];
		}
		delete[]dst_pixel_mat;
	}
}