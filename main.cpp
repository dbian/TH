#include "nvidia\nvapi.h"
//#include   "NvApiDriverSettings.h"
//
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "io.h"
#include "windows.h"
#include <windowsx.h>
#include "CommCtrl.h"
#include <windef.h>
#include <gdiplus.h>

#include "resource.h"
#include "main.h"

#define DEBUG 1
#define TIMER 2000

void  PrintError( NvAPI_Status status)
{
#ifdef DEBUG
	NvAPI_ShortString szDesc = {0};
	NvAPI_GetErrorMessage (status, szDesc);
	MessageBoxA(0,szDesc,"",0);
	//printf ( " NVAPI error: %s\n", szDesc);
	//exit(-1);
#endif
}

void GetMaskBitmaps(HBITMAP hSourceBitmap, 
		COLORREF clrTransparent, 
		HBITMAP *hAndMaskBitmap, 
		HBITMAP *hXorMaskBitmap);



int PASCAL WinMain( HINSTANCE hInstance, //当前实例句柄
		HINSTANCE hPrevInstance, //前一个实例句柄
		LPSTR lpCmdLine, //命令行字符
		int nCmdShow) //窗口显示方式
{
	MSG msg;
	//创建主窗口
	if ( !InitWindow( hInstance, nCmdShow ) )
		return FALSE;
	//进入消息循环：
	//从该应用程序的消息队列中检取消息，送到消息处理过程，
	//当检取到WM_QUIT消息时，退出消息循环。
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	//程序结束
	return msg.wParam;
}

static BOOL InitWindow( HINSTANCE hInstance, int nCmdShow )
{
	WNDCLASS wc; //窗口类结构
	//填充窗口类结构
	wc.style = CS_VREDRAW | CS_HREDRAW;
	wc.lpfnWndProc = (WNDPROC)WinProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon( hInstance, LPCTSTR("MAINAPP"));
	wc.hCursor = LoadCursor( NULL, IDC_ARROW );
	wc.hbrBackground =(HBRUSH) CreateSolidBrush(RGB(240,240,240));
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "tmpHacker";
	//注册窗口类
	RegisterClass( &wc );
	//创建主窗口
        long screenw,screenh;
        screenw=GetSystemMetrics(SM_CXSCREEN);
        screenh=GetSystemMetrics(SM_CYSCREEN);
        long ww=327,hh=180;
	wnd = CreateWindowW(
			L"tmpHacker", //窗口类名称
			L" GPU Temperature Hacker ", //窗口标题
			WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX, //窗口风格，定义为普通型
			(screenw-ww)/2, //窗口位置的x坐标
			(screenh-hh)/2, //窗口位置的y坐标
			ww, //窗口的宽度
			hh, //窗口的高度
			NULL, //父窗口句柄
			NULL, //菜单句柄
			hInstance, //应用程序实例句柄
			NULL ); //窗口创建数据指针
	if( !wnd ) return FALSE;
	//
	hinstance=hInstance;
	//显示并更新窗口
	ShowWindow( wnd, SW_HIDE);
	//UpdateWindow( wnd );
	setIcon();
	SetTimer(wnd,TIMER,5000,NULL);

	return TRUE;
}
//******************************************************************
//函数：WinProc()
//功能：处理主窗口消息
//******************************************************************
BOOL CALLBACK WinProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
		case WM_CREATE:
			{
				getLocalSettings();
				initializeGpuServices();
				//hinstance=(( LPCREATESTRUCT) lParam)->hInstance ;
				menu=LoadMenu(hinstance, MAKEINTRESOURCE (IDR_MENU1)) ;
				createCtrls(hWnd,wParam);
				break;
			}
		case WM_TIMER:
			{
				setIcon();
				break;
			}
		case WM_COMMAND:
			{
				switch( LOWORD( wParam))
				{
					case IDM_1://show or hide
						{
							if(IsWindowVisible(wnd))
								ShowWindow(wnd,SW_HIDE);
							else
								ShowWindow(wnd,SW_NORMAL);
							BringWindowToTop(wnd);
							break;
						}
					case IDM_4://close
						{
							DestroyWindow(wnd);
							break;
						}
					case COMBOGPU:
						{
							if(HIWORD(wParam)==CBN_SELCHANGE)
							{
								curGpu=SendMessage(comboGpu, CB_GETCURSEL, 0, 0);
                                                                //refresh once
                                                                setIcon();
								saveSettings();
							}
							break;
						}
					case COMBOFONT:
						{
							if(HIWORD(wParam)==CBN_SELCHANGE)
							{
								curFontSize=SendMessage(comboFont, CB_GETCURSEL, 0, 0);
								DeleteObject(hf);
								setFont();
								saveSettings();
                                                                //refresh once
                                                                setIcon();
							}
							break;
						}
					case BTCOLORCHOOSE:
						{
							CHOOSECOLOR cc;                 // common dialog box structure 
							static COLORREF acrCustClr[16]; // array of custom colors 

							// Initialize CHOOSECOLOR 
							ZeroMemory(&cc, sizeof(cc));
							cc.lStructSize = sizeof(cc);
							cc.hwndOwner = wnd;
							cc.lpCustColors = (LPDWORD) acrCustClr;
							cc.rgbResult = colorTray;
							cc.Flags = CC_FULLOPEN | CC_RGBINIT;

							if (ChooseColor(&cc)==TRUE) 
							{
								colorTray = cc.rgbResult; 
                                                                //refresh once
                                                                setIcon();

							}
							saveSettings();

							break;
						}
					case BTWEBSITE:
						{
							ShellExecuteA(NULL,"open","http://tmphacker.sinaapp.com",NULL,NULL,SW_SHOW);
							break;
						}
					case CHECKAUTORUN:
						{
							autoStart=SendMessage(checkAutoRun,BM_GETCHECK, 0, 0);
							setAutoStart(autoStart);
							saveSettings();
							break;
						}

				}
				break;
			}
		case TRAY_NOTIFY:
			{
				onTray(wParam,lParam);
				break;
			}
			/*case WM_CTLCOLOR:
			  {

			  SetBkColor((HDC)wParam,COLOR_BACKGROUND);
			  break;
			  }*/
		case WM_DESTROY://退出消息
			{
				endGpuServices();

				setIcon(true);
				PostQuitMessage( 0 );//调用退出函数
				break;
			}
	}
	//调用缺省消息处理过程
	return DefWindowProc(hWnd, message, wParam, lParam);
} 

