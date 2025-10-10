#ifndef QMath_H
#define QMath_H

#define INV_32767 (1.0f / 32767.0f)
#define PI 					3.1415f
#define PI_INV2			1.57f
/*����������Ǻ�����0.5�㲽��*/
/*
	ע�⣺�� �� �� �� �� �� �� �� �� ��
*/
float sin_fast(float angle);
float cos_fast(float angle);
float tan_fast(float angle);
float asin_fast(float asin);
float acos_fast(float acos);
float atan_fast(float atan);
float atan2_fast(float y,float x);

#endif
