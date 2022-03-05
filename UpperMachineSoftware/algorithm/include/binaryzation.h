#pragma once
// 工程预先添加 BINARYZATION_EXPORTS 宏
#ifdef BINARYZATION_EXPORTS
#define  BINARYZATION_API __declspec(dllexport)
#else
#define  BINARYZATION_API __declspec(dllimport)
#endif
extern"C" BINARYZATION_API unsigned char** BinaryzationProcess(unsigned char** pixel_mat, int rows, int cols, unsigned char& threshold_value);