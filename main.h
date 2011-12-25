
#pragma once
//functions------------------------
//windows
static BOOL InitWindow( HINSTANCE hInstance, int nCmdShow );
void createCtrls(HWND&hwnd, WPARAM wParam);
BOOL CALLBACK WinProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
void onTray(WPARAM wParam,LPARAM lParam);
//icon
void setIcon(bool deleteIt=false);
void createIcon(HICON*iconn,int num);
//font
void setFont();
//gpu
void initializeGpuServices(void);
void endGpuServices(void);
int getCurGpuTemp();
//settings
//if the file is NOT exist,create it,and assign default settings
void getLocalSettings();
void saveSettings();
//autostart
void setAutoStart(bool start);
//variables-------------------------
//
//
static HWND wnd;
bool firstSetIcon=true;
static HINSTANCE hinstance;
static HMENU menu;
//control
//
static HWND comboGpu;
static HWND comboFont;
static HWND btColorChoose;
static HWND checkAutoRun;
static HWND btWebSite;
enum ID_CTRL
{
	COMBOGPU=3500,
	COMBOFONT,
	BTCOLORCHOOSE,
	CHECKAUTORUN,
	BTWEBSITE

};
//settings,saved locally
static int curGpu=0;
static int curFontSize=0;//0 1 2
static int autoStart=0;
static COLORREF colorTray=RGB(238,243,12);
//
static int gpuCounts=0;
static NvPhysicalGpuHandle physicalGpuArray[NVAPI_MAX_PHYSICAL_GPUS];

static NV_GPU_THERMAL_SETTINGS  thermalSet={0};
static NvAPI_Status status;
//
static HFONT   hf;   
static LOGFONT   lf;   
//define
#define TRAY_NOTIFY 3001

