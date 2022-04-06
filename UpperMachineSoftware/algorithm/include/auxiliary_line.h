#pragma once
#ifdef AUXILIARYLINE_EXPORTS
#define AUXILIARYLINE_API __declspec(dllexport)
#else
#define  AUXILIARYLINE_API __declspec(dllimport)
#endif
typedef struct Pos
{
	int x;
	int y;
}Pos;

typedef struct LineArray
{
	Pos* _array;  // 数组指针, 注意该数组的内存全权交由dll处理
	short _size;  // 数组的大小 
	short _index; // 当前索引
}LineArray;

// 调用该函数的程序不提供左、中、右线内存，线内存管理全权由此dll管理！请务必避免内存泄漏！
extern"C" AUXILIARYLINE_API unsigned char** AuxiliaryProcess(unsigned char** src_pixel_mat, int src_rows, int src_cols, unsigned char threshold_val, LineArray * left_line, LineArray * mid_line, LineArray * right_line);
extern"C" AUXILIARYLINE_API void DeleteAuxiliaryProcessLines(LineArray * left_line, LineArray * mid_line, LineArray * right_line);