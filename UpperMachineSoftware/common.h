#pragma once
typedef struct Pos
{
	int x;
	int y;
}Pos;

typedef struct LineArray
{
	Pos* _array;  // ����ָ��, ע���������ڴ�ȫȨ����dll����
	short _size;  // ����Ĵ�С 
	short _index; // �´ν�ʹ�õ�������ע�⣡��ֵ
}LineArray;