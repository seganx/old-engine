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

SEGAN_INLINE float gen_matrix_det( Matrix* mat )
{
	float tmp0 = ( ( mat->m22 * mat->m03 ) - ( mat->m02 * mat->m23 ) );
	float tmp1 = ( ( mat->m32 * mat->m13 ) - ( mat->m12 * mat->m33 ) );
	float tmp2 = ( ( mat->m01 * mat->m22 ) - ( mat->m21 * mat->m02 ) );
	float tmp3 = ( ( mat->m11 * mat->m32 ) - ( mat->m31 * mat->m12 ) );
	float tmp4 = ( ( mat->m21 * mat->m03 ) - ( mat->m01 * mat->m23 ) );
	float tmp5 = ( ( mat->m31 * mat->m13 ) - ( mat->m11 * mat->m33 ) );
	float dx = ( ( ( mat->m21 * tmp1 ) - ( mat->m23 * tmp3 ) ) - ( mat->m22 * tmp5 ) );
	float dy = ( ( ( mat->m31 * tmp0 ) - ( mat->m33 * tmp2 ) ) - ( mat->m32 * tmp4 ) );
	float dz = ( ( ( mat->m03 * tmp3 ) + ( mat->m02 * tmp5 ) ) - ( mat->m01 * tmp1 ) );
	float dw = ( ( ( mat->m13 * tmp2 ) + ( mat->m12 * tmp4 ) ) - ( mat->m11 * tmp0 ) );
	return ( ( ( mat->m00 * dx ) + ( mat->m10 * dy ) ) + ( mat->m20 * dz ) ) + ( mat->m30 * dw );
}

SEGAN_INLINE void gen_matrix_transpose( Matrix* res, const Matrix* mat )
{
	Matrix outMat;
	
	outMat.m01 = mat->m10;
	outMat.m02 = mat->m20;
	outMat.m03 = mat->m30;

	outMat.m10 = mat->m01;
	outMat.m12 = mat->m21;
	outMat.m13 = mat->m31;

	outMat.m20 = mat->m02;
	outMat.m21 = mat->m12;
	outMat.m23 = mat->m32;

	outMat.m30 = mat->m03;
	outMat.m31 = mat->m13;
	outMat.m32 = mat->m23;

	*res = outMat;
}

