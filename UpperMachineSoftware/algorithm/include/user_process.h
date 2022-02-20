#pragma once
/*
* @note : ����Ŀ������ x64 ƽ̨�½��б���
* @warning : ��ҳ������д�������Ķ�
*/
// ��Ԥ������ĿԤ�����������м��� USERPROCESS_EXPORTS
#ifdef USERPROCESS_EXPORTS
#define  USERPROCESS_API __declspec(dllexport)
#else
#define  USERPROCESS_API __declspec(dllimport)
#endif
#include <string>
#include <iostream>
typedef struct UserProcessRet
{
	// �Ѵ�����ϵĵ����ؾ���
	unsigned char** dst_pixel_mat;
	// �Ѵ�����ϵĵ����ؾ��������
	size_t dst_rows;
	// �Ѵ�����ϵĵ����ؾ��������
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
*				src_pixel_mat : ��һ����׶εõ������ؾ���, ���������ͷŸö�ά�����ڴ�
*					 src_rows : ��һ����׶εõ������ؾ��������
*					 src_cols : ��һ����׶εõ������ؾ��������
*				 user_rgb_mat : �˲�������ʾ��λ������src_pixel_matһ��, �����ڻҶ�ͼ"��"����RGB��ͼ. ע��: �˲��������ҳ���Ͻ��ṩ��ʾ����
*					left_line : �������ߴ����õ���������߽���(��δ�������ߴ���ò���Ϊ NULL ), ���������ͷŸ������ڴ�
*					 mid_line : �������ߴ����õ�����������	(��δ�������ߴ���ò���Ϊ NULL ), ���������ͷŸ������ڴ�
*				   right_line : �������ߴ����õ��������ұ߽���(��δ�������ߴ���ò���Ϊ NULL ), ���������ͷŸ������ڴ�
*			is_show_left_line : �Ƿ���ʾ������, Ĭ��Ϊ true
*			 is_show_mid_line : �Ƿ���ʾ�и�����, Ĭ��Ϊ true
*		   is_show_right_line : �Ƿ���ʾ�Ҹ�����, Ĭ��Ϊ true
*			  threshold_value : ��ֵ
*	perspective_transform_mat : 3x3 ͸�ӱ任����
*		  user_data_for_input : �ò����е�����Ϊ�ú���ִ��ǰ�������½ǿ�д�ı��༭����ı�����
*		 user_data_for_output : �ò����е�������ÿһ�θ���ͼ������ʱ��ʾ�ڴ������½ǵ�ֻ���ı��༭��(ע:�ò��������ݲ���������ͼ����¶����, ����ɾ��ԭ��������->user_data_for_output.clear())
*						slope : ������ͼ����ʽֱ�����б�ʴ�С. ע��: slope Ϊ����ʱ VEER ���巽��ָʾֱ���� �� ��б, slope->�� ʱָʾֱ��ˮƽ, slope = 0 ʱָʾֱ�ߴ�ֱ��ˮƽ����
*
* @brief : �ú���������
*/
extern"C" USERPROCESS_API UserProcessRet UserProcess(unsigned char** src_pixel_mat, size_t src_rows, size_t src_cols, UserRGB * *user_rgb_mat,
	size_t * left_line, size_t * mid_line, size_t * right_line,
	bool& is_show_left_line, bool& is_show_mid_line, bool& is_show_right_line,
	unsigned int& threshold_value,
	const double* const* perspective_transform_mat,
	const std::string & user_data_for_input, std::string & user_data_for_output,
	double* slope);
/*
* @brief : �ͷ��Ѵ�����ϵĵ����ؾ����ڴ� (��δ malloc �µ����ؾ��󣬸ú���������)
*/
extern"C" USERPROCESS_API void UserDeleteDstMat(unsigned char** dst_pixel_mat, size_t dst_rows);
