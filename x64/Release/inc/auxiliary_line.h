#pragma once
#ifdef AUXILIARYLINE_EXPORTS
#define AUXILIARYLINE_API __declspec(dllexport)
#else
#define  AUXILIARYLINE_API __declspec(dllimport)
#endif
// ���øú����ĳ����ṩ���С������ڴ�
extern"C" AUXILIARYLINE_API unsigned char** AuxiliaryProcess(unsigned char** src_pixel_mat, size_t src_rows, size_t src_cols, unsigned char threshold_val, size_t * left_line, size_t * mid_line, size_t * right_line);
