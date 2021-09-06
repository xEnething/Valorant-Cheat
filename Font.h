

#include <tchar.h>
#include <D3DX9.h>


#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

#define D3DFONT_BOLD        0x0001
#define D3DFONT_ITALIC      0x0002
#define D3DFONT_ZENABLE     0x0004
#define D3DFONT_CENTERED    0x0001
#define D3DFONT_TWOSIDED    0x0002
#define D3DFONT_FILTERED    0x0004
#define DT_SHADOW           0x0040      // non standard

class CD3DFont
{
    static const int MAX_CHAR_INDEX = 255;
    TCHAR   m_strFontName[80];            
    DWORD   m_dwFontHeight;
    DWORD   m_dwFontFlags;
    LPDIRECT3DDEVICE9       m_pd3dDevice; // A D3DDevice used for rendering
    LPDIRECT3DTEXTURE9      m_pTexture;   // The d3d texture for this font
    LPDIRECT3DVERTEXBUFFER9 m_pVB;        // VertexBuffer for rendering text
    DWORD   m_dwTexWidth;                 // Texture dimensions
    DWORD   m_dwTexHeight;
    FLOAT   m_fTextScale;
    FLOAT   m_fTexCoords[MAX_CHAR_INDEX + 1 - 32][4];
    LPDIRECT3DSTATEBLOCK9 m_pStateBlockSaved;
    LPDIRECT3DSTATEBLOCK9 m_pStateBlockDrawTextX;

public:
    HRESULT DrawTextX( FLOAT x, FLOAT y, DWORD dwColor, const char* strText, DWORD dwFlags=0L );
    HRESULT DrawTextXScaled( FLOAT x, FLOAT y, FLOAT z, FLOAT fXScale,FLOAT fYScale,DWORD dwColor,const char* strText, DWORD dwFlags=0L );
    HRESULT Render3DText( const char* strText, DWORD dwFlags=0L );
    HRESULT GetTextExtent( const char* strText, SIZE* pSize );
    HRESULT InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
    HRESULT RestoreDeviceObjects();
    HRESULT InvalidateDeviceObjects();
    HRESULT DeleteDeviceObjects();
    CD3DFont( const TCHAR* strFontName, DWORD dwHeight, DWORD dwFlags=0L );
    ~CD3DFont();

private:
   bool CheckChar( unsigned char c) { return c < 32 || c > MAX_CHAR_INDEX; }
};