void createCtrls(HWND&hwnd,WPARAM wParam)
{
	LOGFONT lf;	
	HFONT hFont;
	GetObject(GetStockObject(DEFAULT_GUI_FONT),sizeof(lf),&lf);
	hFont=CreateFontIndirect(&lf);

	comboGpu=CreateWindowW( L"COMBOBOX",L"",
			CBS_DROPDOWNLIST | CBS_HASSTRINGS|WS_VISIBLE|WS_CHILD,
			79, 8, 72, 76,hwnd, (HMENU)COMBOGPU,hinstance,NULL);
	SendMessage(comboGpu,WM_SETFONT,(WPARAM)hFont,(LPARAM)TRUE);
	wchar_t str[10];
	for(int i=0;i<gpuCounts;i++)
	{
		swprintf(str,L"GPU-%d",i+1);
		SendMessageW(comboGpu, CB_ADDSTRING, (WPARAM)0, (LPARAM)str);
	}
	if(gpuCounts!=0)
		SendMessage(comboGpu, CB_SETCURSEL , curGpu, 0L); 

	comboFont=CreateWindowW( L"COMBOBOX",L"",
			CBS_DROPDOWNLIST | CBS_HASSTRINGS|WS_VISIBLE|WS_CHILD,
			79, 33, 72, 60,hwnd, (HMENU)COMBOFONT,hinstance,NULL);
	SendMessage(comboFont,WM_SETFONT,(WPARAM)hFont,(LPARAM)TRUE);

	SendMessageW(comboFont, CB_ADDSTRING, (WPARAM)0, (LPARAM)L"大");
	SendMessageW(comboFont, CB_ADDSTRING, (WPARAM)0, (LPARAM)L"中");
	SendMessageW(comboFont, CB_ADDSTRING, (WPARAM)0, (LPARAM)L"小");
	SendMessage(comboFont, CB_SETCURSEL , curFontSize, 0L); 

	btColorChoose=CreateWindowW( L"BUTTON",L"设置颜色",
			WS_VISIBLE|WS_CHILD,
			18, 62, 100, 23,hwnd, (HMENU)BTCOLORCHOOSE,hinstance,NULL);
	SendMessage(btColorChoose,WM_SETFONT,(WPARAM)hFont,(LPARAM)TRUE);
	btWebSite=CreateWindowW( L"BUTTON",L"项目主页",
			WS_VISIBLE|WS_CHILD,
			203, 111, 87, 23,hwnd, (HMENU)BTWEBSITE,hinstance,NULL);
	SendMessage(btWebSite,WM_SETFONT,(WPARAM)hFont,(LPARAM)TRUE);
	checkAutoRun=CreateWindowW( L"BUTTON",L"自启动(需要管理员权限运行)",
			WS_VISIBLE | WS_CHILD | WS_TABSTOP | 0x00000003, 14, 101, 180,50,hwnd, (HMENU)CHECKAUTORUN,hinstance,NULL);
	SendMessage(checkAutoRun,WM_SETFONT,(WPARAM)hFont,(LPARAM)TRUE);
	SendMessage(checkAutoRun,BM_SETCHECK, autoStart?BST_CHECKED:BST_UNCHECKED, 0);

	//
	HWND s1=CreateWindowW( L"STATIC",L"GPU 监控:",
			WS_VISIBLE | WS_CHILD | WS_GROUP | SS_LEFT, 14, 11, 63, 13,hwnd, NULL,hinstance,NULL);
	SendMessage(s1,WM_SETFONT,(WPARAM)hFont,(LPARAM)TRUE);
	s1=CreateWindowW( L"STATIC",L"字体大小:",
			WS_VISIBLE | WS_CHILD | WS_GROUP | SS_LEFT, 14, 36, 63, 13, hwnd, NULL,hinstance,NULL);
	SendMessage(s1,WM_SETFONT,(WPARAM)hFont,(LPARAM)TRUE);
	s1=CreateWindowW( L"STATIC",L"By BIAN\nbhd39428@gmail.com\n",
			WS_VISIBLE | WS_CHILD | WS_GROUP | SS_RIGHT, 180, 16, 123, 48,hwnd, NULL,hinstance,NULL);
	SendMessage(s1,WM_SETFONT,(WPARAM)hFont,(LPARAM)TRUE);
	s1=CreateWindowW( L"STATIC",L"2011-12 Mingw-Build-1\nNVIDIA GPU 温度监控",
			WS_VISIBLE | WS_CHILD | WS_GROUP | SS_RIGHT, 180, 57, 123, 48,hwnd, NULL,hinstance,NULL);
	SendMessage(s1,WM_SETFONT,(WPARAM)hFont,(LPARAM)TRUE);

	//
	setFont();
	//here is fault,for Get from stock,point to a constant
	//DeleteObject(hFont);
}

