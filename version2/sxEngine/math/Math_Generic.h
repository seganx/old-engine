/********************************************************************
	created:	2012/05/02
	filename: 	Math_Generic.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain some math functions with generic mode
*********************************************************************/
#ifndef GUARD_Math_Generic_HEADER_FILE
#define GUARD_Math_Generic_HEADER_FILE

#include "Math.h"

SEGAN_INLINE float gen_matrix_det( Matrix& mat )
{
	float tmp0 = ( ( mat._33 * mat._14 ) - ( mat._13 * mat._34 ) );
	float tmp1 = ( ( mat._43 * mat._24 ) - ( mat._23 * mat._44 ) );
	float tmp2 = ( ( mat._12 * mat._33 ) - ( mat._32 * mat._13 ) );
	float tmp3 = ( ( mat._22 * mat._43 ) - ( mat._42 * mat._23 ) );
	float tmp4 = ( ( mat._32 * mat._14 ) - ( mat._12 * mat._34 ) );
	float tmp5 = ( ( mat._42 * mat._24 ) - ( mat._22 * mat._44 ) );
	float dx = ( ( ( mat._32 * tmp1 ) - ( mat._34 * tmp3 ) ) - ( mat._33 * tmp5 ) );
	float dy = ( ( ( mat._42 * tmp0 ) - ( mat._44 * tmp2 ) ) - ( mat._43 * tmp4 ) );
	float dz = ( ( ( mat._14 * tmp3 ) + ( mat._13 * tmp5 ) ) - ( mat._12 * tmp1 ) );
	float dw = ( ( ( mat._24 * tmp2 ) + ( mat._23 * tmp4 ) ) - ( mat._22 * tmp0 ) );
	return ( ( ( mat._11 * dx ) + ( mat._21 * dy ) ) + ( mat._31 * dz ) ) + ( mat._41 * dw );
}

SEGAN_INLINE void gen_matrix_Transpose( Matrix* res, const Matrix* mat )
{
	Matrix outMat;
	
	outMat._12 = mat->_21;
	outMat._13 = mat->_31;
	outMat._14 = mat->_41;

	outMat._21 = mat->_12;
	outMat._23 = mat->_32;
	outMat._24 = mat->_42;

	outMat._31 = mat->_13;
	outMat._32 = mat->_23;
	outMat._34 = mat->_43;

	outMat._41 = mat->_14;
	outMat._42 = mat->_24;
	outMat._43 = mat->_34;

	*res = outMat;
}

