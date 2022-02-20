#pragma once
#ifndef _MATRIX_H_
#define _MATRIX_H_
namespace My
{
	typedef struct {
		int row, col;
		float** element;
		unsigned char init;
	}Mat;
}
My::Mat* MatCreate(My::Mat* mat, int row, int col);
void MatDelete(My::Mat* mat);
My::Mat* MatSetVal(My::Mat* mat, float* val);
void MatDump(const My::Mat* mat);

My::Mat* MatZeros(My::Mat* mat);
My::Mat* MatEye(My::Mat* mat);

My::Mat* MatAdd(My::Mat* src1, My::Mat* src2, My::Mat* dst);
My::Mat* MatSub(My::Mat* src1, My::Mat* src2, My::Mat* dst);
My::Mat* MatMul(My::Mat* src1, My::Mat* src2, My::Mat* dst);
My::Mat* MatTrans(My::Mat* src, My::Mat* dst);
float MatDet(My::Mat* mat);
My::Mat* MatAdj(My::Mat* src, My::Mat* dst);
My::Mat* MatInv(My::Mat* src, My::Mat* dst);

#endif