void initializeGpuServices()
{
	status =  NvAPI_Initialize ();
	if (status != NVAPI_OK ) 
		PrintError(status);
	NvU32 gpuCount=0;
	status=NvAPI_EnumPhysicalGPUs(physicalGpuArray,&gpuCount);
	if (status != NVAPI_OK ) 
		PrintError(status);
	else
		gpuCounts=gpuCount;
	//try to find the compatible version of sensors
	memset(&thermalSet,0,sizeof(NV_GPU_THERMAL_SETTINGS));
	thermalSet.version=NV_GPU_THERMAL_SETTINGS_VER_1;
	thermalSet.count=NVAPI_MAX_THERMAL_SENSORS_PER_GPU;//
	status=NvAPI_GPU_GetThermalSettings(physicalGpuArray[curGpu],0,&thermalSet);
	if (status == NVAPI_INCOMPATIBLE_STRUCT_VERSION )
	{

		thermalSet.version=NV_GPU_THERMAL_SETTINGS_VER_2;
		status=NvAPI_GPU_GetThermalSettings(physicalGpuArray[curGpu],0,&thermalSet);
	}
	if (status != NVAPI_OK ) 
		PrintError(status);

}

void endGpuServices()
{

}

void onTray(WPARAM wParam,LPARAM lParam)
{
	UINT uMouseMsg = (UINT) lParam;

	switch(uMouseMsg){

		case WM_LBUTTONDBLCLK:
			{
				//show or hide the dialog
				if(IsWindowVisible(wnd))
					ShowWindow(wnd,SW_HIDE);
				else
					ShowWindow(wnd,SW_NORMAL);
				//BringWindowToTop(wnd);
                                //this API can make the window be seen
                                SetForegroundWindow(wnd);
				break;
			}
		case WM_RBUTTONDOWN:
			{
				HMENU popmenu=GetSubMenu(menu,0);
				POINT point;
				GetCursorPos(&point);
                                //this can makesure menu get focus right now
                                SetForegroundWindow(wnd);
				TrackPopupMenu (popmenu, TPM_RIGHTBUTTON, point.x, point.y, 0, wnd, NULL) ;
				break;

			}
		default: 
			break;
	}




}

