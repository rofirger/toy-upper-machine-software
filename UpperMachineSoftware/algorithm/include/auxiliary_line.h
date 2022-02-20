#pragma once
#ifdef AUXILIARYLINE_EXPORTS
#define AUXILIARYLINE_API __declspec(dllexport)
#else
#define  AUXILIARYLINE_API __declspec(dllimport)
#endif
// 调用该函数的程序提供左、中、右线内存
extern"C" AUXILIARYLINE_API unsigned char** AuxiliaryProcess(unsigned char** src_pixel_mat, size_t src_rows, size_t src_cols, unsigned char threshold_val, size_t * left_line, size_t * mid_line, size_t * right_line);
