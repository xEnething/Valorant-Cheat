
#define STRICT
#include <stdio.h>
#include <tchar.h>
#include <d3dx9.h>
#include "Font.h"

#define MAX_NUM_VERTICES 50*6

struct FONT2DVERTEX { D3DXVECTOR4 p;   DWORD color;     FLOAT tu, tv; };
struct FONT3DVERTEX { D3DXVECTOR3 p;   D3DXVECTOR3 n;   FLOAT tu, tv; };

#define D3DFVF_FONT2DVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)
#define D3DFVF_FONT3DVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1)

inline FONT2DVERTEX InitFont2DVertex( const D3DXVECTOR4& p, D3DCOLOR color,
                                      FLOAT tu, FLOAT tv )
{
    FONT2DVERTEX v;   v.p = p;   v.color = color;   v.tu = tu;   v.tv = tv;
    return v;
}

inline FONT3DVERTEX InitFont3DVertex( const D3DXVECTOR3& p, const D3DXVECTOR3& n,
                                      FLOAT tu, FLOAT tv )
{
    FONT3DVERTEX v;   v.p = p;   v.n = n;   v.tu = tu;   v.tv = tv;
    return v;
}

CD3DFont::CD3DFont( const TCHAR* strFontName, DWORD dwHeight, DWORD dwFlags )
{
    _tcsncpy( m_strFontName, strFontName, sizeof(m_strFontName) / sizeof(TCHAR) );
    m_strFontName[sizeof(m_strFontName) / sizeof(TCHAR) - 1] = _T('\0');
    m_dwFontHeight         = dwHeight;
    m_dwFontFlags          = dwFlags;

    m_pd3dDevice           = NULL;
    m_pTexture             = NULL;
    m_pVB                  = NULL;

    m_pStateBlockSaved     = NULL;
    m_pStateBlockDrawTextX  = NULL;
}


CD3DFont::~CD3DFont()
{
    InvalidateDeviceObjects();
    DeleteDeviceObjects();
}

