#include "auxiliary_line.h"
unsigned char** AuxiliaryProcess(unsigned char** src_pixel_mat, size_t src_rows, size_t src_cols, unsigned char threshold_val, size_t* left_line, size_t* mid_line, size_t* right_line)
{
	size_t mid_point = src_cols >> 1;
	for (int i = src_rows - 1; i >= 0; --i)
	{
		size_t cur_point = mid_point;
		// É¨Ãè×óÏß
		while (cur_point - 2 > 0)
		{
			left_line[i] = 0;
			if (src_pixel_mat[i][cur_point] < threshold_val &&
				src_pixel_mat[i][cur_point - 1] < threshold_val &&
				src_pixel_mat[i][cur_point - 2] < threshold_val)
			{
				src_pixel_mat[i][cur_point] = 0;
				left_line[i] = cur_point;
				break;
			}
			--cur_point;
		}
		// É¨ÃèÓÒÏß
		cur_point = mid_point;
		while (cur_point + 2 < src_cols)
		{
			right_line[i] = src_cols - 1;
			if (src_pixel_mat[i][cur_point] < threshold_val &&
				src_pixel_mat[i][cur_point + 1] < threshold_val &&
				src_pixel_mat[i][cur_point + 2] < threshold_val)
			{
				src_pixel_mat[i][cur_point] = 0;
				right_line[i] = cur_point;
				break;
			}
			++cur_point;
		}
		mid_line[i] = (right_line[i] + left_line[i]) >> 1;
		src_pixel_mat[i][mid_line[i]] = 0;
		mid_point = mid_line[i];
	}
	return src_pixel_mat;
}