SEGAN_INLINE void gen_matrix_inv( Matrix* res, const Matrix* mat )
{
	float tmp0 = ( ( mat->m22 * mat->m03 ) - ( mat->m02 * mat->m23 ) );
	float tmp1 = ( ( mat->m32 * mat->m13 ) - ( mat->m12 * mat->m33 ) );
	float tmp2 = ( ( mat->m01 * mat->m22 ) - ( mat->m21 * mat->m02 ) );
	float tmp3 = ( ( mat->m11 * mat->m32 ) - ( mat->m31 * mat->m12 ) );
	float tmp4 = ( ( mat->m21 * mat->m03 ) - ( mat->m01 * mat->m23 ) );
	float tmp5 = ( ( mat->m31 * mat->m13 ) - ( mat->m11 * mat->m33 ) );

	float res0[4];
	res0[0] = ( ( ( mat->m21 * tmp1 ) - ( mat->m23 * tmp3 ) ) - ( mat->m22 * tmp5 ) );
	res0[1] = ( ( ( mat->m31 * tmp0 ) - ( mat->m33 * tmp2 ) ) - ( mat->m32 * tmp4 ) );
	res0[2] = ( ( ( mat->m03 * tmp3 ) + ( mat->m02 * tmp5 ) ) - ( mat->m01 * tmp1 ) );
	res0[3] = ( ( ( mat->m13 * tmp2 ) + ( mat->m12 * tmp4 ) ) - ( mat->m11 * tmp0 ) );

	float detInv = ( 1.0f / ( ( ( ( mat->m00 * res0[0] ) + ( mat->m10 * res0[1] ) ) + ( mat->m20 * res0[2] ) ) + ( mat->m30 * res0[3] ) ) );

	float res1[4];
	res1[0] = mat->m20 * tmp1;
	res1[1] = mat->m30 * tmp0;
	res1[2] = mat->m00 * tmp1;
	res1[3] = mat->m10 * tmp0;
	
	float res2[4];	
	res2[0] = mat->m20 * tmp5;
	res2[1] = mat->m30 * tmp4;
	res2[2] = mat->m00 * tmp5;
	res2[3] = mat->m10 * tmp4;

	float res3[4];
	res3[0] = mat->m20 * tmp3;
	res3[1] = mat->m30 * tmp2;
	res3[2] = mat->m00 * tmp3;
	res3[3] = mat->m10 * tmp2;

	tmp0 = ( ( mat->m20 * mat->m01 ) - ( mat->m00 * mat->m21 ) );
	tmp1 = ( ( mat->m30 * mat->m11 ) - ( mat->m10 * mat->m31 ) );
	tmp2 = ( ( mat->m20 * mat->m03 ) - ( mat->m00 * mat->m23 ) );
	tmp3 = ( ( mat->m30 * mat->m13 ) - ( mat->m10 * mat->m33 ) );
	tmp4 = ( ( mat->m20 * mat->m02 ) - ( mat->m00 * mat->m22 ) );
	tmp5 = ( ( mat->m30 * mat->m12 ) - ( mat->m10 * mat->m32 ) );

	res2[0] =  ( ( ( mat->m23 * tmp1 ) - ( mat->m21 * tmp3 ) ) + res2[0] );
	res2[1] =  ( ( ( mat->m33 * tmp0 ) - ( mat->m31 * tmp2 ) ) + res2[1] );
	res2[2] =  ( ( ( mat->m01 * tmp3 ) - ( mat->m03 * tmp1 ) ) - res2[2] );
	res2[3] =  ( ( ( mat->m11 * tmp2 ) - ( mat->m13 * tmp0 ) ) - res2[3] );

	res3[0] =  ( ( ( mat->m21 * tmp5 ) - ( mat->m22 * tmp1 ) ) + res3[0] );
	res3[1] =  ( ( ( mat->m31 * tmp4 ) - ( mat->m32 * tmp0 ) ) + res3[1] );
	res3[2] =  ( ( ( mat->m02 * tmp1 ) - ( mat->m01 * tmp5 ) ) - res3[2] );
	res3[3] =  ( ( ( mat->m12 * tmp0 ) - ( mat->m11 * tmp4 ) ) - res3[3] );

	res1[0] =  ( ( ( mat->m22 * tmp3 ) - ( mat->m23 * tmp5 ) ) - res1[0] );
	res1[1] =  ( ( ( mat->m32 * tmp2 ) - ( mat->m33 * tmp4 ) ) - res1[1] );
	res1[2] =  ( ( ( mat->m03 * tmp5 ) - ( mat->m02 * tmp3 ) ) + res1[2] );
	res1[3] =  ( ( ( mat->m13 * tmp4 ) - ( mat->m12 * tmp2 ) ) + res1[3] );

	res->m00 = res0[0] * detInv;
	res->m01 = res0[1] * detInv;
	res->m02 = res0[2] * detInv;
	res->m03 = res0[3] * detInv;

	res->m10 = res1[0] * detInv;
	res->m11 = res1[1] * detInv;
	res->m12 = res1[2] * detInv;
	res->m13 = res1[3] * detInv;

	res->m20 = res2[0] * detInv;
	res->m21 = res2[1] * detInv;
	res->m22 = res2[2] * detInv;
	res->m33 = res2[3] * detInv;

	res->m30 = res3[0] * detInv;
	res->m31 = res3[1] * detInv;
	res->m32 = res3[2] * detInv;
	res->m33 = res3[3] * detInv;
}

SEGAN_INLINE void gen_matrix_add( Matrix* res, const Matrix* mat1, const Matrix* mat2 )
{
	res->m00 = mat1->m00 + mat2->m00;
	res->m10 = mat1->m10 + mat2->m10;
	res->m20 = mat1->m20 + mat2->m20;
	res->m30 = mat1->m30 + mat2->m30;

	res->m01 = mat1->m01 + mat2->m01;
	res->m11 = mat1->m11 + mat2->m11;
	res->m21 = mat1->m21 + mat2->m21;
	res->m31 = mat1->m31 + mat2->m31;

	res->m02 = mat1->m02 + mat2->m02;
	res->m12 = mat1->m12 + mat2->m12;
	res->m22 = mat1->m22 + mat2->m22;
	res->m32 = mat1->m32 + mat2->m32;

	res->m03 = mat1->m03 + mat2->m03;
	res->m13 = mat1->m13 + mat2->m13;
	res->m23 = mat1->m23 + mat2->m23;
	res->m33 = mat1->m33 + mat2->m33;
}

SEGAN_INLINE void gen_matrix_sub( Matrix* res, const Matrix* mat1, const Matrix* mat2 )
{
	res->m00 = mat1->m00 - mat2->m00;
	res->m10 = mat1->m10 - mat2->m10;
	res->m20 = mat1->m20 - mat2->m20;
	res->m30 = mat1->m30 - mat2->m30;

	res->m01 = mat1->m01 - mat2->m01;
	res->m11 = mat1->m11 - mat2->m11;
	res->m21 = mat1->m21 - mat2->m21;
	res->m31 = mat1->m31 - mat2->m31;

	res->m02 = mat1->m02 - mat2->m02;
	res->m12 = mat1->m12 - mat2->m12;
	res->m22 = mat1->m22 - mat2->m22;
	res->m32 = mat1->m32 - mat2->m32;

	res->m03 = mat1->m03 - mat2->m03;
	res->m13 = mat1->m13 - mat2->m13;
	res->m23 = mat1->m23 - mat2->m23;
	res->m33 = mat1->m33 - mat2->m33;
}