HRESULT CD3DFont::InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
    HRESULT hr;
    m_pd3dDevice = pd3dDevice;
    m_fTextScale  = 1.0f; // Draw fonts into texture without scaling
    if( m_dwFontHeight > 40 )
        m_dwTexWidth = m_dwTexHeight = 1024;
    else if( m_dwFontHeight > 20 )
        m_dwTexWidth = m_dwTexHeight = 512;
    else
        m_dwTexWidth  = m_dwTexHeight = 256;

    // If requested texture is too big, use a smaller texture and smaller font,
    // and scale up when rendering.
    D3DCAPS9 d3dCaps;
    m_pd3dDevice->GetDeviceCaps( &d3dCaps );

    if( m_dwTexWidth > d3dCaps.MaxTextureWidth )
    {
        m_fTextScale = (FLOAT)d3dCaps.MaxTextureWidth / (FLOAT)m_dwTexWidth;
        m_dwTexWidth = m_dwTexHeight = d3dCaps.MaxTextureWidth;
    }

    // Create a new texture for the font
    hr = m_pd3dDevice->CreateTexture( m_dwTexWidth, m_dwTexHeight, 1,
                                      0, D3DFMT_A4R4G4B4,
                                      D3DPOOL_MANAGED, &m_pTexture, 0 );
    if( FAILED(hr) )
        return hr;

    // Prepare to create a bitmap
    DWORD*      pBitmapBits;
    BITMAPINFO bmi;
    ZeroMemory( &bmi.bmiHeader,  sizeof(BITMAPINFOHEADER) );
    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       =  (int)m_dwTexWidth;
    bmi.bmiHeader.biHeight      = -(int)m_dwTexHeight;
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biBitCount    = 32;

    // Create a DC and a bitmap for the font
    HDC     hDC       = CreateCompatibleDC( NULL );
    HBITMAP hbmBitmap = CreateDIBSection( hDC, &bmi, DIB_RGB_COLORS,(void**)&pBitmapBits, NULL, 0 );
    SetMapMode( hDC, MM_TEXT );

    INT nHeight    = -MulDiv( m_dwFontHeight, (INT)(GetDeviceCaps(hDC, LOGPIXELSY) * m_fTextScale), 72 );
    DWORD dwBold   = (m_dwFontFlags&D3DFONT_BOLD)   ? FW_EXTRABOLD : FW_EXTRABOLD;
    DWORD dwItalic = (m_dwFontFlags&D3DFONT_ITALIC) ? TRUE    : FALSE;
    HFONT hFont    = CreateFont( nHeight, 0, 0, 0, dwBold, dwItalic,
                          FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                          CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                          VARIABLE_PITCH, m_strFontName );
    if( NULL==hFont )
        return E_FAIL;

    HGDIOBJ hPrevBitmap = SelectObject( hDC, hbmBitmap );
    HGDIOBJ hPrevFont = SelectObject( hDC, hFont );
    SetTextColor( hDC, RGB(255,255,255) );
    SetBkColor(   hDC, 0x00000000 );
    SetTextAlign( hDC, TA_TOP );
    DWORD x = 0;
    DWORD y = 0;
    char str[2] = "x";
    SIZE size;

    for( int c=32; c <= MAX_CHAR_INDEX; c++ )
    {
        str[0] = c;
        GetTextExtentPoint32A( hDC, str, 1, &size );

        if( (DWORD)(x+size.cx+1) > m_dwTexWidth )
        {
            x  = 0;
            y += size.cy+1;
        }

        ExtTextOutA( hDC, x+0, y+0, ETO_OPAQUE, NULL, str, 1, NULL );

        m_fTexCoords[c-32][0] = ((FLOAT)(x+0))/m_dwTexWidth;
        m_fTexCoords[c-32][1] = ((FLOAT)(y+0))/m_dwTexHeight;
        m_fTexCoords[c-32][2] = ((FLOAT)(x+0+size.cx))/m_dwTexWidth;
        m_fTexCoords[c-32][3] = ((FLOAT)(y+0+size.cy))/m_dwTexHeight;

        x += size.cx+1;
    }

    // Lock the surface and write the alpha values for the set pixels
    D3DLOCKED_RECT d3dlr;
    m_pTexture->LockRect( 0, &d3dlr, 0, 0 );
    BYTE* pDstRow = (BYTE*)d3dlr.pBits;
    WORD* pDst16;
    BYTE bAlpha; // 4-bit measure of pixel intensity

    for( y=0; y < m_dwTexHeight; y++ )
    {
        pDst16 = (WORD*)pDstRow;
        for( x=0; x < m_dwTexWidth; x++ )
        {
            bAlpha = (BYTE)((pBitmapBits[m_dwTexWidth*y + x] & 0xff) >> 4);
            if (bAlpha > 0)
            {
                *pDst16++ = (bAlpha << 12) | 0x0fff;
            }
            else
            {
                *pDst16++ = 0x0000;
            }
        }
        pDstRow += d3dlr.Pitch;
    }
    m_pTexture->UnlockRect(0);

    //Необходимо установить первоначальные объекты перед удалением
    SelectObject( hDC, hPrevBitmap );
    SelectObject( hDC, hPrevFont );

    DeleteObject( hbmBitmap );
    DeleteDC( hDC );
    DeleteObject( hFont );

    return S_OK;
}


