#include "binaryzation.h"
#include<math.h>
void GetHistGram(unsigned char** image, int width, int height, int* hist_gram)
{
	for (int i_ = 0; i_ < 256; ++i_)
	{
		hist_gram[i_] = 0;
	}
	for (int i_ = 0; i_ < height; ++i_)
	{
		for (int j_ = 0; j_ < width; ++j_)
		{
			hist_gram[image[i_][j_]]++;
		}
	}
}
unsigned char OTSUThreshold(int* hist_gram)
{
	int X, Y, amount = 0;
	int pixel_back = 0, pixel_fore = 0, pixel_integral_back = 0, pixel_integral_fore = 0, pixel_integral = 0;
	double OmegaBack, OmegaFore, MicroBack, MicroFore, SigmaB, Sigma; // 类间方差;
	int pixel_min_value, pixel_max_value;
	int threshold = 0;

	for (pixel_min_value = 0; pixel_min_value < 256 && hist_gram[pixel_min_value] == 0; pixel_min_value++)
		;
	for (pixel_max_value = 255; pixel_max_value > pixel_min_value && hist_gram[pixel_max_value] == 0; pixel_max_value--)
		;
	if (pixel_max_value == pixel_min_value)
		return pixel_max_value; // 图像中只有一个颜色
	if (pixel_min_value + 1 == pixel_max_value)
		return pixel_min_value; // 图像中只有二个颜色

	for (Y = pixel_min_value; Y <= pixel_max_value; Y++)
		amount += hist_gram[Y]; //  像素总数

	pixel_integral = 0;
	for (Y = pixel_min_value; Y <= pixel_max_value; Y++)
		pixel_integral += hist_gram[Y] * Y;
	SigmaB = -1;
	for (Y = pixel_min_value; Y < pixel_max_value; Y++)
	{
		pixel_back = pixel_back + hist_gram[Y];
		pixel_fore = amount - pixel_back;
		OmegaBack = (double)pixel_back / amount;
		OmegaFore = (double)pixel_fore / amount;
		pixel_integral_back += hist_gram[Y] * Y;
		pixel_integral_fore = pixel_integral - pixel_integral_back;
		MicroBack = (double)pixel_integral_back / pixel_back;
		MicroFore = (double)pixel_integral_fore / pixel_fore;
		Sigma = OmegaBack * OmegaFore * (MicroBack - MicroFore) * (MicroBack - MicroFore);
		if (Sigma > SigmaB)
		{
			SigmaB = Sigma;
			threshold = Y;
		}
	}
	return threshold;
}
int hist_gram[256];
unsigned char** BinaryzationProcess(unsigned char** pixel_mat, int rows, int cols, unsigned char& threshold_value)
{
	GetHistGram(pixel_mat, cols, rows, hist_gram);
	int th = OTSUThreshold(hist_gram);
	for (int i = 0; i < rows; ++i)
	{
		for (int j = 0; j < cols; ++j)
		{
			pixel_mat[i][j] = pixel_mat[i][j] > th ? 255 : 0;
		}
	}
	threshold_value = th;
	return pixel_mat;
}