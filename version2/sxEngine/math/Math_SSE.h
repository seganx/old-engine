/********************************************************************
	created:	2012/05/02
	filename: 	Math_SSE.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain some math functions implemented by
				SSE instructions



				NOTE : sse math functions in this part are property of
				Sony Computer Entertainment Inc with some modification
				to make compatible with engine. here is the original
				header file :

				//////////////////////////////////////////////////////////////////////////
				Copyright (C) 2006, 2010 Sony Computer Entertainment Inc.
				All rights reserved.

				Redistribution and use in source and binary forms,
				with or without modification, are permitted provided that the
				following conditions are met:
				* Redistributions of source code must retain the above copyright
				  notice, this list of conditions and the following disclaimer.
				* Redistributions in binary form must reproduce the above copyright
				  notice, this list of conditions and the following disclaimer in the
				  documentation and/or other materials provided with the distribution.
				* Neither the name of the Sony Computer Entertainment Inc nor the names
				  of its contributors may be used to endorse or promote products derived
				  from this software without specific prior written permission.

				THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
				AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
				IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
				ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
				LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
				CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
				SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
				INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
				CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
				ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
				POSSIBILITY OF SUCH DAMAGE.

*********************************************************************/
#ifndef GUARD_Math_SSE_HEADER_FILE
#define GUARD_Math_SSE_HEADER_FILE

#include "Math.h"

#include <xmmintrin.h>
#include <emmintrin.h>

//#include "D:\SeganX\version2\sxEngine\math\vectormathlibrary\include\vectormath\SSE\cpp\vectormath_aos.h"

#define USE_SSE2_LDDQU
#ifdef USE_SSE2_LDDQU
#include <intrin.h>  //used for _mm_lddqu_si128
#endif //USE_SSE2_LDDQU

#if !defined(vec_splat)

#define _SINCOS_CC0  -0.0013602249f
#define _SINCOS_CC1   0.0416566950f
#define _SINCOS_CC2  -0.4999990225f
#define _SINCOS_SC0  -0.0001950727f
#define _SINCOS_SC1   0.0083320758f
#define _SINCOS_SC2  -0.1666665247f
#define _SINCOS_KC1  1.57079625129f
#define _SINCOS_KC2  7.54978995489e-8f

#define vec_splat(x, e)		_mm_shuffle_ps( x, x, _MM_SHUFFLE(e,e,e,e) )
#define vec_nmsub(a,b,c)	_mm_sub_ps( c, _mm_mul_ps( a, b ) )
#define vec_madd(a, b, c)	_mm_add_ps( c, _mm_mul_ps( a, b ) )
#define _mm_ror_ps(vec,i)	(((i)%4) ? (_mm_shuffle_ps(vec,vec, _MM_SHUFFLE((unsigned char)(i+3)%4,(unsigned char)(i+2)%4,(unsigned char)(i+1)%4,(unsigned char)(i+0)%4))) : (vec))
#define _mm_rol_ps(vec,i)	(((i)%4) ? (_mm_shuffle_ps(vec,vec, _MM_SHUFFLE((unsigned char)(7-i)%4,(unsigned char)(6-i)%4,(unsigned char)(5-i)%4,(unsigned char)(4-i)%4))) : (vec))