SEGAN_INLINE void gen_matrix_mul( Matrix* res, const Matrix* mat1, const Matrix* mat2 )
{
	Matrix outMat;

	outMat.m00 = mat1->m00 * mat2->m00 + mat1->m01 * mat2->m10 + mat1->m02 * mat2->m20 + mat1->m03 * mat2->m30;
	outMat.m01 = mat1->m00 * mat2->m01 + mat1->m01 * mat2->m11 + mat1->m02 * mat2->m21 + mat1->m03 * mat2->m31;
	outMat.m02 = mat1->m00 * mat2->m02 + mat1->m01 * mat2->m12 + mat1->m02 * mat2->m22 + mat1->m03 * mat2->m32;
	outMat.m03 = mat1->m00 * mat2->m03 + mat1->m01 * mat2->m13 + mat1->m02 * mat2->m23 + mat1->m03 * mat2->m33;

	outMat.m10 = mat1->m10 * mat2->m00 + mat1->m11 * mat2->m10 + mat1->m12 * mat2->m20 + mat1->m13 * mat2->m30;
	outMat.m11 = mat1->m10 * mat2->m01 + mat1->m11 * mat2->m11 + mat1->m12 * mat2->m21 + mat1->m13 * mat2->m31;
	outMat.m12 = mat1->m10 * mat2->m02 + mat1->m11 * mat2->m12 + mat1->m12 * mat2->m22 + mat1->m13 * mat2->m32;
	outMat.m13 = mat1->m10 * mat2->m03 + mat1->m11 * mat2->m13 + mat1->m12 * mat2->m23 + mat1->m13 * mat2->m33;

	outMat.m20 = mat1->m20 * mat2->m00 + mat1->m21 * mat2->m10 + mat1->m22 * mat2->m20 + mat1->m23 * mat2->m30;
	outMat.m21 = mat1->m20 * mat2->m01 + mat1->m21 * mat2->m11 + mat1->m22 * mat2->m21 + mat1->m23 * mat2->m31;
	outMat.m22 = mat1->m20 * mat2->m02 + mat1->m21 * mat2->m12 + mat1->m22 * mat2->m22 + mat1->m23 * mat2->m32;
	outMat.m23 = mat1->m20 * mat2->m03 + mat1->m21 * mat2->m13 + mat1->m22 * mat2->m23 + mat1->m23 * mat2->m33;

	outMat.m30 = mat1->m30 * mat2->m00 + mat1->m31 * mat2->m10 + mat1->m32 * mat2->m20 + mat1->m33 * mat2->m30;
	outMat.m31 = mat1->m30 * mat2->m01 + mat1->m31 * mat2->m11 + mat1->m32 * mat2->m21 + mat1->m33 * mat2->m31;
	outMat.m32 = mat1->m30 * mat2->m02 + mat1->m31 * mat2->m12 + mat1->m32 * mat2->m22 + mat1->m33 * mat2->m32;
	outMat.m33 = mat1->m30 * mat2->m03 + mat1->m31 * mat2->m13 + mat1->m32 * mat2->m23 + mat1->m33 * mat2->m33;

	*res = outMat;
}

SEGAN_INLINE void gen_matrix_setrotate_pyr( Matrix* m, const float pitch, const float yaw, const float roll )
{
	float sX, cX, sY, cY, sZ, cZ;
	sx_sin_cos( pitch, sX, cX );
	sx_sin_cos( yaw, sY, cY );
	sx_sin_cos( roll, sZ, cZ );
	float tmp0 = ( cZ * sY );
	float tmp1 = ( sZ * sY );
	m->m00 = ( cZ * cY );
	m->m01 = ( sZ * cY );
	m->m02 = - sY;
	m->m03 = 0.0f;

	m->m10 = ( tmp0 * sX ) - ( sZ * cX );
	m->m11 = ( tmp1 * sX ) + ( cZ * cX );
	m->m12 = ( cY * sX );
	m->m13 = 0.0f;

	m->m20 = ( tmp0 * cX ) + ( sZ * sX );
	m->m21 = ( tmp1 * cX ) - ( cZ * sX );
	m->m22 = ( cY * cX );
	m->m23 = 0.0f;

	m->m33 = 1;
}

