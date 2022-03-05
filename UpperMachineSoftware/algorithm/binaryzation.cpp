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
unsigned char OTSUThreshold(int* HistGram)
{
	int X, Y, Amount = 0;
	int PixelBack = 0, PixelFore = 0, PixelIntegralBack = 0, PixelIntegralFore = 0, PixelIntegral = 0;
	double OmegaBack, OmegaFore, MicroBack, MicroFore, SigmaB, Sigma;              // 类间方差;
	int MinValue, MaxValue;
	int Threshold = 0;

	for (MinValue = 0; MinValue < 256 && HistGram[MinValue] == 0; MinValue++);
	for (MaxValue = 255; MaxValue > MinValue && HistGram[MinValue] == 0; MaxValue--);
	if (MaxValue == MinValue) return MaxValue;          // 图像中只有一个颜色             
	if (MinValue + 1 == MaxValue) return MinValue;      // 图像中只有二个颜色

	for (Y = MinValue; Y <= MaxValue; Y++) Amount += HistGram[Y];        //  像素总数

	PixelIntegral = 0;
	for (Y = MinValue; Y <= MaxValue; Y++) PixelIntegral += HistGram[Y] * Y;
	SigmaB = -1;
	for (Y = MinValue; Y < MaxValue; Y++)
	{
		PixelBack = PixelBack + HistGram[Y];
		PixelFore = Amount - PixelBack;
		OmegaBack = (double)PixelBack / Amount;
		OmegaFore = (double)PixelFore / Amount;
		PixelIntegralBack += HistGram[Y] * Y;
		PixelIntegralFore = PixelIntegral - PixelIntegralBack;
		MicroBack = (double)PixelIntegralBack / PixelBack;
		MicroFore = (double)PixelIntegralFore / PixelFore;
		Sigma = OmegaBack * OmegaFore * (MicroBack - MicroFore) * (MicroBack - MicroFore);
		if (Sigma > SigmaB)
		{
			SigmaB = Sigma;
			Threshold = Y;
		}
	}
	return Threshold;
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
	return pixel_mat;
}