SEGAN_INLINE __m128 vec_cts( __m128 x, int a )
{
	sx_assert( a == 0 ); // Only 2^0 supported
	(void)a;
	__m128i result = _mm_cvtps_epi32( x );
	return (__m128 &)result;
}
SEGAN_INLINE __m128 vec_ctf( __m128 x, int a )
{
	sx_assert(a == 0); // Only 2^0 supported
	(void)a;
	return _mm_cvtepi32_ps( (__m128i &)x );
}
SEGAN_INLINE __m128 vec_sel(__m128 a, __m128 b, __m128 mask)
{
	return _mm_or_ps(_mm_and_ps(mask, b), _mm_andnot_ps(mask, a));
}
SEGAN_INLINE __m128 vec_sel(__m128 a, __m128 b, const unsigned int *_mask)
{
	return vec_sel(a, b, _mm_loadu_ps((float *)_mask));
}
SEGAN_INLINE __m128 vec_sel(__m128 a, __m128 b, unsigned int _mask)
{
	return vec_sel(a, b, _mm_set1_ps(*(float *)&_mask));
}
SEGAN_INLINE __m128 toM128( unsigned int x )
{
	return _mm_set1_ps( *(float *)&x );
}
SEGAN_INLINE void sincosf4( __m128 x, __m128* s, __m128* c )
{
	__m128 xl = _mm_mul_ps( x, _mm_set1_ps( 0.63661977236f ) );
	__m128 q = vec_cts( xl, 0 );
	__m128 offsetSin = _mm_and_ps( q, toM128( (int)0x3 ) );
	__m128i temp = _mm_add_epi32( _mm_set1_epi32( 1 ), (__m128i &)offsetSin );
	__m128 offsetCos = (__m128 &)temp;
	__m128 qf = vec_ctf( q , 0 );
	xl = _mm_sub_ps( _mm_sub_ps( x, _mm_mul_ps( qf, _mm_set1_ps( _SINCOS_KC1 ) ) ), _mm_mul_ps( qf, _mm_set1_ps( _SINCOS_KC2 ) ) );
	__m128 xl2 = _mm_mul_ps( xl, xl );
	__m128 xl3 = _mm_mul_ps( xl2, xl );
	__m128 cx =	vec_madd( vec_madd(	vec_madd( _mm_set1_ps( _SINCOS_CC0 ), xl2, _mm_set1_ps( _SINCOS_CC1 ) ), xl2, _mm_set1_ps( _SINCOS_CC2 ) ), xl2, _mm_set1_ps( 1.0f ) );
	__m128 sx =	vec_madd( vec_madd(	vec_madd( _mm_set1_ps( _SINCOS_SC0 ), xl2, _mm_set1_ps( _SINCOS_SC1 ) ), xl2, _mm_set1_ps( _SINCOS_SC2 ) ), xl3, xl );
	__m128 sinMask = _mm_cmpeq_ps( _mm_and_ps( offsetSin, toM128( 0x1 ) ), _mm_setzero_ps() );
	__m128 cosMask = _mm_cmpeq_ps( _mm_and_ps( offsetCos, toM128( 0x1 ) ), _mm_setzero_ps() );    
	*s = vec_sel(cx,sx,sinMask);
	*c = vec_sel(cx,sx,cosMask);
	sinMask = _mm_cmpeq_ps( _mm_and_ps( offsetSin, toM128( 0x2 ) ), _mm_setzero_ps() );
	cosMask = _mm_cmpeq_ps( _mm_and_ps( offsetCos, toM128( 0x2 ) ), _mm_setzero_ps() );
	*s = vec_sel( _mm_xor_ps( toM128( 0x80000000 ), *s ), *s, sinMask );
	*c = vec_sel( _mm_xor_ps( toM128( 0x80000000 ), *c ), *c, cosMask );    
}

#endif


SEGAN_INLINE void sse_matrix_add( matrix* res, const matrix* mat1, const matrix* mat2 )
{
	_mm_storeu_ps( res->m[0], _mm_add_ps( _mm_loadu_ps( mat1->m[0] ), _mm_loadu_ps( mat2->m[0] ) ) );
	_mm_storeu_ps( res->m[1], _mm_add_ps( _mm_loadu_ps( mat1->m[1] ), _mm_loadu_ps( mat2->m[1] ) ) );
	_mm_storeu_ps( res->m[2], _mm_add_ps( _mm_loadu_ps( mat1->m[2] ), _mm_loadu_ps( mat2->m[2] ) ) );
	_mm_storeu_ps( res->m[3], _mm_add_ps( _mm_loadu_ps( mat1->m[3] ), _mm_loadu_ps( mat2->m[3] ) ) );
}

SEGAN_INLINE void sse_matrix_sub( matrix* res, const matrix* mat1, const matrix* mat2 )
{
	_mm_storeu_ps( res->m[0], _mm_sub_ps( _mm_loadu_ps( mat1->m[0] ), _mm_loadu_ps( mat2->m[0] ) ) );
	_mm_storeu_ps( res->m[1], _mm_sub_ps( _mm_loadu_ps( mat1->m[1] ), _mm_loadu_ps( mat2->m[1] ) ) );
	_mm_storeu_ps( res->m[2], _mm_sub_ps( _mm_loadu_ps( mat1->m[2] ), _mm_loadu_ps( mat2->m[2] ) ) );
	_mm_storeu_ps( res->m[3], _mm_sub_ps( _mm_loadu_ps( mat1->m[3] ), _mm_loadu_ps( mat2->m[3] ) ) );
}