SEGAN_INLINE void gen_matrix_Inverse( Matrix* res, const Matrix* mat )
{
	float tmp0 = ( ( mat->_33 * mat->_14 ) - ( mat->_13 * mat->_34 ) );
	float tmp1 = ( ( mat->_43 * mat->_24 ) - ( mat->_23 * mat->_44 ) );
	float tmp2 = ( ( mat->_12 * mat->_33 ) - ( mat->_32 * mat->_13 ) );
	float tmp3 = ( ( mat->_22 * mat->_43 ) - ( mat->_42 * mat->_23 ) );
	float tmp4 = ( ( mat->_32 * mat->_14 ) - ( mat->_12 * mat->_34 ) );
	float tmp5 = ( ( mat->_42 * mat->_24 ) - ( mat->_22 * mat->_44 ) );

	float res0[4];
	res0[0] = ( ( ( mat->_32 * tmp1 ) - ( mat->_34 * tmp3 ) ) - ( mat->_33 * tmp5 ) );
	res0[1] = ( ( ( mat->_42 * tmp0 ) - ( mat->_44 * tmp2 ) ) - ( mat->_43 * tmp4 ) );
	res0[2] = ( ( ( mat->_14 * tmp3 ) + ( mat->_13 * tmp5 ) ) - ( mat->_12 * tmp1 ) );
	res0[3] = ( ( ( mat->_24 * tmp2 ) + ( mat->_23 * tmp4 ) ) - ( mat->_22 * tmp0 ) );

	float detInv = ( 1.0f / ( ( ( ( mat->_11 * res0[0] ) + ( mat->_21 * res0[1] ) ) + ( mat->_31 * res0[2] ) ) + ( mat->_41 * res0[3] ) ) );

	float res1[4];
	res1[0] = mat->_31 * tmp1;
	res1[1] = mat->_41 * tmp0;
	res1[2] = mat->_11 * tmp1;
	res1[3] = mat->_21 * tmp0;
	
	float res2[4];	
	res2[0] = mat->_31 * tmp5;
	res2[1] = mat->_41 * tmp4;
	res2[2] = mat->_11 * tmp5;
	res2[3] = mat->_21 * tmp4;

	float res3[4];
	res3[0] = mat->_31 * tmp3;
	res3[1] = mat->_41 * tmp2;
	res3[2] = mat->_11 * tmp3;
	res3[3] = mat->_21 * tmp2;

	tmp0 = ( ( mat->_31 * mat->_12 ) - ( mat->_11 * mat->_32 ) );
	tmp1 = ( ( mat->_41 * mat->_22 ) - ( mat->_21 * mat->_42 ) );
	tmp2 = ( ( mat->_31 * mat->_14 ) - ( mat->_11 * mat->_34 ) );
	tmp3 = ( ( mat->_41 * mat->_24 ) - ( mat->_21 * mat->_44 ) );
	tmp4 = ( ( mat->_31 * mat->_13 ) - ( mat->_11 * mat->_33 ) );
	tmp5 = ( ( mat->_41 * mat->_23 ) - ( mat->_21 * mat->_43 ) );

	res2[0] =  ( ( ( mat->_34 * tmp1 ) - ( mat->_32 * tmp3 ) ) + res2[0] );
	res2[1] =  ( ( ( mat->_44 * tmp0 ) - ( mat->_42 * tmp2 ) ) + res2[1] );
	res2[2] =  ( ( ( mat->_12 * tmp3 ) - ( mat->_14 * tmp1 ) ) - res2[2] );
	res2[3] =  ( ( ( mat->_22 * tmp2 ) - ( mat->_24 * tmp0 ) ) - res2[3] );

	res3[0] =  ( ( ( mat->_32 * tmp5 ) - ( mat->_33 * tmp1 ) ) + res3[0] );
	res3[1] =  ( ( ( mat->_42 * tmp4 ) - ( mat->_43 * tmp0 ) ) + res3[1] );
	res3[2] =  ( ( ( mat->_13 * tmp1 ) - ( mat->_12 * tmp5 ) ) - res3[2] );
	res3[3] =  ( ( ( mat->_23 * tmp0 ) - ( mat->_22 * tmp4 ) ) - res3[3] );

	res1[0] =  ( ( ( mat->_33 * tmp3 ) - ( mat->_34 * tmp5 ) ) - res1[0] );
	res1[1] =  ( ( ( mat->_43 * tmp2 ) - ( mat->_44 * tmp4 ) ) - res1[1] );
	res1[2] =  ( ( ( mat->_14 * tmp5 ) - ( mat->_13 * tmp3 ) ) + res1[2] );
	res1[3] =  ( ( ( mat->_24 * tmp4 ) - ( mat->_23 * tmp2 ) ) + res1[3] );

	res->_11 = res0[0] * detInv;
	res->_12 = res0[1] * detInv;
	res->_13 = res0[2] * detInv;
	res->_14 = res0[3] * detInv;

	res->_21 = res1[0] * detInv;
	res->_22 = res1[1] * detInv;
	res->_23 = res1[2] * detInv;
	res->_24 = res1[3] * detInv;

	res->_31 = res2[0] * detInv;
	res->_32 = res2[1] * detInv;
	res->_33 = res2[2] * detInv;
	res->_44 = res2[3] * detInv;

	res->_41 = res3[0] * detInv;
	res->_42 = res3[1] * detInv;
	res->_43 = res3[2] * detInv;
	res->_44 = res3[3] * detInv;
}

SEGAN_INLINE void gen_matrix_add( Matrix& OUT resMat, const Matrix& mat1, const Matrix& mat2 )
{
	resMat._11 = mat1._11 + mat2._11;
	resMat._21 = mat1._21 + mat2._21;
	resMat._31 = mat1._31 + mat2._31;
	resMat._41 = mat1._41 + mat2._41;

	resMat._12 = mat1._12 + mat2._12;
	resMat._22 = mat1._22 + mat2._22;
	resMat._32 = mat1._32 + mat2._32;
	resMat._42 = mat1._42 + mat2._42;

	resMat._13 = mat1._13 + mat2._13;
	resMat._23 = mat1._23 + mat2._23;
	resMat._33 = mat1._33 + mat2._33;
	resMat._43 = mat1._43 + mat2._43;

	resMat._14 = mat1._14 + mat2._14;
	resMat._24 = mat1._24 + mat2._24;
	resMat._34 = mat1._34 + mat2._34;
	resMat._44 = mat1._44 + mat2._44;
}

SEGAN_INLINE void gen_matrix_sub( Matrix& OUT resMat, const Matrix& mat1, const Matrix& mat2 )
{
	resMat._11 = mat1._11 - mat2._11;
	resMat._21 = mat1._21 - mat2._21;
	resMat._31 = mat1._31 - mat2._31;
	resMat._41 = mat1._41 - mat2._41;

	resMat._12 = mat1._12 - mat2._12;
	resMat._22 = mat1._22 - mat2._22;
	resMat._32 = mat1._32 - mat2._32;
	resMat._42 = mat1._42 - mat2._42;

	resMat._13 = mat1._13 - mat2._13;
	resMat._23 = mat1._23 - mat2._23;
	resMat._33 = mat1._33 - mat2._33;
	resMat._43 = mat1._43 - mat2._43;

	resMat._14 = mat1._14 - mat2._14;
	resMat._24 = mat1._24 - mat2._24;
	resMat._34 = mat1._34 - mat2._34;
	resMat._44 = mat1._44 - mat2._44;
}