HRESULT CD3DFont::RestoreDeviceObjects()
{
    HRESULT hr;

    int vertexSize = max( sizeof(FONT2DVERTEX), sizeof(FONT3DVERTEX ) );
    if( FAILED( hr = m_pd3dDevice->CreateVertexBuffer( MAX_NUM_VERTICES * vertexSize,
                                                       D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 0,
                                                       D3DPOOL_DEFAULT, &m_pVB, 0 ) ) )
    {
        return hr;
    }

    // Create the state blocks for rendering text
    for( UINT which=0; which<2; which++ )
    {
        m_pd3dDevice->BeginStateBlock();
        m_pd3dDevice->SetTexture( 0, m_pTexture );

        if ( D3DFONT_ZENABLE & m_dwFontFlags )
            m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
        else
            m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );

        m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
        m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,   D3DBLEND_SRCALPHA );
        m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND,  D3DBLEND_INVSRCALPHA );
        m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  TRUE );
        m_pd3dDevice->SetRenderState( D3DRS_ALPHAREF,         0x08 );
        m_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC,  D3DCMP_GREATEREQUAL );
        m_pd3dDevice->SetRenderState( D3DRS_FILLMODE,   D3DFILL_SOLID );
        m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,   D3DCULL_CCW );
        m_pd3dDevice->SetRenderState( D3DRS_STENCILENABLE,    FALSE );
        m_pd3dDevice->SetRenderState( D3DRS_CLIPPING,         TRUE );
        m_pd3dDevice->SetRenderState( D3DRS_CLIPPLANEENABLE,  FALSE );
        m_pd3dDevice->SetRenderState( D3DRS_VERTEXBLEND,      D3DVBF_DISABLE );
        m_pd3dDevice->SetRenderState( D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE );
        m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE,        FALSE );
        m_pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE,
            D3DCOLORWRITEENABLE_RED  | D3DCOLORWRITEENABLE_GREEN |
            D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
        m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
        m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
        m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
        m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
        m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );

        if( which==0 )
            m_pd3dDevice->EndStateBlock( &m_pStateBlockSaved );
        else
            m_pd3dDevice->EndStateBlock( &m_pStateBlockDrawTextX );
    }

    return S_OK;
}

HRESULT CD3DFont::InvalidateDeviceObjects()
{
    SAFE_RELEASE( m_pVB );
    SAFE_RELEASE( m_pStateBlockSaved );
    SAFE_RELEASE( m_pStateBlockDrawTextX );

    return S_OK;
}


HRESULT CD3DFont::DeleteDeviceObjects()
{
    SAFE_RELEASE( m_pTexture );
    m_pd3dDevice = NULL;

    return S_OK;
}

HRESULT CD3DFont::GetTextExtent( const char* strText, SIZE* pSize )
{
    if( NULL==strText || NULL==pSize )
        return E_FAIL;

    FLOAT fRowWidth  = 0.0f;
    FLOAT fRowHeight = (m_fTexCoords[0][3]-m_fTexCoords[0][1])*m_dwTexHeight;
    FLOAT fWidth     = 0.0f;
    FLOAT fHeight    = fRowHeight;

    while( *strText )
    {
        unsigned char c = *strText++;

        if( c == '\n' )
        {
            fRowWidth = 0.0f;
            fHeight  += fRowHeight;
        }

        if( CheckChar(c) )
            continue;

        FLOAT tx1 = m_fTexCoords[c-32][0];
        FLOAT tx2 = m_fTexCoords[c-32][2];

        fRowWidth += (tx2-tx1)*m_dwTexWidth;

        if( fRowWidth > fWidth )
            fWidth = fRowWidth;
    }

    pSize->cx = (int)fWidth;
    pSize->cy = (int)fHeight;

    return S_OK;
}