SEGAN_INLINE void sse_matrix_mul( matrix* res, const matrix* mat1, const matrix* mat2 )
{
	__m128 m1r0 = _mm_loadu_ps( mat1->m[0] );
	__m128 m1r1 = _mm_loadu_ps( mat1->m[1] );
	__m128 m1r2 = _mm_loadu_ps( mat1->m[2] );
	__m128 m1r3 = _mm_loadu_ps( mat1->m[3] );

	__m128 m2r0 = _mm_loadu_ps( mat2->m[0] );
	__m128 m2r1 = _mm_loadu_ps( mat2->m[1] );
	__m128 m2r2 = _mm_loadu_ps( mat2->m[2] );
	__m128 m2r3 = _mm_loadu_ps( mat2->m[3] );

	_mm_storeu_ps( res->m[0], 
		_mm_add_ps(
		_mm_add_ps( 
		_mm_add_ps(
		_mm_mul_ps( m2r0, _mm_shuffle_ps( m1r0, m1r0, _MM_SHUFFLE( 0, 0, 0, 0 ) ) ) ,
		_mm_mul_ps( m2r1, _mm_shuffle_ps( m1r0, m1r0, _MM_SHUFFLE( 1, 1, 1, 1 ) ) ) ),
		_mm_mul_ps( m2r2, _mm_shuffle_ps( m1r0, m1r0, _MM_SHUFFLE( 2, 2, 2, 2 ) ) ) ),
		_mm_mul_ps( m2r3, _mm_shuffle_ps( m1r0, m1r0, _MM_SHUFFLE( 3, 3, 3, 3 ) ) )	)
		);

	_mm_storeu_ps( res->m[1],
		_mm_add_ps(
		_mm_add_ps( 
		_mm_add_ps(
		_mm_mul_ps( m2r0, _mm_shuffle_ps( m1r1, m1r1, _MM_SHUFFLE( 0, 0, 0, 0 ) ) ) ,
		_mm_mul_ps( m2r1, _mm_shuffle_ps( m1r1, m1r1, _MM_SHUFFLE( 1, 1, 1, 1 ) ) ) ),
		_mm_mul_ps( m2r2, _mm_shuffle_ps( m1r1, m1r1, _MM_SHUFFLE( 2, 2, 2, 2 ) ) ) ),
		_mm_mul_ps( m2r3, _mm_shuffle_ps( m1r1, m1r1, _MM_SHUFFLE( 3, 3, 3, 3 ) ) )	)
		);

	_mm_storeu_ps( res->m[2],
		_mm_add_ps(
		_mm_add_ps( 
		_mm_add_ps(
		_mm_mul_ps( m2r0, _mm_shuffle_ps( m1r2, m1r2, _MM_SHUFFLE( 0, 0, 0, 0 ) ) ) ,
		_mm_mul_ps( m2r1, _mm_shuffle_ps( m1r2, m1r2, _MM_SHUFFLE( 1, 1, 1, 1 ) ) ) ),
		_mm_mul_ps( m2r2, _mm_shuffle_ps( m1r2, m1r2, _MM_SHUFFLE( 2, 2, 2, 2 ) ) ) ),
		_mm_mul_ps( m2r3, _mm_shuffle_ps( m1r2, m1r2, _MM_SHUFFLE( 3, 3, 3, 3 ) ) )	)
		);

	_mm_storeu_ps( res->m[3],
		_mm_add_ps(
		_mm_add_ps( 
		_mm_add_ps(
		_mm_mul_ps( m2r0, _mm_shuffle_ps( m1r3, m1r3, _MM_SHUFFLE( 0, 0, 0, 0 ) ) ) ,
		_mm_mul_ps( m2r1, _mm_shuffle_ps( m1r3, m1r3, _MM_SHUFFLE( 1, 1, 1, 1 ) ) ) ),
		_mm_mul_ps( m2r2, _mm_shuffle_ps( m1r3, m1r3, _MM_SHUFFLE( 2, 2, 2, 2 ) ) ) ),
		_mm_mul_ps( m2r3, _mm_shuffle_ps( m1r3, m1r3, _MM_SHUFFLE( 3, 3, 3, 3 ) ) )	)
		);
}