void setFont()
{
	lf.lfHeight  = curFontSize==0?16:(curFontSize==1?15:14);
	lf.lfWidth   = 0;
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	lf.lfWeight  = 700;
	lf.lfItalic=false;
	lf.lfUnderline=false;
	lf.lfStrikeOut  = 0;
	lf.lfCharSet  = ANSI_CHARSET;
	lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfQuality  = PROOF_QUALITY;
	lf.lfPitchAndFamily=VARIABLE_PITCH | FF_SWISS;
	strcpy (lf.lfFaceName, "微软雅黑");
	//lstrcpy((LPSTR)lf.lfFaceName,(LPSTR)"Tahoma");   
	//****old one not delete
	hf=CreateFontIndirect(&lf);   

}
//assume *iconn do not point to sth.
void createIcon(HICON* iconn,int num)
{
	//using namespace Gdiplus;

	//we need a widechar array for the value
	wchar_t strValue[4];
	int size = sizeof(strValue)/sizeof(wchar_t);// because the wchar_t requires two byte!!!
	swprintf( strValue, L"%d", num);//the prefix L means, that the string should be a widechar

	HDC windowDc=GetDC(wnd);
	HDC memdc=CreateCompatibleDC(windowDc);
	SetTextColor(memdc,colorTray);
	SetBkMode(memdc,TRANSPARENT);
	//not from memdc,this will create a blackwhite bmp
	HBITMAP bitmap=CreateCompatibleBitmap(windowDc,16,16);

	//use the bitmap to draw
	HBITMAP hOldBitmap  = (HBITMAP)SelectObject(memdc,bitmap);
	//draw string
	HFONT hOldFont=(HFONT) SelectObject(memdc,hf);
	//DrawTextW( memdc,strValue,-1,&rct,DT_LEFT|DT_SINGLELINE);
	TextOutW(memdc,0,0,strValue,wcslen(strValue));

	SelectObject(memdc,hOldFont);
	SelectObject(memdc,hOldBitmap);
	DeleteDC(memdc);//core! can't select the same bitmap at the same time
	ReleaseDC(wnd,windowDc);

	HBITMAP andBMP,xorBMP;
	GetMaskBitmaps(bitmap,RGB(0,0,0),&andBMP,&xorBMP);
	DeleteObject(bitmap);

	static ICONINFO iconinfo;
	iconinfo.fIcon=true;
	iconinfo.xHotspot=iconinfo.yHotspot=0;
	iconinfo.hbmMask=andBMP;
	iconinfo.hbmColor=xorBMP;

	*iconn=CreateIconIndirect(&iconinfo);
	///fixed
	//GDI memory leak
	DeleteObject(andBMP);
	DeleteObject(xorBMP);
	//

}
void setIcon(bool deleteIt)
{
	// set NOTIFYCONDATA structure	
	NOTIFYICONDATA tnid;

	tnid.cbSize = sizeof(NOTIFYICONDATA); 
	tnid.hWnd = wnd; 
	tnid.uID = 0x1; //Resourcename
	tnid.uFlags = NIF_MESSAGE |NIF_ICON | NIF_TIP; //
	tnid.uCallbackMessage = TRAY_NOTIFY;//user message 
	HICON iconnew;
	createIcon(&iconnew,getCurGpuTemp());
	tnid.hIcon = iconnew; //handle of the created icon

	//copy the string to the NOTIFYCONDATA structure
	lstrcpyn(tnid.szTip, " GPU Temperature Monitor by DBIAN ", sizeof(tnid.szTip));
	if(deleteIt)
	{
		Shell_NotifyIcon(NIM_DELETE,&tnid);
		return;
	}

	if(firstSetIcon){ 

		//for the first time we have to use the Shell_NotifyIcon with NIM_ADD
		//to add the icon to the tray
		Shell_NotifyIcon(NIM_ADD, &tnid); 
		firstSetIcon= false;
	}
	else{
		//the icon already exists
		//Shell_NotifyIcon with NIM_MODIFY
		Shell_NotifyIcon(NIM_MODIFY, &tnid);
	}

	// free icon 
	DestroyIcon(iconnew); 
}

int getCurGpuTemp()
{
	status=NvAPI_GPU_GetThermalSettings(physicalGpuArray[curGpu],0,&thermalSet);
	if (status != NVAPI_OK ) 
		PrintError(status);
	else
	{
		//get tmp success
		return thermalSet.sensor[0].currentTemp;
		//printf(" gpu tmp : %d\n",curTmp);
	}
	return 0;
}

