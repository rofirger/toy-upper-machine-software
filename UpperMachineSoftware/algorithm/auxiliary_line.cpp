#include "auxiliary_line.h"
// 上位机
#define UPPER_COMPUTER
// 下位机
//#define LOWER_COMPUTER
#ifdef UPPER_COMPUTER
unsigned char** AuxiliaryProcess(unsigned char** src_pixel_mat, size_t src_rows, size_t src_cols, unsigned char threshold_val, size_t* left_line, size_t* mid_line, size_t* right_line)
#endif // UPPER_COMPUTER
#ifdef LOWER_COMPUTER
void AuxiliaryProcess(unsigned char** src_pixel_mat, uint8_t src_rows, uint8_t src_cols, unsigned char threshold_val, uint8_t* left_line, uint8_t* mid_line, uint8_t* right_line)
#endif // LOWER_COMPUTER
{
#ifdef UPPER_COMPUTER
	size_t mid_point = src_cols >> 1;
	size_t left_right_miss_point = 0;
#endif // UPPER_COMPUTER
#ifdef LOWER_COMPUTER
	uint8_t mid_point = src_cols >> 1;
	uint8_t left_right_miss_point = 0;
#endif // LOWER_COMPUTER
	for (int i = src_rows - 1; i >= 0; --i)
	{
#ifdef UPPER_COMPUTER
		size_t cur_point = mid_point;
#endif // UPPER_COMPUTER
#ifdef LOWER_COMPUTER
		uint8_t cur_point = mid_point;
#endif // LOWER_COMPUTER
		// 扫描左线
		while (cur_point - 2 > 0)
		{
			left_line[i] = 0;
			if (src_pixel_mat[i][cur_point] < threshold_val &&
				src_pixel_mat[i][cur_point - 1] < threshold_val &&
				src_pixel_mat[i][cur_point - 2] < threshold_val)
			{
				left_line[i] = cur_point;
				break;
			}
			--cur_point;
		}
		// 扫描右线
		cur_point = mid_point;
		while (cur_point + 2 < src_cols)
		{
			right_line[i] = src_cols - 1;
			if (src_pixel_mat[i][cur_point] < threshold_val &&
				src_pixel_mat[i][cur_point + 1] < threshold_val &&
				src_pixel_mat[i][cur_point + 2] < threshold_val)
			{
				right_line[i] = cur_point;
				break;
			}
			++cur_point;
		}
		if (left_line[i] == 0 && right_line[i] == src_cols - 1 && left_right_miss_point == 0)
		{
			left_right_miss_point = i;
#ifdef UPPER_COMPUTER
			size_t now_min = src_rows - 1;
			size_t now_min_col = src_cols >> 1;
			size_t begine_fine_point = src_cols >> 3;
			size_t end_find_point = src_cols - begine_fine_point;
#endif // UPPER_COMPUTER
#ifdef LOWER_COMPUTER
			uint8_t now_min = src_rows - 1;
			uint8_t now_min_col = src_cols >> 1;
			uint8_t begine_fine_point = src_cols >> 3;
			uint8_t end_find_point = src_cols - begine_fine_point;
#endif // LOWER_COMPUTER
			// 开启纵向寻线
			for (int j = begine_fine_point; j < end_find_point; ++j)
			{
				for (int k = left_right_miss_point; k > 0; --k)
				{
					if (src_pixel_mat[k][j] > threshold_val &&
						src_pixel_mat[k - 1][j] > threshold_val
						)
					{
						if (now_min > k)
						{
							now_min = k;
							now_min_col = j;
						}
					}
					else
						break;
				}
			}
			mid_point = now_min_col;
			for (int j = now_min; j < left_right_miss_point; ++j)
			{
				cur_point = mid_point;
				while (cur_point - 2 > 0)
				{
					left_line[j] = 0;
					if (src_pixel_mat[j][cur_point] < threshold_val &&
						src_pixel_mat[j][cur_point - 1] < threshold_val &&
						src_pixel_mat[j][cur_point - 2] < threshold_val)
					{
						left_line[j] = cur_point;
						break;
					}
					--cur_point;
				}
				// 扫描右线
				cur_point = mid_point;
				while (cur_point + 2 < src_cols)
				{
					right_line[j] = src_cols - 1;
					if (src_pixel_mat[j][cur_point] < threshold_val &&
						src_pixel_mat[j][cur_point + 1] < threshold_val &&
						src_pixel_mat[j][cur_point + 2] < threshold_val)
					{
						right_line[j] = cur_point;
						break;
					}
					++cur_point;
				}
				mid_line[j] = (right_line[j] + left_line[j]) >> 1;
				mid_point = mid_line[j];
			}
			i = now_min;
		}

		mid_line[i] = (right_line[i] + left_line[i]) >> 1;
		if (left_line[i] != 0 || right_line[i] != src_cols - 1)
		{
			mid_point = mid_line[i];
		}
		else
		{
			mid_point = src_cols >> 1;
		}
	}
#ifdef UPPER_COMPUTER
	return src_pixel_mat;
#endif // UPPER_COMPUTER
}
