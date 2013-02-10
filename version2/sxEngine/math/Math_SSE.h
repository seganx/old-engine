/********************************************************************
	created:	2012/05/02
	filename: 	Math_SSE.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain some math functions implemented by
				SSE instructions
*********************************************************************/
#ifndef GUARD_Math_SSE_HEADER_FILE
#define GUARD_Math_SSE_HEADER_FILE

#include "Math.h"

//#include "vectormathlibrary/include/vectormath/SSE/cpp/vectormath_aos.h"


SEGAN_INLINE void sse_matrix_add( Matrix& OUT resMat, const Matrix& mat1, const Matrix& mat2 )
{
	resMat.m128[0] = _mm_add_ps( mat1.m128[0], mat2.m128[0] );
	resMat.m128[1] = _mm_add_ps( mat1.m128[1], mat2.m128[1] );
	resMat.m128[2] = _mm_add_ps( mat1.m128[2], mat2.m128[2] );
	resMat.m128[3] = _mm_add_ps( mat1.m128[3], mat2.m128[3] );
}

SEGAN_INLINE void sse_matrix_sub( Matrix& OUT resMat, const Matrix& mat1, const Matrix& mat2 )
{
	resMat.m128[0] = _mm_sub_ps( mat1.m128[0], mat2.m128[0] );
	resMat.m128[1] = _mm_sub_ps( mat1.m128[1], mat2.m128[1] );
	resMat.m128[2] = _mm_sub_ps( mat1.m128[2], mat2.m128[2] );
	resMat.m128[3] = _mm_sub_ps( mat1.m128[3], mat2.m128[3] );
}

SEGAN_INLINE void sse_matrix_mul( Matrix& OUT resMat, const Matrix& mat1, const Matrix& mat2 )
{
	resMat.m128[0] = _mm_add_ps(	
		_mm_add_ps( 
		_mm_mul_ps( mat1.m128[0], _mm_shuffle_ps( mat2.m128[0], mat2.m128[0], _MM_SHUFFLE(0,0,0,0) ) ), 
		_mm_mul_ps( mat1.m128[1], _mm_shuffle_ps( mat2.m128[0], mat2.m128[0], _MM_SHUFFLE(1,1,1,1) ) ) ),
		_mm_add_ps(
		_mm_mul_ps( mat1.m128[2], _mm_shuffle_ps( mat2.m128[0], mat2.m128[0], _MM_SHUFFLE(2,2,2,2) ) ),
		_mm_mul_ps( mat1.m128[3], _mm_shuffle_ps( mat2.m128[0], mat2.m128[0], _MM_SHUFFLE(3,3,3,3) ) ) ) 
		);

	resMat.m128[1] = _mm_add_ps(	
		_mm_add_ps( 
		_mm_mul_ps( mat1.m128[0], _mm_shuffle_ps( mat2.m128[1], mat2.m128[1], _MM_SHUFFLE(0,0,0,0) ) ), 
		_mm_mul_ps( mat1.m128[1], _mm_shuffle_ps( mat2.m128[1], mat2.m128[1], _MM_SHUFFLE(1,1,1,1) ) ) ),
		_mm_add_ps(
		_mm_mul_ps( mat1.m128[2], _mm_shuffle_ps( mat2.m128[1], mat2.m128[1], _MM_SHUFFLE(2,2,2,2) ) ),
		_mm_mul_ps( mat1.m128[3], _mm_shuffle_ps( mat2.m128[1], mat2.m128[1], _MM_SHUFFLE(3,3,3,3) ) ) ) 
		);

	resMat.m128[2] = _mm_add_ps(	
		_mm_add_ps( 
		_mm_mul_ps( mat1.m128[0], _mm_shuffle_ps( mat2.m128[2], mat2.m128[2], _MM_SHUFFLE(0,0,0,0) ) ), 
		_mm_mul_ps( mat1.m128[1], _mm_shuffle_ps( mat2.m128[2], mat2.m128[2], _MM_SHUFFLE(1,1,1,1) ) ) ),
		_mm_add_ps(
		_mm_mul_ps( mat1.m128[2], _mm_shuffle_ps( mat2.m128[2], mat2.m128[2], _MM_SHUFFLE(2,2,2,2) ) ),
		_mm_mul_ps( mat1.m128[3], _mm_shuffle_ps( mat2.m128[2], mat2.m128[2], _MM_SHUFFLE(3,3,3,3) ) ) ) 
		);

	resMat.m128[3] = _mm_add_ps(	
		_mm_add_ps( 
		_mm_mul_ps( mat1.m128[0], _mm_shuffle_ps( mat2.m128[3], mat2.m128[3], _MM_SHUFFLE(0,0,0,0) ) ), 
		_mm_mul_ps( mat1.m128[1], _mm_shuffle_ps( mat2.m128[3], mat2.m128[3], _MM_SHUFFLE(1,1,1,1) ) ) ),
		_mm_add_ps(
		_mm_mul_ps( mat1.m128[2], _mm_shuffle_ps( mat2.m128[3], mat2.m128[3], _MM_SHUFFLE(2,2,2,2) ) ),
		_mm_mul_ps( mat1.m128[3], _mm_shuffle_ps( mat2.m128[3], mat2.m128[3], _MM_SHUFFLE(3,3,3,3) ) ) ) 
		);
}

SEGAN_INLINE void sse_matrix_div( Matrix& OUT resMat, const Matrix& mat1, const Matrix& mat2 )
{

}

#endif	//	GUARD_Math_SSE_HEADER_FILE