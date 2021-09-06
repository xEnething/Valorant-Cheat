#pragma once

int  X,Y;
#define	lm1	   GetAsyncKeyState(VK_LBUTTON)&1
#define	rm1    GetAsyncKeyState(VK_RBUTTON)&1

// bone - key - fov - smooth - visMode
char* optTeamSelect[3] = { " Enemy"," Both"," Bot" };
char *optBone     [3] = { " Head"," Neck"," Body" };
char *optKey     [11] = { " CapsLock"," CTRL"," ALT"," LShift"," Fire"," E"," F"," T", " V"," C"," X" };
char *optFov     [9] = { " 5"," 10"," 15"," 20"," 25"," 30"," 60"," 90"," 120"};
char *optSmooth     [11] = { " 0"," 1"," 2"," 3"," 4"," 5"," 6"," 7"," 8" ," 9" ," 10" };
char *optVisMode     [3] = { " NoVisible"," LastRenderTime"," LineOfSight" };
struct cFun
{
/*Main*/
	int Menu1 = 1;
	int Menu2;

/*Function*/
int Scroll_Function1, Scroll_Function2, Scroll_Function3, Scroll_Function4, Scroll_Function5, Scroll_Function6, Scroll_Function7, Scroll_Function8, Scroll_Function9;
int cBox, cLine, cCharacter, cRadar, cHealthbar, cSkeleton, cHead, cDistance, cAbility, cSpike;
int cAim, cAimDrone, cDrawFov, cRCS;
};
extern cFun Fun;