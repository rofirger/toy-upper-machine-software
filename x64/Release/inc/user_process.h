#pragma once
/*
* @note : 该项目必须在 x64 平台下进行编译
* @warning : 该页面的所有代码请勿改动
*/
// 请预先在项目预处理器定义中加入 USERPROCESS_EXPORTS
#ifdef USERPROCESS_EXPORTS
#define  USERPROCESS_API __declspec(dllexport)
#else
#define  USERPROCESS_API __declspec(dllimport)
#endif
#include <string>
#include <iostream>
typedef struct UserProcessRet
{
	// 已处理完毕的的像素矩阵
	unsigned char** dst_pixel_mat;
	// 已处理完毕的的像素矩阵的行数
	size_t dst_rows;
	// 已处理完毕的的像素矩阵的列数
	size_t dst_cols;
}UserProcessRet;
/*
* @param:
*				src_pixel_mat : 上一处理阶段得到的像素矩阵
*					 src_rows : 上一处理阶段得到的像素矩阵的行数
*					 src_cols : 上一处理阶段得到的像素矩阵的列数
*					left_line : 经辅助线处理后得到的赛道左边界线(若未经辅助线处理该参数为 NULL )
*					 mid_line : 经辅助线处理后得到的赛道中线	(若未经辅助线处理该参数为 NULL )
*				   right_line : 经辅助线处理后得到的赛道右边界线(若未经辅助线处理该参数为 NULL )
*			  threshold_value : 阈值
*	perspective_transform_mat : 3x3 透视变换矩阵
*		 user_data_for_output : 该参数中的数据在每一次更新图像数据时显示于窗体左下角的文本编辑框(注:该参数的数据并不会随着图像更新而清除, 如需删除原有数据请->user_data_for_output.clear())
*
* @brief : 该函数可留空
*/
extern"C" USERPROCESS_API UserProcessRet UserProcess(unsigned char** src_pixel_mat, size_t src_rows, size_t src_cols, size_t * left_line, size_t * mid_line, size_t * right_line, unsigned int threshold_value, const double* const* perspective_transform_mat, std::string & user_data_for_output);
/*
* @brief : 释放已处理完毕的的像素矩阵内存 (若未 malloc 新的像素矩阵，该函数可留空)
*/
extern"C" USERPROCESS_API void UserDeleteDstMat(unsigned char** dst_pixel_mat, size_t dst_rows);