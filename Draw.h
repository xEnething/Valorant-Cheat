#include "Font.h"
#include "Color.h"
#include "Structur.h"
#include <time.h>
int H_m,w,h;
FILE *LOG;
int WHID=161;
D3DCOLOR text;
LPD3DXFONT pFont;
int  BTEX,BDIR;
CD3DFont *font;
char*	fps[5];
char dllpath[255];
char logfile[255];
char*	TIME(void);
int Ch,Ch1,Ch2,Ch3,Ch4,Ch5;
//=============================================//
void  PostReset(LPDIRECT3DDEVICE9 pDevice)
{
font = new CD3DFont("Arial", 8);
font->InitDeviceObjects(pDevice);
font->RestoreDeviceObjects();	
}
//=============================================//
void  PreReset(void)
{
font->InvalidateDeviceObjects();
font->DeleteDeviceObjects();
delete font;
font = NULL;
}
//=============================================//
char* FPS(void)
{

static int	 FPScounter = 0;
static float FPSfLastTickCount = 0.0f;
static float FPSfCurrentTickCount;
static char  cfps[6] = "";

FPSfCurrentTickCount = clock() * 0.001f;
FPScounter++;
if((FPSfCurrentTickCount - FPSfLastTickCount) > 1.0f)
{
   FPSfLastTickCount = FPSfCurrentTickCount;
   sprintf(cfps,"[%d]",FPScounter);
   FPScounter = 0;
}
return cfps;
}
//=============================================//
void  DrawBox( int x, int y, int w, int h, D3DCOLOR Color, LPDIRECT3DDEVICE9 pDevice)
{
	 struct Vertex 
{
 float x,y,z,ht;
 DWORD Color;
}
V[4] ={{x,y+h, 0.0f, 0.0f, Color}, {x,y, 0.0f, 0.0f, Color},{x+w,y+h, 0.0f, 0.0f, Color},{x+w,y, 0.0f, 0.0f, Color}};
 pDevice->SetTexture(0, NULL);
 pDevice->SetPixelShader( 0 );
 pDevice->SetFVF(D3DFVF_XYZRHW|D3DFVF_DIFFUSE);
 pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,true);
 pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
 pDevice->SetRenderState(D3DRS_ZENABLE , FALSE);
 pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
 pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,V,sizeof(Vertex));
 return;
}
/*void DrawBox(int x, int y, int w, int h, D3DCOLOR color, LPDIRECT3DDEVICE9 pDevice)
{
	D3DRECT rec = { x, y, x + w, y + h };
	pDevice->Clear(1, &rec, D3DCLEAR_TARGET, color, 0, 0);
}*/
//=============================================//
void  DrawBorder(int x, int y, int w, int h, D3DCOLOR Color,IDirect3DDevice9* pDevice)
{
	DrawBox(x,  y, 1,  h,Color,pDevice);
	DrawBox(x,y+h, w,  1,Color,pDevice);
	DrawBox(x,  y, w,  1,Color,pDevice);
	DrawBox(x+w,y, 1,h+1,Color,pDevice);
}