SEGAN_INLINE void gen_matrix_mul( Matrix& OUT resMat, const Matrix& mat1, const Matrix& mat2 )
{
	Matrix outMat;

	outMat._11 = mat1._11 * mat2._11 + mat1._12 * mat2._21 + mat1._13 * mat2._31 + mat1._14 * mat2._41;
	outMat._12 = mat1._11 * mat2._12 + mat1._12 * mat2._22 + mat1._13 * mat2._32 + mat1._14 * mat2._42;
	outMat._13 = mat1._11 * mat2._13 + mat1._12 * mat2._23 + mat1._13 * mat2._33 + mat1._14 * mat2._43;
	outMat._14 = mat1._11 * mat2._14 + mat1._12 * mat2._24 + mat1._13 * mat2._34 + mat1._14 * mat2._44;

	outMat._21 = mat1._21 * mat2._11 + mat1._22 * mat2._21 + mat1._23 * mat2._31 + mat1._24 * mat2._41;
	outMat._22 = mat1._21 * mat2._12 + mat1._22 * mat2._22 + mat1._23 * mat2._32 + mat1._24 * mat2._42;
	outMat._23 = mat1._21 * mat2._13 + mat1._22 * mat2._23 + mat1._23 * mat2._33 + mat1._24 * mat2._43;
	outMat._24 = mat1._21 * mat2._14 + mat1._22 * mat2._24 + mat1._23 * mat2._34 + mat1._24 * mat2._44;

	outMat._31 = mat1._31 * mat2._11 + mat1._32 * mat2._21 + mat1._33 * mat2._31 + mat1._34 * mat2._41;
	outMat._32 = mat1._31 * mat2._12 + mat1._32 * mat2._22 + mat1._33 * mat2._32 + mat1._34 * mat2._42;
	outMat._33 = mat1._31 * mat2._13 + mat1._32 * mat2._23 + mat1._33 * mat2._33 + mat1._34 * mat2._43;
	outMat._34 = mat1._31 * mat2._14 + mat1._32 * mat2._24 + mat1._33 * mat2._34 + mat1._34 * mat2._44;

	outMat._41 = mat1._41 * mat2._11 + mat1._42 * mat2._21 + mat1._43 * mat2._31 + mat1._44 * mat2._41;
	outMat._42 = mat1._41 * mat2._12 + mat1._42 * mat2._22 + mat1._43 * mat2._32 + mat1._44 * mat2._42;
	outMat._43 = mat1._41 * mat2._13 + mat1._42 * mat2._23 + mat1._43 * mat2._33 + mat1._44 * mat2._43;
	outMat._44 = mat1._41 * mat2._14 + mat1._42 * mat2._24 + mat1._43 * mat2._34 + mat1._44 * mat2._44;

	resMat = outMat;
}

SEGAN_INLINE void gen_matrix_setrotate_pyr( Matrix* m, const float pitch, const float yaw, const float roll )
{
	float sX, cX, sY, cY, sZ, cZ;
	sx_sin_cos( pitch, sX, cX );
	sx_sin_cos( yaw, sY, cY );
	sx_sin_cos( roll, sZ, cZ );
	float tmp0 = ( cZ * sY );
	float tmp1 = ( sZ * sY );
	m->_11 = ( cZ * cY );
	m->_12 = ( sZ * cY );
	m->_13 = - sY;
	m->_14 = 0.0f;

	m->_21 = ( tmp0 * sX ) - ( sZ * cX );
	m->_22 = ( tmp1 * sX ) + ( cZ * cX );
	m->_23 = ( cY * sX );
	m->_24 = 0.0f;

	m->_31 = ( tmp0 * cX ) + ( sZ * sX );
	m->_32 = ( tmp1 * cX ) - ( cZ * sX );
	m->_33 = ( cY * cX );
	m->_34 = 0.0f;

	m->_44 = 1;
}

SEGAN_INLINE void gen_matrix_scale( Matrix* m, const float x, const float y, const float z )
{
	m->_11 = x;
	m->_12 = 0.0f;
	m->_13 = 0.0f;
	m->_14 = 0.0f;
	m->_21 = 0.0f;
	m->_22 = y;
	m->_23 = 0.0f;
	m->_24 = 0.0f;
	m->_31 = 0.0f;
	m->_32 = 0.0f;
	m->_33 = z;
	m->_34 = 0.0f;
	m->_41 = 0.0f;
	m->_42 = 0.0f;
	m->_43 = 0.0f;
	m->_44 = 1.0f;
}


#endif	//	GUARD_Math_Generic_HEADER_FILE