void GetMaskBitmaps(HBITMAP hSourceBitmap, 
		COLORREF clrTransparent, 
		HBITMAP *hAndMaskBitmap, 
		HBITMAP *hXorMaskBitmap)
{
	HDC hDC        = ::GetDC(NULL);
	HDC hMainDC      = ::CreateCompatibleDC(hDC); 
	HDC hAndMaskDC      = ::CreateCompatibleDC(hDC); 
	HDC hXorMaskDC      = ::CreateCompatibleDC(hDC); 

	//Get the dimensions of the source bitmap

	BITMAP bm;
	::GetObject(hSourceBitmap,sizeof(BITMAP),&bm);


	*hAndMaskBitmap  = ::CreateCompatibleBitmap(hDC,bm.bmWidth,bm.bmHeight);
	*hXorMaskBitmap  = ::CreateCompatibleBitmap(hDC,bm.bmWidth,bm.bmHeight);

	//Select the bitmaps to DC

	HBITMAP hOldMainBitmap = (HBITMAP)::SelectObject(hMainDC,hSourceBitmap);
	HBITMAP hOldAndMaskBitmap  = (HBITMAP)::SelectObject(hAndMaskDC,
			*hAndMaskBitmap);
	HBITMAP hOldXorMaskBitmap  = (HBITMAP)::SelectObject(hXorMaskDC,
			*hXorMaskBitmap);

	//Scan each pixel of the souce bitmap and create the masks

	COLORREF MainBitPixel;
	for(int x=0;x<bm.bmWidth;++x)
	{
		for(int y=0;y<bm.bmHeight;++y)
		{
			MainBitPixel = ::GetPixel(hMainDC,x,y);
			if(MainBitPixel == clrTransparent)
			{
				::SetPixel(hAndMaskDC,x,y,RGB(255,255,255));
				::SetPixel(hXorMaskDC,x,y,RGB(0,0,0));
			}
			else
			{
				::SetPixel(hAndMaskDC,x,y,RGB(0,0,0));
				::SetPixel(hXorMaskDC,x,y,MainBitPixel);
			}
		}
	}

	::SelectObject(hMainDC,hOldMainBitmap);
	::SelectObject(hAndMaskDC,hOldAndMaskBitmap);
	::SelectObject(hXorMaskDC,hOldXorMaskBitmap);

	::DeleteDC(hXorMaskDC);
	::DeleteDC(hAndMaskDC);
	::DeleteDC(hMainDC);

	::ReleaseDC(NULL,hDC);
}


void getLocalSettings()
{
	if(access("cfg",0)==-1)
	{
		//create
		FILE *file=fopen("cfg","w+");
		const char defSets[]="0 0 0 238 243 12";
		fprintf(file,defSets);
		fclose(file);
		return;
	}
	else
	{
		FILE *file=fopen("cfg","r");
		int sets[6];
		for( int i=0;i<6;i++)
			fscanf(file,"%d",&sets[i]);
		fclose(file);
		curGpu=sets[0];
		curFontSize=sets[1];
		autoStart=sets[2];
		colorTray=RGB(sets[3],sets[4],sets[5]);

		return;


	}
}

void saveSettings()
{
	FILE *file=fopen("cfg","w+");
	fprintf(file,"%d %d %d %d %d %d",curGpu,curFontSize,autoStart,GetRValue(colorTray),\
			GetGValue(colorTray),GetBValue(colorTray));
	fclose(file);
}


void setAutoStart(bool start)
{
	const wchar_t regPath[255]=L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";
	char curPath[255];
	GetModuleFileNameA(NULL,curPath,MAX_PATH);
	HKEY hRegKey;
	LONG res=0;
	char ssss[20];
	res=RegOpenKeyExW(HKEY_LOCAL_MACHINE,regPath,0,KEY_ALL_ACCESS ,&hRegKey);
	if(res==ERROR_SUCCESS)
	{
		if(start)
		{
			res=RegSetValueExA(hRegKey,"tmpHacker",0,REG_SZ,\
					(const unsigned char *)curPath,  \
					MAX_PATH);
			if(res==5)
			{
				MessageBoxW(0,L"请以管理员身份运行",L"设置失败",0);
			}
		}
		else
		{
			res=RegDeleteValueA(hRegKey,"tmpHacker");
		}
		RegCloseKey(hRegKey);
	}
	else
	{
		sprintf(ssss,"open reg fail code:%d",res);
		MessageBox(0,ssss,"",0);
	}


}

bool getAutoStart()
{
}