/*void DrawBorder(int x, int y, int w, int h, int px, D3DCOLOR BorderColor, IDirect3DDevice9* pDevice)
{
	DrawRect(x, (y + h - px), w, px, BorderColor, pDevice);
	DrawRect(x, y, px, h, BorderColor, pDevice);
	DrawRect(x, y, w, px, BorderColor, pDevice);
	DrawRect((x + w - px), y, px, h, BorderColor, pDevice);
}*/
//=============================================//
void Draw_Text(int x,int y,DWORD color,LPSTR text, const char *fmt)
{
	RECT rect, rect2;
	SetRect( &rect, x, y, x, y );
	SetRect( &rect2, x - 0.1, y + 0.2, x - 0.1, y + 0. );
	pFont->DrawTextA(NULL,text,-1,&rect2, DT_LEFT|DT_NOCLIP, 0x00000000 );
	pFont->DrawTextA(NULL,text,-1,&rect, DT_LEFT|DT_NOCLIP, color );
}
//=============================================//
BOOL IsInBox(int x,int y,int w,int h)
{
	POINT MousePosition; 
	GetCursorPos(&MousePosition); 
	ScreenToClient(GetForegroundWindow(),&MousePosition);
	return(MousePosition.x >= x && MousePosition.x <= x + w && MousePosition.y >= y && MousePosition.y <= y + h);
}
//=============================================//
void  Draw_Menu(int w, int h, char *Text, IDirect3DDevice9* pDevice)
{
    DrawBox(X+130,Y+26,184,20, Silver_Menu,pDevice);//üst
	DrawBox(X+132,Y+48,180,h,DARK_Menu,pDevice);
	DrawBorder(X+130,Y+26,184,20,BorderWhite,pDevice);
	DrawBorder(X+132,Y+48,180,h,BorderWhite,pDevice);
	font->DrawTextX(X+190,Y+30, BLACK,Text,DT_LEFT);
}
//=============================================//
void DrawBorderMenu(int x, int y, int w, int h, D3DCOLOR Color,IDirect3DDevice9* pDevice)
{
if(Ch3!=20){Ch3++;}
DrawBorder(x-2 ,y, w+1, Ch3 , BorderWhite	 , pDevice);
DrawBorder(x-2 ,y+20, w+1, Ch3 , BorderWhite	 , pDevice);
/*DrawBorder(x-2 ,y+40, w+1, Ch3 , BorderWhite	 , pDevice);
DrawBorder(x-2 ,y+60, w+1, Ch3 , BorderWhite	 , pDevice);
DrawBorder(x-2 ,y+80, w+1, Ch3 , BorderWhite	 , pDevice);
DrawBorder(x-2 ,y+100, w+1, Ch3 , BorderWhite	 , pDevice);
DrawBorder(x-2 ,y+120, w+1, Ch3 , BorderWhite	 , pDevice);*/
}
//=============================================//
void  Draw_Menu_1(int w, int h,char *Text,  LPDIRECT3DDEVICE9 pDevice)
{
if(Ch2!=40){Ch2++;}
DrawBox(X+6,Y+26, 105+w,20, Silver_Menu, pDevice);//üst
DrawBox(X+9,Y+48, 100+w,Ch2,DARK, pDevice);
DrawBorder(X+6,Y+26,105+w,20,BorderWhite,pDevice);
font->DrawTextX(X+28,Y+30,BLACK,Text,DT_LEFT);
//RtlSecureZeroMemory(Text, sizeof(Text));

}
void XVPVQW_Menu(int x, int ye, int w, int h, char* Button_Text, int& Var, LPDIRECT3DDEVICE9 pDevice)
{
	int y = Y + ye;
	if (IsInBox(x, y, w, h)) {
		if (GetAsyncKeyState(VK_LBUTTON) & 1)
		{
			if (Var == 1 && Var != 0)
			{
				Var = 0;
			}
			else if (Var == 0)
			{
				if (Var == 0)
				{
					Fun.Menu1 = Fun.Menu2 = 0; Var = 1;
				}
			}
		}
	}
	if (Var == 0) {
		font->DrawTextX(X + 15, y + 3, WHITE, Button_Text, DT_LEFT);
	}
	if (Var == 1) {
		DrawBox(X + 10, y + 1, w, 20, DARKGREY, pDevice);
		font->DrawTextX(X + 15, y + 3, WHITE, Button_Text, DT_LEFT);
	}
	//RtlSecureZeroMemory(Button_Text, sizeof(Button_Text));
}
//=============================================//
void XVPVQW(int x, int y, int w, int h, int& Var, const char* Button_Text, LPDIRECT3DDEVICE9 pDevice)
{
	int y_ = (y)+(h / 2) - 7;
	int x_ = x + w + 2;
	if (IsInBox(x, y - 5, w + 161, h))
	{
		DrawBox(x, y, w, h, GreenButton, pDevice);
		DrawBox(x + 15, y - 2, w + 141, h + 3, Functions, pDevice);
		if (GetAsyncKeyState(VK_LBUTTON) & 1)
		{
			DrawBorder(x - 1, y - 1, w, h, GREEN, pDevice);
			if (Var == 1 && Var != 0) { Var = 0; }
			else
				if (Var == 0) { Var = 1; }
		}
	}
	if (Var == 0) {
		DrawBox(x, y, w, h, TxtOFF, pDevice);
		font->DrawTextX(x_, y_, WHITE, Button_Text, DT_LEFT);
		DrawBorder(x - 1, y - 1, w, h, DARKGREY, pDevice);
	}
	if (Var == 1) {
		DrawBox(x + 1, y + 1, w - 3, h - 3, MenuGreen, pDevice);
		font->DrawTextX(x_, y_, WHITE, Button_Text, DT_LEFT);
		DrawBorder(x - 1, y - 1, w, h, DARKGREY, pDevice);
	}

	//RtlSecureZeroMemory(Button_Text, sizeof(Button_Text));
}
//=============================================//
void Draw_Chams(int x, int ye, char* Text, int& Var, DWORD CoLoR1, DWORD CoLoR2, DWORD CoLoR3, DWORD CoLoR4, LPDIRECT3DDEVICE9 pDevice)
{
	int y = Y + ye;
	DWORD CoL;
	DWORD R_COL = DARKGREY;
	if (IsInBox(X + 115, y, WHID - 9, 15))
	{
		R_COL = WHITE; DrawBox(x + 40, y, 140, 16, Functions, pDevice);
		if (lm1)
		{
			if (Var >= 0) { Var++; }
		}
		if (rm1)
		{
			if (Var != 0) { Var--; }
		}
	}
	switch (Var)
	{
	case 1: CoL = CoLoR1;      break;
	case 2: CoL = CoLoR2;      break;
	case 3: CoL = CoLoR3;      break;
	case 4: CoL = CoLoR4;      break;
	case 5:                Var = 0;    break;
	}
	if (Var > 0)
	{
		DrawBox(x + 12, y + 2, 22, 12, CoL, pDevice);
	}
	font->DrawTextX(x + 35, y, WHITE, Text, DT_LEFT);
	DrawBorder(x + 10, y, 25, 15, R_COL, pDevice);
}

