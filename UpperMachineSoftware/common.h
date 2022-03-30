#pragma once
typedef struct Pos
{
	int x;
	int y;
}Pos;

typedef struct LineArray
{
	Pos* _array;  // 数组指针, 注意该数组的内存全权交由dll处理
	short _size;  // 数组的大小 
	short _index; // 下次将使用的索引，注意！该值
}LineArray;