HRESULT CD3DFont::DrawTextXScaled( FLOAT x, FLOAT y, FLOAT z,
                                  FLOAT fXScale, FLOAT fYScale, DWORD dwColor,
                                  const char* strText, DWORD dwFlags )
{
    if( m_pd3dDevice == NULL )
        return E_FAIL;
    m_pStateBlockSaved->Capture();
    m_pStateBlockDrawTextX->Apply();
    m_pd3dDevice->SetFVF( D3DFVF_FONT2DVERTEX );
    m_pd3dDevice->SetPixelShader( NULL );
    m_pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof(FONT2DVERTEX) );
    if( dwFlags & D3DFONT_FILTERED )
    {
        m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
        m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
    }
	   if( dwFlags & DT_RIGHT )
   { 
   SIZE sz;
   GetTextExtent( strText, &sz ); x -= (FLOAT)sz.cx;
   }
   if( dwFlags & DT_CENTER )
   { 
   SIZE sz; 
   GetTextExtent( strText, &sz ); x -= (FLOAT)(sz.cx / 2.0); 
   } 
    D3DVIEWPORT9 vp;
    m_pd3dDevice->GetViewport( &vp );
    FLOAT sx  = (x+1.0f)*vp.Width/2;
    FLOAT sy  = (y-1.0f)*vp.Height/2;
    FLOAT sz  = z;
    FLOAT rhw = 1.0f;
    FLOAT fStartX = sx;
    FLOAT fLineHeight = ( m_fTexCoords[0][3] - m_fTexCoords[0][1] ) * m_dwTexHeight;
    FONT2DVERTEX* pVertices;
    DWORD         dwNumTriangles = 0L;
    m_pVB->Lock( 0, 0, (void**)&pVertices, D3DLOCK_DISCARD );

    while( *strText )
    {
        unsigned char c = *strText++;

        if( c == '\n' )
        {
            sx  = fStartX;
            sy += fYScale*vp.Height;
        }

        if( CheckChar(c) )
            continue;

        FLOAT tx1 = m_fTexCoords[c-32][0];
        FLOAT ty1 = m_fTexCoords[c-32][1];
        FLOAT tx2 = m_fTexCoords[c-32][2];
        FLOAT ty2 = m_fTexCoords[c-32][3];

        FLOAT w = (tx2-tx1)*m_dwTexWidth;
        FLOAT h = (ty2-ty1)*m_dwTexHeight;

        w *= (fXScale*vp.Height)/fLineHeight;
        h *= (fYScale*vp.Height)/fLineHeight;

        if( c != ' ' )
        {
			if( dwFlags & DT_SHADOW ) {
            float sxa,sya;
            sxa = sx;
            sya = sy;
            sxa=sx+1.0f;
               *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sxa+0-0.5f,sy+h-0.5f,0.9f,1.0f), 0xff000000, tx1, ty2 );
               *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sxa+0-0.5f,sy+0-0.5f,0.9f,1.0f), 0xff000000, tx1, ty1 );
               *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sxa+w-0.5f,sy+h-0.5f,0.9f,1.0f), 0xff000000, tx2, ty2 );
               *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sxa+w-0.5f,sy+0-0.5f,0.9f,1.0f), 0xff000000, tx2, ty1 );
               *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sxa+w-0.5f,sy+h-0.5f,0.9f,1.0f), 0xff000000, tx2, ty2 );
              *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sxa+0-0.5f,sy+0-0.5f,0.9f,1.0f), 0xff000000, tx1, ty1 );

            sxa=sx-1.0f;
               *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sxa+0-0.5f,sy+h-0.5f,0.9f,1.0f), 0xff000000, tx1, ty2 );
               *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sxa+0-0.5f,sy+0-0.5f,0.9f,1.0f), 0xff000000, tx1, ty1 );
               *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sxa+w-0.5f,sy+h-0.5f,0.9f,1.0f), 0xff000000, tx2, ty2 );
               *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sxa+w-0.5f,sy+0-0.5f,0.9f,1.0f), 0xff000000, tx2, ty1 );
               *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sxa+w-0.5f,sy+h-0.5f,0.9f,1.0f), 0xff000000, tx2, ty2 );
               *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sxa+0-0.5f,sy+0-0.5f,0.9f,1.0f), 0xff000000, tx1, ty1 );

            sya=sy-1.0f;
               *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+0-0.5f,sya+h-0.5f,0.9f,1.0f), 0xff000000, tx1, ty2 );
               *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+0-0.5f,sya+0-0.5f,0.9f,1.0f), 0xff000000, tx1, ty1 );
               *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+w-0.5f,sya+h-0.5f,0.9f,1.0f), 0xff000000, tx2, ty2 );
               *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+w-0.5f,sya+0-0.5f,0.9f,1.0f), 0xff000000, tx2, ty1 );
               *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+w-0.5f,sya+h-0.5f,0.9f,1.0f), 0xff000000, tx2, ty2 );
               *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+0-0.5f,sya+0-0.5f,0.9f,1.0f), 0xff000000, tx1, ty1 );

            sya=sy+1.0f;
               *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+0-0.5f,sya+h-0.5f,0.9f,1.0f), 0xff000000, tx1, ty2 );
               *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+0-0.5f,sya+0-0.5f,0.9f,1.0f), 0xff000000, tx1, ty1 );
               *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+w-0.5f,sya+h-0.5f,0.9f,1.0f), 0xff000000, tx2, ty2 );
               *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+w-0.5f,sya+0-0.5f,0.9f,1.0f), 0xff000000, tx2, ty1 );
               *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+w-0.5f,sya+h-0.5f,0.9f,1.0f), 0xff000000, tx2, ty2 );
               *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+0-0.5f,sya+0-0.5f,0.9f,1.0f), 0xff000000, tx1, ty1 );
               dwNumTriangles += 8;
         }
            *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+0-0.5f,sy+h-0.5f,0.9f,1.0f), dwColor, tx1, ty2 );
            *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+0-0.5f,sy+0-0.5f,0.9f,1.0f), dwColor, tx1, ty1 );
            *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+w-0.5f,sy+h-0.5f,0.9f,1.0f), dwColor, tx2, ty2 );
            *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+w-0.5f,sy+0-0.5f,0.9f,1.0f), dwColor, tx2, ty1 );
            *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+w-0.5f,sy+h-0.5f,0.9f,1.0f), dwColor, tx2, ty2 );
            *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+0-0.5f,sy+0-0.5f,0.9f,1.0f), dwColor, tx1, ty1 );
            dwNumTriangles += 2;

            if( dwNumTriangles*3 > (MAX_NUM_VERTICES-6) )
            {
                // Unlock, render, and relock the vertex buffer
                m_pVB->Unlock();
                m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, dwNumTriangles );
                m_pVB->Lock( 0, 0, (void**)&pVertices, D3DLOCK_DISCARD );
                dwNumTriangles = 0L;
            }
        }
        sx += w;
    }
    m_pVB->Unlock();
    if( dwNumTriangles > 0 )
        m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, dwNumTriangles );
    m_pStateBlockSaved->Apply();

    return S_OK;
}