//=============================================//
void Scrol(int x, int y, int& Var, char** typ, int max, char* text, LPDIRECT3DDEVICE9 pDevice) {
	DrawBox(x - 1, y, 12, 12, DARKGREY, pDevice);
	DrawBorder(x - 1, y, 12, 12, DARKGREY, pDevice);
	font->DrawTextX(x + 6, y - 1, WHITE, "<", DT_CENTER);
	DrawBox(x + 73, y, 12, 12, DARKGREY, pDevice);
	DrawBorder(x + 73, y, 12, 12, DARKGREY, pDevice);
	font->DrawTextX(x + 80, y - 1, WHITE, ">", DT_CENTER);

	if (IsInBox(x, y - 4, 12, 12)) {
		DrawBorder(x - 1, y - 1, 12, 12, MenuGreen, pDevice);
		if (GetAsyncKeyState(VK_LBUTTON)) {
			if (Var != 0) {
				Var--;
				Sleep(100);
			}
		}
	}
	if (IsInBox(x + 73, y - 4, 12, 12)) {
		DrawBorder(x + 73, y - 1, 12, 12, MenuGreen, pDevice);
		if (GetAsyncKeyState(VK_LBUTTON)) {
			if (Var >= 0 && Var < max) {
				Var++;
				Sleep(100);
			}
		}
	}
	font->DrawTextX(x + 18, y - 1, ORANGE, typ[Var], DT_SHADOW);
	font->DrawTextX(x + 84, y - 1, FUCK_ON, text, DT_SHADOW);

	//RtlSecureZeroMemory(text, sizeof(text));
}
LPD3DXLINE S_Line;
void DrawLine(LONG Xa, LONG Ya, LONG Xb, LONG Yb, DWORD dwWidth, D3DCOLOR Color)
{
	D3DXVECTOR2 vLine[2];
	S_Line->SetAntialias(0);

	S_Line->SetWidth(dwWidth);
	S_Line->Begin();

	vLine[0][0] = Xa;
	vLine[0][1] = Ya;
	vLine[1][0] = Xb;
	vLine[1][1] = Yb;

	S_Line->Draw(vLine, 2, Color);
	S_Line->End();
}





