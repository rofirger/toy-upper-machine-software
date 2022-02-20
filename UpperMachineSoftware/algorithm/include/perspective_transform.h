#pragma once
#ifdef PERSPECTIVETRANSFORM_EXPORTS
#define  PERSPECTIVETRANSFORM_API __declspec(dllexport)
#else
#define  PERSPECTIVETRANSFORM_API __declspec(dllimport)
#endif
#include <math.h>
extern"C" PERSPECTIVETRANSFORM_API unsigned char** PerspectiveTransformProcess(unsigned char** src_pixel_mat, size_t src_rows, size_t src_cols, size_t dst_rows, size_t dst_cols, const double* const* perspective_transform_mat);
extern"C" PERSPECTIVETRANSFORM_API void DeleteDstMat(size_t dst_rows);