SEGAN_INLINE void sse_matrix_inv( matrix* res, const matrix* mat )
{
	__m128 Va,Vb,Vc;
	__m128 r1,r2,r3,tt,tt2;
	__m128 sum,Det,RDet;
	__m128 trns0,trns1,trns2,trns3;

	__m128 _L1 = _mm_loadu_ps( mat->m[0] );
	__m128 _L2 = _mm_loadu_ps( mat->m[1] );
	__m128 _L3 = _mm_loadu_ps( mat->m[2] );
	__m128 _L4 = _mm_loadu_ps( mat->m[3] );
	// Calculating the minterms for the first line.

	// _mm_ror_ps is just a macro using _mm_shuffle_ps().
	tt = _L4; tt2 = _mm_ror_ps(_L3,1); 
	Vc = _mm_mul_ps(tt2,_mm_ror_ps(tt,0));					// V3'·V4
	Va = _mm_mul_ps(tt2,_mm_ror_ps(tt,2));					// V3'·V4"
	Vb = _mm_mul_ps(tt2,_mm_ror_ps(tt,3));					// V3'·V4^

	r1 = _mm_sub_ps(_mm_ror_ps(Va,1),_mm_ror_ps(Vc,2));		// V3"·V4^ - V3^·V4"
	r2 = _mm_sub_ps(_mm_ror_ps(Vb,2),_mm_ror_ps(Vb,0));		// V3^·V4' - V3'·V4^
	r3 = _mm_sub_ps(_mm_ror_ps(Va,0),_mm_ror_ps(Vc,1));		// V3'·V4" - V3"·V4'

	tt = _L2;
	Va = _mm_ror_ps(tt,1);		sum = _mm_mul_ps(Va,r1);
	Vb = _mm_ror_ps(tt,2);		sum = _mm_add_ps(sum,_mm_mul_ps(Vb,r2));
	Vc = _mm_ror_ps(tt,3);		sum = _mm_add_ps(sum,_mm_mul_ps(Vc,r3));

	// Calculating the determinant.
	Det = _mm_mul_ps(sum,_L1);
	Det = _mm_add_ps(Det,_mm_movehl_ps(Det,Det));

	__declspec(align(16)) const unsigned int _vmathPNPN[4] = {0x00000000, 0x80000000, 0x00000000, 0x80000000};
	__declspec(align(16)) const unsigned int _vmathNPNP[4] = {0x80000000, 0x00000000, 0x80000000, 0x00000000};
	__declspec(align(16)) const float _vmathZERONE[4] = {1.0f, 0.0f, 0.0f, 1.0f};
	const __m128 Sign_PNPN = _mm_loadu_ps((float *)_vmathPNPN);
	const __m128 Sign_NPNP = _mm_loadu_ps((float *)_vmathNPNP);

	__m128 mtL1 = _mm_xor_ps(sum,Sign_PNPN);

	// Calculating the minterms of the second line (using previous results).
	tt = _mm_ror_ps(_L1,1);		sum = _mm_mul_ps(tt,r1);
	tt = _mm_ror_ps(tt,1);		sum = _mm_add_ps(sum,_mm_mul_ps(tt,r2));
	tt = _mm_ror_ps(tt,1);		sum = _mm_add_ps(sum,_mm_mul_ps(tt,r3));
	__m128 mtL2 = _mm_xor_ps(sum,Sign_NPNP);

	// Testing the determinant.
	Det = _mm_sub_ss(Det,_mm_shuffle_ps(Det,Det,1));

	// Calculating the minterms of the third line.
	tt = _mm_ror_ps(_L1,1);
	Va = _mm_mul_ps(tt,Vb);									// V1'·V2"
	Vb = _mm_mul_ps(tt,Vc);									// V1'·V2^
	Vc = _mm_mul_ps(tt,_L2);								// V1'·V2

	r1 = _mm_sub_ps(_mm_ror_ps(Va,1),_mm_ror_ps(Vc,2));		// V1"·V2^ - V1^·V2"
	r2 = _mm_sub_ps(_mm_ror_ps(Vb,2),_mm_ror_ps(Vb,0));		// V1^·V2' - V1'·V2^
	r3 = _mm_sub_ps(_mm_ror_ps(Va,0),_mm_ror_ps(Vc,1));		// V1'·V2" - V1"·V2'

	tt = _mm_ror_ps(_L4,1);		sum = _mm_mul_ps(tt,r1);
	tt = _mm_ror_ps(tt,1);		sum = _mm_add_ps(sum,_mm_mul_ps(tt,r2));
	tt = _mm_ror_ps(tt,1);		sum = _mm_add_ps(sum,_mm_mul_ps(tt,r3));
	__m128 mtL3 = _mm_xor_ps(sum,Sign_PNPN);

	// Dividing is FASTER than rcp_nr! (Because rcp_nr causes many register-memory RWs).
	RDet = _mm_div_ss(_mm_load_ss((float *)&_vmathZERONE), Det); // TODO: just 1.0f?
	RDet = _mm_shuffle_ps(RDet,RDet,0x00);

	// Devide the first 12 minterms with the determinant.
	mtL1 = _mm_mul_ps(mtL1, RDet);
	mtL2 = _mm_mul_ps(mtL2, RDet);
	mtL3 = _mm_mul_ps(mtL3, RDet);

	// Calculate the minterms of the forth line and devide by the determinant.
	tt = _mm_ror_ps(_L3,1);		sum = _mm_mul_ps(tt,r1);
	tt = _mm_ror_ps(tt,1);		sum = _mm_add_ps(sum,_mm_mul_ps(tt,r2));
	tt = _mm_ror_ps(tt,1);		sum = _mm_add_ps(sum,_mm_mul_ps(tt,r3));
	__m128 mtL4 = _mm_xor_ps(sum,Sign_NPNP);
	mtL4 = _mm_mul_ps(mtL4, RDet);

	// Now we just have to transpose the minterms matrix.
	trns0 = _mm_unpacklo_ps(mtL1,mtL2);
	trns1 = _mm_unpacklo_ps(mtL3,mtL4);
	trns2 = _mm_unpackhi_ps(mtL1,mtL2);
	trns3 = _mm_unpackhi_ps(mtL3,mtL4);
	_L1 = _mm_movelh_ps(trns0,trns1);
	_L2 = _mm_movehl_ps(trns1,trns0);
	_L3 = _mm_movelh_ps(trns2,trns3);
	_L4 = _mm_movehl_ps(trns3,trns2);

	_mm_storeu_ps( res->m[0], _L1 );
	_mm_storeu_ps( res->m[1], _L2 );
	_mm_storeu_ps( res->m[2], _L3 );
	_mm_storeu_ps( res->m[3], _L4 );
}