HRESULT CD3DFont::DrawTextX( FLOAT sx, FLOAT sy, DWORD dwColor,
                            const char* strText, DWORD dwFlags )
{
    if( m_pd3dDevice == NULL )
        return E_FAIL;
    m_pStateBlockSaved->Capture();
    m_pStateBlockDrawTextX->Apply();
    m_pd3dDevice->SetFVF( D3DFVF_FONT2DVERTEX );
    m_pd3dDevice->SetPixelShader( NULL );
    m_pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof(FONT2DVERTEX) );
    if( dwFlags & D3DFONT_FILTERED )
    {
        m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
        m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
    }
	  if( dwFlags & DT_RIGHT )
   { 
   SIZE sz;
   GetTextExtent( strText, &sz ); sx -= (FLOAT)sz.cx;
   }
   if( dwFlags & DT_CENTER )
   { 
   SIZE sz; 
   GetTextExtent( strText, &sz ); sx -= (FLOAT)(sz.cx / 2.0); 
   } 
    FLOAT fStartX = sx;
    FONT2DVERTEX* pVertices = NULL;
    DWORD         dwNumTriangles = 0;
    m_pVB->Lock( 0, 0, (void**)&pVertices, D3DLOCK_DISCARD );
    while( *strText )
    {
        unsigned char c = *strText++;
        if( c == '\n' )
        {
            sx = fStartX;
            sy += (m_fTexCoords[0][3]-m_fTexCoords[0][1])*m_dwTexHeight;
        }
        if( CheckChar(c) )
            continue;
        FLOAT tx1 = m_fTexCoords[c-32][0];
        FLOAT ty1 = m_fTexCoords[c-32][1];
        FLOAT tx2 = m_fTexCoords[c-32][2];
        FLOAT ty2 = m_fTexCoords[c-32][3];

        FLOAT w = (tx2-tx1) *  m_dwTexWidth / m_fTextScale;
        FLOAT h = (ty2-ty1) * m_dwTexHeight / m_fTextScale;

        if( c != ' ' )
        {
           if( dwFlags & DT_SHADOW ) {
            float sxa,sya;
            sxa = sx;
            sya = sy;
            sxa=sx+1.0f;
               *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sxa+0-0.5f,sy+h-0.5f,0.9f,1.0f), 0xff000000, tx1, ty2 );
               *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sxa+0-0.5f,sy+0-0.5f,0.9f,1.0f), 0xff000000, tx1, ty1 );
               *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sxa+w-0.5f,sy+h-0.5f,0.9f,1.0f), 0xff000000, tx2, ty2 );
               *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sxa+w-0.5f,sy+0-0.5f,0.9f,1.0f), 0xff000000, tx2, ty1 );
               *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sxa+w-0.5f,sy+h-0.5f,0.9f,1.0f), 0xff000000, tx2, ty2 );
              *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sxa+0-0.5f,sy+0-0.5f,0.9f,1.0f), 0xff000000, tx1, ty1 );

            sxa=sx-1.0f;
               *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sxa+0-0.5f,sy+h-0.5f,0.9f,1.0f), 0xff000000, tx1, ty2 );
               *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sxa+0-0.5f,sy+0-0.5f,0.9f,1.0f), 0xff000000, tx1, ty1 );
               *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sxa+w-0.5f,sy+h-0.5f,0.9f,1.0f), 0xff000000, tx2, ty2 );
               *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sxa+w-0.5f,sy+0-0.5f,0.9f,1.0f), 0xff000000, tx2, ty1 );
               *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sxa+w-0.5f,sy+h-0.5f,0.9f,1.0f), 0xff000000, tx2, ty2 );
               *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sxa+0-0.5f,sy+0-0.5f,0.9f,1.0f), 0xff000000, tx1, ty1 );

            sya=sy-1.0f;
               *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+0-0.5f,sya+h-0.5f,0.9f,1.0f), 0xff000000, tx1, ty2 );
               *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+0-0.5f,sya+0-0.5f,0.9f,1.0f), 0xff000000, tx1, ty1 );
               *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+w-0.5f,sya+h-0.5f,0.9f,1.0f), 0xff000000, tx2, ty2 );
               *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+w-0.5f,sya+0-0.5f,0.9f,1.0f), 0xff000000, tx2, ty1 );
               *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+w-0.5f,sya+h-0.5f,0.9f,1.0f), 0xff000000, tx2, ty2 );
               *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+0-0.5f,sya+0-0.5f,0.9f,1.0f), 0xff000000, tx1, ty1 );
            sya=sy+1.0f;
               *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+0-0.5f,sya+h-0.5f,0.9f,1.0f), 0xff000000, tx1, ty2 );
               *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+0-0.5f,sya+0-0.5f,0.9f,1.0f), 0xff000000, tx1, ty1 );
               *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+w-0.5f,sya+h-0.5f,0.9f,1.0f), 0xff000000, tx2, ty2 );
               *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+w-0.5f,sya+0-0.5f,0.9f,1.0f), 0xff000000, tx2, ty1 );
               *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+w-0.5f,sya+h-0.5f,0.9f,1.0f), 0xff000000, tx2, ty2 );
               *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+0-0.5f,sya+0-0.5f,0.9f,1.0f), 0xff000000, tx1, ty1 );
               dwNumTriangles += 8;
         }
            *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+0-0.5f,sy+h-0.5f,0.9f,1.0f), dwColor, tx1, ty2 );
            *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+0-0.5f,sy+0-0.5f,0.9f,1.0f), dwColor, tx1, ty1 );
            *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+w-0.5f,sy+h-0.5f,0.9f,1.0f), dwColor, tx2, ty2 );
            *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+w-0.5f,sy+0-0.5f,0.9f,1.0f), dwColor, tx2, ty1 );
            *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+w-0.5f,sy+h-0.5f,0.9f,1.0f), dwColor, tx2, ty2 );
            *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+0-0.5f,sy+0-0.5f,0.9f,1.0f), dwColor, tx1, ty1 );
            dwNumTriangles += 2;
            if( dwNumTriangles*3 > (MAX_NUM_VERTICES-6) )
            {
                m_pVB->Unlock();
                m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, dwNumTriangles );
                pVertices = NULL;
                m_pVB->Lock( 0, 0, (void**)&pVertices, D3DLOCK_DISCARD );
                dwNumTriangles = 0L;
            }
        }
        sx += w;
    }
    m_pVB->Unlock();
    if( dwNumTriangles > 0 )
        m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, dwNumTriangles );
    m_pStateBlockSaved->Apply();

    return S_OK;
}