SEGAN_INLINE void gen_matrix_scale( Matrix* m, const float x, const float y, const float z )
{
	m->m00 = x;
	m->m01 = 0.0f;
	m->m02 = 0.0f;
	m->m03 = 0.0f;
	m->m10 = 0.0f;
	m->m11 = y;
	m->m12 = 0.0f;
	m->m13 = 0.0f;
	m->m20 = 0.0f;
	m->m21 = 0.0f;
	m->m22 = z;
	m->m23 = 0.0f;
	m->m30 = 0.0f;
	m->m31 = 0.0f;
	m->m32 = 0.0f;
	m->m33 = 1.0f;
}

SEGAN_INLINE void gen_matrix_lookat( Matrix* m, const float* eye, const float* at, const float* up )
{
	float dir[3] = { at[0] - eye[0], at[1] - eye[1], at[2] - eye[2] };
	float dirlen = sx_sqrt_fast( dir[0] * dir[0] + dir[1] * dir[1] + dir[2] * dir[2] );
	dir[0] /= dirlen;
	dir[1] /= dirlen;
	dir[2] /= dirlen;

	float side[3] = { dir[1] * up[2] - dir[2] * up[1], dir[2] * up[0] - dir[0] * up[2], dir[0] * up[1] - dir[1] * up[0] };
	float sidelen = sx_sqrt_fast( side[0] * side[0] + side[1] * side[1] + side[2] * side[2] );
	side[0] /= sidelen;
	side[1] /= sidelen;
	side[2] /= sidelen;

	float upd[3] = { side[1] * dir[2] - side[2] * dir[1], side[2] * dir[0] - side[0] * dir[2], side[0] * dir[1] - side[1] * dir[0] };
	float uplen = sx_sqrt_fast( upd[0] * upd[0] + upd[1] * upd[1] + upd[2] * upd[2] );
	upd[0] /= uplen;
	upd[1] /= uplen;
	upd[2] /= uplen;

	m->m00 = side[0];
	m->m01 = upd[0];
	m->m02 = dir[0];
	m->m03 = 0;

	m->m10 = side[1];
	m->m11 = upd[1];
	m->m12 = dir[1];
	m->m13 = 0;

	m->m20 = side[2];
	m->m21 = upd[2];
	m->m22 = dir[2];
	m->m23 = 0;

	m->m30 = - ( side[0] * eye[0] + side[1] * eye[1] + side[2] * eye[2] );
	m->m31 = - ( upd[0]  * eye[0] + upd[1]  * eye[1] + upd[2]  * eye[2] );
	m->m32 = - ( dir[0]  * eye[0] + dir[1]  * eye[1] + dir[2]  * eye[2] );
	m->m33 = 1.0f;
}

SEGAN_INLINE void gen_matrix_direction( Matrix* m, const float* dir, const float* up )
{
	float fwr[3] = { - dir[0], - dir[1], - dir[2] };
	float dirlen = sx_sqrt_fast( fwr[0] * fwr[0] + fwr[1] * fwr[1] + fwr[2] * fwr[2] );
	fwr[0] /= dirlen;
	fwr[1] /= dirlen;
	fwr[2] /= dirlen;

	//float side[3] = { fwr[1] * up[2] - fwr[2] * up[1], fwr[2] * up[0] - fwr[0] * up[2], fwr[0] * up[1] - fwr[1] * up[0] };
	float side[3] = { fwr[1] * up[2] - fwr[2] * up[1], fwr[2] * up[0] - fwr[0] * up[2], fwr[0] * up[1] - fwr[1] * up[0] };
	float sidelen = sx_sqrt_fast( side[0] * side[0] + side[1] * side[1] + side[2] * side[2] );
	side[0] /= sidelen;
	side[1] /= sidelen;
	side[2] /= sidelen;

	float upd[3] = { side[1] * fwr[2] - side[2] * fwr[1], side[2] * fwr[0] - side[0] * fwr[2], side[0] * fwr[1] - side[1] * fwr[0] };
	float uplen = sx_sqrt_fast( upd[0] * upd[0] + upd[1] * upd[1] + upd[2] * upd[2] );
	upd[0] /= uplen;
	upd[1] /= uplen;
	upd[2] /= uplen;

	m->m00 = side[0];	m->m01 = side[1];	m->m02 = side[2];	m->m03 = 0;
	m->m10 = upd[0];	m->m11 = upd[1];	m->m12 = upd[2];	m->m13 = 0;
	m->m20 = fwr[0];	m->m21 = fwr[1];	m->m22 = fwr[2];	m->m23 = 0;
	m->m30 = 0;			m->m31 = 0;			m->m32 = 0;			m->m33 = 1.0f;
}

#endif	//	GUARD_Math_Generic_HEADER_FILE