SEGAN_INLINE float sse_matrix_det( matrix* mat )
{
	__m128 Va,Vb,Vc;
	__m128 r1,r2,r3,tt,tt2;
	__m128 sum,Det;

	__m128 _L1 = _mm_loadu_ps( mat->m[0] );
	__m128 _L2 = _mm_loadu_ps( mat->m[1] );
	__m128 _L3 = _mm_loadu_ps( mat->m[2] );
	__m128 _L4 = _mm_loadu_ps( mat->m[3] );
	// Calculating the minterms for the first line.

	// _mm_ror_ps is just a macro using _mm_shuffle_ps().
	tt = _L4; tt2 = _mm_ror_ps(_L3,1); 
	Vc = _mm_mul_ps(tt2,_mm_ror_ps(tt,0));					// V3'·V4
	Va = _mm_mul_ps(tt2,_mm_ror_ps(tt,2));					// V3'·V4"
	Vb = _mm_mul_ps(tt2,_mm_ror_ps(tt,3));					// V3'·V4^

	r1 = _mm_sub_ps(_mm_ror_ps(Va,1),_mm_ror_ps(Vc,2));		// V3"·V4^ - V3^·V4"
	r2 = _mm_sub_ps(_mm_ror_ps(Vb,2),_mm_ror_ps(Vb,0));		// V3^·V4' - V3'·V4^
	r3 = _mm_sub_ps(_mm_ror_ps(Va,0),_mm_ror_ps(Vc,1));		// V3'·V4" - V3"·V4'

	tt = _L2;
	Va = _mm_ror_ps(tt,1);		sum = _mm_mul_ps(Va,r1);
	Vb = _mm_ror_ps(tt,2);		sum = _mm_add_ps(sum,_mm_mul_ps(Vb,r2));
	Vc = _mm_ror_ps(tt,3);		sum = _mm_add_ps(sum,_mm_mul_ps(Vc,r3));

	// Calculating the determinant.
	Det = _mm_mul_ps(sum,_L1);
	Det = _mm_add_ps(Det,_mm_movehl_ps(Det,Det));

	// Calculating the minterms of the second line (using previous results).
	tt = _mm_ror_ps(_L1,1);		sum = _mm_mul_ps(tt,r1);
	tt = _mm_ror_ps(tt,1);		sum = _mm_add_ps(sum,_mm_mul_ps(tt,r2));
	tt = _mm_ror_ps(tt,1);		sum = _mm_add_ps(sum,_mm_mul_ps(tt,r3));

	// Testing the determinant.
	Det = _mm_sub_ss(Det,_mm_shuffle_ps(Det,Det,1));
	return *( (float*)&Det );
}

