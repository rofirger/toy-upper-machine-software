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
/*
* @param:
*				src_pixel_mat : ��һ����׶εõ������ؾ���
*					 src_rows : ��һ����׶εõ������ؾ��������
*					 src_cols : ��һ����׶εõ������ؾ��������
*					left_line : �������ߴ����õ���������߽���(��δ�������ߴ���ò���Ϊ NULL )
*					 mid_line : �������ߴ����õ�����������	(��δ�������ߴ���ò���Ϊ NULL )
*				   right_line : �������ߴ����õ��������ұ߽���(��δ�������ߴ���ò���Ϊ NULL )
*			  threshold_value : ��ֵ
*	perspective_transform_mat : 3x3 ͸�ӱ任����
*		 user_data_for_output : �ò����е�������ÿһ�θ���ͼ������ʱ��ʾ�ڴ������½ǵ��ı��༭��(ע:�ò��������ݲ���������ͼ����¶����, ����ɾ��ԭ��������->user_data_for_output.clear())
*
* @brief : �ú���������
*/
extern"C" USERPROCESS_API UserProcessRet UserProcess(unsigned char** src_pixel_mat, size_t src_rows, size_t src_cols, size_t * left_line, size_t * mid_line, size_t * right_line, unsigned int threshold_value, const double* const* perspective_transform_mat, std::string & user_data_for_output);
/*
* @brief : �ͷ��Ѵ�����ϵĵ����ؾ����ڴ� (��δ malloc �µ����ؾ��󣬸ú���������)
*/
extern"C" USERPROCESS_API void UserDeleteDstMat(unsigned char** dst_pixel_mat, size_t dst_rows);