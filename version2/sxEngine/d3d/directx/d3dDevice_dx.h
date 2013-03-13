/********************************************************************
	created:	2012/05/07
	filename: 	d3dDevice_dx.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain the class of device 3d used directx
*********************************************************************/
#ifndef GUARD_d3dDevice_dx_HEADER_FILE
#define GUARD_d3dDevice_dx_HEADER_FILE


#include "../d3dDevice.h"


#include <d3d9.h>
#pragma comment( lib, "d3d9.lib" )

#include <d3dx9.h>
#pragma comment( lib, "d3dx9.lib" )


class SEGAN_ENG_API d3dDevice_dx: public d3dDevice
{
	SEGAN_STERILE_CLASS( d3dDevice_dx );

public:

	d3dDevice_dx( void );
	virtual ~d3dDevice_dx( void );
 
 	virtual void Initialize( const handle displayHandle );
 
 	virtual void Finalize( void );
 
	virtual bool SetSize( const uint width, const uint height, const dword SX_D3D_ flag );

	virtual void CreateVertexBuffer( d3dVertexBuffer*& OUT vertexBuffer );

	virtual void DestroyVertexBuffer( d3dVertexBuffer*& IN_OUT vertexBuffer );

	virtual void SetVertexBuffer( const d3dVertexBuffer* vertexBuffer, uint streamIndex );

	virtual void CreateIndexBuffer( d3dIndexBuffer*& OUT indexBuffer );

	virtual void SetIndexBuffer( const d3dIndexBuffer* indexBuffer );

	virtual void DestroyIndexBuffer( d3dIndexBuffer*& IN_OUT indexBuffer );

	virtual void CreateTexture( d3dTexture*& OUT texture );

	virtual void DestroyTexture( d3dTexture*& IN_OUT texture );

	virtual void SetTexture( const d3dTexture* texture, uint stage = 0 );

	virtual void CreateShader( d3dShader*& OUT shader );

	virtual void DestroyShader( d3dShader*& IN_OUT shader );

	virtual void SetViewport( const d3dViewport* viewport );

	virtual void SetMatrix( const d3dMatrixMode mode, const float* _matrix );

	virtual const float* GetMatrix( const d3dMatrixMode mode );

	virtual void SetRenderState( const d3dRenderState type, const uint mode );

	virtual uint GetRenderState( const d3dRenderState type );

	virtual void DrawPrimitive(const d3dPrimitiveType primType, const int firstVertex, const int vertexCount);

	virtual void DrawIndexedPrimitive(const d3dPrimitiveType primType, const int firstIndex, const int indicesCount, const int firstVertex, const int vertexCount);

	virtual void DrawDebug( const d3dPrimitiveType primType, const uint vertxcount, const float* vertices, const dword color );

	virtual bool BeginScene( void );

	virtual void EndScene( void );

	virtual void Present( void );

	virtual void ClearScreen( const dword bgcolor );

	virtual void ClearTarget( const dword bgcolor );

	virtual void ClearZBuffer( void );
 
 	virtual void SetClipPlane( const uint index, const float* pplane );
 
 	virtual void GetClipPlane( const uint index, float* pplane );

public:

	void PreReset( void );		//	call before reset device
	void PostReset( void );		//	call after reset device
	void ResetStates( void );

public:

	struct {
		D3DPRESENT_PARAMETERS	prentParam;
		LPDIRECT3DTEXTURE9		depthTexture;
		LPDIRECT3DSURFACE9		depthSurface;
		LPDIRECT3DSURFACE9		colorSurface;
		LPDIRECT3DSTATEBLOCK9	resetStateBlock;
	}							m_initData;
	LPDIRECT3D9					m_direct3D;
	IDirect3DDevice9*			m_device3D;
	Array<LPDIRECT3DRESOURCE9>	m_resources;
	bool						m_rs_zEnabled;
	bool						m_rs_zWritable;
	bool						m_rs_culling;
	bool						m_rs_alphaBlend;
	bool						m_rs_Anisotropy;
	bool						m_rs_wireFrame;
	bool						m_rs_fog;

	matrix						m_world;
	matrix						m_view;
	matrix						m_projection;
};


#endif	//	GUARD_d3dDevice_dx_HEADER_FILE