SEGAN_INLINE void sse_matrix_transpose( matrix* res, const matrix* mat )
{
	__m128 m0 = _mm_loadu_ps( mat->m[0] );
	__m128 m1 = _mm_loadu_ps( mat->m[1] );
	__m128 m2 = _mm_loadu_ps( mat->m[2] );
	__m128 m3 = _mm_loadu_ps( mat->m[3] );
	__m128 tmp0 = _mm_unpacklo_ps( m0, m2 );
	__m128 tmp1 = _mm_unpacklo_ps( m1, m3 );
	__m128 tmp2 = _mm_unpackhi_ps( m0, m2 );
	__m128 tmp3 = _mm_unpackhi_ps( m1, m3 );
	_mm_storeu_ps( res->m[0], _mm_unpacklo_ps( tmp0, tmp1 ) );
	_mm_storeu_ps( res->m[1], _mm_unpackhi_ps( tmp0, tmp1 ) );
	_mm_storeu_ps( res->m[2], _mm_unpacklo_ps( tmp2, tmp3 ) );
	_mm_storeu_ps( res->m[3], _mm_unpackhi_ps( tmp2, tmp3 ) );
}

SEGAN_INLINE void sse_matrix_setrotate_pyr( matrix* mat, const float pitch, const float yaw, const float roll )
{
	float pyr[4] = { pitch, yaw, roll, 0.0f };
	__m128 angles = _mm_loadu_ps( pyr );

	__m128 s, c;
	sincosf4( angles, &s, &c );
	
	__m128 negS = _mm_sub_ps( _mm_setzero_ps(), s );
	__m128 Z0 = _mm_unpackhi_ps( c, s );
	__m128 Z1 = _mm_unpackhi_ps( negS, c );

	__declspec(align(16)) unsigned int select_xyz[4] = {0xffffffff, 0xffffffff, 0xffffffff, 0};
	Z1 = _mm_and_ps( Z1, _mm_loadu_ps( (float *)select_xyz ) );

	__m128 Y0 = _mm_shuffle_ps( c, negS, _MM_SHUFFLE(0,1,1,1) );
	__m128 Y1 = _mm_shuffle_ps( s, c, _MM_SHUFFLE(0,1,1,1) );
	__m128 X0 = vec_splat( s, 0 );
	__m128 X1 = vec_splat( c, 0 );
	__m128 tmp = _mm_mul_ps( Z0, Y1 );

	_mm_storeu_ps( mat->m[0], _mm_mul_ps( Z0, Y0 ) );
	_mm_storeu_ps( mat->m[1], _mm_add_ps( _mm_mul_ps( tmp, X0 ), _mm_mul_ps( Z1, X1 ) ) );
	_mm_storeu_ps( mat->m[2], _mm_sub_ps( _mm_mul_ps( tmp, X1 ), _mm_mul_ps( Z1, X0 ) ) );
	_mm_storeu_ps( mat->m[3], _mm_setr_ps( 0.0f, 0.0f, 0.0f, 1.0f ) );
}

SEGAN_INLINE void sse_matrix_scale( matrix* mat, const float x, const float y, const float z )
{
	_mm_storeu_ps( mat->m[0], _mm_set_ps( x, 0.0f, 0.0f, 0.0f ) );
	_mm_storeu_ps( mat->m[1], _mm_set_ps( 0.0f, y, 0.0f, 0.0f ) );
	_mm_storeu_ps( mat->m[2], _mm_set_ps( 0.0f, 0.0f, x, 0.0f ) );
	_mm_storeu_ps( mat->m[3], _mm_set_ps( 0.0f, 0.0f, 0.0f, 1.0f ) );
}

#if 0
SEGAN_INLINE void sse_matrix_lookat( Matrix* m, const float* eye, const float* at, const float* up )
{
#error " sse_matrix_lookat not implemented !"
}
#endif

