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
	Pos* _array;  // ����ָ��, ע���������ڴ�ȫȨ����dll����
	short _size;  // ����Ĵ�С 
	short _index; // ��ǰ����
}LineArray;

// ���øú����ĳ����ṩ���С������ڴ棬���ڴ����ȫȨ�ɴ�dll��������ر����ڴ�й©��
extern"C" AUXILIARYLINE_API unsigned char** AuxiliaryProcess(unsigned char** src_pixel_mat, int src_rows, int src_cols, unsigned char threshold_val, LineArray * left_line, LineArray * mid_line, LineArray * right_line);
extern"C" AUXILIARYLINE_API void DeleteAuxiliaryProcessLines(LineArray * left_line, LineArray * mid_line, LineArray * right_line);