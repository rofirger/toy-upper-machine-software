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
typedef struct UserRGB
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
	bool is_show;
}UserRGB;
/*
* @param:
*				src_pixel_mat : 上一处理阶段得到的像素矩阵, 请勿自行释放该二维数组内存
*					 src_rows : 上一处理阶段得到的像素矩阵的行数
*					 src_cols : 上一处理阶段得到的像素矩阵的列数
*				 user_rgb_mat : 此参数在显示的位置上与src_pixel_mat一致, 方便在灰度图"上"进行RGB绘图. 注意: 此参数在软件页面上仅提供显示服务
*					left_line : 经辅助线处理后得到的赛道左边界线(若未经辅助线处理该参数为 NULL ), 请勿自行释放该数组内存
*					 mid_line : 经辅助线处理后得到的赛道中线	(若未经辅助线处理该参数为 NULL ), 请勿自行释放该数组内存
*				   right_line : 经辅助线处理后得到的赛道右边界线(若未经辅助线处理该参数为 NULL ), 请勿自行释放该数组内存
*			is_show_left_line : 是否显示左辅助线, 默认为 true
*			 is_show_mid_line : 是否显示中辅助线, 默认为 true
*		   is_show_right_line : 是否显示右辅助线, 默认为 true
*			  threshold_value : 阈值
*	perspective_transform_mat : 3x3 透视变换矩阵
*		  user_data_for_input : 该参数中的数据为该函数执行前窗体左下角可写文本编辑框的文本数据
*		 user_data_for_output : 该参数中的数据在每一次更新图像数据时显示于窗体左下角的只读文本编辑框(注:该参数的数据并不会随着图像更新而清除, 如需删除原有数据请->user_data_for_output.clear())
*						slope : 用于以图像形式直观输出斜率大小. 注意: slope 为正的时 VEER 窗体方向指示直线向 左 倾斜, slope->∞ 时指示直线水平, slope = 0 时指示直线垂直于水平方向
*
* @brief : 该函数可留空
*/
extern"C" USERPROCESS_API UserProcessRet UserProcess(unsigned char** src_pixel_mat, size_t src_rows, size_t src_cols, UserRGB * *user_rgb_mat,
	size_t * left_line, size_t * mid_line, size_t * right_line,
	bool& is_show_left_line, bool& is_show_mid_line, bool& is_show_right_line,
	unsigned int& threshold_value,
	const double* const* perspective_transform_mat,
	const std::string & user_data_for_input, std::string & user_data_for_output,
	double* slope);
/*
* @brief : 释放已处理完毕的的像素矩阵内存 (若未 malloc 新的像素矩阵，该函数可留空)
*/
extern"C" USERPROCESS_API void UserDeleteDstMat(unsigned char** dst_pixel_mat, size_t dst_rows);