SEGAN_INLINE void sse_matrix_transform_norm( float* dest, const float* src, const matrix* mat )
{
	__declspec(align(16)) const float vsrc[4] = { src[0], src[1], src[2], 0.0f };
	__m128 vec = _mm_load_ps( vsrc );
	__m128 row0 = _mm_loadu_ps( mat->m[0] );
	__m128 row1 = _mm_loadu_ps( mat->m[1] );
	__m128 row2 = _mm_loadu_ps( mat->m[2] );	
	__declspec(align(16)) float vdst[4];
	_mm_store_ps( vdst, 
		_mm_add_ps(
		_mm_add_ps(
		_mm_mul_ps( row0, _mm_shuffle_ps( vec, vec, _MM_SHUFFLE( 0, 0, 0, 0 ) ) ) , 
		_mm_mul_ps( row1, _mm_shuffle_ps( vec, vec, _MM_SHUFFLE( 1, 1, 1, 1 ) ) ) ) ,
		_mm_mul_ps( row2, _mm_shuffle_ps( vec, vec, _MM_SHUFFLE( 2, 2, 2, 2 ) ) ) )
		);
	memcpy( dest, vdst, 12 );
}

SEGAN_INLINE void sse_matrix_transform_point( float* dest, const float* src, const matrix* mat )
{
	__declspec(align(16)) const float vsrc[4] = { src[0], src[1], src[2], 0.0f };
	__m128 vec = _mm_load_ps( vsrc );
	__m128 row0 = _mm_loadu_ps( mat->m[0] );
	__m128 row1 = _mm_loadu_ps( mat->m[1] );
	__m128 row2 = _mm_loadu_ps( mat->m[2] );
	__m128 row3 = _mm_loadu_ps( mat->m[3] );
	__declspec(align(16)) float vdst[4];
	_mm_store_ps( vdst, 
		_mm_add_ps(
		_mm_add_ps(
		_mm_mul_ps( row0, _mm_shuffle_ps( vec, vec, _MM_SHUFFLE( 0, 0, 0, 0 ) ) ) ,
		_mm_mul_ps( row1, _mm_shuffle_ps( vec, vec, _MM_SHUFFLE( 1, 1, 1, 1 ) ) ) ) ,
		_mm_add_ps(
		_mm_mul_ps( row2, _mm_shuffle_ps( vec, vec, _MM_SHUFFLE( 2, 2, 2, 2 ) ) ) , row3 ) )
		);
	memcpy( dest, vdst, 12 );
}

SEGAN_INLINE void sse_quat_mul( quat* res, const quat* quat0, const quat* quat1 )
{
	__m128 ldata = _mm_loadu_ps( quat0->e );
	__m128 rdata = _mm_loadu_ps( quat1->e );
	__m128 tmp0 = _mm_shuffle_ps( ldata, ldata, _MM_SHUFFLE( 3, 0, 2, 1 ) );
	__m128 tmp1 = _mm_shuffle_ps( rdata, rdata, _MM_SHUFFLE( 3, 1, 0, 2 ) );
	__m128 tmp2 = _mm_shuffle_ps( ldata, ldata, _MM_SHUFFLE( 3, 1, 0, 2 ) );
	__m128 tmp3 = _mm_shuffle_ps( rdata, rdata, _MM_SHUFFLE( 3, 0, 2, 1 ) );
	__m128 qv = _mm_mul_ps( vec_splat( ldata, 3 ), rdata );
	qv = vec_madd( vec_splat( rdata, 3 ), ldata, qv );
	qv = vec_madd( tmp0, tmp1, qv );
	qv = vec_nmsub( tmp2, tmp3, qv );
	__m128 product = _mm_mul_ps( ldata, rdata );
	__m128 l_wxyz = _mm_ror_ps( ldata, 3 );
	__m128 r_wxyz = _mm_ror_ps( rdata, 3 );
	__m128 qw = vec_nmsub( l_wxyz, r_wxyz, product );
	__m128 xy = vec_madd( l_wxyz, r_wxyz, product );
	qw = _mm_sub_ps( qw, _mm_ror_ps( xy, 2 ) );
	__declspec(align(16)) unsigned int sw[4] = { 0, 0, 0, 0xffffffff };
	_mm_storeu_ps( res->e, vec_sel( qv, qw, sw ) );
}

#endif	//	GUARD_Math_SSE_HEADER_FILE