HRESULT CD3DFont::Render3DText( const char* strText, DWORD dwFlags )
{
    if( m_pd3dDevice == NULL )
        return E_FAIL;
    m_pStateBlockSaved->Capture();
    m_pStateBlockDrawTextX->Apply();
    m_pd3dDevice->SetFVF( D3DFVF_FONT3DVERTEX );
    m_pd3dDevice->SetPixelShader( NULL );
    m_pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof(FONT3DVERTEX) );
    if( dwFlags & D3DFONT_FILTERED )
    {
        m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
        m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
    }
    FLOAT x = 0.0f;
    FLOAT y = 0.0f;
    if( dwFlags & D3DFONT_CENTERED )
    {
        SIZE sz;
        GetTextExtent( strText, &sz );
        x = -(((FLOAT)sz.cx)/10.0f)/2.0f;
        y = -(((FLOAT)sz.cy)/10.0f)/2.0f;
    }
    if( dwFlags & D3DFONT_TWOSIDED )
        m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
    FLOAT fStartX = x;
    unsigned char c;
    FONT3DVERTEX* pVertices;
    DWORD         dwNumTriangles = 0L;
    m_pVB->Lock( 0, 0, (void**)&pVertices, D3DLOCK_DISCARD );
    while( (c = *strText++) != 0 )
    {
        if( c == '\n' )
        {
            x = fStartX;
            y -= (m_fTexCoords[0][3]-m_fTexCoords[0][1])*m_dwTexHeight/10.0f;
        }

        if( CheckChar(c) )
            continue;
        FLOAT tx1 = m_fTexCoords[c-32][0];
        FLOAT ty1 = m_fTexCoords[c-32][1];
        FLOAT tx2 = m_fTexCoords[c-32][2];
        FLOAT ty2 = m_fTexCoords[c-32][3];
        FLOAT w = (tx2-tx1) * m_dwTexWidth  / ( 10.0f * m_fTextScale );
        FLOAT h = (ty2-ty1) * m_dwTexHeight / ( 10.0f * m_fTextScale );

        if( c != ' ' )
        {
            *pVertices++ = InitFont3DVertex( D3DXVECTOR3(x+0,y+0,0), D3DXVECTOR3(0,0,-1), tx1, ty2 );
            *pVertices++ = InitFont3DVertex( D3DXVECTOR3(x+0,y+h,0), D3DXVECTOR3(0,0,-1), tx1, ty1 );
            *pVertices++ = InitFont3DVertex( D3DXVECTOR3(x+w,y+0,0), D3DXVECTOR3(0,0,-1), tx2, ty2 );
            *pVertices++ = InitFont3DVertex( D3DXVECTOR3(x+w,y+h,0), D3DXVECTOR3(0,0,-1), tx2, ty1 );
            *pVertices++ = InitFont3DVertex( D3DXVECTOR3(x+w,y+0,0), D3DXVECTOR3(0,0,-1), tx2, ty2 );
            *pVertices++ = InitFont3DVertex( D3DXVECTOR3(x+0,y+h,0), D3DXVECTOR3(0,0,-1), tx1, ty1 );
            dwNumTriangles += 2;

            if( dwNumTriangles*3 > (MAX_NUM_VERTICES-6) )
            {
                m_pVB->Unlock();
                m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, dwNumTriangles );
                m_pVB->Lock( 0, 0, (void**)&pVertices, D3DLOCK_DISCARD );
                dwNumTriangles = 0L;
            }
        }
        x += w;
    }
    m_pVB->Unlock();
    if( dwNumTriangles > 0 )
        m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, dwNumTriangles );
    m_pStateBlockSaved->Apply();

    return S_OK;
}




