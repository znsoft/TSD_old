
#define UNICODE
#define WINCE_DEFAULT_LIBS
#include <windows.h>
#include <windowsx.h>
#include <aygshell.h>
#include <shellapi.h>
#include <sipapi.h>
#include <Wingdi.h>
#include <winuser.h>
#include <winuserm.h>
#include <Connmgr.h>
#include <Wininet.h>
#include <tpcshell.h>
#include <mmsystem.h>
#include <DeviceAPI.h>
#include "main.h"
#include "winsock.h"
//#include "ntddndis.h"
#pragma comment( lib, "ws2" )
#pragma comment( lib, "coredll")
#define  ScanBuf    10	//буфер сканирования кэш
#define  GUIDSIZE   2000	//размер временной таблицы гуидов (строк)
#define  BUFSIZE    200000	// размер выделяемой памяти для буфера приема отсылки
#define  TempSize   5000
#define  CFGFILE    L"tsdconfig.xml"
#define  VERSION    L"103"
#define  NO_ST FALSE
#define  CODEPAGE  CP_UTF8	//1С работает только с кодировкой UTF8 а windows только с 1251 нужно постоянно конвертировать
#define WSAGETSELECTERROR(lParam)  HIWORD(lParam)
#define WSAGETSELECTEVENT(lParam)  LOWORD(lParam)
#define NELEMS(a)  (sizeof(a) / sizeof((a)[0]))
/** Prototypes **************************************************************/
static LRESULT CALLBACK MainDlgProc(HWND, UINT, WPARAM, LPARAM);
static LRESULT CALLBACK command1Proc(HWND, UINT, WPARAM, LPARAM);
static LRESULT CALLBACK GoodsListProc(HWND, UINT, WPARAM, LPARAM);
static _Bool DoCommands(HWND, wchar_t *);
static LRESULT CALLBACK Autorization(HWND, UINT, WPARAM, LPARAM);
static LRESULT CALLBACK OptionsProc(HWND, UINT, WPARAM, LPARAM);
static LRESULT CALLBACK TreeGoodsProc(HWND, UINT, WPARAM, LPARAM);
static LRESULT CALLBACK QuestSelect(HWND, UINT, WPARAM, LPARAM);
static LRESULT CALLBACK VvodKolvo(HWND, UINT, WPARAM, LPARAM);
static LRESULT CALLBACK Erroring(HWND, UINT, WPARAM, LPARAM);
static LRESULT CALLBACK InfoProc(HWND, UINT, WPARAM, LPARAM);
static LRESULT CALLBACK Vopros(HWND, UINT, WPARAM, LPARAM);
static LRESULT CALLBACK ScanForm(HWND, UINT, WPARAM, LPARAM);
static LRESULT CALLBACK DefragProc(HWND, UINT, WPARAM, LPARAM);

static void getform1c();
static void refreshtree(HWND);
static _Bool waitforServer(HWND, _Bool);
static _Bool waitforServerList(HWND, _Bool);


typedef
int NDIS_STATUS, *PNDIS_STATUS;

//enum states{petya,vasya,stop}state;

typedef struct {
	DWORD dwDataLen;
	LPBYTE pData;
} RAW_DATA, *PRAW_DATA;

typedef UCHAR NDIS_802_11_MAC_ADDRESS[6];
///*Работа с вифи
typedef struct _NDIS_802_11_SSID {
	ULONG SsidLength;
	UCHAR Ssid[32];
} NDIS_802_11_SSID, *PNDIS_802_11_SSID;

typedef
LONG NDIS_802_11_RSSI;

typedef enum _NDIS_802_11_NETWORK_TYPE {
	Ndis802_11FH,
	Ndis802_11DS,
	Ndis802_11NetworkTypeMax,
} NDIS_802_11_NETWORK_TYPE, *PNDIS_802_11_NETWORK_TYPE;

//IOCTL_NDIS_GET_ADAPTER_NAMES  // need to search in msdn

typedef enum _NDIS_802_11_WEP_STATUS {
	Ndis802_11WEPEnabled,
	Ndis802_11Encryption1Enabled = Ndis802_11WEPEnabled,
	Ndis802_11WEPDisabled,
	Ndis802_11EncryptionDisabled = Ndis802_11WEPDisabled,
	Ndis802_11WEPKeyAbsent,
	Ndis802_11Encryption1KeyAbsent = Ndis802_11WEPKeyAbsent,
	Ndis802_11WEPNotSupported,
	Ndis802_11EncryptionNotSupported = Ndis802_11WEPNotSupported,
	Ndis802_11Encryption2Enabled,
	Ndis802_11Encryption2KeyAbsent,
	Ndis802_11Encryption3Enabled,
	Ndis802_11Encryption3KeyAbsent
} NDIS_802_11_WEP_STATUS, *PNDIS_802_11_WEP_STATUS, NDIS_802_11_ENCRYPTION_STATUS, *PNDIS_802_11_ENCRYPTION_STATUS;

typedef struct {
	LPWSTR wszGuid;
	LPWSTR wszDescr;
	ULONG ulMediaState;
	ULONG ulMediaType;
	ULONG ulPhysicalMediaType;
	INT nInfraMode;
	INT nAuthMode;
	INT nWepStatus;
	DWORD dwCtlFlags;
	DWORD dwCapabilities;
	RAW_DATA rdSSID;
	RAW_DATA rdBSSID;
	RAW_DATA rdBSSIDList;
	RAW_DATA rdStSSIDList;
	BOOL bInitialized;
} INTF_ENTRY, *PINTF_ENTRY;

/*
typedef struct
{
    ULONG Length;
    DWORD dwCtlFlags;
    NDIS_802_11_MAC_ADDRESS MacAddress;
    UCHAR Reserved[2];
    NDIS_802_11_SSID Ssid;
    ULONG Privacy;
    NDIS_802_11_RSSI Rssi;
    NDIS_802_11_NETWORK_TYPE NetworkTypeInUse;
    
	NDIS_802_11_CONFIGURATION Configuration;
 
   NDIS_802_11_NETWORK_INFRASTRUCTURE  InfrastructureMode;
    NDIS_802_11_RATES SupportedRates;
    ULONG KeyIndex;
    ULONG KeyLength;
    UCHAR   KeyMaterial[WZCCTL_MAX_WEPK_MATERIAL];
    NDIS_802_11_AUTHENTICATION_MODE AuthenticationMode;
    RAW_DATA rdUserData;
    WZC_EAPOL_PARAMS EapolParams;
    RAW_DATA rdNetworkData;
    ULONG WPAMCastCipher;
    ULONG ulMediaType;
} WZC_WLAN_CONFIG, *PWZC_WLAN_CONFIG;

void WINAPI WZCQueryInterface(LPWSTR,DWORD,PINTF_ENTRY,LPDWORD);
typedef void (WINAPI *PFN_WZCQueryInterface)(LPWSTR,DWORD,PINTF_ENTRY,LPDWORD);

void WINAPI WZCSetInterface(LPWSTR,DWORD,PINTF_ENTRY,LPDWORD);
typedef void (WINAPI *PFN_WZCSetInterface)(LPWSTR,DWORD,PINTF_ENTRY,LPDWORD);

void WINAPI WZCRefreshInterface(LPWSTR,DWORD,PINTF_ENTRY,LPDWORD);
typedef void (WINAPI *PFN_WZCRefreshInterface)(LPWSTR,DWORD,PINTF_ENTRY,LPDWORD);
*/
// setIP.cpp : Defines the entry point for the application.
#define FILE_DEVICE_PHYSICAL_NETCARD    0x00000017
#define FILE_ANY_ACCESS                 0
#define WINCE_IOCTL_START     8
#define METHOD_OUT_DIRECT               2
#define CTL_CODE( DeviceType, Function, Method, Access ) ( ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method) )
#define NDIS_CONTROL_CODE(request,method) CTL_CODE(FILE_DEVICE_PHYSICAL_NETCARD, request, method, FILE_ANY_ACCESS)
#define IOCTL_NDIS_REBIND_ADAPTER    NDIS_CONTROL_CODE( WINCE_IOCTL_START+3, METHOD_OUT_DIRECT )
#define IOCTL_NDIS_GET_ADAPTER_NAMES   NDIS_CONTROL_CODE(WINCE_IOCTL_START+6,METHOD_OUT_DIRECT)
//int restartadapter()
//{
	//WCHAR Names[50];
	//DWORD bytes;
	//HANDLE hNdis = CreateFile(_T("NDS0:"),
		//GENERIC_READ | GENERIC_WRITE,
		//FILE_SHARE_READ | FILE_SHARE_WRITE,
		//NULL, OPEN_EXISTING,
		//0, NULL);
	//if (hNdis == INVALID_HANDLE_VALUE)
	//{
		//return 0;
	//}
	//if (!DeviceIoControl(hNdis, IOCTL_NDIS_GET_ADAPTER_NAMES, NULL, 0, Names, 50, &bytes, NULL))
	//{
	//}
	//DWORD len = wcslen(Names);
	//Names[len] = 0;
	//Names[len + 1] = 0;
	//wchar_t keyName[256];
	//wsprintf(keyName, _T("Comm\\%s\\Parms\\TCPIP"), Names);
	//if (!DeviceIoControl(hNdis, IOCTL_NDIS_REBIND_ADAPTER, Names, wcslen(Names) * 2,  // buf contains the name of the
			//NULL, 0, NULL, NULL))
	//{
	//}
	//CloseHandle(hNdis);
	//return 1;
//}
////////////////////////////////
//
#define C_PAGES 2

typedef struct tag_tabhdr {
	HWND hTab;	// tab control
	int iIndex;	// current tab index
	BOOL bValid;	// current tab status
	HWND hDlg[C_PAGES];	// child windows
} TABHDR;


struct guids {
	wchar_t guid[256];	//, name[50];
	wchar_t Adress[10];
	wchar_t Article[55];
	_Bool flag;
	int kolvo;
	int Param;
	HTREEITEM hti;
}    *uuid, tmp[3][ScanBuf];
//HMENU hPopupMenu;
static _Bool UseMotorolla = TRUE;
static int tab1l;	//размер 1 таблицы сканов 
static int tab2l;	//размер 2 таблицы сканов 
static _Bool nowtab1;	//текущая таблица кэш сканов
static int GUIDSCOUNT;	//счетчик загруженных кэш данных 
static HINTERNET MyInternet;	//глобальная переменая работы с интеренет в режиме удержания сессии 1с
static HINTERNET MyConnect;	//глобальная переменая работы с интеренет в режиме удержания сессии 1с
static _Bool inthread;	//флаг запущенного потока
static _Bool RepaintMainMenu;	//флаг перерисовки основного окна
static _Bool cachemode;	//флаг режима кэширования данных
static _Bool NowSending;	//флаг занятости модуля связи
static _Bool debugmode;	//флаг экранной отладки
static _Bool tomainmenu;	//флаг закрытия всех окон кроме главного
static wchar_t CONFILE[200];
static DWORD dwThread;	//глобальный идентификатор потока
//static wchar_t oldsession[200];   //адрес сервера
static wchar_t serverWS[300];	//адрес сервера
static wchar_t PortWS[100];	//порт сервера
static wchar_t serviceWS[200];	//текущая настройка сервиса 1с на сервере
static wchar_t UserWS[200];	//текущий логин пользователя
static wchar_t PswWS[200];	//текущий пароль пользователя в открытом виде
static wchar_t oldadres[300];
static wchar_t raskrit[100];	//раскрытый адрес в дереве подбора
static HANDLE g_hInstance;
static HWND ghwndMB;	//Идентификатор глобального окна программы
static HWND MainMenuhwnd = 0;
static HTREEITEM hti[10];	//количество максимально уровней дерева
//static int leds;
static int tabsize;
static int Windows;
static int kbitsec = 0;	//измеренная скорось приемо/передачи данных
static int sndkbitsec = 0;	//измеренная скорось передачи данных
static unsigned short prt;	//целочисленное представление порта соединения
static int sx, sy, Goodslistmode, ListMode2;	//размер экрана и текущий тип операции 
static HWND tekWnd;	//хэндл текущего окна для получения расположения контролов из 1с
static HWND hwnd;	//хэндл глобального текщего окна приложения
static _Bool ReconnectAllways;	//опция по удержанию сессии 1с
static BYTE keys[300], Pressed[300];
static long int editc;	//флаг сейчас редактируется текст в ячейке
static ULONG lastoperation;
static unsigned long int tekpos;	//текущая позиция курсора в списке
RECT rect;
//wchar_t filialname[300];
//wchar_t filialguid[200];
wchar_t zagolovokokna[100];	//срока которая передается в 1с для получения расположения элементов формы
wchar_t knopkadalee[100];	//текст на кнопке Далее закрыть разных форм , еще используется в операциях списка
wchar_t label1[500];	//надпись лэйбл на форме списка
wchar_t sklad[200];	//текущий выбранный адрес
wchar_t vsklad[200];	//адрес куда помещаем товар
wchar_t otsklad[200];	//адрес откуда перемещаем/получаем товар
wchar_t selfdir[500];	//каталог приложения
wchar_t ipadr[100];	//айпи адрес мобильного устр-ва
wchar_t scanline[200];	//считаный штрих-код
//wchar_t ips[100];
char localhost[100];	//имя хоста мобильного устройства
IN_ADDR ip;	//работа с getsocopt 
static long packetsize, timeforpacket;	//для замеров скорости передачи данных
//HANDLE g_ConnHandle;
DWORD dwBytesRead;	//количество считанных байт пакета
char *szData, *otvet;	//буфера приема 
wchar_t *wzData, *wotvet;	//буфера парсинга пакетов
//HMIXEROBJ Mixer;
HMIXER Mixer;	//управление громкостью звука
MIXERLINE MixerLine;
MIXERCONTROL MixerControl;
MIXERLINECONTROLS MixerLineControls;
MIXERCONTROLDETAILS pmxcd;
HCURSOR hOldCur;
int maxvol;

//показываем или непоказываем часики
void WaitBox(_Bool wait)
{
	if (!UseMotorolla)
	{
		if (wait)
		{
			hOldCur = SetCursor(LoadCursor(NULL, IDC_WAIT));
		}
		else
		{
			SetCursor(hOldCur);
		}
	}
}
//
void SipDown(HWND hwnd)
{


	SIPINFO *pSipInfo = (SIPINFO *)malloc(sizeof(SIPINFO));
	memset(pSipInfo, 0, sizeof(SIPINFO));

	BOOL res = SipGetInfo(pSipInfo);
	pSipInfo->fdwFlags = SIPF_OFF;
	res = SipSetInfo(pSipInfo);
//if(!res)if(!
	SipShowIM(SIPF_OFF);	//)
//{
	SHSipPreference(hwnd, SIP_DOWN);
	HWND sip = FindWindow(TEXT("SipWndClass"), NULL);
//RECT oldSize; 
//GetWindowRect(sip,&oldSize);
//LONG x =0;
//LONG y =195;
//LONG w =oldSize.right-oldSize.left;
//LONG h = oldSize.bottom-oldSize.top;
//SetWindowPos(sip,HWND_BOTTOM, x, y,w,h,SWP_NOZORDER);
////показать клаву
//ShowWindow(sip, SW_SHOW);
//скрыть клаву
	ShowWindow(sip, SW_HIDE);
	sip = FindWindow(TEXT("MS_SIPBUTTON"), NULL);
	ShowWindow(sip, SW_HIDE);
}
//функция выключающая звук 
_Bool initmute(HWND hwnd, DWORD hInstance)
{
	MMRESULT er;
	maxvol = 0xFFFF;
	waveOutGetVolume(0, &maxvol);
	if (maxvol == 0)
		maxvol = 0xffff;
	UINT puMxId = mixerGetNumDevs();
	wchar_t err[1000];
	hwnd = GetForegroundWindow();
	Mixer = NULL;
	er = mixerGetID((HMIXEROBJ)mixerGetNumDevs(), &puMxId, MIXER_OBJECTF_MIXER);
	//if(er!= MMSYSERR_NOERROR){    
	wsprintf(err, L"%x ", puMxId);

//  MessageBox(0,L"необходимо самостоятельно отключить звук сканера ШК",err,0);
	//MessageBox(0,err,err,0);
//}
//  waveOutSetVolume(0, 0x48444844);
	waveOutSetVolume(0, 0);
//          er=mixerOpen(&Mixer, 0, (DWORD) hwnd, hInstance,  MIXER_OBJECTF_MIXER|CALLBACK_WINDOW);
	er = mixerOpen(&Mixer, puMxId, 0, 0, MIXER_OBJECTF_MIXER);
	//er=mixerOpen(&Mixer, 0, 0, 0, 0);
	if (er != MMSYSERR_NOERROR)
	{
		wsprintf(err, L"%x = %x", er, MMSYSERR_BADDEVICEID);

//  MessageBox(0,L"необходимо самостоятельно отключить звук сканера ШК",err,0);
		//MessageBox(0,err,err,0);
		//  ExitProcess(0);
		return FALSE;
	}
	MixerLine.cbStruct = sizeof(MIXERLINE);
	MixerLine.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
	mixerGetLineInfo((HMIXEROBJ)Mixer, &MixerLine, MIXER_GETLINEINFOF_COMPONENTTYPE);
	MixerControl.cbStruct = sizeof(MIXERCONTROL);
	MixerLineControls.cbStruct = sizeof(MIXERLINECONTROLS);
	MixerLineControls.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
	MixerLineControls.dwLineID = MixerLine.dwLineID;
	MixerLineControls.cControls = 1;
	MixerLineControls.cbmxctrl = sizeof(MIXERCONTROL);
	MixerLineControls.pamxctrl = &MixerControl;

	if (mixerGetLineControls((HMIXEROBJ)Mixer, &MixerLineControls, MIXER_GETLINECONTROLSF_ONEBYTYPE) != MMSYSERR_NOERROR)
	{
		return FALSE;
	}

	int LineID = MixerLineControls.dwLineID;
	int VolumeControlID = MixerControl.dwControlID;
	int ChannelsCount = MixerLine.cChannels;
	int MinimalVolume = MixerControl.Bounds.dwMinimum;
	int MaximalVolume = MixerControl.Bounds.dwMaximum;

	MixerLineControls.dwControlType = MIXERCONTROL_CONTROLTYPE_MUTE;
	ZeroMemory(&MixerControl, sizeof(MIXERCONTROL));

	if (mixerGetLineControls((HMIXEROBJ)Mixer, &MixerLineControls, MIXER_GETLINECONTROLSF_ONEBYTYPE) != MMSYSERR_NOERROR)
	{
		return FALSE;
	}
	int MuteControlID = MixerControl.dwControlID;

	MIXERCONTROLDETAILS_BOOLEAN mxcdMute = { (LONG)TRUE };

	MIXERCONTROLDETAILS_BOOLEAN mxcdMute1;
	pmxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
	pmxcd.dwControlID = MuteControlID;
	pmxcd.cChannels = ChannelsCount;
	pmxcd.cMultipleItems = 0;
	pmxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
	pmxcd.paDetails = &mxcdMute1;

	if (mixerGetControlDetails((HMIXEROBJ)Mixer, &pmxcd, MIXER_OBJECTF_HMIXER | MIXER_GETCONTROLDETAILSF_VALUE) != MMSYSERR_NOERROR)
	{
		return FALSE;
	}

	pmxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
	pmxcd.dwControlID = MuteControlID;
	pmxcd.cChannels = ChannelsCount;
	pmxcd.cMultipleItems = 0;
	pmxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
	pmxcd.paDetails = &mxcdMute;

	if (mixerSetControlDetails((HMIXEROBJ)Mixer, &pmxcd, MIXER_OBJECTF_HMIXER | MIXER_SETCONTROLDETAILSF_VALUE) != MMSYSERR_NOERROR)
	{
		return FALSE;
	}
	return TRUE;
}
//функция воспроизведения звука только из нашего приложения
void _PlaySound(LPCWSTR l, HMODULE h, DWORD d)
{
	// NLedSetDevice(
	waveOutSetVolume(0, maxvol);
	PlaySound(l, h, d);
//PlaySound(TEXT("*vibrate*"), NULL, SND_ALIAS);
	waveOutSetVolume(0, 0);
}
//функция воспроизведения звука "порядок" / "ошибка"
void SoundOK(_Bool isOK)
{
	//doLeds(isOK);
	if (isOK)
	{

		_PlaySound(TEXT("SystemQuestion"), NULL, SND_ALIAS);
	}
	else
		_PlaySound(TEXT("SystemAsterisk"), NULL, SND_ALIAS);
	//doLeds(isOK);
}
//функция удаленного получения снмка экрана устройства
//LPBYTE screenshot(wchar_t *filename)
//{
	//HDC hdcScr = GetDC(0);
	//HDC hdcCop = CreateCompatibleDC(hdcScr);
	//int x = GetSystemMetrics(SM_CXSCREEN);
	//int y = GetSystemMetrics(SM_CYSCREEN);
	//BITMAPINFO *pbmi;
	//pbmi = (BITMAPINFO *)malloc(sizeof(BITMAPINFOHEADER));
	//pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	//pbmi->bmiHeader.biWidth = 2 * x / 3;
	//pbmi->bmiHeader.biHeight = 2 * y / 3;
	//pbmi->bmiHeader.biPlanes = 1;
	//pbmi->bmiHeader.biBitCount = 24;
	//pbmi->bmiHeader.biCompression = BI_RGB;
	//BYTE *rawBits;
	//HBITMAP HBM = CreateDIBSection(NULL, pbmi, DIB_RGB_COLORS, (void **)&rawBits, NULL, 0);
	//SelectObject(hdcCop, HBM);
	//StretchBlt(hdcCop, 0, 0, 2 * x / 3, 2 * y / 3, hdcScr, 0, 0, x, y, SRCCOPY);
	////BitBlt(hdcCop, 0, 0, x, y, hdcScr, 0, 0, SRCCOPY);
	//BITMAP BM;
	//BITMAPFILEHEADER BFH;
	//LPBITMAPINFO BIP;
	//BYTE *BmpData;
	//DWORD ColorSize, DataSize;
	//WORD BitCount;


	//GetObject(HBM, sizeof(BITMAP), (LPSTR) & BM);
	//BitCount = (WORD)BM.bmPlanes * BM.bmBitsPixel;
	//ColorSize = (1 << BitCount);
	//if (ColorSize > 256)
		//ColorSize = 0;
	//DataSize = ((BM.bmWidth * BitCount + 31) & ~31) / 8 * BM.bmHeight;

	//BIP = (BITMAPINFO *)malloc(sizeof(BITMAPINFOHEADER));

	//BIP->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	//BIP->bmiHeader.biWidth = BM.bmWidth;
	//BIP->bmiHeader.biHeight = BM.bmHeight;
	//BIP->bmiHeader.biPlanes = 1;
	//BIP->bmiHeader.biBitCount = BitCount;
	//BIP->bmiHeader.biCompression = 0;
	//BIP->bmiHeader.biSizeImage = DataSize;
	//BIP->bmiHeader.biXPelsPerMeter = 0;
	//BIP->bmiHeader.biYPelsPerMeter = 0;
	////if (BitCount < 16)
	//BIP->bmiHeader.biClrUsed = 0;
	//BIP->bmiHeader.biClrImportant = 0;


	//WORD bfType = 0x4d42;
	//DWORD bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + BIP->bmiHeader.biClrUsed * sizeof(RGBQUAD) + DataSize;
	//DWORD bfReserved = 0;
	//DWORD bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + BIP->bmiHeader.biClrUsed * sizeof(RGBQUAD);
	//BmpData = (BYTE *) (BM.bmBits);
	//ReleaseDC(0, hdcScr);

	//DWORD dwTemp;
	//HANDLE FP = CreateFile(filename, GENERIC_READ | GENERIC_WRITE, 0, NULL,
		//CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	//// WriteFile(FP,&BFH,sizeof(BITMAPFILEHEADER),&dwTemp,NULL);
	//WriteFile(FP, &bfType, sizeof(bfType), &dwTemp, NULL);
	//WriteFile(FP, &bfSize, sizeof(bfSize), &dwTemp, NULL);
	//WriteFile(FP, &bfReserved, sizeof(bfReserved), &dwTemp, NULL);
	//WriteFile(FP, &bfOffBits, sizeof(bfOffBits), &dwTemp, NULL);


	//WriteFile(FP, BIP, 40, &dwTemp, NULL);
	//WriteFile(FP, BmpData, DataSize, &dwTemp, NULL);

	//CloseHandle(FP);
	//DeleteObject(HBM);
	//DeleteObject(hdcCop);
	//return BmpData;
//}
//получаем данные о заряде батареи
void getbat(wchar_t *text)
{
	SYSTEM_POWER_STATUS_EX2 ps2;
	GetSystemPowerStatusEx2(&ps2, sizeof(ps2), TRUE);
	wsprintf(text, L"BackupBatteryVoltage=%d:BackupBatteryLifePercent=%d:BatteryCurrent=%d:BatteryVoltage=%d:BatteryTemperature=%d:ACLineStatus=%d:BatteryAverageCurrent=%d:BackupBatteryFlag=%d:BackupBatteryFullLifeTime=%u:BackupBatteryLifeTime=%u:BatteryAverageInterval=%d:BatteryFlag=%d:BatterymAHourConsumed=%d:BatteryFullLifeTime=%u:BatteryLifePercent=%d:LastPacketSize=%u:LastPacketTime=%d:kbitsec=%d:sndkbitsec=%d:cachemode=%d:BatteryChemistry=%d", ps2.BackupBatteryVoltage, ps2.BackupBatteryLifePercent, ps2.BatteryCurrent, ps2.BatteryVoltage, ps2.BatteryTemperature, ps2.ACLineStatus, ps2.BatteryAverageCurrent, ps2.BackupBatteryFlag, ps2.BackupBatteryFullLifeTime, ps2.BackupBatteryLifeTime, ps2.BatteryAverageInterval, ps2.BatteryFlag, ps2.BatterymAHourConsumed, ps2.BatteryFullLifeTime, ps2.BatteryLifePercent, packetsize, timeforpacket, kbitsec, sndkbitsec, (int)cachemode, ps2.BatteryChemistry);
}
//выводим смс с текстом в углу экрана поверх всех окон
//void smshint(wchar_t *text, int i)
//{
	//HDC dc = GetDC(0);
	//RECT rc;
	//HWND hw = GetDesktopWindow();
	//GetWindowRect(hw, &rc);
	//HFONT fnt;
	//LOGFONT lgfnt;

	//fnt = (HFONT)SendMessage(hw, WM_GETFONT, 0, 0);
	//lgfnt.lfHeight = 11;
	//lgfnt.lfWidth = 6;
	//lgfnt.lfEscapement = 0;
	//lgfnt.lfOrientation = 0;
	//lgfnt.lfWeight = 400;
	//lgfnt.lfCharSet = RUSSIAN_CHARSET;
	//lgfnt.lfOutPrecision = OUT_DEFAULT_PRECIS;
	//lgfnt.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	//lgfnt.lfPitchAndFamily = FF_DONTCARE;
	//lgfnt.lfQuality = DEFAULT_QUALITY;
	//lgfnt.lfItalic = FALSE;
	//lgfnt.lfUnderline = FALSE;
	//lgfnt.lfStrikeOut = FALSE;
	//fnt = (HFONT)CreateFontIndirect(&lgfnt);


	////rc.top = 0;
	//rc.bottom = lgfnt.lfHeight;
	//rc.right = lgfnt.lfWidth * (wcslen(text) + 1);
	//HBRUSH br = CreateSolidBrush(65535 - i);
	//SelectObject(dc, br);
	//FillRect(dc, &rc, br);
	//DeleteObject(br);
	//SetTextColor(dc, i);
	//SelectObject(dc, fnt);
	//SetBkMode(dc, TRANSPARENT);
	//DrawText(dc, text, wcslen(text), &rc, DT_NOCLIP | DT_WORDBREAK);
	//DeleteObject(fnt);

//}
//выводим текст посередине экрана поверх всех окон
void MyMessageWait(wchar_t *text)
{
	RECT rc;
	GetWindowRect(GetDesktopWindow(), &rc);
	rc.left = rc.right / 3;
	rc.top = rc.bottom / 2;
	if (!(text[0] == '.' || text[0] == ' '))
	{
		DrawText(GetDC(0), text, wcslen(text), &rc, DT_WORDBREAK);
	}
	else
	{
		WaitBox(text[0] == '.');
	}
}
//выводим текст на весь экран поверх всех окон
int MyMessageBox(HWND hwnd, wchar_t *text, wchar_t *header, int timer)
{
	int r;
	RECT rc;
	if (!debugmode)
		return 0;
	//KillTimer(hwnd,WM_USER+timer);
	GetWindowRect(GetDesktopWindow(), &rc);
	//r = MessageBox(hwnd,text,header,MB_ICONINFORMATION|MB_TOPMOST);
	DrawText(GetDC(0), text, wcslen(text), &rc, DT_WORDBREAK);
	//SetTimer(hwnd,WM_USER+timer,1000,NULL);
	return r;
}
//работа с буфером обмена
void getscanlinen(HWND wnd)
{
	wchar_t *buffer;
	if (OpenClipboard(wnd))
	{
		HANDLE hData = GetClipboardData(CF_UNICODETEXT);
		if (hData != NULL)
		{
			buffer = (wchar_t *)GlobalLock(hData);	//пример из инета и метод проб и ошибок
			wcscpy(scanline, buffer);
			GlobalUnlock(hData);
		}
		EmptyClipboard();
		CloseClipboard();
	}

}
//работа с буфером обмена
void getscanline(HWND wnd, wchar_t *code)
{
	char barcode[200];
	if (UseMotorolla)
	{
		memset(barcode, 0, 200);
		Barcode1D_scan(barcode);

		if (strlen(barcode) > 3)
		{
			MultiByteToWideChar(CODEPAGE, 0, barcode, strlen(barcode) + 1, code, 100);
			return;
		}

	}

	wcscpy(scanline, L"");
	getscanlinen(wnd);
	wcscpy(code, scanline);
	//if(wcslen(code)>2)_PlaySound(TEXT("Menu Selection"), NULL, SND_ALIAS);
}
//окно с текстом и кнопкой 
void ShowError(HWND hwndDlg, wchar_t *text)
{
	wchar_t errscn[100];
	EmptyClipboard();
	CloseClipboard();

	SoundOK(FALSE);	//)
	//wsprintf(text,L"%x",GetLastError());
	KillTimer(hwndDlg, WM_USER + 2);
	KillTimer(hwndDlg, WM_USER + 1);
	wcsncpy(label1, text, 500);
	getscanline(hwndDlg, errscn);
	DialogBox(g_hInstance, MAKEINTRESOURCE(1014), hwndDlg, (DLGPROC)Erroring);
	getscanline(hwndDlg, errscn);
	EmptyClipboard();
	CloseClipboard();

	SetTimer(hwndDlg, WM_USER + 2, 1000, NULL);
	SetTimer(hwndDlg, WM_USER + 1, 1000, NULL);
	return;

	//int x = GetSystemMetrics(SM_CXSCREEN);
	//int y = GetSystemMetrics(SM_CYSCREEN);
	//SetWindowPos(GetDlgItem(hwndDlg, 4500), 0, 0, 20, x - 30, 40, SWP_SHOWWINDOW);
	//SetWindowPos(GetDlgItem(hwndDlg, 4501), 0, x - 30, 20, 25, 40, SWP_SHOWWINDOW);
	//ShowWindow(GetDlgItem(hwndDlg, 4500), SW_SHOW);
	//ShowWindow(GetDlgItem(hwndDlg, 4501), SW_SHOW);
	//SetWindowLong(GetDlgItem(hwndDlg, 4500), GWL_STYLE, ES_MULTILINE | ES_AUTOVSCROLL);
	//SetWindowText(GetDlgItem(hwndDlg, 4500), text);
}
//интерактивный ответ на вопрос из 1с
int sendOtvetVopros(HWND hwndDlg, wchar_t *kudaotvet, wchar_t *zagolovok, wchar_t *vopros, wchar_t *numb, int i)
{
	wcscpy(zagolovokokna, zagolovok);
	wcscpy(label1, vopros);
	wcscpy(knopkadalee, kudaotvet);
	tekpos = i;
	tekWnd = hwndDlg;
	KillTimer(hwndDlg, WM_USER + 2);
	KillTimer(hwndDlg, WM_USER + 1);
	SoundOK(FALSE);	//)
	tomainmenu = FALSE;
	DialogBox(g_hInstance, MAKEINTRESOURCE(Voprosfrm), hwndDlg, (DLGPROC)Vopros);
	SetTimer(hwndDlg, WM_USER + 2, 1000, NULL);
	SetTimer(hwndDlg, WM_USER + 1, 1000, NULL);
	return tekpos;

}

//
int GetKeyboardState(BYTE *Keys)
{
	int i = 255, r = 0;
	for (; i--;)
	{
		Keys[i] = GetAsyncKeyState(i);	// (GetAsyncKeyState(i)==0)?0:(Keys[i]<250?Keys[i]++:250);
		if (Keys[i] != 0)
			r = i;

	}
	return r;
}


//получение локального айпи адреса
void localip(void)
{
	HOSTENT *phe;
	char answer[300];
	gethostname(answer, sizeof(answer));
	phe = gethostbyname(answer);
	ip = *(struct in_addr *)(phe->h_addr);
	strcpy(localhost, inet_ntoa(ip));
	MultiByteToWideChar(CODEPAGE, 0, localhost, strlen(localhost), ipadr, 100);
//  ip.S_un.S_un_b.s_b4++;
}

//парсинг
int searchblok(char *src, int dlina, char *srch)
{

	_Bool b = TRUE;
	int i = strlen(srch), n = 0;
	while (b)
		b = (strncmp(src + n, srch, i) != 0) && ((++n + i) <= dlina);
	n = (n + i) < dlina ? n : -1;
	return n;
}
//парсинг
int searchblokw(wchar_t *src, int dlina, wchar_t *srch)
{

	_Bool b = TRUE;
	int i = wcslen(srch), n = 0;
	while (b)
		b = ((wcsncmp(src + n, srch, i) != 0) && (src[n] != 0) && ((++n) <= dlina));
	//wsprintf(PswWS,L"%s %u %u %u",srch,i,n,dlina);
	//MessageBox(0,PswWS,src+n,n);
	n = ((n + i) < dlina) && src[n] != 0 ? n : -1;
	return n;
}
//------------------///----------------------------search string in text---------------------------------------------------------------------------------
int sstr(char *t, char *s)
{
	int j = 1, i = 0;
	while (t[i] > 0 && s[--j] > 0)
	{
		j = 1;
		_Bool b = TRUE;
		while (b)
			b = (t[i] != 0 && t[i++] != s[0]);	//search 1 char
		if (t[i] == 0)
			return 0;
		b = !b;
		while (b)
			b = ((t[i] == s[j++]) && (t[i++] != 0));	//check full string if char found
	}
	return i;
}
//парсинг
int xmlvalue(int start, char *valuename)
{
	//parsing return and them > and <
	unsigned int z = 0;
	memset(otvet, 0, BUFSIZE);
	z = sstr(szData + start, valuename);
	int i = start + z;
	if (z == 0)
		return -1;
	z = sstr(szData + i, ">");
	i += z;
	//int r = i;
	if (z == 0)
		return -1;
	strcpy(otvet, szData + i);	//,sstr(szData+i,"return"));
	//i=searchblok(otvet,strlen(otvet),"<");
	z = searchblok(otvet, strlen(otvet), "<");
	otvet[z] = 0;
	otvet[z + 1] = 0;
	return i;
}
//парсинг
int xmlgetw(int start, wchar_t *valuename)
{
	//parsing return and them > and <
	int z, d = wcslen(wzData);
	if (start > d)
		return -1;	//||start<0)return -1;
	memset(wotvet, 0, BUFSIZE);
	//WideCharToMultiByte(CODEPAGE,0,valuename,wcslen(valuename),teg,wcslen(valuename),0,0);
	z = searchblokw(wzData + start, d - start, valuename);
	//z =sstr( szData+start,teg);
	int i = start + z;
	if (z < 1 || d <= i)
		return -1;
	z = searchblokw(wzData + i, d - i, L">");
	//z= sstr( szData+i,">");
	i += z + 1;
	//int r = i;
	if (z < 1 || d <= i)
		return -1;
	z = searchblokw(wzData + i, d - i, L"<");
	if (z < 1 || d <= i)
		return -1;
	wcsncpy(wotvet, wzData + i, z);
	//wotvet[z]=0;
	//strcpy(otvet,szData+i);//,sstr(szData+i,"return"));
	//i=searchblok(otvet,strlen(otvet),"<");
	//otvet[searchblok(otvet,strlen(otvet),"<")]=0;
	return i;
}
//парсинг
int xmlget(int start, wchar_t *valuename)
{
	//parsing return and them > and <
	char teg[100];
	int z;
	memset(otvet, 0, BUFSIZE);
	WideCharToMultiByte(CODEPAGE, 0, valuename, wcslen(valuename), teg, wcslen(valuename), 0, 0);
	z = searchblok(szData + start, dwBytesRead - start, teg);
	//z =sstr( szData+start,teg);
	int i = start + z;
	if (z < 1 || dwBytesRead <= i)
		return -1;
	z = searchblok(szData + i, dwBytesRead - i, ">");
	//z= sstr( szData+i,">");
	i += z;
	//int r = i;
	if (z < 1 || dwBytesRead <= i)
		return -1;
	z = searchblok(szData + i, dwBytesRead - i, "<");
	if (z < 1 || dwBytesRead <= i)
		return -1;
	strncpy(otvet, szData + i, z);
	otvet[z] = 0;
	//strcpy(otvet,szData+i);//,sstr(szData+i,"return"));
	//i=searchblok(otvet,strlen(otvet),"<");
	//otvet[searchblok(otvet,strlen(otvet),"<")]=0;
	return i;
}
//парсинг
//void syntaxchk(wchar_t *code)
//{
	//return;
	//long int i = 0, z = 0;
	//while (i >= 0)
	//{
		//i = searchblokw(code + z, wcslen(code + z), L"&");
		//if (i < 0)
			//break;
		//wcscpy(wotvet, code + i + 1 + z);
		//code[i] = 0;
		//wcscat(code + z, L"&amp;");
		//wcscat(code + z, wotvet);
		//z = i + 1;
	//}
//}

//прием/передача пакета
void sendws1(wchar_t *text)
{
	//syntaxchk(text);
	//MyMessageWait(L".");
	MyMessageBox(0, L"connect 1С.", 0, 100);

	int i;
	HANDLE hFile;
	DWORD dwNumberOfBytesRead;
	memset(szData, 0, BUFSIZE);
	dwBytesRead = 0;
	wchar_t usr[150], psw[150];
	char answer[300];
	i = WideCharToMultiByte(CODEPAGE, 0, UserWS, wcslen(UserWS), answer, 300, 0, 0);
	memset(usr, 0, sizeof(usr));
	MultiByteToWideChar(CP_ACP, 0, answer, i, usr, sizeof(usr));
	memset(answer, 0, sizeof(answer));
	i = WideCharToMultiByte(CODEPAGE, 0, PswWS, wcslen(PswWS), answer, 300, 0, 0);
	memset(psw, 0, sizeof(psw));
	MultiByteToWideChar(CP_ACP, 0, answer, i, psw, sizeof(psw));
	wchar_t head[] = L"\nAccept: */*\nSOAPAction: \"\"\nContent-Type: text/xml; charset=utf-8";
	HINTERNET hInternet = InternetOpen(L"", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	//INTERNET_OPTION_CONNECT_TIMEOUT = 2
	int lTimeoutInMs = 60000;
	InternetSetOption(hInternet, INTERNET_OPTION_CONNECT_TIMEOUT, &lTimeoutInMs, 4);
	//MyMessageWait(L" ");
	HINTERNET hConnect = InternetConnectW(hInternet, serverWS, prt, usr, psw, INTERNET_SERVICE_HTTP, 0, 1);
	MyMessageWait(L".");
	HINTERNET hRequest = HttpOpenRequest(hConnect, L"POST", serviceWS, NULL, NULL, 0, INTERNET_FLAG_KEEP_CONNECTION, 1);
	if (hRequest == NULL)
	{
		MyMessageBox(0, L"hRequest", 0, 0);
		Sleep(1000);
		goto ehit;
	}

	i = WideCharToMultiByte(CODEPAGE, 0, text, -1, szData, BUFSIZE, 0, 0);
	//TxtFilter(szData);
	int szdta = strlen(szData);
	_Bool bSend = HttpSendRequest(hRequest, head, 65, szData, szdta);	//собсно посылаем нужные данные xml в 1с  wcslen(head)=65
	if (!bSend)
	{
		MyMessageBox(0, L"send fault", 0, 100);
		Sleep(500);
		goto ehit;
	}
	if (hRequest != NULL)
		MyMessageBox(0, L"Чтение ответа..", 0, 100);
	int fps = GetTickCount();
	_Bool bRead = InternetReadFile(hRequest, szData, BUFSIZE, &dwBytesRead);	//принимаем данные от 1с .. тут все побыстрей как то
	fps = GetTickCount() - fps;
	if (fps > 0)
		kbitsec = (((dwBytesRead) << 3) / fps);	//замер скорости
	//MyMessageWait(L" ");
	if (!bRead)
		MyMessageBox(0, L"Ошибка чтения", 0, 100);
  ehit:
	InternetCloseHandle(hRequest);
	// закрываем сессию
	InternetCloseHandle(hConnect);
	// закрываем WinInet
	InternetCloseHandle(hInternet);
	memset(wzData, 0, BUFSIZE);
	MyMessageWait(L" ");
}

////прием/передача пакета
//void OpenConnection()
//{
	//MyMessageBox(0, L"connect 1С.", 0, 100);
	//int i;
	//HANDLE hFile;
	//DWORD dwNumberOfBytesRead;
	//dwBytesRead = 0;
	//wchar_t usr[150], psw[150];
	//char answer[300];
	//i = WideCharToMultiByte(CODEPAGE, 0, UserWS, wcslen(UserWS), answer, 300, 0, 0);
	//memset(usr, 0, sizeof(usr));
	//MultiByteToWideChar(CP_ACP, 0, answer, i, usr, sizeof(usr));
	//memset(answer, 0, sizeof(answer));
	//i = WideCharToMultiByte(CODEPAGE, 0, PswWS, wcslen(PswWS), answer, 300, 0, 0);
	//memset(psw, 0, sizeof(psw));
	//MultiByteToWideChar(CP_ACP, 0, answer, i, psw, sizeof(psw));
	//MyInternet = InternetOpen(L"", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	//int lTimeoutInMs = 10000;
	//InternetSetOption(MyInternet, INTERNET_OPTION_CONNECT_TIMEOUT, &lTimeoutInMs, 4);
	//MyConnect = InternetConnectW(MyInternet, serverWS, prt, usr, psw, INTERNET_SERVICE_HTTP, 0, 1);
	//MyMessageWait(L".");
//}

////прием/передача пакета
void CloseConnection()
{
	InternetCloseHandle(MyConnect);
	InternetCloseHandle(MyInternet);
	MyConnect = NULL;
}
////прием/передача пакета
//void sendws2(wchar_t *text)
//{
	//int i;
	//HANDLE hFile;
	//DWORD dwNumberOfBytesRead;
	//memset(szData, 0, BUFSIZE);
	//dwBytesRead = 0;
	//MyMessageWait(L".");
	//HINTERNET hRequest;
	//i = 0;
	//wchar_t head[] = L"\nAccept: */*\nSOAPAction: \"\"\nContent-Type: text/xml; charset=utf-8";
	//MyMessageBox(0, L"HttpOpenRequest.", 0, 100);


  //again:
	//if (MyConnect == NULL)
	//{
		//CloseConnection();
		//OpenConnection();
	//}

	//hRequest = HttpOpenRequest(MyConnect, L"POST", serviceWS, NULL, NULL, 0, INTERNET_FLAG_KEEP_CONNECTION, 1);
	//if (hRequest == NULL)
		//if (i > 0)
		//{
			//MyMessageBox(0, L"hRequest", 0, 0);
			//Sleep(1000);
			//goto ehit;
		//}
		//else
		//{
			//MyConnect = NULL;
			//i++;
			//goto again;

		//}


	//i = WideCharToMultiByte(CODEPAGE, 0, text, -1, szData, BUFSIZE, 0, 0);
	//int szdta = strlen(szData);
	//_Bool bSend = HttpSendRequest(hRequest, head, 65, szData, szdta);	//собсно посылаем нужные данные xml в 1с  wcslen(head)=65
	//if (!bSend)
	//{
		//MyMessageBox(0, L"send fault", 0, 100);
		//Sleep(500);
		//goto ehit;
	//}
	//if (hRequest != NULL)
		//MyMessageBox(0, L"Чтение ответа..", 0, 100);
	//int fps = GetTickCount();
	//_Bool bRead = InternetReadFile(hRequest, szData, BUFSIZE, &dwBytesRead);	//принимаем данные от 1с .. тут все побыстрей как то
	//fps = GetTickCount() - fps;
	//if (fps > 0)
		//kbitsec = (((dwBytesRead) << 3) / fps);	//замер скорости
	////MyMessageWait(L" ");
	//if (!bRead)
		//MyMessageBox(0, L"Ошибка чтения", 0, 100);
  //ehit:
	//InternetCloseHandle(hRequest);
	//memset(wzData, 0, BUFSIZE);
	//MyMessageWait(L" ");
//}

//прием/передача пакета
void sendws(wchar_t *text)
{
	//if (ReconnectAllways)
	//{
		sendws1(text);
	//}
	//else
	//{
		//sendws2(text);
	//}
}

//Проверка обновлений на сервере
void sndupdate(wchar_t *text)
{
	wchar_t xmlstr[] = L"<soap:Envelope xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\"><soap:Header/><soap:Body><m:updatefirmware xmlns:m=\"http://www.dns-shop.tsd.ru\"><m:version xmlns:xs=\"http://www.w3.org/2001/XMLSchema\"\n		xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">%s</m:version>\n	</m:updatefirmware>\n	</soap:Body>\n</soap:Envelope>";
	wchar_t xmlstr1[TempSize];
	wsprintf(xmlstr1, xmlstr, text);
	sendws(xmlstr1);	//посыл данных 
}

//формирование пакета
void addxmlteg(wchar_t *xmlstring, wchar_t *teg, wchar_t *value)
{
	//wchar_t wzData[1000];
	wsprintf(wzData, teg, value);
	wcscat(xmlstring, wzData);
}
//загрузка фалов
HANDLE loadfile(wchar_t *filename, char *szf, int sz, int *d)
{
	HANDLE hFile;
	DWORD dwNumberOfBytesRead;
	hFile = CreateFile(filename, GENERIC_READ + GENERIC_WRITE, FILE_SHARE_READ + FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	*d = 0;
	if (hFile == INVALID_HANDLE_VALUE)
		return hFile;
	ReadFile(hFile, szf, sz, &dwNumberOfBytesRead, NULL);
	szf[dwNumberOfBytesRead] = 0;
	*d = dwNumberOfBytesRead;
	return hFile;
}
//сохранение параметров
void SaveParam()
{
	char port[200];

	wchar_t temp[TempSize];
	int i;
	wcscpy(temp, L"<xml>");
	if (wcslen(serverWS) > 1)
		addxmlteg(temp, L"<Server>%s</Server>\n", serverWS);	//else MessageBox(0,L"Server",L"write",0);
	if (wcslen(PortWS) > 1)
		addxmlteg(temp, L"<Port>%s</Port>\n", PortWS);	//else MessageBox(0,L"Port",L"write",0);
	if (wcslen(UserWS) > 1)
		addxmlteg(temp, L"<UserWS>%s</UserWS>\n", UserWS);	//else MessageBox(0,L"User",L"write",0);
	if (wcslen(serviceWS) > 1)
		addxmlteg(temp, L"<ServiceWS>%s</ServiceWS>\n", serviceWS);	//else MessageBox(0,L"Service",L"write",0);
	if (cachemode)
		addxmlteg(temp, L"<Cachemode>%s</Cachemode>\n", L"1");
	else
		addxmlteg(temp, L"<Cachemode>%s</Cachemode>\n", L"0");

	if (ReconnectAllways)
		addxmlteg(temp, L"<ReconnectAllways>%s</ReconnectAllways>\n", L"1");
	else
		addxmlteg(temp, L"<ReconnectAllways>%s</ReconnectAllways>\n", L"0");

	wcscat(temp, L"</xml>");
	WideCharToMultiByte(CODEPAGE, 0, temp, -1, szData, TempSize, 0, 0);

	HANDLE h = CreateFile(CONFILE, GENERIC_READ + GENERIC_WRITE, FILE_SHARE_READ + FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	WriteFile(h, szData, strlen(szData), &i, NULL);
	CloseHandle(h);
	WideCharToMultiByte(CODEPAGE, 0, PortWS, -1, port, TempSize, 0, 0);
	prt = atoi(port);
	if (prt == 0)
		prt = 52081;


}
//загрузка параметров
void loadconfigxml()
{
	unsigned long i = 0, l, z, q, k, b;
//char * f;
	wchar_t temp[TempSize];
	wcscpy(serverWS, L"art-sql1.partner.ru");
	wcscpy(PortWS, L"52081");
	wcscpy(serviceWS, L"/WS_Sklad/ws/TSD.1cws");
	wcscpy(UserWS, L"WebConnection");
	wcscpy(PswWS, L"");
	HANDLE h = loadfile(CONFILE, szData, TempSize, &q);
	CloseHandle(h);

	if (q > 10)
	{
		MultiByteToWideChar(CODEPAGE, 0, szData, -1, wzData, TempSize);
		i = xmlgetw(0, L"<Serve");
		if (i > 1)
			wcscpy(serverWS, wotvet);	//else MessageBox(0,L"Server",L"read",0);
		i = xmlgetw(0, L"<Port");
		if (i > 1)
			wcscpy(PortWS, wotvet);	//else MessageBox(0,L"Port",L"read",0);
		i = xmlgetw(0, L"<UserW");
		if (i > 1)
			wcscpy(UserWS, wotvet);	//else MessageBox(0,L"User",L"read",0);
		i = xmlgetw(0, L"<Service");
		if (i > 1)
			wcscpy(serviceWS, wotvet);	//else MessageBox(0,L"Service",L"read",0);
		i = xmlgetw(0, L"<Cachemode");
		if (i > 1)
			cachemode = wcscmp(wotvet, L"1") == 0;
		i = xmlgetw(0, L"<ReconnectAllways");
		if (i > 1)
			ReconnectAllways = wcscmp(wotvet, L"1") == 0;
	}
	SaveParam();
}
//глобальный выход из программы с высвобождением памяти
void clearExit()
{
	//if(UseMotorolla){
	//Barcode1D_free();
	//}
	CloseConnection();
	LocalFree(szData);
	LocalFree(otvet);
	LocalFree((char *)wzData);
	LocalFree((char *)wotvet);
	LocalFree((char *)uuid);
//  LocalFree((char *)tmp[1]);
//  LocalFree((char *)tmp[2]);
	dwThread = 0;
	waveOutSetVolume(0, maxvol);
	ExitProcess(0);
}
//работа с элементами формы
void fontialog(HWND hw, int xsize, int ysize, int bold, int angle)
{
	HFONT fnt;
	LOGFONT lgfnt;
//  unsigned long int k, l, m, p, o, n, x, y;
//      GetWindowRect(hw, &rect);
//      k = (rect.left * GetSystemMetrics(SM_CXSCREEN) / form.right);
//      l = (rect.top * GetSystemMetrics(SM_CYSCREEN) / form.bottom);
//      m = (rect.right * GetSystemMetrics(SM_CXSCREEN) / form.right) - k;
//      p = (rect.bottom * GetSystemMetrics(SM_CYSCREEN) / form.bottom) - l;
	fnt = (HFONT)SendMessage(hw, WM_GETFONT, 0, 0);
	lgfnt.lfHeight = ysize;
	lgfnt.lfWidth = xsize;
	lgfnt.lfEscapement = angle;
	//lgfnt.lfPitchAndFamily = angle;
	lgfnt.lfOrientation = angle;
	lgfnt.lfWeight = bold;
	lgfnt.lfCharSet = RUSSIAN_CHARSET;
	lgfnt.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lgfnt.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lgfnt.lfPitchAndFamily = FF_DONTCARE;
	lgfnt.lfQuality = DEFAULT_QUALITY;
	lgfnt.lfItalic = FALSE;
	lgfnt.lfUnderline = FALSE;
	lgfnt.lfStrikeOut = FALSE;
	fnt = (HFONT)CreateFontIndirect(&lgfnt);
	SendMessage(hw, WM_SETFONT, (long int)fnt, TRUE);
}
//автоматическая уставновка одного шрифта всем элементам формы
void autofontialog(HWND hwndDlg, int xsize, int ysize)
{
	HWND ow, hw;
	HFONT fnt;
	LOGFONT lgfnt;
	unsigned long int k, l, m, p, o, n, x, y;
	RECT form;
	GetWindowRect(hwndDlg, &form);
	hw = GetNextDlgTabItem(hwndDlg, 0, FALSE);
	ow = hw;
	while (hw != NULL)
	{
		GetWindowRect(hw, &rect);
		k = (rect.left * GetSystemMetrics(SM_CXSCREEN) / form.right);
		l = (rect.top * GetSystemMetrics(SM_CYSCREEN) / form.bottom);
		m = (rect.right * GetSystemMetrics(SM_CXSCREEN) / form.right) - k;
		p = (rect.bottom * GetSystemMetrics(SM_CYSCREEN) / form.bottom) - l;


		fnt = (HFONT)SendMessage(hw, WM_GETFONT, 0, 0);
		lgfnt.lfHeight = ysize;
		lgfnt.lfWidth = xsize;
		lgfnt.lfEscapement = 0;
		lgfnt.lfOrientation = 0;
		lgfnt.lfWeight = 700;
		lgfnt.lfCharSet = RUSSIAN_CHARSET;
		lgfnt.lfOutPrecision = OUT_DEFAULT_PRECIS;
		lgfnt.lfClipPrecision = CLIP_DEFAULT_PRECIS;
		lgfnt.lfPitchAndFamily = FF_DONTCARE;
		lgfnt.lfQuality = DEFAULT_QUALITY;
		lgfnt.lfItalic = FALSE;
		lgfnt.lfUnderline = FALSE;
		lgfnt.lfStrikeOut = FALSE;
		//lgfnt.lfFaceName = L"";
		//fnt = (HFONT)CreateFont(0,20,NULL,700,FALSE,FALSE,FALSE,RUSSIAN_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,FF_DONTCARE,NULL);
		fnt = (HFONT)CreateFontIndirect(&lgfnt);

		SendMessage(hw, WM_SETFONT, (long int)fnt, TRUE);

		//MoveWindow(hw,k,l,m,p,TRUE);
		hw = GetNextDlgTabItem(hwndDlg, hw, FALSE);
		if (ow == hw)
			return;
	}

}
////авто растяжка сжатие всех элементов формы
//void autoscaledialog(HWND hwndDlg, _Bool fullscr)
//{
	//HWND ow, hw = NULL;
	//unsigned long int k = 0, l = 0, m = 0, p = 0, o = 0, n = 0, x = 0, y = 0;
	//RECT form;	//__try{
	//GetWindowRect(hwndDlg, &form);
	////x = GetSystemMetrics(SM_CXSCREEN);
	////y = GetSystemMetrics(SM_CYSCREEN);
	////SetWindowPos(hwndDlg,hwndDlg,0,0,x,y,SWP_NOZORDER);
	//MoveWindow(hwndDlg, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), TRUE);
	//hw = GetNextDlgTabItem(hwndDlg, 0, FALSE);
	//ow = hw;	//}__finally{}
	//while (hw != NULL)
	//{
		////__try{
		//GetWindowRect(hw, &rect);
		//k = (rect.left * GetSystemMetrics(SM_CXSCREEN) / form.right);
		//l = (rect.top * GetSystemMetrics(SM_CYSCREEN) / form.bottom);
		//m = (rect.right * GetSystemMetrics(SM_CXSCREEN) / form.right) - k;
		//p = (rect.bottom * GetSystemMetrics(SM_CYSCREEN) / form.bottom) - l;
		//__try
		//{
			//MoveWindow(hw, k, l, m, p, TRUE);
		//}
		//__except(EXCEPTION_EXECUTE_HANDLER)
		//{
			//MessageBox(0, 0, 0, 0);
		//}
		//hw = GetNextDlgTabItem(hwndDlg, hw, FALSE);
		//if (ow == hw)
			//return;
	//}

//}
//не используется .. дизаблит все элементы формы
void enabledialog(HWND hwndDlg, _Bool en)
{
	return;
	HWND ow, hw = NULL;
	int c = 0;
	hw = GetNextDlgTabItem(hwndDlg, 0, FALSE);
	ow = hw;
	while (hw != NULL)
	{
		if (10 < c++)
			return;
		EnableWindow(hw, en);
		hw = GetNextDlgTabItem(hwndDlg, hw, FALSE);
		if (ow == hw)
			return;
	}

}
//парсинг ответа базы из отдельного потока
_Bool checkreturn(HWND hwndDlg)
{
	if (xmlvalue(0, "return") < 1)
	{
		if (xmlvalue(0, "datail") > 1)
			MultiByteToWideChar(CODEPAGE, 0, otvet, strlen(otvet), wotvet, BUFSIZE);
		else if (xmlvalue(0, "faultstring") > 1)
			MultiByteToWideChar(CODEPAGE, 0, otvet, strlen(otvet), wotvet, BUFSIZE);
		else
			MultiByteToWideChar(CODEPAGE, 0, szData, strlen(szData), wotvet, BUFSIZE);

		ShowError(hwndDlg, wotvet);
		return TRUE;
	}
	if (dwBytesRead < 1)
	{
		ShowError(hwndDlg, L"Ошибка сети");
		return TRUE;
	}
	return FALSE;
}


//------------кодирование BASE64------------------------------------------------------------------------
/*
** Translation Table as described in RFC1113
*/
static const char cb64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/*
** Translation Table to decode (created by author)
*/
static const char cd64[] = "|$$$}rstuvwxyz{$$$$$$$>?@ABCDEFGHIJKLMNOPQRSTUVW$$$$$$XYZ[\\]^_`abcdefghijklmnopq";

/*
** encodeblock
**
** encode 3 8-bit binary bytes as 4 '6-bit' characters
*/
void encodeblock(unsigned char in[3], unsigned char out[4], int len)
{
	out[0] = cb64[in[0] >> 2];
	out[1] = cb64[((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4)];
	out[2] = (unsigned char)(len > 1 ? cb64[((in[1] & 0x0f) << 2) | ((in[2] & 0xc0) >> 6)] : '=');
	out[3] = (unsigned char)(len > 2 ? cb64[in[2] & 0x3f] : '=');
}

_Bool encode(wchar_t *infile, char *outfile)
{

	_Bool rr = FALSE;
	HANDLE h = CreateFile(infile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (h == INVALID_HANDLE_VALUE)
		return FALSE;
	unsigned char *in, out[4], v;
	in = LocalAlloc(0, 300000);
	out[4] = 0;
	int i, len;
	_Bool theend = FALSE;
	if (!ReadFile(h, in, 300000, &i, NULL))
		return FALSE;
	len = 0;

	while (!theend)
	{

		if (len < (i - 3))
		{
			encodeblock(in + len, out, 3);
			strcat(outfile, out);
			len += 3;
		}
		else
		{
			encodeblock(in + len, out, i - len);
			strcat(outfile, out);
			len += i - len;
			theend = TRUE;
		}
		if (i <= len)
			theend = TRUE;

	}
	LocalFree(in);
	return TRUE;
}



void decodeblock(unsigned char in[4], unsigned char out[3])
{
	out[0] = (unsigned char)(in[0] << 2 | in[1] >> 4);
	out[1] = (unsigned char)(in[1] << 4 | in[2] >> 2);
	out[2] = (unsigned char)(((in[2] << 6) & 0xc0) | in[3]);
}

_Bool decodebase64(char *infile, wchar_t *outfile)
{
	_Bool rr = FALSE;
	HANDLE h = CreateFile(outfile, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (h == INVALID_HANDLE_VALUE)
		return FALSE;
	unsigned char in[4], out[3], v;
	int i, len;
	_Bool theend = FALSE;

	while (!theend)
	{

		for (len = 0, i = 0; i < 4 && !theend; i++)
		{
			v = 0;
			while (!theend && v == 0)
			{
				v = (unsigned char)*(infile++);
				theend = (v == 0);
				v = (unsigned char)((v < 43 || v > 122) ? 0 : cd64[v - 43]);
				if (v)
					v = (unsigned char)((v == '$') ? 0 : v - 61);

			}

			if (!theend)
			{
				len++;
				if (v)
				{
					in[i] = (unsigned char)(v - 1);
				}
			}
			else
			{
				in[i] = 0;
			}
		}
		if (len)
		{
			decodeblock(in, out);
			if (!WriteFile(h, out, 3, &i, NULL))
				return FALSE;
		}
	}

	CloseHandle(h);

	return TRUE;

}
//проверка обновлений на сервере
static _Bool checkUpdate(void)
{
	wchar_t selfname[300];
	_Bool rr;
	MyMessageBox(0, L"Update ", 0, 100);

	wcscpy(UserWS, L"WebConnection");
	wcscpy(PswWS, L"951");
	wcscpy(selfname, selfdir);
	wcscat(selfname, L"tsdupdate.exe");
	DeleteFile(selfname);
	sndupdate(VERSION);	//посылаем пакет в 1с с текущим номером верии 
	if (xmlvalue(0, "return") < 1)
	{
		MyMessageBox(0, L"NO UPDATE", 0, 100);
		return FALSE;
	}

	rr = decodebase64(otvet, selfname);
	if (!rr)
	{
		MyMessageBox(0, L"download error", 0, 100);
		return rr;
	}

	STARTUPINFO si = { sizeof(STARTUPINFO) };
	si.dwFlags = 0;
	si.dwFlags = 0x21;
	si.wShowWindow = SW_SHOW;
	SHELLEXECUTEINFO se = { sizeof(SHELLEXECUTEINFO) };
	se.lpFile = selfname;
	se.nShow = SW_SHOW;
	se.fMask = SEE_MASK_FLAG_NO_UI;
	PROCESS_INFORMATION pi;
//  rr = rr&&(CreateProcess(selfname, NULL, NULL, NULL, FALSE, 0x00000100, NULL, selfdir, &si, &pi)>0);
//  rr = rr&&(CreateProcess(selfname, NULL, NULL, NULL, FALSE, 0x00000100, NULL, NULL, NULL, &pi)>0);
	rr = rr && (ShellExecuteEx(&se));
	if (rr)
		MyMessageBox(0, L">>>>START<<<", 0, 100);
	else

		MyMessageBox(0, L"Start Error", 0, 100);
	return rr;
}
//появление мерцающего текста поверх всех окон
//DWORD WINAPI getsms()
//{
	//int i = 0;

	//while (dwThread > 0)
	//{
		//i++;
		//smshint(L"sms", i * 3276);
		//Sleep(1000 * 5);
	//}
	//dwThread = 0;
	//ExitThread(0);
	//return 0;
//}

//посыл сформированного запроса на сервер
void sendGoodslist(wchar_t *Operation, wchar_t *code, wchar_t *name, wchar_t *kolvo, _Bool sendmetric)
{
	wchar_t txt[200], info[1000];
//metka:
	//wchar_t temp[5000];
	NowSending = TRUE;

	memset(wzData, 0, BUFSIZE);
	wcscpy(wzData, L"<soap:Envelope xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\"><soap:Header/><soap:Body><m:ОбменТСД xmlns:m=\"http://www.dns-shop.tsd.ru\"><m:ВидОперации xmlns:xs=\"http://www.w3.org/2001/XMLSchema\"  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">");
	wcscat(wzData, Operation);
	wcscat(wzData, L"</m:ВидОперации><m:Список xmlns:xs=\"http://www.w3.org/2001/XMLSchema\"	xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">");
	wcscat(wzData, L"<m:Номенклатура><m:Код>");
	wcscat(wzData, code);
	wcscat(wzData, L"</m:Код><m:Наименование>");
	wcscat(wzData, name);
	wcscat(wzData, L"</m:Наименование><m:Количество>");
	wcscat(wzData, kolvo);
	wcscat(wzData, L"</m:Количество></m:Номенклатура>");
	if (sendmetric)
	{
		getbat(info);
		wcscat(wzData, L"<m:Номенклатура><m:Код>");
		wcscat(wzData, ipadr);
		wcscat(wzData, L"</m:Код><m:Наименование>");
		wcscat(wzData, info);
		wcscat(wzData, L"</m:Наименование><m:Количество>");
		wcscat(wzData, VERSION);
		wcscat(wzData, L"</m:Количество></m:Номенклатура>");
	}
	wcscat(wzData, L"</m:Список></m:ОбменТСД></soap:Body></soap:Envelope>");
	long tmp = GetTickCount();
	sendws(wzData);
	packetsize = strlen(szData);
	timeforpacket = GetTickCount() - tmp;
	memset(wzData, 0, BUFSIZE);
	MultiByteToWideChar(CODEPAGE, 0, szData, packetsize, wzData, BUFSIZE);
	if (xmlgetw(0, L"return") < 2)
	{
		if (xmlgetw(0, L"detail") < 2)
		{
			strcpy(szData, " ----- <detail> Произошла ошибка сети, Network error </detail> </end>");
			wcscpy(wzData, L" ----- <detail> Произошла ошибка сети, Network error </detail> <return>");
			wcscat(wzData, L"<m:Список xmlns:xs=\"http://www.w3.org/2001/XMLSchema\"	xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">");
			wcscat(wzData, L"<m:Номенклатура><m:Код>");
			wcscat(wzData, L"Ошибка");
			wcscat(wzData, L"</m:Код><m:Наименование>");
			wcscat(wzData, L"Произошла ошибка сети");
			wcscat(wzData, L"</m:Наименование><m:Количество>");
			wcscat(wzData, L"0");
			wcscat(wzData, L"</m:Количество></m:Номенклатура>");
		}
		return;
	}

	if (xmlgetw(3, L"<m:Код") <= 0)
		return;
	if (wcscmp(wotvet, L"ЗакрытьСессию") == 0)
	{
		xmlgetw(3, L"<m:Наименов");
		ShowError(NULL, wotvet);
		clearExit();
	}
	NowSending = FALSE;

	if (wcscmp(wotvet, L"ВывестиСообщение") == 0)
	{
		//KillTimer(
		xmlgetw(3, L"<m:Наименов");
		ShowError(NULL, wotvet);
	}

	if (wcscmp(wotvet, L"НетСообщений") == 0)
		dwThread == 0;

	if (wcscmp(wotvet, L"ЕстьСообщения") == 0)
	{
		//if (dwThread == 0)	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)getsms, NULL, 0, &dwThread);
	}
	if (wcscmp(wotvet, L"ЗадатьВопрос") == 0)	//посылаем интерактивный вопрос пользователю с дальнейшими действиями
	{
		if (xmlgetw(3, L"<m:Количеств") <= 0)
			return;
		int i = _wtoi(wotvet);
		xmlgetw(3, L"<m:Наименов");
		wsprintf(txt, L"%u", MessageBox(0, wotvet, L"Вопрос", MB_ICONINFORMATION | MB_TOPMOST | MB_YESNO));
		sendGoodslist(L"ОтветНаВопрос", txt, txt, txt, TRUE);	// и ответ пользователя снова посылаем в базу этой же функцикей
		return;
	}

	if (wcscmp(wotvet, L"ОбновитьВерсиюПрошивки") == 0)
	{
		xmlgetw(3, L"<m:Наименов");
		debugmode = TRUE;
		ShowError(NULL, wotvet);
		if (checkUpdate())
			clearExit();
	}

	if (wcscmp(wotvet, L"ВыключитьТСД") == 0)
	{
		//  ExitWindowsEx(8,0);
		xmlgetw(3, L"<m:Наименов");
		ShowError(NULL, wotvet);
		clearExit();
	}

	if (wcscmp(wotvet, L"Скриншот") == 0)
	{
		xmlgetw(3, L"<m:Наименов");
		//screenshot(wotvet);
//  wcscpy(thisfile, selfdir);
//  wcscat(thisfile, L"testdialog.bmp");
//  screenshot(thisfile);

	}


	memset(wotvet, 0, 255);
}
//прием/передача пакета
void sndxdto(wchar_t *text, wchar_t *command)
{

	sendGoodslist(L"ТСД", command, text, L"0", TRUE);
	xmlgetw(3, L"<m:Наименов");
	return;

	//wchar_t xmlstr[] = L"<soap:Envelope xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\"><soap:Header/><soap:Body><m:TSD xmlns:m=\"http://www.dns-shop.tsd.ru\"><m:ШК xmlns:xs=\"http://www.w3.org/2001/XMLSchema\"\n		xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">%s</m:ШК>\n	<m:ВидОперации xmlns:xs=\"http://www.w3.org/2001/XMLSchema\"\n		xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">%s</m:ВидОперации>\n	</m:TSD>\n	</soap:Body>\n</soap:Envelope>";
	//wchar_t xmlstr1[TempSize];
	//wsprintf(xmlstr1, xmlstr, text, command);
	//sendws(xmlstr1);
}
//очистка очереди сообщений windows
void freeMessages(LPMSG pMsg, HWND hwndDlg)	//Высвобождает очередь сообщений
{
	MSG Msg;
	PeekMessage(&Msg, hwndDlg, WM_KEYFIRST, WM_KEYLAST, PM_REMOVE);
	PeekMessage(&Msg, hwndDlg, WM_KEYFIRST, WM_KEYLAST, PM_REMOVE);
	PeekMessage(&Msg, hwndDlg, WM_KEYFIRST, WM_KEYLAST, PM_REMOVE);
	PeekMessage(&Msg, hwndDlg, WM_KEYFIRST, WM_KEYLAST, PM_REMOVE);
	PeekMessage(&Msg, hwndDlg, WM_KEYFIRST, WM_KEYLAST, PM_REMOVE);
	PeekMessage(&Msg, hwndDlg, WM_KEYFIRST, WM_KEYLAST, PM_REMOVE);
	PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE);
	PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE);
	PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE);
	PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE);
	PeekMessage(&Msg, NULL, WM_MOUSEFIRST, WM_MOUSELAST, PM_REMOVE);
	PeekMessage(&Msg, NULL, WM_MOUSEFIRST, WM_MOUSELAST, PM_REMOVE);
	PeekMessage(&Msg, NULL, WM_MOUSEFIRST, WM_MOUSELAST, PM_REMOVE);


}

//Точка входа------------------------------------------------------------------------------------
int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpszCmdLine, int nCmdShow)
{	// struct NLED_COUNT_INFO Leds;
	//MessageBox(0,L"1",L"1",0);
	MyConnect = NULL;
	ReconnectAllways = TRUE;
	wchar_t thisfile[300], updfile[300];
	cachemode = FALSE;
	NowSending = FALSE;
	MyMessageWait(VERSION);	// отображаем на экране номер версии 
	Sleep(2000);
	//MessageBox(0,L"2",L"2",0);
	debugmode = TRUE;
	inthread = FALSE;
	szData = LocalAlloc(0, BUFSIZE);	//получаем память буферов обмена с базой utf8 
	uuid = (struct guids *)LocalAlloc(0, GUIDSIZE * sizeof(struct guids));	//память для хранения гуидов 
//  tmp[1] = (struct guids *)LocalAlloc(0, ScanBuf * sizeof(struct guids)); //память для хранения barcode 
//  tmp[2] = (struct guids *)LocalAlloc(0, ScanBuf * sizeof(struct guids)); //память для хранения barcode 
//  tmp[0] = (struct guids *)LocalAlloc(0, ScanBuf * sizeof(struct guids)); //память для хранения barcode 
//  tmp[3] = (struct guids *)LocalAlloc(0, ScanBuf * sizeof(struct guids)); //память для хранения barcode 
	GUIDSCOUNT = GUIDSIZE;
	memset((char *)uuid, 0, GUIDSIZE * sizeof(struct guids));
	tab1l = 0;
	tab2l = 0;
	nowtab1 = TRUE;
	otvet = LocalAlloc(0, BUFSIZE);	//получаем память буферов обмена с базой utf8
	wzData = (wchar_t *)LocalAlloc(0, BUFSIZE * sizeof(wchar_t));	// 1251 юникод
	wotvet = (wchar_t *)LocalAlloc(0, BUFSIZE * sizeof(wchar_t));	// 1251
	WSADATA wsadata;	// начало работы с сокетами
	WSAStartup(MAKEWORD(2, 2), &wsadata);	// начало работы с сокетами
	localip();	//получаем ИП
	WNDCLASS wc;
	GetModuleFileName(NULL, selfdir, 255);	//получаем имя своего файла для апдейтов
	int i = wcslen(selfdir);
	for (; i > 1; i--)
		if (selfdir[i] == '\\')
			break;
	selfdir[i + 1] = 0;
	wcscpy(CONFILE, selfdir);
	wcscat(CONFILE, CFGFILE);

	//wcscpy(thisfile, selfdir);
	//wcscat(thisfile, L"testdialog.bmp");
	//screenshot(thisfile);

	loadconfigxml();
	cachemode = FALSE;
	if (wcscmp(selfdir + i + 2 + 2, L"update.exe") != 0)
	{
		if (checkUpdate())	//проверим обновления
		{
			clearExit();	//и выйдем из проги если есть обнова
			return 0;
		}
	}
	else
	{
		wcscpy(thisfile, selfdir);
		wcscat(thisfile, L"testdialog.exe");
		i = 15;
		while (!DeleteFile(thisfile) && i > 0)
		{
			Sleep(1000);
			i--;
		}
		GetModuleFileName(NULL, updfile, 255);
		MoveFile(updfile, thisfile);
	}
	loadconfigxml();

	hwnd = FindWindow(L"testdialClass", NULL);
	if (hwnd)
	{
		SetForegroundWindow((HWND) ((ULONG)hwnd | 0x01));
		return 0;
	}

	HANDLE hLib = LoadLibrary(L"DeviceAPI.Dll");
	UseMotorolla = (hLib != NULL);
	if (UseMotorolla)
	{
		//StartShake = GetProcAddress(hLib, (L"StartShake"));
		Barcode1D_init = GetProcAddress(hLib, (L"Barcode1D_init"));
		//private static extern void Barcode1D_init();
		Barcode1D_scan = GetProcAddress(hLib, (L"Barcode1D_scan"));
		//private static extern int Barcode1D_scan(byte[] pszData);
		Barcode1D_free = GetProcAddress(hLib, (L"Barcode1D_free"));
		//private static extern void Barcode1D_free();
		//StartShake(20000);
		Barcode1D_init();
		Barcode1D_scan(szData);
	}
	initmute(NULL, (DWORD)hInstance);
	SHInitExtraControls();
	//MessageBox(0,selfdir+i+2+2,selfdir,0);
	dwThread = 0;
	RepaintMainMenu = FALSE;
	g_hInstance = hInstance;
	if (!GetClassInfo(NULL, L"Dialog", &wc))
		return 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDR_ICO_MAIN));
	wc.lpszClassName = L"testdialClass";
	if (!RegisterClass(&wc))
		clearExit();
	int z = 0;
	debugmode = FALSE;
	OpenClipboard(hwnd);
	EmptyClipboard();
	CloseClipboard();	/*
						   HINSTANCE hWZClib = LoadLibrary(L"wzcsapi.dll");
						   pfnWZCQueryInterface =  (PFN_WZCQueryInterface)GetProcAddress(hWZClib,L"WZCQueryInterface");
						   pfnWZCSetInterface = (PFN_WZCSetInterface)GetProcAddress(hWZClib,L"WZCSetInterface");
						   pfnWZCRefreshInterface = (PFN_WZCRefreshInterface)GetProcAddress(hWZClib,L"WZCRefreshInterface");
						 */
	tomainmenu = FALSE;
	while (z != 130)	//проверяю что диалог выбора пользователя вернул значение 130 (придумал из головы) тогда продолжаю следующее окно 
	{
		z = DialogBox(g_hInstance, MAKEINTRESOURCE(Login), NULL, (DLGPROC)Autorization);
		SetForegroundWindow(hwnd);
		if (z == 333)	// в случае ошибки сервера возвращается число 333
		{
			clearExit();
			return 0;
		}
	}
	tomainmenu = TRUE;
	NowSending = FALSE;
	DialogBox(g_hInstance, MAKEINTRESOURCE(mainform), NULL, (DLGPROC)MainDlgProc);
	sndxdto(L"Выход", L"Выход");
	clearExit();
	return 0;
}
//получаем от сервера указание что делать дальше
int selectNext(HWND hwndDlg, wchar_t *txt)
{
	wsprintf(wotvet, L"%u", Goodslistmode);	//Строка(Goodslistmode) это номер "страницы"  текущего окна на тсд

	sendGoodslist(L"СледующаяСтраница", txt, zagolovokokna, wotvet, TRUE);
	MultiByteToWideChar(CODEPAGE, 0, szData, dwBytesRead, wzData, dwBytesRead);
	int i = xmlgetw(0, L"return");
	if (i < 1)
	{
		xmlgetw(0, L"detail");
		ShowError(hwndDlg, wotvet);
		MyMessageBox(0, wotvet, 0, 100);
		return -1;
	}
	if (xmlgetw(i, L"<m:Номенклатур") < 1)
		return -1;
	if (xmlgetw(i, L"<m:Код") < 1)
		return -1;
	if (wcscmp(wotvet, L"Ошибко") == 0 || wcscmp(wotvet, L"Error") == 0)
		return -1;
	if (xmlgetw(i, L"<m:Количество") < 1)
		return -1;
	return _wtoi(wotvet);
}

//заполнение таблицы формы по комманде от сервера
void Filltabl(HWND hwndDlg, unsigned long long *n)
{
	wchar_t code[100], tovar[200], kolvo[100];
	int d, c, z, o, k;
	HWND hwndList = GetDlgItem(hwndDlg, list);

	LV_ITEM lvi;
	int i = *n;
	z = 3;
	c = 0;
	GUIDSCOUNT = 0;
	while (z > 1)
	{
		lvi.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE | LVIF_STATE;

		z = xmlgetw(i, L"<m:Номенклатур");
		i = z + 1;
		if (z < 1)
			break;
		if (xmlgetw(i, L"<m:Код") < 1)
			break;
		if (wcscmp(wotvet, L"Ошибка") == 0 || wcscmp(wotvet, L"Error") == 0)
		{
			xmlgetw(i, L"<m:Наименов");
			ShowError(hwndDlg, wotvet);
			MyMessageBox(0, wotvet, 0, 100);
			continue;
		}
		wcscpy(code, wotvet);
		if (xmlgetw(i, L"<m:Наименов") < 1)
			break;
		wcscpy(tovar, wotvet);

		if (xmlgetw(i, L"<m:Количество") < 1)
			break;
		wcscpy(kolvo, wotvet);

		//wcscpy(uuid[c].guid, code);
		if (wcscmp(code, L"КонецТаблицы") == 0)
			break;

		if (wcscmp(code, L"ОчиститьТаблицу") == 0)
		{
			GUIDSCOUNT = 0;

			ListView_DeleteAllItems(hwndList);
			//SendMessage(GetDlgItem(hwndDlg, list), LVM_DELETEALLITEMS, 0, 0); 
			continue;
		}
		if (wcscmp(code, L"Артикул") == 0)
		{
			wcscpy(uuid[GUIDSCOUNT].Article, tovar);
			continue;
		}
		if (wcscmp(code, L"Количество") == 0)
		{
			uuid[GUIDSCOUNT].kolvo = _wtoi(kolvo);
			lvi.pszText = kolvo;
			lvi.iSubItem = 3;
			lvi.cchTextMax = wcslen(kolvo);
			SendMessage(hwndList, LVM_SETITEMTEXT, 0, (LPARAM) & lvi);


			continue;
		}

		if (wcscmp(code, L"КоличествоКэш") == 0)
		{
			uuid[GUIDSCOUNT].kolvo = _wtoi(kolvo);
			continue;
		}




		if (wcscmp(code, L"Колонка") == 0)
		{
			k = _wtoi(kolvo);
			lvi.pszText = tovar;
			lvi.iSubItem = k;
			lvi.cchTextMax = wcslen(tovar);
			SendMessage(hwndList, LVM_SETITEMTEXT, 0, (LPARAM) & lvi);
			continue;
		}
		if (wcscmp(code, L"GUID") == 0)
		{
			wcscpy(uuid[GUIDSCOUNT].guid, tovar);
			continue;
		}


		wcscpy(uuid[++GUIDSCOUNT].guid, tovar);
		uuid[GUIDSCOUNT].Param = _wtoi(kolvo);
		wcscpy(uuid[GUIDSCOUNT].Article, code);
		wcscpy(uuid[GUIDSCOUNT].Adress, code);

		lvi.pszText = code;
		lvi.cchTextMax = wcslen(code);
		lvi.iItem = 0;
		lvi.iImage = 1;
		lvi.iSubItem = 0;	//0 обязательно
		lvi.lParam = _wtoi(kolvo);
		SendMessage(hwndList, LVM_INSERTITEM, 0, (LPARAM) & lvi);

		lvi.iSubItem = 1;
		SendMessage(hwndList, LVM_SETITEMTEXT, 0, (LPARAM) & lvi);

		lvi.pszText = tovar;
		lvi.iSubItem = 2;
		lvi.cchTextMax = wcslen(tovar);

		SendMessage(hwndList, LVM_SETITEMTEXT, 0, (LPARAM) & lvi);
		ListView_SetCheckState(hwndList, 0, FALSE);
		//lvi.state = 1<<12;
		//SendMessage(hwndList, LVM_SETITEMSTATE, 0, (LPARAM)&lvi);

	}
	GUIDSCOUNT++;
	*n = i;

}


_Bool getformparse()
{
	unsigned long long subItem, e, i, z, f, b, d, r, t, u, v;
	RECT rc;
	LV_COLUMN lvc;
	LV_ITEM lvi;
	LVFINDINFO lvf;
	wchar_t code[200], tovar[2000], kolvo[200];
	HWND hwndDlg = tekWnd;
	GetWindowRect(hwndDlg, &rc);
	i = 0;
	z = 3;
	//wcscpy(oldsession,L"");
	int c = 0;
	subItem = 0;
	while (z > 1 && c < 50)
	{
		c++;
		z = xmlgetw(i, L"<m:Номенклатур");
		i = z;
		if (i < 0)
			break;
		if (xmlgetw(i, L"<m:Количество") < 1)
			break;
		wcscpy(kolvo, wotvet);
		if (xmlgetw(i, L"<m:Код") < 1)
			break;
		wcscpy(code, wotvet);
		if (xmlgetw(i, L"<m:Наименов") <= 0)
			break;
		if (wcslen(wotvet) < 2000)
			wcscpy(tovar, wotvet);
		f = _wtoll(kolvo);
		r = (f >> 9) & 511;	//распаковка координат x,y 
		t = f & 511;
		u = (f >> 18) & 127;	//и размеров элемента w,h
		v = (f >> 25);
		if (wcscmp(code, L"СледующийУспех") == 0)
			continue;
		if (wcscmp(code, L"ТекстИСвойства") == 0)
		{

			d = f & 65535;
			b = (f >> 16) & 1;
			e = (f >> 17) & 1;
			u = (f >> 18) & 1;	//Select All text
			//SetWindowLong(
			SetWindowText(GetDlgItem(hwndDlg, d), wotvet);
			EnableWindow(GetDlgItem(hwndDlg, d), !b);
			ShowWindow(GetDlgItem(hwndDlg, d), (e) ? SW_HIDE : SW_SHOW);
			if (!u)
			{
				SendMessage(GetDlgItem(hwndDlg, d), EM_SETSEL, 0, -1);
			}
			continue;
		}

		if (wcscmp(code, L"BM_CLICK") == 0)
		{

			d = f & 65535;
			//wcscpy(oldsession,tovar);
			SendMessage(GetDlgItem(hwndDlg, d), BM_CLICK, 0, 0);
			continue;
		}

		if (wcscmp(code, L"Выход") == 0)
		{
			KillTimer(hwndDlg, WM_USER + 1);
			KillTimer(hwndDlg, WM_USER + 2);
			EndDialog(hwndDlg, f);
			return FALSE;
		}
		if (wcscmp(code, L"ВыходСОшибкой") == 0)
		{
			KillTimer(hwndDlg, WM_USER + 1);
			KillTimer(hwndDlg, WM_USER + 2);
			ShowError(hwndDlg, wotvet);
			EndDialog(hwndDlg, f);
			return FALSE;
		}



		if ((wcscmp(code, L"Выполнить комманду") == 0) || (wcscmp(code, L"Do command") == 0))
		{

			DoCommands(hwndDlg, tovar);

			return TRUE;
		}
		//SendMessage(GetDlgItem(hwndDlg, combo), EM_SETSEL,0,-1);
		if (wcscmp(code, L"СообщениеОкну") == 0)
		{

			d = f & 65535;
			r = (f >> 16);
			t = _wtoll(tovar);

			if (t == 0)
			{
			}
			else
			{
				SendMessage(GetDlgItem(hwndDlg, d), r, 0, -1);
			}
			continue;
		}


		if (wcscmp(code, L"УдалитьКолонкуСписка") == 0)
		{
			ListView_DeleteColumn(GetDlgItem(hwndDlg, list), f);
			continue;
		}
		if (wcscmp(code, L"ОчиститьТаблицу") == 0)
		{
			GUIDSCOUNT = 0;

			ListView_DeleteAllItems(GetDlgItem(hwndDlg, list));
			//SendMessage(GetDlgItem(hwndDlg, list), LVM_DELETEALLITEMS, 0, 0); 
			continue;
		}

		if (wcsncmp(code, L"ДобавитьКолонкуСписка", wcslen(L"ДобавитьКолонкуСписка")) == 0)
		{


			lvc.mask = (LVCF_TEXT + LVCF_WIDTH + LVCF_FMT + LVCF_SUBITEM + LVCF_ORDER);
			lvc.fmt = LVCFMT_LEFT;
			lvc.iSubItem = ++subItem;
			lvc.cx = f;
			lvc.iOrder = _wtoi(code + wcslen(L"ДобавитьКолонкуСписка"));
			lvc.pszText = tovar;
			lvc.cchTextMax = wcslen(tovar);
			SendMessage(GetDlgItem(hwndDlg, list), LVM_INSERTCOLUMN, 0, (LPARAM) & lvc);
			ListView_SetBkColor(GetDlgItem(hwndDlg, list), RGB(250, 250, 250));
			continue;
		}
		if (wcscmp(code, L"ПоменятьШрифт") == 0)
		{

			fontialog(GetDlgItem(hwndDlg, _wtoll(tovar)), u, v, r * 10, t);
			continue;
		}
		if (wcscmp(code, L"ПоменятьВидимость") == 0)
		{

			ShowWindow(GetDlgItem(hwndDlg, _wtoll(tovar)), (f) ? SW_HIDE : SW_SHOW);
			continue;
		}

		if (wcscmp(code, L"ЗаполнитьТаблицу") == 0)
		{
			//ListView_SetExtendedListViewStyle(GetDlgItem(hwndDlg, list), LVS_EX_ONECLICKACTIVATE | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP | LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES);
			ListView_SetExtendedListViewStyle(GetDlgItem(hwndDlg, list), f);	//0x00000040 + 0x00000001 +0x00000010 +0x00000020 LVS_EX_ONECLICKACTIVATE | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP | LVS_EX_FULLROWSELECT);
			d = 0;
			d = _wtoi(tovar);
			ListView_SetBkColor(GetDlgItem(hwndDlg, list), RGB(250, 250, 250) - d);
			Filltabl(hwndDlg, &i);
			continue;
		}

		if (wcscmp(code, L"СменитьGUID") == 0)
		{
			wcscpy(ipadr,tovar);
			continue;
		}

		if (wcscmp(code, L"КонецТаблицы") == 0)
			continue;
		d = (_wtoll(code)) & 65535;
		b = (_wtoll(code) >> 16) & 1;
		e = (_wtoll(code) >> 17) & 1;



		if (wcscmp(code, L"Расположение") == 0)
		{
			d = (_wtoll(tovar)) & 65535;
			b = (_wtoll(tovar) >> 16) & 1;
			e = (_wtoll(tovar) >> 17) & 1;
			GetWindowRect(GetDlgItem(hwndDlg, d), &rc);
			if (u == 0)
				u = rc.right - rc.left;
			if (v == 0)
				v = rc.bottom - rc.top;
			//SetWindowText(GetDlgItem(hwndDlg, d), tovar);
			SetWindowPos(GetDlgItem(hwndDlg, d), GetDlgItem(hwndDlg, d), r, t, u, v, SWP_NOZORDER);
			EnableWindow(GetDlgItem(hwndDlg, d), !b);
			ShowWindow(GetDlgItem(hwndDlg, d), (e) ? SW_HIDE : SW_SHOW);
			continue;
		}



		if (d == 0 && (wcslen(tovar) == 0))
			continue;
		if (d == 0)
		{
			SetWindowLong(hwndDlg, GWL_STYLE, GetWindowLong(hwndDlg, GWL_STYLE) | WS_CAPTION);
			SetWindowPos(hwndDlg, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_DRAWFRAME);

			SetWindowText(hwndDlg, tovar);

			if ((u + v) > 0)
				autofontialog(hwndDlg, u, v);
			continue;
		}
		//b = (f>>32)&1;
		//GetWindowRect(GetDlgItem(hwndDlg, d), &rc);
		if (u == 0)	//           u = GetSystemMetrics(SM_CXSCREEN);
			u = rc.right - rc.left;
		if (v == 0)
			v = rc.bottom - rc.top;

		SetWindowText(GetDlgItem(hwndDlg, d), tovar);
		SetWindowPos(GetDlgItem(hwndDlg, d), GetDlgItem(hwndDlg, d), r, t, u, v, SWP_NOZORDER);
		EnableWindow(GetDlgItem(hwndDlg, d), !b);
		ShowWindow(GetDlgItem(hwndDlg, d), (e) ? SW_HIDE : SW_SHOW);
	}
	return TRUE;
}
//расположение элементов формы по комманде от сервера
static void getform1c()
{
	unsigned long long subItem, e, i, z, f, b, d, r, t, u, v;
	RECT rc;
	LV_COLUMN lvc;
	LV_ITEM lvi;
	LVFINDINFO lvf;
	wchar_t code[200], tovar[200], kolvo[200];
	HWND hwndDlg = tekWnd;
	GetWindowRect(hwndDlg, &rc);
	memset(wotvet, 0, BUFSIZE);
	wsprintf(code, L"%u", rc.bottom);
	wsprintf(kolvo, L"%u", rc.right);
	sendGoodslist(L"РазрешеныКнопки", code, zagolovokokna, kolvo, TRUE);
	getformparse();
	return;
	//i = 0;
	//z = 3;
	//int c = 0;
	//subItem = 0;
	//while (z > 1 && c < 50)
	//{
	//c++;
	//z = xmlgetw(i, L"<m:Номенклатур");
	//i = z - 1;
	//if (z < 1)
	//continue;
	//if (xmlgetw(i, L"<m:Код") <= 0)
	//continue;
	//wcscpy(code, wotvet);
	//xmlgetw(i, L"<m:Наименов");
	//wcscpy(tovar, wotvet);
	//xmlgetw(i, L"<m:Количе");
	//wcscpy(kolvo, wotvet);

	//f = _wtoll(kolvo);
	////if(wcscmp(code,L"ПоменятьШиринуВысоту")==0){
	////SetWindowPos(GetDlgItem(hwndDlg, d), GetDlgItem(hwndDlg, d), r, t, u, v, SWP_NOZORDER);
	////continue;
	////}

	//r = (f >> 9) & 511;   //распаковка координат x,y 
	//t = f & 511;
	//u = (f >> 18) & 127;  //и размеров элемента w,h
	//v = (f >> 25);
	//if (wcscmp(code, L"ТекстИСвойства") == 0)
	//{

	//d = f & 65535;

	//b = (f >> 16) & 1;
	//e = (f >> 17) & 1;
	////SetWindowLong(
	//SetWindowText(GetDlgItem(hwndDlg, d), tovar);
	//EnableWindow(GetDlgItem(hwndDlg, d), !b);
	//ShowWindow(GetDlgItem(hwndDlg, d), (e) ? SW_HIDE : SW_SHOW);
	//continue;
	//}



	//if (wcscmp(code, L"УдалитьКолонкуСписка") == 0)
	//{
	//ListView_DeleteColumn(GetDlgItem(hwndDlg, list), f);
	//continue;
	//}
	//if (wcscmp(code, L"ОчиститьТаблицу") == 0)
	//{
	//GUIDSCOUNT = 0;

	//ListView_DeleteAllItems(GetDlgItem(hwndDlg, list));
	////SendMessage(GetDlgItem(hwndDlg, list), LVM_DELETEALLITEMS, 0, 0); 
	//continue;
	//}

	//if (wcsncmp(code, L"ДобавитьКолонкуСписка", wcslen(L"ДобавитьКолонкуСписка")) == 0)
	//{


	//lvc.mask = (LVCF_TEXT + LVCF_WIDTH + LVCF_FMT + LVCF_SUBITEM + LVCF_ORDER);
	//lvc.fmt = LVCFMT_LEFT;
	//lvc.iSubItem = ++subItem;
	//lvc.cx = f;
	//lvc.iOrder = _wtoi(code + wcslen(L"ДобавитьКолонкуСписка"));
	//lvc.pszText = tovar;
	//lvc.cchTextMax = wcslen(tovar);
	//SendMessage(GetDlgItem(hwndDlg, list), LVM_INSERTCOLUMN, 0, (LPARAM) & lvc);
	//ListView_SetBkColor(GetDlgItem(hwndDlg, list), RGB(250, 250, 250));
	//continue;
	//}
	//if (wcscmp(code, L"ПоменятьШрифт") == 0)
	//{

	//fontialog(GetDlgItem(hwndDlg, _wtoll(tovar)), u, v, r * 10, t);
	//continue;
	//}
	//if (wcscmp(code, L"ПоменятьВидимость") == 0)
	//{

	//ShowWindow(GetDlgItem(hwndDlg, _wtoll(tovar)), (f) ? SW_HIDE : SW_SHOW);
	//continue;
	//}

	//if (wcscmp(code, L"ЗаполнитьТаблицу") == 0)
	//{

	//ListView_SetExtendedListViewStyle(GetDlgItem(hwndDlg, list), f);  //0x00000040 + 0x00000001 +0x00000010 +0x00000020 LVS_EX_ONECLICKACTIVATE | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP | LVS_EX_FULLROWSELECT);
	//d =0;
	//d = _wtoi(tovar);
	//ListView_SetBkColor(GetDlgItem(hwndDlg, list),RGB(250, 250, 250)-d);
	//Filltabl(hwndDlg, &i);
	//continue;
	//}
	//if (wcscmp(code, L"КонецТаблицы") == 0)
	//continue;
	//d = (_wtoll(code)) & 65535;
	//b = (_wtoll(code) >> 16) & 1;
	//e = (_wtoll(code) >> 17) & 1;



	//if (wcscmp(code, L"Расположение") == 0)
	//{
	//d = (_wtoll(tovar)) & 65535;
	//b = (_wtoll(tovar) >> 16) & 1;
	//e = (_wtoll(tovar) >> 17) & 1;
	//GetWindowRect(GetDlgItem(hwndDlg, d), &rc);
	//if (u == 0)
	//u = rc.right - rc.left;
	//if (v == 0)
	//v = rc.bottom - rc.top;
	////SetWindowText(GetDlgItem(hwndDlg, d), tovar);
	//SetWindowPos(GetDlgItem(hwndDlg, d), GetDlgItem(hwndDlg, d), r, t, u, v, SWP_NOZORDER);
	//EnableWindow(GetDlgItem(hwndDlg, d), !b);
	//ShowWindow(GetDlgItem(hwndDlg, d), (e) ? SW_HIDE : SW_SHOW);
	//continue;
	//}



	//if (d == 0 && (wcslen(tovar) == 0))
	//continue;
	//if (d == 0)
	//{
	//SetWindowLong(hwndDlg, GWL_STYLE, GetWindowLong(hwndDlg, GWL_STYLE) | WS_CAPTION);
	//SetWindowPos(hwndDlg, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_DRAWFRAME);

	//SetWindowText(hwndDlg, tovar);

	//if ((u + v) > 0)
	//autofontialog(hwndDlg, u, v);
	//continue;
	//}
	////b = (f>>32)&1;
	//GetWindowRect(GetDlgItem(hwndDlg, d), &rc);
	//if (u == 0)
	//u = rc.right - rc.left;
	//if (v == 0)
	//v = rc.bottom - rc.top;
	//SetWindowText(GetDlgItem(hwndDlg, d), tovar);
	//SetWindowPos(GetDlgItem(hwndDlg, d), GetDlgItem(hwndDlg, d), r, t, u, v, SWP_NOZORDER);
	//EnableWindow(GetDlgItem(hwndDlg, d), !b);
	//ShowWindow(GetDlgItem(hwndDlg, d), (e) ? SW_HIDE : SW_SHOW);
////      ShowWindow(GetDlgItem(hwndDlg, d), (e)?SW_SHOW);
	//}

////ExitThread(0);
////return 0;
}

//Посыл снимка экрана на сервер
//_Bool sendScreenShot()
//{
	//char *name;
	//wchar_t tovar[500];
	//name = LocalAlloc(0, 500000);
	//wcscpy(tovar, selfdir);
	//wcscat(tovar, L"testdialog.bmp");
	//screenshot(tovar);
	//MyMessageWait(L" Отсылка снимка экрана. Ожидайте...");
	//memset(name, 0, 500000);
	//wchar_t txt[200], info[1000];
	//memset(wzData, 0, BUFSIZE);
	//wcscpy(wzData, L"<soap:Envelope xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\"><soap:Header/><soap:Body><m:ОбменТСД xmlns:m=\"http://www.dns-shop.tsd.ru\"><m:ВидОперации xmlns:xs=\"http://www.w3.org/2001/XMLSchema\"  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">");
	//wcscat(wzData, L"Помощь");
	//wcscat(wzData, L"</m:ВидОперации><m:Список xmlns:xs=\"http://www.w3.org/2001/XMLSchema\"	xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">");
	//wcscat(wzData, L"<m:Номенклатура><m:Код>");
	//wcscat(wzData, L"Снимок экрана");
	//wcscat(wzData, L"</m:Код><m:Наименование>");
	//WideCharToMultiByte(CODEPAGE, 0, wzData, -1, name, BUFSIZE, 0, 0);
	//encode(tovar, name);
	//wcscpy(wzData, L"</m:Наименование><m:Количество>");
	//wcscat(wzData, L"0");
	//wcscat(wzData, L"</m:Количество></m:Номенклатура>");
	//wcscat(wzData, L"</m:Список></m:ОбменТСД></soap:Body></soap:Envelope>");
	//WideCharToMultiByte(CODEPAGE, 0, wzData, -1, szData, BUFSIZE, 0, 0);
	//strcat(name, szData);
	//int i;
	//HANDLE hFile;
	//DWORD dwNumberOfBytesRead;
	//memset(szData, 0, BUFSIZE);
	//dwBytesRead = 0;
	//wchar_t usr[150], psw[150];
	//char answer[300];
	//i = WideCharToMultiByte(CODEPAGE, 0, UserWS, wcslen(UserWS), answer, 300, 0, 0);
	//memset(usr, 0, sizeof(usr));
	//MultiByteToWideChar(CP_ACP, 0, answer, i, usr, sizeof(usr));
	//memset(answer, 0, sizeof(answer));
	//i = WideCharToMultiByte(CODEPAGE, 0, PswWS, wcslen(PswWS), answer, 300, 0, 0);
	//memset(psw, 0, sizeof(psw));
	//MultiByteToWideChar(CP_ACP, 0, answer, i, psw, sizeof(psw));
	//wchar_t head[] = L"\nAccept: */*\nSOAPAction: \"\"\nContent-Type: text/xml; charset=utf-8";
	//HINTERNET hInternet = InternetOpen(L"", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	//int lTimeoutInMs = 60000;
	//InternetSetOption(hInternet, INTERNET_OPTION_CONNECT_TIMEOUT, &lTimeoutInMs, 4);
	//HINTERNET hConnect = InternetConnectW(hInternet, serverWS, prt, usr, psw, INTERNET_SERVICE_HTTP, 0, 1);
	//HINTERNET hRequest = HttpOpenRequest(hConnect, L"POST", serviceWS, NULL, NULL, 0, INTERNET_FLAG_KEEP_CONNECTION, 1);
	//int szdta = strlen(name);
	//int fps = GetTickCount();
	//_Bool bSend = HttpSendRequest(hRequest, head, 65, name, szdta);	//собсно посылаем нужные данные xml в 1с  wcslen(head)=65
	//fps = GetTickCount() - fps;
	//if (fps > 0)
		//sndkbitsec = (((szdta) << 3) / fps);	//замер скорости

	//_Bool bRead = InternetReadFile(hRequest, szData, BUFSIZE, &dwBytesRead);	//принимаем данные от 1с .. тут все побыстрей как то
	//InternetCloseHandle(hRequest);
	//InternetCloseHandle(hConnect);
	//InternetCloseHandle(hInternet);
	//memset(wzData, 0, BUFSIZE);
	////memset(szData, 0, BUFSIZE);
	//LocalFree(name);
	//return TRUE;
//}

//посыл снимка по нажатию из окна помощи
void helpme(HWND hwndDlg)
{
	//wchar_t tovar[500], kolvo[100], code[100];
	//wcscpy(tovar, selfdir);
	//wcscat(tovar, L"testdialog.bmp");
	//screenshot(tovar);
	return;
	//KillTimer(hwndDlg, WM_USER + 2);
	//KillTimer(hwndDlg, WM_USER + 1);
	//wsprintf(kolvo, L"%u", Goodslistmode);
	//sendScreenShot(kolvo, zagolovokokna);
	////sendGoodslist(L"Помощь", zagolovokokna, otsklad, kolvo);
	//if (xmlgetw(3, L"<m:Наименов") < 1)
	//{
		//xmlgetw(3, L"detail");
		//ShowError(hwndDlg, wotvet);
		//return;
	//}
	//wcscpy(tovar, wotvet);
	//if (xmlgetw(3, L"<m:Код") < 1)
	//{
		//xmlgetw(3, L"detail");
		//ShowError(hwndDlg, wotvet);
		//return;
	//}
	//wcscpy(code, wotvet);
	//if (xmlgetw(3, L"<m:Количеств") < 1)
	//{
		//xmlgetw(3, L"detail");
		//ShowError(hwndDlg, wotvet);
		//return;
	//}
	//wcscpy(kolvo, wotvet);
	//if ((wcslen(code) < 2) || (wcscmp(code, L"Error") == 0))
	//{
		//ShowError(hwndDlg, tovar);
		//return;
	//}

}
//секвенс отображения окон и операций по коммандам сервера
void WindowCycle(HWND hwndDlg, int ModeOffset, wchar_t *prefix)
{
	wchar_t code[200], tovar[200], kolvo[200];
	int f;
	Goodslistmode = ModeOffset;
	f = 130;
	wcscpy(zagolovokokna, L"Выбор ");
	wcscat(zagolovokokna, prefix);
	while (Goodslistmode != 4)
	{
		//if (f == 0) Goodslistmode = 1300;
		//wcscpy(zagolovokokna,prefix);
		wcscpy(tovar, prefix);
		wcscat(tovar, L" ПодборЗадания");
		Goodslistmode = selectNext(hwndDlg, tovar);
		wcscpy(zagolovokokna, prefix);
		if (Goodslistmode == (ModeOffset + 37))
		{

			tomainmenu = FALSE;
			wcscat(zagolovokokna, L" Список");
			f = DialogBox(g_hInstance, MAKEINTRESOURCE(viborzadaniya), NULL, (DLGPROC)QuestSelect);

		}

		if (Goodslistmode == (ModeOffset + 40))
		{

			tomainmenu = FALSE;
			wcscat(zagolovokokna, L" СписокВыбора");
			f = DialogBox(g_hInstance, MAKEINTRESOURCE(viborzadaniya), NULL, (DLGPROC)QuestSelect);

		}


		if (Goodslistmode == (ModeOffset + 38))
		{

			tomainmenu = FALSE;
			wcscat(zagolovokokna, L" СписокСтандартный");
			f = DialogBox(g_hInstance, MAKEINTRESOURCE(goodslist), NULL, (DLGPROC)GoodsListProc);

		}


		if (Goodslistmode == (ModeOffset + 41))
		{

			tomainmenu = FALSE;
			wcscat(zagolovokokna, L" Список2");
			wcscpy(otsklad, L"Список2");
			f = DialogBox(g_hInstance, MAKEINTRESOURCE(goodslist), NULL, (DLGPROC)GoodsListProc);

		}


		if (Goodslistmode == (ModeOffset + 35))
		{
			tomainmenu = FALSE;

			wcscat(zagolovokokna, L" Форма перемещения товара");
			f = DialogBox(g_hInstance, MAKEINTRESOURCE(GoodsTree), NULL, (DLGPROC)TreeGoodsProc);
		}

		if (Goodslistmode == (ModeOffset + 44))
		{
			tomainmenu = FALSE;
			wcscat(zagolovokokna, L" Сканирование Адреса");
			if (!(DialogBox(g_hInstance, MAKEINTRESOURCE(nasklad), NULL, (DLGPROC)command1Proc) == 130))
			{
				//wcscpy(zagolovokokna, L"Отмена сканирования адреса");
				Goodslistmode = (ModeOffset + 45);

			}



		}

		if (Goodslistmode == (ModeOffset + 46))
		{

			tomainmenu = FALSE;
			wcscat(zagolovokokna, L" Список3");
			wcscpy(otsklad, sklad);
			f = DialogBox(g_hInstance, MAKEINTRESOURCE(goodslist), NULL, (DLGPROC)GoodsListProc);

		}

		if (Goodslistmode == (ModeOffset + 42))
		{
			tomainmenu = FALSE;

			wcscat(zagolovokokna, L" Дерево Поиск");
			f = DialogBox(g_hInstance, MAKEINTRESOURCE(GoodsTree), NULL, (DLGPROC)TreeGoodsProc);
		}

		if (Goodslistmode == (ModeOffset + 30))
		{
			tomainmenu = FALSE;
			wcscat(zagolovokokna, L" Форма подбора товара");
			f = DialogBox(g_hInstance, MAKEINTRESOURCE(GoodsTree), NULL, (DLGPROC)TreeGoodsProc);
		}
		if (Goodslistmode == (ModeOffset + 31))
		{
			tomainmenu = FALSE;
			wcscat(zagolovokokna, L" Выбор Транспорта");
			f = DialogBox(g_hInstance, MAKEINTRESOURCE(scanfrm), NULL, (DLGPROC)ScanForm);
		}
		if (Goodslistmode == (ModeOffset + 32))
		{
			tomainmenu = FALSE;
			wcscat(zagolovokokna, L" Адреса Задания");
			f = DialogBox(g_hInstance, MAKEINTRESOURCE(viborzadaniya), NULL, (DLGPROC)QuestSelect);
		}
		if (Goodslistmode == (ModeOffset + 36))
		{
			tomainmenu = FALSE;
			wcscat(zagolovokokna, L" Сотрудники Задания");
			f = DialogBox(g_hInstance, MAKEINTRESOURCE(viborzadaniya), NULL, (DLGPROC)QuestSelect);
		}
		if (Goodslistmode == (ModeOffset + 33))
		{
			tomainmenu = FALSE;
			wcscat(zagolovokokna, L" Сканирование Адреса Задания");
			f = DialogBox(g_hInstance, MAKEINTRESOURCE(scanfrm), NULL, (DLGPROC)ScanForm);
		}
		if (Goodslistmode == (ModeOffset + 39))
		{
			tomainmenu = FALSE;
			wsprintf(code, L"%u", MessageBox(hwndDlg, L"Cледующая секция", L"Вопрос", MB_ICONINFORMATION | MB_TOPMOST | MB_YESNO));
			wcscat(zagolovokokna, L"ОтветНаВопрос");
			sendGoodslist(zagolovokokna, code, code, code, TRUE);
		}
		if ((Goodslistmode < (ModeOffset + 30)) & (Goodslistmode != 4))
		{
			tomainmenu = FALSE;
			wcscat(zagolovokokna, L" Выбор задания");
			f = DialogBox(g_hInstance, MAKEINTRESOURCE(viborzadaniya), NULL, (DLGPROC)QuestSelect);
		}
	}



}
//корректное завершение любой операций
void exitFromButton(HWND hwndDlg)
{
	LPMSG pMsg;
	enabledialog(hwndDlg, TRUE);
	freeMessages(pMsg, hwndDlg);
	SetFocus(hwndDlg);
	wcscpy(zagolovokokna, L"ОсновноеОкно");
	tekWnd = hwndDlg;
	RepaintMainMenu = FALSE;
	tomainmenu = TRUE;
	getform1c();

//SetWindowLong(hwndDlg, DWL_MSGRESULT, DLGC_WANTCHARS| DLGC_WANTALLKEYS); // |DLGC_WANTARROWS
}


int NextWindow(HWND hwndDlg, wchar_t *txt)
{
	wchar_t tovar[255], code[255], kolvo[200];
	DLGPROC prc;
	long d;
	tomainmenu = FALSE;
	wsprintf(wotvet, L"%u", Goodslistmode);	//Строка(Goodslistmode) это номер "страницы"  текущего окна на тсд
	sendGoodslist(L"СледующееОкно", txt, zagolovokokna, wotvet, TRUE);
	MultiByteToWideChar(CODEPAGE, 0, szData, dwBytesRead, wzData, dwBytesRead);
	int i = xmlgetw(0, L"return");
	if (i < 1)
	{
		xmlgetw(0, L"detail");
		ShowError(hwndDlg, wotvet);
		MyMessageBox(0, wotvet, 0, 100);
		return -1;
	}
	if (xmlgetw(i, L"<m:Номенклатур") < 1)
		return -1;
	if (xmlgetw(i, L"<m:Наименовани") < 1)
		return -1;
	wcscpy(tovar, wotvet);
	if (xmlgetw(i, L"<m:Код") < 1)
		return -1;
	if (wcscmp(wotvet, L"Ошибка") == 0 || wcscmp(wotvet, L"Error") == 0)
	{
		ShowError(hwndDlg, tovar);
		return 0;
	}
	wcscpy(code, wotvet);
	if (xmlgetw(i, L"<m:Количество") < 1)
		return -1;

	wcscpy(zagolovokokna, tovar);
	Goodslistmode = _wtoi(wotvet);
	if (wcscmp(code, L"Окно сканирования адреса") == 0)
	{
		prc = (DLGPROC)command1Proc;
		d = nasklad;
	}
	if (wcscmp(code, L"Окно списка товаров") == 0)
	{
		prc = (DLGPROC)GoodsListProc;
		d = goodslist;
	}
	if (wcscmp(code, L"Окно информации") == 0)
	{
		prc = (DLGPROC)InfoProc;
		d = InfoForm;
	}
	if (wcscmp(code, L"Окно выбора из списка") == 0)
	{
		prc = (DLGPROC)QuestSelect;
		d = viborzadaniya;
	}
	if (wcscmp(code, L"Окно дерева товаров") == 0)
	{
		prc = (DLGPROC)TreeGoodsProc;
		d = GoodsTree;
	}
	if (wcscmp(code, L"Окно сканирования ТС") == 0)
	{
		prc = (DLGPROC)ScanForm;
		d = scanfrm;
	}
	if (wcscmp(code, L"Выход") == 0)
	{
		tomainmenu = TRUE;
		return -1;
	}
	return DialogBox(g_hInstance, MAKEINTRESOURCE(d), NULL, prc);
}



void nextWindowCycle(HWND hwndDlg, wchar_t *txt)
{
	int f;
	LPMSG pMsg;
	f = 130;
	while (f >= 0)
	{
		f = NextWindow(hwndDlg, txt);

		freeMessages(pMsg, hwndDlg);
	}

}


static _Bool DoCommands(HWND hwndDlg, wchar_t *tovar)
{
	wchar_t temp[255], code[255], kolvo[200];
	long int v, u, r, t, x, y, z, f, e, d, c, i;
	_Bool cycle, b;
	LPMSG pMsg;
	wcscpy(code, tovar + 2);
	if ((wcscmp(code, L"Выход") == 0) || (wcscmp(tovar, L"Выход") == 0) || (wcscmp(tovar, L"X") == 0))
	{
		tomainmenu = TRUE;
		EndDialog(hwndDlg, 0);
		sndxdto(L"Выход", L"Выход");
		clearExit();
		return TRUE;
	}
	if (wcscmp(code, L"Поиск товара") == 0)
	{
		wcscpy(zagolovokokna, L"Информация о товаре");
		tomainmenu = FALSE;
		f = DialogBox(g_hInstance, MAKEINTRESOURCE(InfoForm), NULL, (DLGPROC)InfoProc);
		wcscpy(code, sklad);
		if (wcscmp(code, L"Выход") == 0)
		{
			exitFromButton(hwndDlg);
			return TRUE;
		}
	}
	if ((wcscmp(code, L"Получить задание на подбор товара") == 0) || (wcscmp(code, L"Подбор") == 0) || (wcsncmp(code, L"Подбор", 6) == 0))
	{
		tomainmenu = FALSE;

		wcscpy(zagolovokokna, L"Выбор задания");
		wcscpy(label1, L"Филиал");
		wcscpy(knopkadalee, L"Далее");
		wcscpy(sklad, L"");
		Goodslistmode = 0;
		f = 130;
		enabledialog(hwndDlg, FALSE);

		while (f == 130 & Goodslistmode != 4)
		{
			//if (f == 130)
			Goodslistmode = selectNext(hwndDlg, L"ПодборЗадания");
			if (Goodslistmode == 3)
			{
				wcscpy(zagolovokokna, L"Форма подбора товара");
				f = DialogBox(g_hInstance, MAKEINTRESOURCE(GoodsTree), NULL, (DLGPROC)TreeGoodsProc);
			}
			if (Goodslistmode == 6)
			{
				wcscpy(zagolovokokna, L"Выбор Транспорта");
				//f = 130;
				DialogBox(g_hInstance, MAKEINTRESOURCE(scanfrm), NULL, (DLGPROC)ScanForm);
			}
			if (Goodslistmode == 7)
			{
				wcscpy(zagolovokokna, L"Выбор Адреса Задания");
				f = DialogBox(g_hInstance, MAKEINTRESOURCE(viborzadaniya), NULL, (DLGPROC)QuestSelect);
			}
			if (Goodslistmode == 8)
			{
				wcscpy(zagolovokokna, L"Сканирование Адреса Задания");
				f = DialogBox(g_hInstance, MAKEINTRESOURCE(scanfrm), NULL, (DLGPROC)ScanForm);
			}
			if (Goodslistmode == 5)
			{
				wsprintf(code, L"%u", MessageBox(hwndDlg, L"Хотите подобрать следующую секцию", L"Вопрос", MB_ICONINFORMATION | MB_TOPMOST | MB_YESNO));
				sendGoodslist(L"ОтветНаВопрос", code, code, code, TRUE);
			}
			if (Goodslistmode < 3 || Goodslistmode == 9)
			{
				wcscpy(zagolovokokna, L"Выбор задания");
				f = DialogBox(g_hInstance, MAKEINTRESOURCE(viborzadaniya), NULL, (DLGPROC)QuestSelect);
			}
			PeekMessage(pMsg, hwndDlg, WM_KEYFIRST, WM_KEYLAST, PM_REMOVE);
		}
		exitFromButton(hwndDlg);


		return TRUE;
	}

	//if (wcscmp(code, L"Подтоварка") == 0)
	//{
		//tomainmenu = FALSE;
		//enabledialog(hwndDlg, FALSE);
		//wcscpy(zagolovokokna, L"Выбор дефрагментации");
		//wcscpy(label1, L"Место");
		//wcscpy(knopkadalee, L"Далее");
		//wcscpy(sklad, L"");
		//Goodslistmode = 10;
		//f = 130;
		//enabledialog(hwndDlg, FALSE);

		//while (Goodslistmode != 4)
		//{
			////if (f == 130)
			//Goodslistmode = selectNext(hwndDlg, L"ДефрагментацияПодборЗадания");
			//if (Goodslistmode == 37)
			//{
				//tomainmenu = FALSE;
				//wcscpy(zagolovokokna, L"Дефрагментация Список");
				//f = DialogBox(g_hInstance, MAKEINTRESOURCE(goodslist), NULL, (DLGPROC)GoodsListProc);

			//}

			//if (Goodslistmode == 35)
			//{
				//tomainmenu = FALSE;
				//wcscpy(zagolovokokna, L"Дефрагментация Форма перемещения товара");
				//f = DialogBox(g_hInstance, MAKEINTRESOURCE(GoodsTree), NULL, (DLGPROC)TreeGoodsProc);
			//}

			//if (Goodslistmode == 30)
			//{
				//tomainmenu = FALSE;
				//wcscpy(zagolovokokna, L"Дефрагментация Форма подбора товара");
				//f = DialogBox(g_hInstance, MAKEINTRESOURCE(GoodsTree), NULL, (DLGPROC)TreeGoodsProc);
			//}
			//if (Goodslistmode == 60)
			//{
				//tomainmenu = FALSE;
				//wcscpy(zagolovokokna, L"Дефрагментация Выбор Транспорта");
				//f = DialogBox(g_hInstance, MAKEINTRESOURCE(scanfrm), NULL, (DLGPROC)ScanForm);
			//}
			//if (Goodslistmode == 70)
			//{
				//tomainmenu = FALSE;
				//wcscpy(zagolovokokna, L"Дефрагментация Выбор Адреса Задания");
				//f = DialogBox(g_hInstance, MAKEINTRESOURCE(viborzadaniya), NULL, (DLGPROC)QuestSelect);
			//}
			//if (Goodslistmode == 90)
			//{
				//tomainmenu = FALSE;
				//wcscpy(zagolovokokna, L"Дефрагментация Адреса");
				//wcscpy(label1, L"Адрес");
				//wcscpy(knopkadalee, L"Далее");
				//wcscpy(sklad, L"");

				//if (DialogBox(g_hInstance, MAKEINTRESOURCE(nasklad), NULL, (DLGPROC)command1Proc) == 130)
				//{
					//sendGoodslist(L"Подтвердить Дефрагментацию", sklad, otsklad, L"0", TRUE);

					//if (xmlgetw(3, L"<m:Количеств") <= 0)
					//{
						//ShowError(NULL, wzData);
					//}
					//else
						//Goodslistmode = _wtoi(wotvet);

					//if (xmlgetw(3, L"<m:Код") <= 0)
					//{
						//ShowError(NULL, wzData);
					//}

					//if ((wcscmp(wotvet, L"Ошибка") == 0) || (wcscmp(wotvet, L"Error") == 0))
					//{
						//xmlgetw(3, L"<m:Наименов");
						//ShowError(NULL, wotvet);

					//}

					//if (wcscmp(wotvet, L"Выход") == 0)
					//{
						//Goodslistmode = 4;
					//}

				//};
			//}

			//if (Goodslistmode == 80)
			//{
				//tomainmenu = FALSE;
				//wcscpy(zagolovokokna, L"Дефрагментация Сканирование Адреса Задания");
				//f = DialogBox(g_hInstance, MAKEINTRESOURCE(scanfrm), NULL, (DLGPROC)ScanForm);
			//}
			//if (Goodslistmode == 50)
			//{
				//tomainmenu = FALSE;
				//wsprintf(code, L"%u", MessageBox(hwndDlg, L"Хотите подобрать следующую секцию", L"Вопрос", MB_ICONINFORMATION | MB_TOPMOST | MB_YESNO));
				//sendGoodslist(L"ДефрагментацияОтветНаВопрос", code, code, code, TRUE);
			//}
			//if ((Goodslistmode < 30) & (Goodslistmode != 4))
			//{
				//tomainmenu = FALSE;
				//wcscpy(zagolovokokna, L"Дефрагментация Выбор задания");
				//f = DialogBox(g_hInstance, MAKEINTRESOURCE(viborzadaniya), NULL, (DLGPROC)QuestSelect);
			//}
		//}

		//exitFromButton(hwndDlg);

		//return TRUE;
	//}

	if (wcscmp(code, L"Разгрузка") == 0)
	{
		//tomainmenu = FALSE;
		tomainmenu = FALSE;
		wcscpy(label1, L"Место");
		wcscpy(knopkadalee, L"Далее");
		wcscpy(sklad, L"");
		enabledialog(hwndDlg, FALSE);
		WindowCycle(hwndDlg, 500, L"Разгрузка");
		exitFromButton(hwndDlg);
		return TRUE;
	}
	if (wcscmp(code, L"Погрузка") == 0)
	{
		tomainmenu = FALSE;
		wcscpy(label1, L"Место");
		wcscpy(knopkadalee, L"Далее");
		wcscpy(sklad, L"");
		WindowCycle(hwndDlg, 400, L"Погрузка");
		exitFromButton(hwndDlg);
		return TRUE;
	}

	if ((wcscmp(code, L"Переместить товар (с полки на полку)") == 0) || (wcscmp(code, L"Перемещение") == 0) || (wcscmp(code, L"Движение товара по складу") == 0) || (wcsncmp(code, L"Переместить с адреса", 20) == 0))
	{


		tomainmenu = FALSE;

		wcscpy(zagolovokokna, L"Полка отправитель");
		wcscpy(label1, L"Адрес");
		wcscpy(knopkadalee, L"Далее");
		wcscpy(sklad, L"");
		Goodslistmode = 0;
		if (wcsncmp(code, L"Переместить с адреса", 20) == 0)
		{
			cycle = TRUE;
			wcscpy(sklad, code + 21);

		}
		else if (DialogBox(g_hInstance, MAKEINTRESOURCE(nasklad), NULL, (DLGPROC)command1Proc) != 130)
			return TRUE;
		wcscpy(otsklad, sklad);
		wcscpy(zagolovokokna, L"ПереместитьСПолкиНаПолку");
		wcscpy(label1, L"Товар");
		wcscpy(knopkadalee, L"Переместить");
		Goodslistmode = 0;
		DialogBox(g_hInstance, MAKEINTRESOURCE(goodslist), NULL, (DLGPROC)GoodsListProc);
		exitFromButton(hwndDlg);
		return TRUE;
	}


//Это код на случай добавления новых кнопок , впринципе можно удалить код про инвентарку по  новой т.к тут будет обработка всего что нужно 

	Goodslistmode = 0;
	nextWindowCycle(hwndDlg, code);
	f = 4;
	exitFromButton(hwndDlg);
	return TRUE;
	//return TRUE;
}



//основное окно меню
static LRESULT CALLBACK MainDlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	wchar_t temp[1000], code[200], tovar[200], kolvo[200];	//,p;
	HWND ow, hw;
	long int v, u, r, t, x, y, z, f, e, d, c, i;
	_Bool cycle, b;
	LPMSG pMsg;
	long int cntrls[] = { IDOK, IDCANCEL, list, combo, ServerName, Userlist, treelist, PortEdit, IDCANCEL, IDOK };
	WNDPROC p = (WNDPROC)GetWindowLong(hwndDlg, GWL_USERDATA);
//if(0x135!=uMsg&&uMsg!=0x138){
	//wsprintf(code,L"---%x;%x;---",uMsg,wParam);
	//MyMessageWait(code);
	//}
//case WM_CTLCOLORLISTBOX:
	//SetBkColor((HDC)wParam, RGB(10,10,10));
	//SetTextColor((HDC)wParam, RGB(100,200,10));
	//return (LRESULT)CreateSolidBrush(RGB(10,50,10));  

	if (UseMotorolla && (!NowSending) && (tomainmenu) && WM_TIMER == uMsg)
	{
		v = GetKeyboardState(keys);
		//wsprintf(code,L"---%x;%x;---",v,wParam);
		//MyMessageWait(code);
		if (v != 0)
		{
//green key
			if (v == 119)
			{
				return SendMessage(GetFocus(), BM_CLICK, 0, 0);
			}
			hw = GetNextDlgTabItem(hwndDlg, 0, FALSE);
			ow = hw;
			while (hw != NULL)
			{
				GetWindowText(hw, code, 100);
				if ((wchar_t)v == code[0])
				{
					SendMessage(hw, BM_CLICK, 0, 0);
					SetWindowLong(hwndDlg, DWL_MSGRESULT, DLGC_WANTCHARS);
					return DWL_MSGRESULT | DLGC_WANTALLKEYS | DLGC_WANTCHARS;	// 
					break;
				}
				hw = GetNextDlgTabItem(hwndDlg, hw, FALSE);
				if (ow == hw)
					break;
			}
			if (uMsg == WM_GETDLGCODE)
				return FALSE;
		}
	}
	switch (uMsg)
	{
		case WM_CTLCOLORBTN:

			if ((HWND)lParam == GetFocus())
			{
				SetBkColor((HDC)wParam, RGB(110, 210, 110));

				return (LRESULT)CreateSolidBrush(RGB(210, 150, 210));
			}

			break;


		case WM_INITDIALOG:
		{
			//SetCaretBlinkTime(100);

			ListMode2 = 0;
			z = 0;
			hwnd = FindWindow(L"testdialClass", NULL);
			if (hwnd)
			{
				SetForegroundWindow((HWND) ((ULONG)hwnd | 0x01));
			}

			tomainmenu = TRUE;
			GetWindowRect(GetDesktopWindow(), &rect);
			x = rect.right - 1;
			y = rect.bottom - 25;
			SetWindowPos(hwndDlg, hwndDlg, 0, 0, x, y, SWP_NOZORDER);
			MainMenuhwnd = hwndDlg;
			c = 0;
			//for (z = 0; z < 4; z++)for (i = 0; i < 2; i++){d = cntrls[c++];SetWindowPos(GetDlgItem(hwndDlg, d), GetDlgItem(hwndDlg, d), 2 + i * (x / 2), 2 + z * (y / 4), x / 2 - 7, y / 4 - 7, SWP_NOZORDER);}
			autofontialog(hwndDlg, 6, 16);
			sx = x;
			sy = y;
			tekWnd = hwndDlg;
			if (UseMotorolla)
				SetTimer(hwndDlg, WM_USER + 33, 15, NULL);
			SetTimer(hwndDlg, WM_USER + 333, 33333, NULL);
			wcscpy(zagolovokokna, L"ОсновноеОкно_Впервые");
			//DWORD dwThreadId;
			SetWindowLong(hwndDlg, DWL_MSGRESULT, DLGC_WANTCHARS | DLGC_WANTALLKEYS | DLGC_WANTARROWS | DLGC_HASSETSEL);	//

			getform1c();
			tomainmenu = TRUE;
			return TRUE;
		}



		case WM_TIMER:
			if (wParam == (WM_USER + 333) && (!NowSending) && (tomainmenu))
			{
				wcscpy(temp, zagolovokokna);
				wcscpy(zagolovokokna, L"ОсновноеОкно");
				tekWnd = hwndDlg;
				RepaintMainMenu = FALSE;
				getform1c();
				wcscpy(zagolovokokna, temp);
			}
			//return DLGC_WANTALLKEYS | DLGC_WANTCHARS | DLGC_HASSETSEL;
			return TRUE;

		case WM_GETDLGCODE:
			//GetKeyboardState(&keys);
//GetKeyboardState(srch);
			pMsg = (LPMSG) (lParam);


			switch (pMsg->lParam)
			{
				case 0xc0000001:
				case WM_KEYUP:
					hw = GetNextDlgTabItem(hwndDlg, 0, FALSE);
					ow = hw;
					while (hw != NULL)
					{
						GetWindowText(hw, code, 100);
						if ((wchar_t)pMsg->wParam == code[0])
						{
							SendMessage(hw, BM_CLICK, 0, 0);
							SetWindowLong(hwndDlg, DWL_MSGRESULT, DLGC_WANTCHARS);
							return DWL_MSGRESULT | DLGC_WANTALLKEYS | DLGC_WANTCHARS;	// 
							break;
						}
						hw = GetNextDlgTabItem(hwndDlg, hw, FALSE);
						if (ow == hw)
							break;
					}
			}

//WNDPROC p = (WNDPROC)GetWindowLong(hwndDlg, GWL_USERDATA);
		//if (uMsg == WM_GETDLGCODE)
		//return DLGC_WANTALLKEYS;
		//return CallWindowProc(p, hwnd, uMsg, wParam, lParam);

			SetWindowLong(hwndDlg, DWL_MSGRESULT, DLGC_WANTALLKEYS | DLGC_WANTCHARS | DLGC_WANTARROWS | DLGC_HASSETSEL);	//
			return CallWindowProc(p, hwndDlg, uMsg, wParam, lParam);
			return DWL_MSGRESULT | DLGC_WANTALLKEYS | DLGC_WANTCHARS | DLGC_HASSETSEL | DLGC_WANTARROWS;	// 
			return TRUE;

//case 0x135:
		// wsprintf(code,L"---%x;%x;---",uMsg,wParam);
		//MyMessageWait(code);
		//SetWindowLong(hwndDlg, DWL_MSGRESULT, DLGC_WANTALLKEYS| DLGC_WANTCHARS |DLGC_WANTARROWS| DLGC_HASSETSEL); //
//      SetWindowLong(hwndDlg, DWL_MSGRESULT, DLGC_WANTALLKEYS); //
		//CallWindowProc(p, hwndDlg, uMsg, wParam, lParam); //tormoz
////return DWL_MSGRESULT|DLGC_WANTALLKEYS | DLGC_WANTCHARS  | DLGC_HASSETSEL|DLGC_WANTARROWS; //

		//return TRUE;


		case WM_COMMAND:
			SetWindowLong(hwndDlg, DWL_MSGRESULT, DLGC_WANTALLKEYS | DLGC_WANTCHARS | DLGC_WANTARROWS | DLGC_HASSETSEL);	//

			ListMode2 = 0;
			tomainmenu = FALSE;
			GetWindowText(GetDlgItem(hwndDlg, GET_WM_COMMAND_ID(wParam, lParam)), tovar, 100);
			return DoCommands(hwndDlg, tovar);

		case WM_CLOSE:
			EndDialog(hwndDlg, 0);
			return TRUE;
	}
// CallWindowProc(p, hwndDlg, uMsg, wParam, lParam);
//SetWindowLong(hwndDlg, DWL_MSGRESULT, DLGC_WANTALLKEYS| DLGC_WANTCHARS |DLGC_WANTARROWS| DLGC_HASSETSEL);
	return FALSE;
}
//проверка корректности ввода адреса
_Bool adrtest(HWND hwndDlg, wchar_t *temp)
{
//                          SoundOK(FALSE);
	wchar_t kolvo[100];
	wsprintf(kolvo, L"%u", Goodslistmode);
	//sendGoodslist(L"ПроверитьАдрес", temp, zagolovokokna, kolvo, TRUE);
	sndxdto(temp, L"ПроверитьАдрес");
	//if(checkreturn(hwndDlg))return TRUE;

	if (xmlgetw(3, L"<m:Наименов") > 1)
	{
		wcscpy(temp, wotvet);
//		MultiByteToWideChar(CODEPAGE, 0, otvet, dwBytesRead, temp, dwBytesRead);

		if (wcsncmp(temp, L"Выход", 5) == 0)
		{
			wcscpy(kolvo, temp + 6);
			KillTimer(hwndDlg, WM_USER + 1);
			EndDialog(hwndDlg, _wtoi(kolvo));
			return TRUE;
		}
		if (wcsncmp(temp, L"Ошибка", 6) == 0)
		{
			SetWindowText(GetDlgItem(hwndDlg, 4001), L"");
			SoundOK(FALSE);
			ShowError(hwndDlg, temp);
			return FALSE;
		}

		if (wcsncmp(temp, L"Заблокирован", 12) == 0)
		{
			SetWindowText(GetDlgItem(hwndDlg, 4004), temp);
			SetWindowText(GetDlgItem(hwndDlg, 4001), L"");
			SoundOK(FALSE);

			return FALSE;
		}


		SetWindowText(GetDlgItem(hwndDlg, 4004), temp);
		if (wcscmp(temp, L"Недействителен") != 0)
		{
			SoundOK(TRUE);
			//SoundOK(FALSE);
			GetWindowText(GetDlgItem(hwndDlg, 4001), sklad, 200);
			SetWindowText(GetDlgItem(hwndDlg, 4001), L"");
			//RemoveClipboardFormatListener(hwndDlg);
			return TRUE;
		}
		else
		{
			SetWindowText(GetDlgItem(hwndDlg, 4001), L"");
			SoundOK(FALSE);
		}
	}
	return FALSE;
}

//окно Ввод Адреса
static LRESULT CALLBACK command1Proc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	wchar_t temp[3000];
	int i;
	RECT rect;
	LPMSG pMsg;
	if (UseMotorolla && (GetTickCount() != lastoperation))
	{

//(...) screen keys 
		int key = 117;
		if (GetAsyncKeyState(key) != 0)
		{
			if (Pressed[key] == 0)
			{
				lastoperation = GetTickCount();
				Pressed[key] = 1;

				SendMessage(GetDlgItem(hwndDlg, 4001), BM_CLICK, 0, 0);

			}
		}
		else
			Pressed[key] = 0;

//(...) screen keys 
		key = 118;
		if (GetAsyncKeyState(key) != 0)
		{
			if (Pressed[key] == 0)
			{
				lastoperation = GetTickCount();
				Pressed[key] = 1;

				SendMessage(GetDlgItem(hwndDlg, 4005), BM_CLICK, 0, 0);

			}
		}
		else
			Pressed[key] = 0;


//green key
		key = 119;
		if (GetAsyncKeyState(key) != 0)
		{
			if (Pressed[key] == 0)
			{
				lastoperation = GetTickCount();
				Pressed[key] = 1;

				SendMessage(GetDlgItem(hwndDlg, 4001), BM_CLICK, 0, 0);

			}
		}
		else
			Pressed[key] = 0;
	}
	//??????????????????????????????????????????????--------------
	switch (uMsg)
	{
		case WM_INITDIALOG:
		{
			SHINITDLGINFO shidi;
			SetFocus(GetDlgItem(hwndDlg, 4001));
			shidi.dwMask = SHIDIM_FLAGS;
			shidi.dwFlags = SHIDIF_SIPDOWN | SHIDIF_SIZEDLGFULLSCREEN;
			shidi.hDlg = hwndDlg;
			int x = GetSystemMetrics(SM_CXSCREEN);
			int y = GetSystemMetrics(SM_CYSCREEN) - 30;
			SetWindowPos(hwndDlg, hwndDlg, 0, 0, x, y, SWP_NOZORDER);
			SetWindowPos(GetDlgItem(hwndDlg, list), GetDlgItem(hwndDlg, list), 1, 40, x - 6, 20, SWP_NOZORDER);
			SetWindowPos(GetDlgItem(hwndDlg, Userlist), GetDlgItem(hwndDlg, Userlist), 1, 60, x - 6, 16, SWP_NOZORDER);
			SetWindowPos(GetDlgItem(hwndDlg, IDOK), GetDlgItem(hwndDlg, IDOK), 3, 84, (x / 2) - 7, 50, SWP_NOZORDER);
			SetWindowPos(GetDlgItem(hwndDlg, 4005), GetDlgItem(hwndDlg, 4005), 3 + (x / 2), 84, (x / 2) - 9, 50, SWP_NOZORDER);
			SetWindowText(hwndDlg, zagolovokokna);
			SetWindowText(GetDlgItem(hwndDlg, combo), label1);
			SetWindowText(GetDlgItem(hwndDlg, 4003), knopkadalee);

			autofontialog(hwndDlg, 6, 16);
			sx = x;
			sy = y;
			tekWnd = hwndDlg;
			DWORD dwThreadId;
			getform1c();
			return TRUE;
		}

		case WM_SIZE:
			SetFocus(GetDlgItem(hwndDlg, 4001));
			return TRUE;

		case 0x80b4:
			getscanline(hwndDlg, temp);
			if (wcslen(temp) > 3)
			{
				SetWindowText(GetDlgItem(hwndDlg, 4001), temp);
				if (adrtest(hwndDlg, temp))
					EndDialog(hwndDlg, 130);
			}
			break;
		case WM_GETDLGCODE:
			pMsg = (LPMSG) (lParam);

		//---------------------------------------------------


		//====================================
			switch (pMsg->lParam)
			{

				case WM_KEYDOWN:

					if (LOWORD(pMsg->wParam) == VK_ESCAPE)
						SendMessage(GetDlgItem(hwndDlg, 4005), BM_CLICK, 0, 0);

					if (LOWORD(pMsg->wParam) == VK_RETURN)
					{
						GetWindowText(GetDlgItem(hwndDlg, 4001), temp, 200);
						SetFocus(GetDlgItem(hwndDlg, 4001));
						//if()
						if (wcslen(temp) > 3)
							if (adrtest(hwndDlg, temp))
								EndDialog(hwndDlg, 130);
					}
			}
			SetWindowLong(hwndDlg, DWL_MSGRESULT, DLGC_WANTALLKEYS | DLGC_WANTCHARS);
			return DLGC_WANTARROWS | DLGC_WANTALLKEYS | DLGC_WANTCHARS;



		case WM_TIMER:

			if (wParam == WM_USER + 1)
			{
				SetFocus(GetDlgItem(hwndDlg, 4001));
				GetWindowText(GetDlgItem(hwndDlg, 4001), temp, 200);
				if (wcslen(temp) < 5)
				{
					if (wcscmp(temp, L"2") == 0)
					{
						KillTimer(hwndDlg, WM_USER + 1);
						EndDialog(hwndDlg, 0);


					}
					if (wcslen(temp) < 5)
						return TRUE;
				}
				if (wcslen(temp) > 5)
				{
					sndxdto(temp, L"ПроверитьАдрес");
					if (dwBytesRead < 1)
					{
						SoundOK(FALSE);
						SetWindowText(GetDlgItem(hwndDlg, 4004), L"Ошибка доступа к сети");
						return TRUE;
					}
					if (xmlvalue(0, "return") > 1)
					{					wcscpy(temp, wotvet);

						//MultiByteToWideChar(CODEPAGE, 0, otvet, dwBytesRead, temp, dwBytesRead);
						SetWindowText(GetDlgItem(hwndDlg, 4004), temp);
						if (wcscmp(temp, L"Недействителен") != 0)
						{
							//SoundOK(FALSE);
							SoundOK(TRUE);
							wcscpy(oldadres, L"");
							SetWindowText(GetDlgItem(hwndDlg, 4001), temp);
							SetWindowText(GetDlgItem(hwndDlg, 4004), L"Ok");
						}

					}
				}
			}
			return TRUE;


		case WM_COMMAND:
			switch (GET_WM_COMMAND_ID(wParam, lParam))
			{
				case 4501:
					//HideError(hwndDlg);
					return TRUE;
				case 4005:
					KillTimer(hwndDlg, WM_USER + 1);
					GetWindowText(GetDlgItem(hwndDlg, 4005), temp, 200);
					adrtest(hwndDlg, temp);
				//sndxdto(temp, L"ПроверитьАдрес");
				//EndDialog(hwndDlg, 0);
					return TRUE;

				case 4001:
					SetTimer(hwndDlg, WM_USER + 1, 800, NULL);
					return TRUE;

				case IDOK:

					if (UseMotorolla)
					{
						getscanline(hwndDlg, temp);
						if (wcslen(temp) > 3)SetWindowText(GetDlgItem(hwndDlg, 4001), temp);
					}
					GetWindowText(GetDlgItem(hwndDlg, 4001), temp, 200);
					SetFocus(GetDlgItem(hwndDlg, 4001));
					if (wcslen(temp) > 3)
						if (adrtest(hwndDlg, temp))
						{
							EndDialog(hwndDlg, 130);
							return TRUE;
						}
					SoundOK(FALSE);
					return TRUE;




			}
			break;
		case 0x53:	//help_context

			helpme(hwndDlg);

			break;

		case WM_CLOSE:
			KillTimer(hwndDlg, WM_USER + 1);
			EndDialog(hwndDlg, 0);
			return TRUE;
	}



	return FALSE;
}

int addline(HWND hwndDlg, wchar_t *code, wchar_t *tovar, wchar_t *count, long int assoc)
{	//на самом деле запись по колонкам : количество , код , товар 
	HWND hLV = GetDlgItem(hwndDlg, list);
	int ActualItem;	// =0;
	ActualItem = SendMessage(hLV, LVM_GETITEMCOUNT, 0, 0);
	LV_ITEM lvi;
	lvi.mask = LVIF_TEXT | LVIF_PARAM;
	lvi.pszText = code;
	lvi.cchTextMax = wcslen(code);
	lvi.iItem = ActualItem;
	lvi.iSubItem = 0;	//0 обязательно
	lvi.lParam = _wtoi(code);
	lvi.lParam = assoc;
	ActualItem = SendMessage(hLV, LVM_INSERTITEM, 0, (LPARAM) & lvi);

	lvi.iItem = ActualItem;
	lvi.pszText = tovar;
	lvi.iSubItem = 1;
	SendMessage(hLV, LVM_SETITEMTEXT, ActualItem, (LPARAM) & lvi);
	lvi.iItem = ActualItem;
	lvi.pszText = count;
	lvi.iSubItem = 2;
	SendMessage(hLV, LVM_SETITEMTEXT, ActualItem, (LPARAM) & lvi);
	return ActualItem;
}



int findcode(HWND hwndDlg, wchar_t *scode, int *kk)
{
	LV_ITEM lvi;
	wchar_t kolvo[100], code[100];
	int z = _wtoi(scode);
	if (z < 1)
		return -1;
	int i = SendMessage(GetDlgItem(hwndDlg, list), LVM_GETITEMCOUNT, 0, 0);
	for (; i >= 0; i--)
	{

		lvi.pszText = kolvo;
		lvi.cchTextMax = sizeof(kolvo);
		lvi.iSubItem = 0;
		SendMessage(GetDlgItem(hwndDlg, list), LVM_GETITEMTEXT, i, (LPARAM) & lvi);
		lvi.pszText = code;
		lvi.cchTextMax = sizeof(code);
		lvi.iSubItem = 1;
		SendMessage(GetDlgItem(hwndDlg, list), LVM_GETITEMTEXT, i, (LPARAM) & lvi);
		if (_wtoi(code) == z)
		{
			*kk = _wtoi(kolvo);
			return i;
		}
	}
	return -1;
}

void mSetFocus(HWND hwnd)
{
	SetFocus(hwnd);
}


DWORD WINAPI filluuidthread()
{
	int i, z;
	HWND wnd = tekWnd;
	wchar_t code[100], tovar[500], kolvo[100];

	wsprintf(kolvo, L"%u", Goodslistmode);

	sendGoodslist(L"ПолучитьОстатки", sklad, otsklad, kolvo, TRUE);
	memset(wzData, 0, BUFSIZE);
	MultiByteToWideChar(CODEPAGE, 0, szData, dwBytesRead, wzData, dwBytesRead);
	if (xmlgetw(0, L"detail") > 1)
		ShowError(wnd, wotvet);
	i = 0;
	z = 3;
	int c = 0;
	GUIDSCOUNT = 0;
	while (z > 1 && !NowSending && GUIDSCOUNT <= GUIDSIZE)
	{
		//c++;
		z = xmlgetw(i, L"<m:Номенклатур");
		i = z;
		if ((z < 1) || (xmlgetw(i, L"<m:Код") <= 0))
			break;
		wcscpy(code, wotvet);
		if (xmlgetw(i, L"<m:Количеств") < 1)
			break;
		wcscpy(kolvo, wotvet);
		if (_wtoi(code) == 0 || wcslen(code) < 3)
			continue;
		xmlgetw(i, L"<m:Наименов");
		wcscpy(tovar, wotvet);
		if (wcscmp(code, L"Артикул") == 0)
		{
			wcscpy(uuid[GUIDSCOUNT].Article, tovar);
			continue;
		}

		wcscpy(uuid[++GUIDSCOUNT].guid, tovar);
		uuid[GUIDSCOUNT].kolvo = _wtoi(kolvo);
		uuid[GUIDSCOUNT].Param = 0;
		wcscpy(uuid[GUIDSCOUNT].Article, code);
		wcscpy(uuid[GUIDSCOUNT].Adress, code);




	}
	GUIDSCOUNT++;
	ExitThread(0);
	return 0;
}
//--------------------------------------------


//DWORD WINAPI fillcbthread()
//{
	//int i, z;
	//HWND wnd = tekWnd;
	//wchar_t code[100], tovar[500], kolvo[100];

	//wsprintf(kolvo, L"%u", Goodslistmode);
	//sendGoodslist(L"ПолучитьОстатки", sklad, otsklad, kolvo, TRUE);
	//memset(wzData, 0, BUFSIZE);
	//MultiByteToWideChar(CODEPAGE, 0, szData, dwBytesRead, wzData, dwBytesRead);
	//if (xmlgetw(0, L"detail") > 1)
		//ShowError(wnd, wotvet);
	//i = 0;
	//z = 3;
	//int c = 0;
	//while (z > 1)
	//{
		//c++;
		//z = xmlgetw(i, L"<m:Номенклатур");
		//i = z;
		//if (z < 1)
			//continue;
		//if (xmlgetw(i, L"<m:Код") <= 0)
			//continue;
		//wcscpy(code, wotvet);
		//if (_wtoi(code) == 0 || wcslen(code) < 3)
			//continue;
		//xmlgetw(i, L"<m:Наименов");
		//wcscpy(tovar, wotvet);
		//wcscat(tovar, L" <code:>");
		//wcscat(tovar, code);
		//wcscat(tovar, L"</code>");
		//SendMessage(GetDlgItem(wnd, combo), CB_ADDSTRING, 0, (LPARAM)tovar);
	//}
	//ExitThread(0);
	//return 0;
//}
////--------------------------------------------

//void fillcb(HWND hwndDlg)
//{

	//if (!cachemode)
		//return;
	////int i, z;
	////wchar_t code[100], tovar[500], kolvo[100];
	//DWORD dwThreadId;
	//tab2l = 0;
	//tab1l = 0;
	////tekpos = z;
	//tekWnd = hwndDlg;
	//CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)filluuidthread, NULL, 0, &dwThreadId);


//}

_Bool chkGoodsCount(HWND hwndDlg, int i, int z)
{
	wchar_t code[100], tovar[500], kolvo[100];
	//int kk;
	//if (wcslen(code) < 3)
	//{
	//SoundOK(FALSE);
	//return;
	//}
	//if (z <= 0 && Goodslistmode != 441)
	//{
	//SendMessage(GetDlgItem(hwndDlg, list), LVM_DELETEITEM, i, 0);
	//return TRUE;
	//}

	ListView_GetItemText(GetDlgItem(hwndDlg, list), i, 1, code, 100);
	//ListView_GetItemText(GetDlgItem(hwndDlg, list),i,0,kolvo,50);
	//z = _wtoi(kolvo)>>4;
	//z = 1;
	//z = (z * 16) + Goodslistmode;
	wsprintf(kolvo, L"%d", z);

	sendGoodslist(L"НайтиОстатокТовар", code, otsklad, kolvo, FALSE);
//MultiByteToWideChar(CODEPAGE, 0, szData, dwBytesRead, wzData, dwBytesRead);
	if (xmlgetw(3, L"<m:Наименов") < 1)
	{
		SoundOK(FALSE);
		xmlgetw(3, L"detail");
		return FALSE;
	}
	wcscpy(tovar, wotvet);
	if (xmlgetw(3, L"<m:Код") < 1)
	{
		SoundOK(FALSE);
		xmlgetw(3, L"detail");
		return FALSE;
	}
	wcscpy(code, wotvet);
	if (xmlgetw(3, L"<m:Количеств") < 1)
	{
		SoundOK(FALSE);
		xmlgetw(3, L"detail");
		return FALSE;
	}
	wcscpy(kolvo, wotvet);
	if ((wcslen(code) < 2) || (wcscmp(code, L"Error") == 0))
	{
		SoundOK(FALSE);
		wcscpy(wotvet, tovar);
		return FALSE;
	}
	//SoundOK(TRUE);
	z = _wtoi(kolvo);
	//if (z == 0 && Goodslistmode != 441)
	//SendMessage(GetDlgItem(hwndDlg, list), LVM_DELETEITEM, i, 0);
	//else
	ListView_SetItemText(GetDlgItem(hwndDlg, list), i, 0, kolvo);
	ListView_SetItemState(GetDlgItem(hwndDlg, list), i, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);

	editc = 0;

	return TRUE;




}

_Bool resultList(HWND hwndDlg, _Bool Event)
{
	int i, z, y, x, f;
	wchar_t code[100], tovar[500], kolvo[100];
	i = 0;
	z = 3;
	int c = 0;
	while (z > 1)
	{
		z = xmlgetw(i, L"<m:Номенклатур");
		i = z;
		if ((z < 1) || (xmlgetw(i, L"<m:Код") <= 0))
			break;
		wcscpy(code, wotvet);
		if (xmlgetw(i, L"<m:Количеств") < 1)
			break;
		wcscpy(kolvo, wotvet);
		xmlgetw(i, L"<m:Наименов");
		wcscpy(tovar, wotvet);

		if (wcscmp(code, L"ВыйтиСОшибкой") == 0)
		{
			ShowError(hwndDlg, tovar);
			KillTimer(hwndDlg, WM_USER + 2);
			EndDialog(hwndDlg, _wtoi(kolvo));

			continue;
		}


		if (wcscmp(code, L"Ошибка") == 0 || wcscmp(code, L"Error") == 0)
		{
			ShowError(hwndDlg, tovar);
			continue;
		}
		if (wcscmp(code, L"Выйти") == 0 || wcscmp(code, L"Успех") == 0)
		{
			KillTimer(hwndDlg, WM_USER + 2);
			EndDialog(hwndDlg, _wtoi(kolvo));
			return TRUE;

		}
		if (wcscmp(code, L"ВозвратРезультата") == 0 )	return _wtoi(kolvo);
		if (wcscmp(code, L"Артикул") == 0)
		{

			for (f = 0; f <= GUIDSCOUNT; f++)
				if ((wcscmp(uuid[f].Article, tovar) == 0))
					break;
			if (f <= GUIDSCOUNT)
				continue;
			wcscpy(uuid[++GUIDSCOUNT].Article, tovar);
			wcscpy(uuid[GUIDSCOUNT].Adress, L"+++");
			uuid[GUIDSCOUNT].kolvo = _wtoi(kolvo);
			continue;
		}

		if (wcscmp(code, L"Удалить") == 0)
		{
			x = findcode(hwndDlg, tovar, &y);
			if (x >= 0)
				SendMessage(GetDlgItem(hwndDlg, list), LVM_DELETEITEM, x, 0);
			continue;
		}

		if ((wcscmp(code, L"Вопрос") == 0))
		{
			sendOtvetVopros(hwndDlg, L"НайтиТовар", L"Выбор товара", tovar, otsklad, i);
			z = 3;
			i = 0;
			continue;
		}
		if (wcscmp(uuid[GUIDSCOUNT].Adress, L"+++") == 0)
		{
			wcscpy(uuid[GUIDSCOUNT].Adress, code);
			wcscpy(uuid[GUIDSCOUNT].guid, tovar);
		}
		//Нашли и изменяем его состояние на экране------------------------------------------
		x = findcode(hwndDlg, code, &y);
		if (_wtoi(kolvo) <= 0)
		{

			if (x < 0)
			{
				if (_wtoi(kolvo) < 0)
				{
					wsprintf(kolvo, L"%u", -_wtoi(kolvo));
					x = addline(hwndDlg, kolvo, code, tovar, x);
				}
			}
			else
			{
				wsprintf(kolvo, L"%u", y - _wtoi(kolvo));
				ListView_SetItemText(GetDlgItem(hwndDlg, list), x, 0, kolvo);
				ListView_SetItemText(GetDlgItem(hwndDlg, list), x, 1, code);
				ListView_SetItemText(GetDlgItem(hwndDlg, list), x, 2, tovar);
			}
		}
		else
		{
			if (x < 0)
				x = addline(hwndDlg, kolvo, code, tovar, x);
			else
			{
				ListView_SetItemText(GetDlgItem(hwndDlg, list), x, 0, kolvo);
				ListView_SetItemText(GetDlgItem(hwndDlg, list), x, 1, code);
				ListView_SetItemText(GetDlgItem(hwndDlg, list), x, 2, tovar);
			}


		}
	}

	ListView_SetItemState(GetDlgItem(hwndDlg, list), x, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
	ListView_EnsureVisible(GetDlgItem(hwndDlg, list), x, TRUE);

	SetFocus(GetDlgItem(hwndDlg, combo));
	SendMessage(GetDlgItem(hwndDlg, combo), CB_SETCURSEL, -1, 0);
	editc = 0;
	return TRUE;

}

//DWORD WINAPI sendListThread()
//{
	//int z = 0, t = 0, c = 0;
	//NowSending = TRUE;
	//int tmpbuf;
	//wchar_t *temp, tmpstr[10], scrstr[10];
	//temp = (wchar_t *)LocalAlloc(0, BUFSIZE * sizeof(wchar_t));	// 1251 юникод
	//if (nowtab1)
	//{
		//t = tab2l;
		//tmpbuf = 2;
		//tab2l = 0;
	//}
	//else
	//{
		//t = tab1l;
		//tmpbuf = 1;
		//tab1l = 0;
	//}
	//wcscpy(temp, L"<soap:Envelope xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\"><soap:Header/><soap:Body><m:ОбменТСД xmlns:m=\"http://www.dns-shop.tsd.ru\"><m:ВидОперации xmlns:xs=\"http://www.w3.org/2001/XMLSchema\"  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">");
	//wcscat(temp, L"НайтиСписокТоваров");
	//wcscat(temp, L"</m:ВидОперации><m:Список xmlns:xs=\"http://www.w3.org/2001/XMLSchema\"	xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">");
	//_Bool isok = FALSE;
	//for (; c <= t; c++)
	//{
		//z = tmp[tmpbuf][c].kolvo;
		//if (z == 0)
			//continue;
		//wsprintf(scrstr, L"%d", tmp[tmpbuf][c].Param);
		//wsprintf(tmpstr, L"%d", z);
		//wcscat(temp, L"<m:Номенклатура><m:Код>");
		//wcscat(temp, tmp[tmpbuf][c].guid);
		//wcscat(temp, L"</m:Код><m:Наименование>");
		//wcscat(temp, scrstr);
		//wcscat(temp, L"</m:Наименование><m:Количество>");
		//wcscat(temp, tmpstr);
		////wcscpy(tmpstr, tmp[tmpbuf][c].guid);
		//wcscat(temp, L"</m:Количество></m:Номенклатура>");
		//isok = TRUE;
	//}
	//wchar_t gettxt[255], modify[255];
	//memset((char *)tmp[tmpbuf], 0, sizeof(struct guids) * ScanBuf);
	//if (!isok)
	//{
		//NowSending = FALSE;
		//LocalFree((char *)temp);
		//return TRUE;
	//}
	//wcscat(temp, L"</m:Список></m:ОбменТСД></soap:Body></soap:Envelope>");
	//sendws(temp);
	//LocalFree((char *)temp);
	//MultiByteToWideChar(CODEPAGE, 0, szData, dwBytesRead, wzData, dwBytesRead);
	//NowSending = FALSE;

	//if (!resultList(tekWnd, FALSE))
		//return 0;	//ShowError(tekWnd,wotvet);
	//if (!NowSending)
		//return waitforServerList(tekWnd, FALSE);
	//return 1;
	////ExitThread(0);
//}


//static _Bool waitforServerList(HWND hwndDlg, _Bool Thread)
//{
	//int t = 0, c = 0;
	//DWORD dwThreadId;
	//int tmpbuf;
	//while (NowSending)
	//{
		//Sleep(1);
		//c++;
	//}
	//if (nowtab1)
	//{
		//t = tab1l;
		//tmpbuf = 1;
	//}
	//else
	//{
		//t = tab2l;
		//tmpbuf = 2;
	//}
	//nowtab1 = !nowtab1;
	////memset((char *)erbuf,0,sizeof(struct guids)*ScanBuf);
	//if (t == 0)
		//return TRUE;
	//NowSending = TRUE;
	//tekWnd = hwndDlg;
	//if (Thread)
		//CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)sendListThread, NULL, 0, &dwThreadId);
	//else
		//return sendListThread();
	//return TRUE;
//}

//void AddListBarcode(wchar_t *scan, int kol)
//{
	//int t = 0, c = 0;
	//int tmpbuf;
	//wchar_t ttt[255];
	//if (nowtab1)
	//{
		//tab1l++;
		//t = tab1l;
		//tmpbuf = 1;
	//}
	//else
	//{
		//tab2l++;
		//t = tab2l;
		//tmpbuf = 2;
	//}
	//for (c = 0; c < t; c++)
		//if ((wcscmp(tmp[tmpbuf][c].guid, scan) == 0))
		//{
			//if (nowtab1)
				//tab1l--;
			//else
				//tab2l--;
			//break;
		//}	//||(uuid[c].kolvo==0)
	//wcscpy(tmp[tmpbuf][c].guid, scan);
	////wcscpy(tmp[0][0].guid,scan);
	//tmp[tmpbuf][c].kolvo++;
	//tmp[tmpbuf][c].Param = kol;
	////wcscpy(ttt,tmp[tmpbuf][c].guid);
	////wsprintf(scan,L"%x %u %u",t,tmp1[c].kolvo,tmp2[c].kolvo);
	//SoundOK(TRUE);
//}

//_Bool chkOffline(HWND hwndDlg, wchar_t *scn)
//{

	//int d, i, c = 0;

	//wchar_t scan[100], gettxt[255], modify[255];
	//wcscpy(scan, scn);
	//LV_ITEM lvi;
	//wchar_t kolvo[100], code[100];
	//for (c = 0; c <= GUIDSCOUNT; c++)
	//{
		//if ((wcscmp(uuid[c].Adress, scan) == 0) || (wcsncmp(uuid[c].Adress, scan, 7) == 0) || (wcscmp(uuid[c].Article, scan) == 0))
			//break;
	//}	//ищем нужный ЕАН in таблиц
	//SetFocus(GetDlgItem(hwndDlg, combo));
	//SendMessage(GetDlgItem(hwndDlg, combo), CB_SETCURSEL, -1, 0);
	//editc = 0;
	////---------------------------------
	//if (c > GUIDSCOUNT)	//not found блеать
	//{
		//AddListBarcode(scan, 0);
		//return (_Bool)waitforServerList(hwndDlg, FALSE);	//)ShowError(tekWnd,wotvet);
	//}	//Нашли и изменяем его состояние на экране------------------------------------------
	//if (wcscmp(uuid[c].Adress, L"+++") == 0)
	//{
		//AddListBarcode(scan, -2);
		//return (_Bool)waitforServerList(hwndDlg, FALSE);	//)ShowError(tekWnd,wotvet);
	//}
	//i = findcode(hwndDlg, uuid[c].Adress, &d);
	//if (i < 0)
	//{
		//addline(hwndDlg, L"1", uuid[c].Adress, uuid[c].guid, c);
		//AddListBarcode(scan, -1);
	//}
	//else
	//{
		////d++;
		//AddListBarcode(scan, d++);
		//wsprintf(kolvo, L"%d", d);
		//ListView_SetItemText(GetDlgItem(hwndDlg, list), i, 0, kolvo);
		//ListView_SetItemState(GetDlgItem(hwndDlg, list), i, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
		//ListView_EnsureVisible(GetDlgItem(hwndDlg, list), i, TRUE);
		//if (uuid[c].kolvo > 0 && d > uuid[c].kolvo)
			//return (_Bool)waitforServerList(hwndDlg, FALSE);

	//}
	//SetFocus(GetDlgItem(hwndDlg, combo));
	//SendMessage(GetDlgItem(hwndDlg, combo), CB_SETCURSEL, -1, 0);
	//editc = 0;
	//_Bool rt = TRUE;
	//if (!NowSending)
		//rt = waitforServerList(tekWnd, TRUE);
	//return rt;
//}



_Bool chkGoods(HWND hwndDlg, wchar_t *code)
{
	wchar_t tovar[500], kolvo[100];
	int kk, i, z;
	_Bool VoprosReady;
	if (wcslen(code) < 3)
	{
		//SoundOK(FALSE);
		return FALSE;
	}

	//if (cachemode)return chkOffline(hwndDlg, code); // отключил режим кеширования данных 

	VoprosReady = FALSE;
	z = 1;
	//z = (z * 16) + Goodslistmode;
	wsprintf(kolvo, L"%d", z);

	//wsprintf(kolvo, L"%u", Goodslistmode);
	sendGoodslist(L"НайтиТовар", code, otsklad, kolvo, FALSE);

	if (xmlgetw(3, L"<m:Код") < 1)
	{
		xmlgetw(3, L"detail");
		ShowError(hwndDlg, wotvet);
		return FALSE;

	}
	wcscpy(code, wotvet);
	if ((wcscmp(code, L"Вопрос") == 0))
	{
		i = xmlgetw(3, L"<m:Наименов");
		sendOtvetVopros(hwndDlg, L"НайтиТовар", L"Выбор товара", wotvet, otsklad, i);
		VoprosReady = TRUE;
		xmlgetw(3, L"<m:Код");
		wcscpy(code, wotvet);
	}


	if (xmlgetw(3, L"<m:Наименов") < 1)
	{
		xmlgetw(3, L"detail");
		ShowError(hwndDlg, wotvet);
		return FALSE;

	}
	wcscpy(tovar, wotvet);
	if (xmlgetw(3, L"<m:Количеств") < 1)
	{
		xmlgetw(3, L"detail");
		ShowError(hwndDlg, wotvet);
		return FALSE;

	}
	wcscpy(kolvo, wotvet);
	if ((wcslen(code) < 2) || (wcscmp(code, L"Error") == 0) || (wcscmp(code, L"Ошибка") == 0))
	{
		ShowError(hwndDlg, tovar);
		return FALSE;

	}
	if (wcscmp(code, L"Выйти") == 0)
	{
		KillTimer(hwndDlg, WM_USER + 2);
		EndDialog(hwndDlg, _wtoi(kolvo));
		return TRUE;

	}
	if (wcscmp(code, L"Ничего") == 0)
	{
		return TRUE;

	}


	//MultiByteToWideChar(CODEPAGE, 0, szData, dwBytesRead, wzData, dwBytesRead);
	//i = xmlgetw(0, L"return");
	//if (xmlgetw(i, L"<m:Код") > 0)
	//{
	//wcscpy(code, wotvet);
	//xmlgetw(i, L"<m:Наименов");
	//wcscpy(tovar, wotvet);
	//xmlgetw(i, L"<m:Количеств");
	//wcscpy(kolvo, wotvet);
	int p = _wtoi(kolvo);
	//if ((wcslen(code) < 2) || (wcscmp(code, L"Error") == 0))
	//{
	//ShowError(hwndDlg, tovar);
	//return;
	//} // товар ненайден в базе можно выдавать сообщение тут
	mSetFocus(GetDlgItem(hwndDlg, list));
	i = findcode(hwndDlg, code, &kk);
	if (i >= 0)
	{
		kk++;
		if (kk > p)
		{
			if (!chkGoodsCount(hwndDlg, i, kk))
			{
				ShowError(hwndDlg, wotvet);
				return FALSE;

			}
		}
		SoundOK(TRUE);
		wsprintf(kolvo, L"%u", kk);
		//ListView_EnsureVisible(GetDlgItem(hwndDlg, list),i,TRUE );  
		ListView_SetItemText(GetDlgItem(hwndDlg, list), i, 0, kolvo);
		ListView_SetItemState(GetDlgItem(hwndDlg, list), i, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
		//  RECT rc;ListView_GetItemRect(GetDlgItem(hwndDlg, list),i,&rc,LVIR_BOUNDS);
		ListView_EnsureVisible(GetDlgItem(hwndDlg, list), i, TRUE);
//      ListView_EnsureVisible(GetDlgItem(hwndDlg, list),ListView_GetSelectionMark(GetDlgItem(hwndDlg, list),FALSE); 
		//ListView_Scroll(GetDlgItem(hwndDlg, list),0,1000);
		SetFocus(GetDlgItem(hwndDlg, combo));
		SendMessage(GetDlgItem(hwndDlg, combo), CB_SETCURSEL, -1, 0);
		editc = 0;
		return TRUE;

	}
	//  MultiByteToWideChar(CODEPAGE, 0, otvet, dwBytesRead, temp, dwBytesRead);

	SoundOK(TRUE);
	i = addline(hwndDlg, VoprosReady ? kolvo : L"1", code, tovar, 33);
	SendMessage(GetDlgItem(hwndDlg, list), LVM_SETSELECTIONMARK, 0, i);
	//ListView_SetSelectionMark(GetDlgItem(hwndDlg, list),1);
	ListView_SetItemState(GetDlgItem(hwndDlg, list), i, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
	ListView_Scroll(GetDlgItem(hwndDlg, list), 0, 1000);
	ListView_EnsureVisible(GetDlgItem(hwndDlg, list), i, TRUE);

	mSetFocus(GetDlgItem(hwndDlg, combo));
	SendMessage(GetDlgItem(hwndDlg, combo), CB_SETCURSEL, -1, 0);
	editc = 0;
	return TRUE;

}

void RecalcALL(HWND fromList, HWND toEdit)
{
	LV_ITEM lvi;
	int i, c;
	wchar_t kolvo[100];
	//lvi.mask= LVIF_TEXT;
	c = 0;
	i = SendMessage(fromList, LVM_GETITEMCOUNT, 0, 0);
	for (; i > 0;)
	{
		i--;
		lvi.pszText = kolvo;
		lvi.cchTextMax = 100;
		lvi.iSubItem = 0;	//0 обязательно
		SendMessage(fromList, LVM_GETITEMTEXT, i, (LPARAM) & lvi);
		c = c + _wtoi(kolvo);
	}
	wsprintf(kolvo, L"%u", c);
	SetWindowText(toEdit, kolvo);


}

_Bool scaningList(HWND hwndDlg, _Bool KeyDown)
{
	wchar_t code[500], kolvo[100];
	int i, z;
	i = ListView_GetSelectionMark(GetDlgItem(hwndDlg, list));
	if (Goodslistmode == 441)
	{
		getscanline(hwndDlg, code);
		if (KeyDown && (i >= 0))
		{
			ListView_GetItemText(GetDlgItem(hwndDlg, list), i, 0, kolvo, 50);
			z = _wtoi(kolvo);
			z++;
			if (!chkGoodsCount(hwndDlg, i, z))
				ShowError(hwndDlg, wotvet);
			RecalcALL(GetDlgItem(hwndDlg, list), GetDlgItem(hwndDlg, 4014));
			SoundOK(TRUE);

		}

	}
	else
	{
		getscanline(hwndDlg, code);
		if (wcslen(code) > 3)
		{
			chkGoods(hwndDlg, code);
			i = SendMessage(GetDlgItem(hwndDlg, list), LVM_GETSELECTIONMARK, 0, 0);
			if (i >= 0)
			{
				ListView_GetItemText(GetDlgItem(hwndDlg, list), i, 2, code, 200);
				SetWindowText(GetDlgItem(hwndDlg, 4011), code);
			}

			ShowWindow(GetDlgItem(hwndDlg, combo), SW_HIDE);
			SetWindowText(GetDlgItem(hwndDlg, combo), L"");
			//mSetFocus(GetDlgItem(hwndDlg, combo));
			RecalcALL(GetDlgItem(hwndDlg, list), GetDlgItem(hwndDlg, 4014));

		}
	}

	return TRUE;


}

_Bool SendListwithCommand(HWND hwndDlg, wchar_t *command, wchar_t *operation,_Bool FlagsOnly)
{
	wchar_t code[100], kolvo[100],tovar[200];
	int i, z;
	RECT rect;
	LV_COLUMN lvc;
	LV_ITEM lvi;
	LVFINDINFO lvf;
	LPNMHDR jj;
	HWND hwndList = GetDlgItem(hwndDlg, list);
	int c, x, y, kk, k;
//  LPMSG pMsg;
	wcscpy(tovar,command);
	wcscpy(wzData, L"<soap:Envelope xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\"><soap:Header/><soap:Body><m:ОбменТСД xmlns:m=\"http://www.dns-shop.tsd.ru\"><m:ВидОперации xmlns:xs=\"http://www.w3.org/2001/XMLSchema\"  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">");
	wcscat(wzData, operation);
	wcscat(wzData, L"</m:ВидОперации><m:Список xmlns:xs=\"http://www.w3.org/2001/XMLSchema\"	xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">");
	i = SendMessage(GetDlgItem(hwndDlg, list), LVM_GETITEMCOUNT, 0, 0);
	z =0;
	for (; i > 0;)
	{

		i--;
		lvi.pszText = kolvo;
		lvi.cchTextMax = sizeof(kolvo);
		lvi.iSubItem = 0;	//0 обязательно
		SendMessage(hwndList, LVM_GETITEMTEXT, i, (LPARAM) & lvi);
		lvi.pszText = code;
		lvi.cchTextMax = sizeof(code);
		lvi.iSubItem = 1;
		SendMessage(hwndList, LVM_GETITEMTEXT, i, (LPARAM) & lvi);
		if(FlagsOnly){
			if(!ListView_GetCheckState(hwndList, i)) continue;
		lvi.pszText = tovar;
		lvi.cchTextMax = sizeof(tovar);
		lvi.iSubItem = 2;
		SendMessage(hwndList, LVM_GETITEMTEXT, i, (LPARAM) & lvi);

			z++;
		}



		wcscat(wzData, L"<m:Номенклатура><m:Код>");
		wcscat(wzData, code);
		wcscat(wzData, L"</m:Код><m:Наименование>");
		wcscat(wzData, command);
		wcscat(wzData, L"</m:Наименование><m:Количество>");
		c = _wtoi(kolvo);
		wsprintf(kolvo, L"%u", c);
		wcscat(wzData, kolvo);
		wcscat(wzData, L"</m:Количество></m:Номенклатура>");
	}
	if(FlagsOnly && z==0) return FALSE;
	wcscat(wzData, L"</m:Список></m:ОбменТСД></soap:Body></soap:Envelope>");
	sendws(wzData);
	MultiByteToWideChar(CODEPAGE, 0, szData, dwBytesRead, wzData, dwBytesRead);
	return resultList(hwndDlg, TRUE);
}





//Работа со списком товара и сканы по списку
static LRESULT CALLBACK GoodsListProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	wchar_t temp[TempSize], code[100], tovar[500], kolvo[100];	//,button[100];
	//char srch[500];
	int i, z;
	RECT rect;
	LV_COLUMN lvc;
	LV_ITEM lvi;
	LVFINDINFO lvf;
	LPNMHDR jj;
	int c, x, y, kk, k;
	LPMSG pMsg;
	_Bool zavershenie;
	HWND hwndList;

	//GetKeyboardState(&keys);
//GetKeyboardState(srch);
	//  wsprintf(code,L"---%x;%x;%x;%x;---",GetKeyState(0xd),GetAsyncKeyState(0x78),GetKeyState(0x79),wParam);
	//  MyMessageWait(code);
	if (UseMotorolla && (GetTickCount() != lastoperation))
	{

		hwndList = GetDlgItem(hwndDlg, list);
		i = ListView_GetSelectionMark(hwndList);


//(...) screen keys 
		int key = 117;
		if (GetAsyncKeyState(key) != 0)
		{
			if (Pressed[key] == 0)
			{
				lastoperation = GetTickCount();
				Pressed[key] = 1;

				SendMessage(GetDlgItem(hwndDlg, 4007), BM_CLICK, 0, 0);

			}
		}
		else
			Pressed[key] = 0;

//(...) screen keys 
		key = 118;
		if (GetAsyncKeyState(key) != 0)
		{
			if (Pressed[key] == 0)
			{
				lastoperation = GetTickCount();
				Pressed[key] = 1;

				SendMessage(GetDlgItem(hwndDlg, 4008), BM_CLICK, 0, 0);

			}
		}
		else
			Pressed[key] = 0;


//green key
		key = 119;



		if (GetAsyncKeyState(key) != 0)
		{
			if (Pressed[key] == 0)
			{
				lastoperation = GetTickCount();
				Pressed[key] = 1;
				if (editc == 0)
				{
					KillTimer(hwndDlg, WM_USER + 2);
					//mSetFocus(hwndList);
					editc = 1;
					LPMSG p1Msg;
					//KillTimer(hwndDlg, WM_USER + 2);
					//freeMessages(p1Msg, hwndDlg);
					SendMessage(hwndList, LVM_EDITLABEL, i, 0);
					//return TRUE;
					SetTimer(hwndDlg, WM_USER + 4, 10, NULL);
				}
				else
				{

					KillTimer(hwndDlg, WM_USER + 4);
					//mSetFocus(GetDlgItem(hwndDlg, combo));
					mSetFocus(GetDlgItem(hwndDlg, list));
				}
			}
		}
		else
			Pressed[key] = 0;
		// right - left
		key = 39;

		//if(keys[39]& 128){
		if (GetAsyncKeyState(key) != 0 && (i >= 0))
		{
			if (Pressed[key] == 0)
			{
				lastoperation = GetTickCount();
				Pressed[key] = 1;
				ListView_GetItemText(hwndList, i, 0, kolvo, 50);
				z = _wtoi(kolvo);
				z++;

				if (!chkGoodsCount(hwndDlg, i, z))
					ShowError(hwndDlg, wotvet);
				RecalcALL(hwndList, GetDlgItem(hwndDlg, 4014));
				editc = 0;

			}
		}
		else
			Pressed[key] = 0;
		key = 37;

		if (GetAsyncKeyState(key) != 0 && (i >= 0))
		{
			if (Pressed[key] == 0)
			{
				lastoperation = GetTickCount();
				Pressed[key] = 1;
				ListView_GetItemText(hwndList, i, 0, kolvo, 50);
				z = _wtoi(kolvo);
				z--;

				if (!chkGoodsCount(hwndDlg, i, z))
					ShowError(hwndDlg, wotvet);
				RecalcALL(hwndList, GetDlgItem(hwndDlg, 4014));
				editc = 0;
			}
		}
		else
			Pressed[key] = 0;
	}
	//-----------------------------------------------------------------------
	switch (uMsg)
	{
		case WM_INITDIALOG:
		{
			GUIDSCOUNT = 0;
			SetWindowLong(hwndDlg, DWL_MSGRESULT, DLGC_WANTALLKEYS | DLGC_WANTCHARS);
			//memset((char*)uuid,0, GUIDSIZE * sizeof(struct guids));
			//SetWindowLong(hwndDlg, GWL_STYLE, GetWindowLong(hwndDlg, GWL_STYLE) | WS_CAPTION);
			SetWindowPos(hwndDlg, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_DRAWFRAME);
			OpenClipboard(hwndDlg);
			EmptyClipboard();
			CloseClipboard();
			SHINITDLGINFO shidi;
			//SetFocus(GetDlgItem(hwndDlg, combo));
			shidi.dwMask = SHIDIM_FLAGS;
			shidi.dwFlags = SHIDIF_SIPDOWN | SHIDIF_SIZEDLGFULLSCREEN;
			shidi.hDlg = hwndDlg;
			SHInitDialog(&shidi);




			x = GetSystemMetrics(SM_CXSCREEN);
			y = GetSystemMetrics(SM_CYSCREEN);
			GetWindowRect(hwndDlg, &rect);
			SetWindowPos(hwndDlg, hwndDlg, 0, 0, x, y, SWP_NOZORDER);
			GetWindowRect(hwndDlg, &rect);
			x = rect.right;
			y = rect.bottom;
			//25 + 45 + 40
			int nl = 25 + 35 + 40 + 20, niz = 25 + 25;

			SetWindowPos(GetDlgItem(hwndDlg, list), GetDlgItem(hwndDlg, list), 0, 20, x, y - nl, SWP_NOZORDER);
			SetWindowPos(GetDlgItem(hwndDlg, combo), GetDlgItem(hwndDlg, combo), 0, 0, x, 40, SWP_NOZORDER);
			SetWindowPos(GetDlgItem(hwndDlg, password), GetDlgItem(hwndDlg, password), 3, y - niz, 80, 20, SWP_NOZORDER);
			SetWindowPos(GetDlgItem(hwndDlg, 4010), GetDlgItem(hwndDlg, 4010), 3 + 80 + 3, y - niz, 80, 20, SWP_NOZORDER);
			ShowWindow(GetDlgItem(hwndDlg, 4010), SW_SHOW);

			SetWindowPos(GetDlgItem(hwndDlg, Namespace), GetDlgItem(hwndDlg, Namespace), 3 + 80 + 3 + 80, y - niz, 80, 20, SWP_NOZORDER);
			fontialog(GetDlgItem(hwndDlg, 4011), 6, 16, 800, 0);
			if (Goodslistmode == 1)
			{
				SetWindowPos(GetDlgItem(hwndDlg, password), GetDlgItem(hwndDlg, password), 3, y - niz, 80, 20, SWP_NOZORDER);
				SetWindowPos(GetDlgItem(hwndDlg, 4010), GetDlgItem(hwndDlg, 4010), 3 + 80 + 3, y - niz, 80, 20, SWP_NOZORDER);
				ShowWindow(GetDlgItem(hwndDlg, 4010), SW_SHOW);
				SetWindowPos(GetDlgItem(hwndDlg, Namespace), GetDlgItem(hwndDlg, Namespace), 3 + 80 + 3 + 80, y - niz, 80, 20, SWP_NOZORDER);
			}
			if (Goodslistmode == 0)
			{
				SetWindowPos(GetDlgItem(hwndDlg, password), GetDlgItem(hwndDlg, password), 3, y - niz, 90, 20, SWP_NOZORDER);
				SetWindowPos(GetDlgItem(hwndDlg, Namespace), GetDlgItem(hwndDlg, Namespace), 3 + 90 + 3, y - niz, 90, 20, SWP_NOZORDER);
			}

			if (Goodslistmode == 2)
			{
				SetWindowPos(GetDlgItem(hwndDlg, password), GetDlgItem(hwndDlg, password), 3, y - niz, 90, 20, SWP_NOZORDER);
				SetWindowPos(GetDlgItem(hwndDlg, Namespace), GetDlgItem(hwndDlg, Namespace), 3 + 90 + 3, y - niz, 90, 20, SWP_NOZORDER);
			}

			SetWindowText(hwndDlg, zagolovokokna);
			SetWindowText(GetDlgItem(hwndDlg, password), knopkadalee);


			ListView_SetExtendedListViewStyle(GetDlgItem(hwndDlg, list), LVS_EX_ONECLICKACTIVATE | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP | LVS_EX_FULLROWSELECT);
			lvc.mask = (LVCF_TEXT + LVCF_WIDTH + LVCF_FMT + LVCF_SUBITEM);
			lvc.fmt = LVCFMT_LEFT;
			lvc.iSubItem = 2;
			lvc.cx = 180;
			lvc.pszText = L"Товар";
			lvc.cchTextMax = 6;
			SendMessage(GetDlgItem(hwndDlg, list), LVM_INSERTCOLUMN, 0, (LPARAM) & lvc);
			lvc.iSubItem = 1;
			lvc.cx = 58;

			lvc.pszText = L"Код";
			lvc.cchTextMax = 3;
			SendMessage(GetDlgItem(hwndDlg, list), LVM_INSERTCOLUMN, 0, (LPARAM) & lvc);
			lvc.iSubItem = 3;
			lvc.cx = 20;
			lvc.pszText = L"Количество";
			lvc.cchTextMax = 10;
			SendMessage(GetDlgItem(hwndDlg, list), LVM_INSERTCOLUMN, 0, (LPARAM) & lvc);
			//SendMessage(GetDlgItem(hwndDlg, combo), CB_RESETCONTENT, 0, 0);
			//ShowWindow(GetDlgItem(hwndDlg, combo), SW_SHOW);
			SetFocus(GetDlgItem(hwndDlg, combo));
			ShowWindow(GetDlgItem(hwndDlg, combo), SW_HIDE);
			//SetTimer(hwndDlg, WM_USER + 2, 1000, NULL);
			//SetFocus(GetDlgItem(hwndDlg, combo));
			if (UseMotorolla)
				mSetFocus(GetDlgItem(hwndDlg, list));
			SetTimer(hwndDlg, WM_USER + 2, 500, NULL);
			sx = x;
			sy = y;
			tekWnd = hwndDlg;
			getform1c();
			//EnableWindow(GetDlgItem(hwndDlg, combo), TRUE);
			//editc = 0;

			//SetForegroundWindow(hwndDlg);
			//fillcb(hwndDlg);
			return TRUE;
		}



		//case 0x100:
		//if(!UseMotorolla)break;
		//if(!(wParam == 0xd||wParam == 0x78||wParam == 0x79||wParam == 0x77))break;
		//wParam = 0xbc01;
		case 0x80b4:

			//SetTimer(hwndDlg, WM_USER + 2, 500, NULL);
			editc = 0;
			return scaningList(hwndDlg, (wParam == 0xbc01));




			return TRUE;

		case WM_CONTEXTMENU:
		{
			// Извлекаем координаты курсора мыши из lParam
			POINT pt;
			pt.x = GET_X_LPARAM(lParam);
			pt.y = GET_Y_LPARAM(lParam);

			if (pt.x == -1 && pt.y == -1)
			{
				// Вызов с клавиатуры
				// RECT rect;
				GetWindowRect(hwndDlg, &rect);
				// Выводим меню радом с левым верхним углом окна
				pt.x = rect.left + 5;
				pt.y = rect.top + 5;
			}

			// Загружаем меню из ресурсов
			HMENU hPopupMenu = CreatePopupMenu();
			AppendMenu(hPopupMenu, MF_STRING, 2002, L"Вычерк");
//AppendMenu(hPopupMenu, MF_SEPARATOR, 0, 0);                             // --------------
//AppendMenu(hPopupMenu, MF_STRING, ID_EDIT_CUT, "Cu&t\tCtrl+X");         // Cut     Ctrl+X
//AppendMenu(hPopupMenu, MF_STRING, ID_EDIT_COPY, "&Copy\tCtrl+C");       // Copy    Ctrl+C
//AppendMenu(hPopupMenu, MF_STRING, ID_EDIT_PASTE, "&Paste\tCtrl+V");     // Paste   Ctrl+V
			//HMENU hMenu, hPopupMenu;
			//hMenu = LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MENU1));
			//hPopupMenu = GetSubMenu(hMenu, 0);

			// Отображаем меню
			TrackPopupMenu(hPopupMenu, TPM_LEFTALIGN, pt.x, pt.y, 0, hwndDlg, NULL);
			// Уничтожаем меню
			DestroyMenu(hPopupMenu);
		}
			break;


		case WM_GETDLGCODE:



			pMsg = (LPMSG) (lParam);
			SipDown(hwndDlg);

		//wsprintf(code,L"%x",pMsg->lParam);
		//MyMessageWait(code);

			switch (pMsg->lParam)
			{
				//case WM_KEYUP:
				//if (((LOWORD(pMsg->wParam) == 0x13) || (LOWORD(pMsg->wParam) == 0x10) || (LOWORD(pMsg->wParam) == VK_RETURN)) && editc == 0)
				//{

				//KillTimer(hwndDlg, WM_USER + 2);
				//mSetFocus(GetDlgItem(hwndDlg, list));
				//i = SendMessage(GetDlgItem(hwndDlg, list), LVM_GETSELECTIONMARK, 0, 0);
				//editc = 1;
				//SendMessage(GetDlgItem(hwndDlg, list), LVM_EDITLABEL, i, 0);
				//return TRUE;
//}
				case WM_KEYDOWN:

					//if (LOWORD(pMsg->wParam) == VK_CANCEL)SendMessage(GetDlgItem(hwndDlg, Namespace), BM_CLICK, 0, 0);
					//if (LOWORD(pMsg->wParam) == VK_BACK)SendMessage(GetDlgItem(hwndDlg, ServerName), BM_CLICK, 0, 0);
					if (LOWORD(pMsg->wParam) == VK_UP)
					{
						mSetFocus(GetDlgItem(hwndDlg, list));
						ListView_SetSelectionMark(GetDlgItem(hwndDlg, list), (ListView_GetSelectionMark(GetDlgItem(hwndDlg, list)) - 1));
						editc = 0;
					}
					if (LOWORD(pMsg->wParam) == VK_DOWN)
					{
						mSetFocus(GetDlgItem(hwndDlg, list));
						ListView_SetSelectionMark(GetDlgItem(hwndDlg, list), ListView_GetSelectionMark(GetDlgItem(hwndDlg, list)) + 1);
						//mSetFocus(GetDlgItem(hwndDlg, combo));

						editc = 0;
					}

					if (LOWORD(pMsg->wParam) == VK_LEFT)
					{
						i = ListView_GetSelectionMark(GetDlgItem(hwndDlg, list));
						if (i >= 0)
						{
							ListView_GetItemText(GetDlgItem(hwndDlg, list), i, 0, kolvo, 50);
							z = _wtoi(kolvo);
							z--;

							if (!chkGoodsCount(hwndDlg, i, z))
								ShowError(hwndDlg, wotvet);
						}
						RecalcALL(GetDlgItem(hwndDlg, list), GetDlgItem(hwndDlg, 4014));
						editc = 0;
					}
					if (LOWORD(pMsg->wParam) == VK_RIGHT)
					{
						i = ListView_GetSelectionMark(GetDlgItem(hwndDlg, list));
						if (i >= 0)
						{
							ListView_GetItemText(GetDlgItem(hwndDlg, list), i, 0, kolvo, 50);
							z = _wtoi(kolvo);
							z++;
							if (!chkGoodsCount(hwndDlg, i, z))
								ShowError(hwndDlg, wotvet);

						}

						RecalcALL(GetDlgItem(hwndDlg, list), GetDlgItem(hwndDlg, 4014));
						editc = 0;
					}

					if (((LOWORD(pMsg->wParam) == 0x13) || (LOWORD(pMsg->wParam) == 0x10) || (LOWORD(pMsg->wParam) == VK_RETURN)) && editc == 0)
					{

						KillTimer(hwndDlg, WM_USER + 2);
						mSetFocus(GetDlgItem(hwndDlg, list));
						i = SendMessage(GetDlgItem(hwndDlg, list), LVM_GETSELECTIONMARK, 0, 0);
						editc = 1;
						LPMSG p1Msg;

						freeMessages(p1Msg, hwndDlg);
						SendMessage(GetDlgItem(hwndDlg, list), LVM_EDITLABEL, i, 0);

						return TRUE;
						GetWindowText(GetDlgItem(hwndDlg, combo), code, 100);
						ShowWindow(GetDlgItem(hwndDlg, combo), SW_HIDE);
						SetWindowText(GetDlgItem(hwndDlg, combo), L"");
						chkGoods(hwndDlg, code);
						SetTimer(hwndDlg, WM_USER + 2, 1000, NULL);
						mSetFocus(GetDlgItem(hwndDlg, combo));
					}
			}
			if (editc == 0)
			{
				SetTimer(hwndDlg, WM_USER + 2, 500, NULL);
			}
			SetWindowLong(hwndDlg, DWL_MSGRESULT, DLGC_WANTALLKEYS | DLGC_WANTCHARS);
			return DLGC_WANTALLKEYS | DLGC_WANTCHARS | DLGC_WANTARROWS;	// | DLGC_HASSETSEL;  

			return DLGC_WANTARROWS | DLGC_WANTALLKEYS | DLGC_WANTCHARS;


		case WM_NOTIFY:
			jj = (LPNMHDR)lParam;
		// wsprintf(temp,L"%x",GetAsyncKeyState(39));
		// MyMessageWait(temp);

			SipDown(hwndDlg);	// 
			if (editc == 0)
			{
				SetTimer(hwndDlg, WM_USER + 2, 1000, NULL);
			}

			if (wParam == list)
			{

				LPNMLVDISPINFOW ttt = (LPNMLVDISPINFOW)lParam;
				if (jj->code == LVN_ENDLABELEDIT)
				{
					KillTimer(hwndDlg, WM_USER + 4);
					k = _wtoi(ttt->item.pszText);
					//  if (k == 0) SendMessage(GetDlgItem(hwndDlg, ServerName), BM_CLICK, 0, 0);
					//if (k > 0||Goodslistmode == 441||Goodslistmode == 1)
					if (!chkGoodsCount(hwndDlg, ttt->item.iItem, k))
						ShowError(hwndDlg, wotvet);
					//ListView_SetItemText(GetDlgItem(hwndDlg, list), ttt->item.iItem, 0, ttt->item.pszText);   // помещаем редактируемый текст в ячейку редактируемой таблицы
					editc = 0;
					RecalcALL(GetDlgItem(hwndDlg, list), GetDlgItem(hwndDlg, 4014));
					//Freemessages(
					if (!UseMotorolla)
					{
						mSetFocus(GetDlgItem(hwndDlg, combo));
						SetTimer(hwndDlg, WM_USER + 2, 1000, NULL);
					}
				}
				else if (jj->code == NM_DBLCLK)	// 0x3e8)// || (jj->code
				{	//долгое удержание стилуса на таблице or dblclick

					//TrackPopupMenu(hPopupMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, hWnd, NULL);

					// Уничтожаем меню
					//DestroyMenu(hMenu);

					KillTimer(hwndDlg, WM_USER + 2);
					mSetFocus(GetDlgItem(hwndDlg, list));
					i = SendMessage(GetDlgItem(hwndDlg, list), LVM_GETSELECTIONMARK, 0, 0);
					editc = 1;
					SendMessage(GetDlgItem(hwndDlg, list), LVM_EDITLABEL, i, 0);
					SipDown(hwndDlg);
					//SipShowIM(SIPF_OFF);

				}
				else if (jj->code == LVN_BEGINLABELEDIT)
				{

					return TRUE;
				}
				else
				{
//wsprintf(tovar,L"--%x--",jj->code);
					//MyMessageWait(tovar);
					i = SendMessage(GetDlgItem(hwndDlg, list), LVM_GETSELECTIONMARK, 0, 0);
					if (i >= 0)
					{
						ListView_GetItemText(GetDlgItem(hwndDlg, list), i, 2, tovar, 200);
						SetWindowText(GetDlgItem(hwndDlg, 4011), tovar);
					}
				}


			}
			else
			{
				mSetFocus(GetDlgItem(hwndDlg, combo));
				SetTimer(hwndDlg, WM_USER + 2, 1000, NULL);
			}
			return TRUE;

		case WM_TIMER:
			//if (wParam == WM_USER + 4){

			//int key = 119;
			//// wsprintf(code,L"---%x---",GetAsyncKeyState(key));
			////MyMessageWait(code);

			//if (GetAsyncKeyState(key) != 0 && Pressed[key] == 0)
			//{lastoperation = GetTickCount(); 
			//Pressed[key] = 1;
			//KillTimer(hwndDlg, WM_USER + 4);
			////editc = 0;
			//hwndList = GetDlgItem(hwndDlg, list);
			////i = ListView_GetSelectionMark(hwndList);
			////LPMSG p1Msg;
			////KillTimer(hwndDlg, WM_USER + 2);
			////freeMessages(p1Msg, hwndDlg);
			//SendMessage(hwndList, LVM_EDITLABEL, i, 0);
			////return TRUE;
			////        }else {
			//SendMessage(hwndList,WM_KEYDOWN, VK_RETURN, 0);   
			////SendMessage(hwndList, LVN_ENDLABELEDIT, i, 0);
			////SetTimer(hwndDlg, WM_USER + 4, 50, NULL);
			//}
			//else
			//Pressed[key] = 0;

			//}
			SetWindowLong(hwndDlg, DWL_MSGRESULT, DLGC_WANTALLKEYS | DLGC_WANTCHARS);
			if (wParam == WM_USER + 2)
				if (editc == 0)
				{
					KillTimer(hwndDlg, WM_USER + 4);
					i = SendMessage(GetDlgItem(hwndDlg, list), LVM_GETSELECTIONMARK, 0, 0);
					if (i >= 0)
					{
						ListView_GetItemText(GetDlgItem(hwndDlg, list), i, 2, tovar, 200);
						SetWindowText(GetDlgItem(hwndDlg, 4011), tovar);
					}

					if (!UseMotorolla)
					{
						mSetFocus(GetDlgItem(hwndDlg, combo));
					}
					//SendMessage(GetDlgItem(hwndDlg, combo), CB_SETEDITSEL, 0, 0xffff);
					EnableWindow(GetDlgItem(hwndDlg, 4010), TRUE);
					EnableWindow(GetDlgItem(hwndDlg, password), TRUE);
					RecalcALL(GetDlgItem(hwndDlg, list), GetDlgItem(hwndDlg, 4014));

				}


			return TRUE;

		case WM_COMMAND:
			KillTimer(hwndDlg, WM_USER + 2);
			wsprintf(kolvo, L"%u", GET_WM_COMMAND_ID(wParam, lParam));
		//MyMessageWait(kolvo);

			GetWindowText(GetDlgItem(hwndDlg, GET_WM_COMMAND_ID(wParam, lParam)), tovar, 100);
			if ((wcscmp(tovar, L"Заполнить все") == 0) || (wcscmp(tovar, L"Убрать все") == 0) || (wcscmp(tovar, L"Выбрать все") == 0) || (wcscmp(tovar, L"Удалить все") == 0) || (wcscmp(tovar, L"Набрать все") == 0) || (wcscmp(tovar, L"Отметить все") == 0))
			{
				tekWnd = hwndDlg;
				wcscpy(zagolovokokna, L"СписокДЯ_");
				wcscat(zagolovokokna, tovar);
				SetTimer(hwndDlg, WM_USER + 2, 500, NULL);
				getform1c();
				return TRUE;
			}


			switch (GET_WM_COMMAND_ID(wParam, lParam))
			{

				case 2002:

					mSetFocus(GetDlgItem(hwndDlg, list));
					i = ListView_GetSelectionMark(GetDlgItem(hwndDlg, list));
					lvi.pszText = kolvo;
					lvi.cchTextMax = sizeof(kolvo);
					lvi.iSubItem = 0;	//0 обязательно
					SendMessage(GetDlgItem(hwndDlg, list), LVM_GETITEMTEXT, i, (LPARAM) & lvi);
					lvi.pszText = code;
					lvi.cchTextMax = sizeof(code);
					lvi.iSubItem = 1;
					SendMessage(GetDlgItem(hwndDlg, list), LVM_GETITEMTEXT, i, (LPARAM) & lvi);


					break;
				case 4009:
					mSetFocus(GetDlgItem(hwndDlg, combo));
					ShowWindow(GetDlgItem(hwndDlg, combo), SW_SHOW);
					SendMessage(GetDlgItem(hwndDlg, combo), CB_SHOWDROPDOWN, TRUE, 0);
					KillTimer(hwndDlg, WM_USER + 2);
					editc = 1;
					SipDown(hwndDlg);
					return TRUE;

				case 4010:
				case password:
					//EnableWindow(GetDlgItem(hwndDlg, 4008), FALSE);

					EnableWindow(GetDlgItem(hwndDlg, 4010), FALSE);
					EnableWindow(GetDlgItem(hwndDlg, password), FALSE);
				//SetTimer(hwndDlg, WM_USER + 2, 1000, NULL);
				//SetTimer(hwndDlg,WM_USER+2,1000,NULL); 
					if (Goodslistmode == 0)	//Перемещение
					{
						wcscpy(zagolovokokna, L"Склад Получатель");
						wcscpy(label1, L"Адрес");
						wcscpy(knopkadalee, L"Далее");
						KillTimer(hwndDlg, WM_USER + 2);
						if (DialogBox(g_hInstance, MAKEINTRESOURCE(nasklad), NULL, (DLGPROC)command1Proc) == 130)
						{
							freeMessages(pMsg,hwndDlg);
							wcscpy(wzData, L"<soap:Envelope xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\"><soap:Header/><soap:Body><m:ОбменТСД xmlns:m=\"http://www.dns-shop.tsd.ru\"><m:ВидОперации xmlns:xs=\"http://www.w3.org/2001/XMLSchema\"  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">Переместить</m:ВидОперации><m:Список xmlns:xs=\"http://www.w3.org/2001/XMLSchema\"	xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">");
							wcscat(wzData, L"<m:Номенклатура><m:Код>");
							wcscat(wzData, sklad);
							wcscat(wzData, L"</m:Код><m:Наименование>");
							wcscat(wzData, otsklad);
							wcscat(wzData, L"</m:Наименование><m:Количество>");
							wcscat(wzData, kolvo);
							wcscat(wzData, L"</m:Количество></m:Номенклатура>");
							i = SendMessage(GetDlgItem(hwndDlg, list), LVM_GETITEMCOUNT, 0, 0);
							for (; i > 0;)	//собираем с экрана все итемы и высылаем их в базу списком
							{
								i--;
								//lvi.mask= LVIF_TEXT;
								lvi.pszText = kolvo;
								lvi.cchTextMax = sizeof(kolvo);
								//lvi.iItem = i;
								lvi.iSubItem = 0;	//0 обязательно
								SendMessage(GetDlgItem(hwndDlg, list), LVM_GETITEMTEXT, i, (LPARAM) & lvi);
								lvi.pszText = code;
								lvi.cchTextMax = sizeof(code);
								lvi.iSubItem = 1;
								SendMessage(GetDlgItem(hwndDlg, list), LVM_GETITEMTEXT, i, (LPARAM) & lvi);
								if (wcslen(code) > 9 || wcslen(kolvo) > 3)
									MyMessageBox(hwndDlg, code, kolvo, 2);
								wcscat(wzData, L"<m:Номенклатура><m:Код>");
								wcscat(wzData, code);
								wcscat(wzData, L"</m:Код><m:Наименование>");
								wcscat(wzData, L"товар");
								wcscat(wzData, L"</m:Наименование><m:Количество>");
								wcscat(wzData, kolvo);
								wcscat(wzData, L"</m:Количество></m:Номенклатура>");
							}

							wcscat(wzData, L"</m:Список></m:ОбменТСД></soap:Body></soap:Envelope>");
							sendws(wzData);
							MultiByteToWideChar(CODEPAGE, 0, szData, dwBytesRead, wzData, dwBytesRead);
							//MessageBox(0,wzData,0,0);
							i = xmlgetw(0, L"return");
							if (i < 2)
							{
								xmlgetw(0, L"detail");
								ShowError(hwndDlg, wotvet);
								MyMessageBox(hwndDlg, wzData, L"Error", 2);
								return TRUE;
							}

							//i = xmlgetw(0,L"return"); if(i<1){if(xmlgetw(0,L"detail")<1)xmlgetw(0,L"faultstring");MyMessageBox(hwndDlg,wotvet,L"Error",2);return TRUE;}
							//KillTimer(hwndDlg,WM_USER+2);
							if (xmlgetw(i, L"<m:Код") > 0)
							{
								wcscpy(code, wotvet);

								xmlgetw(i, L"<m:Наименов");
								wcscpy(tovar, wotvet);
								if ((wcscmp(code, L"Error") == 0))
								{
									ShowError(hwndDlg, tovar);
									return TRUE;
								}
								else if (wcscmp(code, L"Выход") == 0 || GET_WM_COMMAND_ID(wParam, lParam) == password)
								{
									KillTimer(hwndDlg, WM_USER + 2);
									EndDialog(hwndDlg, 130);
									return TRUE;
								}
								SendMessage(GetDlgItem(hwndDlg, list), LVM_DELETEALLITEMS, 0, 0);
								tekWnd = hwndDlg;
								wcscpy(zagolovokokna, L"Остаток ДЯ");
								getform1c();

							}
							else
							{

								if (xmlgetw(0, L"detail") < 1)
								{
									xmlgetw(0, L"faultstring");
									ShowError(hwndDlg, wotvet);
									MyMessageBox(hwndDlg, wotvet, kolvo, 2);
								}
							}
						}
						SetTimer(hwndDlg, WM_USER + 2, 1000, NULL);
					}

					if (Goodslistmode == 1)	//Инвентаризация
					{
						wcscpy(wzData, L"<soap:Envelope xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\"><soap:Header/><soap:Body><m:ОбменТСД xmlns:m=\"http://www.dns-shop.tsd.ru\"><m:ВидОперации xmlns:xs=\"http://www.w3.org/2001/XMLSchema\"  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">Инвентаризация</m:ВидОперации><m:Список xmlns:xs=\"http://www.w3.org/2001/XMLSchema\"	xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">");
						i = SendMessage(GetDlgItem(hwndDlg, list), LVM_GETITEMCOUNT, 0, 0);
						for (; i > 0;)
						{
							i--;
							//lvi.mask= LVIF_TEXT;
							lvi.pszText = kolvo;
							lvi.cchTextMax = sizeof(kolvo);
							//lvi.iItem = i;
							lvi.iSubItem = 0;	//0 обязательно
							SendMessage(GetDlgItem(hwndDlg, list), LVM_GETITEMTEXT, i, (LPARAM) & lvi);
							lvi.pszText = code;
							lvi.cchTextMax = sizeof(code);
							lvi.iSubItem = 1;
							SendMessage(GetDlgItem(hwndDlg, list), LVM_GETITEMTEXT, i, (LPARAM) & lvi);
							//if(wcslen(code)>9||wcslen(kolvo)>3)MessageBox(0,code,kolvo,0);
							wcscat(wzData, L"<m:Номенклатура><m:Код>");
							wcscat(wzData, code);
							wcscat(wzData, L"</m:Код><m:Наименование>");
							//wcscat(temp,L"товар");
							wcscat(wzData, tovar);
							wcscat(wzData, L"</m:Наименование><m:Количество>");
							wcscat(wzData, kolvo);
							wcscat(wzData, L"</m:Количество></m:Номенклатура>");
						}
						wcscat(wzData, L"</m:Список></m:ОбменТСД></soap:Body></soap:Envelope>");
						sendws(wzData);
						MultiByteToWideChar(CODEPAGE, 0, szData, dwBytesRead, wzData, dwBytesRead);
						//MessageBox(0,wzData,0,0);
						i = xmlgetw(0, L"return");
						if (i < 2)
						{
							xmlgetw(0, L"detail");
							MyMessageBox(hwndDlg, wzData, L"Error", 2);
							return TRUE;
						}

						//  i = xmlgetw(0,L"return"); if(i<1){if(xmlgetw(0,L"detail")<1)xmlgetw(0,L"faultstring");MyMessageBox(hwndDlg,wotvet,L"Error",2);return TRUE;}
						//KillTimer(hwndDlg,WM_USER+2);
						if (xmlgetw(i, L"<m:Код") > 0)
						{
							wcscpy(code, wotvet);
							xmlgetw(i, L"<m:Наименов");
							wcscpy(tovar, wotvet);
							if ((wcscmp(code, L"Error") == 0))
							{
								ShowError(hwndDlg, tovar);
								return TRUE;
							}
							else
								//if (wcscmp(code, L"Успех") == 0)
							{
								KillTimer(hwndDlg, WM_USER + 2);
								//ShowError(hwndDlg, tovar);
								//Sleep(1000);
								//MessageBox(0,tovar,L"Создан Документ",MB_ICONINFORMATION|MB_TOPMOST); 
								EndDialog(hwndDlg, LOWORD(wParam) == 4010 ? 0 : 130);
								return TRUE;
							}
						}
						else
						{
							if (xmlgetw(0, L"detail") < 1)
								xmlgetw(0, L"faultstring");
							ShowError(hwndDlg, wotvet);
							MyMessageBox(hwndDlg, wotvet, L"Error", 2);
						}
					}

					if (Goodslistmode == 2)	//РазместитьТовар
					{
						if (!NO_ST)
						{
							wcscpy(tovar, zagolovokokna);
							wcscpy(zagolovokokna, L"Адрес получатель:");
							KillTimer(hwndDlg, WM_USER + 2);
							KillTimer(hwndDlg, WM_USER + 1);

							if (DialogBox(g_hInstance, MAKEINTRESOURCE(nasklad), NULL, (DLGPROC)command1Proc) != 130)
							{
								wcscpy(zagolovokokna, tovar);
								SetTimer(hwndDlg, WM_USER + 2, 500, NULL);

								EnableWindow(GetDlgItem(hwndDlg, 4008), TRUE);

								EnableWindow(GetDlgItem(hwndDlg, 4010), TRUE);
								EnableWindow(GetDlgItem(hwndDlg, password), TRUE);
								break;
							}
							wcscpy(zagolovokokna, tovar);
							SetTimer(hwndDlg, WM_USER + 2, 500, NULL);

							wcscpy(vsklad, sklad);
						}
						wcscpy(wzData, L"<soap:Envelope xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\"><soap:Header/><soap:Body><m:ОбменТСД xmlns:m=\"http://www.dns-shop.tsd.ru\"><m:ВидОперации xmlns:xs=\"http://www.w3.org/2001/XMLSchema\"  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">РазместитьТовар</m:ВидОперации><m:Список xmlns:xs=\"http://www.w3.org/2001/XMLSchema\"	xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">");
						i = SendMessage(GetDlgItem(hwndDlg, list), LVM_GETITEMCOUNT, 0, 0);
						for (; i > 0;)
						{
							i--;
							//lvi.mask= LVIF_TEXT;
							lvi.pszText = kolvo;
							lvi.cchTextMax = sizeof(kolvo);
							//lvi.iItem = i;
							lvi.iSubItem = 0;	//0 обязательно
							SendMessage(GetDlgItem(hwndDlg, list), LVM_GETITEMTEXT, i, (LPARAM) & lvi);
							lvi.pszText = code;
							lvi.cchTextMax = sizeof(code);
							lvi.iSubItem = 1;
							SendMessage(GetDlgItem(hwndDlg, list), LVM_GETITEMTEXT, i, (LPARAM) & lvi);
							//if(wcslen(code)>9||wcslen(kolvo)>3)MessageBox(0,code,kolvo,0);
							wcscat(wzData, L"<m:Номенклатура><m:Код>");
							wcscat(wzData, code);
							wcscat(wzData, L"</m:Код><m:Наименование>");
							wcscat(wzData, vsklad);
							wcscat(wzData, L"</m:Наименование><m:Количество>");
							wcscat(wzData, kolvo);
							wcscat(wzData, L"</m:Количество></m:Номенклатура>");
						}
						wcscat(wzData, L"</m:Список></m:ОбменТСД></soap:Body></soap:Envelope>");

						sendws(wzData);
						MultiByteToWideChar(CODEPAGE, 0, szData, dwBytesRead, wzData, dwBytesRead);
						//MessageBox(0,wzData,0,0);
						i = xmlgetw(0, L"return");
						if (i < 2)
						{
							xmlgetw(0, L"detail");
							ShowError(hwndDlg, wotvet);
							MyMessageBox(hwndDlg, wzData, L"Error", 2);
							return TRUE;
						}
						//KillTimer(hwndDlg,WM_USER+2);
						if (xmlgetw(i, L"<m:Код") > 0)
						{
							wcscpy(code, wotvet);
							xmlgetw(i, L"<m:Наименов");
							wcscpy(tovar, wotvet);
							if ((wcscmp(code, L"Error") == 0))
							{
								ShowError(hwndDlg, tovar);
								return TRUE;
							}
							if (wcscmp(code, L"Успех") == 0)
							{
								KillTimer(hwndDlg, WM_USER + 2);
								//ShowError(hwndDlg, tovar);
								Sleep(1000);
								//MessageBox(0,tovar,L"Создан Документ",MB_ICONINFORMATION|MB_TOPMOST);
								EndDialog(hwndDlg, 130);
							}

							if (wcscmp(code, L"СледующийУспех") == 0)
							{
								KillTimer(hwndDlg, WM_USER + 2);
								EnableWindow(GetDlgItem(hwndDlg, 4008), TRUE);
								EnableWindow(GetDlgItem(hwndDlg, 4010), TRUE);
								EnableWindow(GetDlgItem(hwndDlg, password), TRUE);

								tekWnd = hwndDlg;
								//wcscpy(zagolovokokna, L"СписокСледуюший_Размещение");
								//wcscat(zagolovokokna, tovar);
								getformparse();
								return TRUE;

							}


						}
						else
							MyMessageBox(hwndDlg, wzData, L"Error", 2);
					}

					if (Goodslistmode > 2)	//списки сотрудников и др.
					{

						if (Goodslistmode == 732)
						{
							wcscpy(tovar, zagolovokokna);
							wcscpy(zagolovokokna, L"Адрес ");
							wcscat(zagolovokokna, tovar);
							KillTimer(hwndDlg, WM_USER + 2);
							KillTimer(hwndDlg, WM_USER + 1);
							if (DialogBox(g_hInstance, MAKEINTRESOURCE(nasklad), NULL, (DLGPROC)command1Proc) != 130)
							{
								wcscpy(zagolovokokna, tovar);
								SetTimer(hwndDlg, WM_USER + 2, 500, NULL);

								EnableWindow(GetDlgItem(hwndDlg, 4008), TRUE);

								EnableWindow(GetDlgItem(hwndDlg, 4010), TRUE);
								EnableWindow(GetDlgItem(hwndDlg, password), TRUE);
								break;
							}
							wcscpy(zagolovokokna, tovar);
							SetTimer(hwndDlg, WM_USER + 2, 500, NULL);

							wcscpy(tovar, sklad);
						}

//return SendListwithCommand(hwndDlg,wchar_t *command,wchar_t *operation)


						wcscpy(wzData, L"<soap:Envelope xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\"><soap:Header/><soap:Body><m:ОбменТСД xmlns:m=\"http://www.dns-shop.tsd.ru\"><m:ВидОперации xmlns:xs=\"http://www.w3.org/2001/XMLSchema\"  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">Список Сканов</m:ВидОперации><m:Список xmlns:xs=\"http://www.w3.org/2001/XMLSchema\"	xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">");
						i = SendMessage(GetDlgItem(hwndDlg, list), LVM_GETITEMCOUNT, 0, 0);
						for (; i > 0;)
						{
							i--;
							lvi.pszText = kolvo;
							lvi.cchTextMax = sizeof(kolvo);
							lvi.iSubItem = 0;	//0 обязательно
							SendMessage(GetDlgItem(hwndDlg, list), LVM_GETITEMTEXT, i, (LPARAM) & lvi);
							lvi.pszText = code;
							lvi.cchTextMax = sizeof(code);
							lvi.iSubItem = 1;
							SendMessage(GetDlgItem(hwndDlg, list), LVM_GETITEMTEXT, i, (LPARAM) & lvi);
							//lvi.pszText = tovar;
							//lvi.cchTextMax = sizeof(tovar);
							//lvi.iSubItem = 2;
							//SendMessage(GetDlgItem(hwndDlg, list), LVM_GETITEMTEXT, i, (LPARAM) & lvi);
							wcscat(wzData, L"<m:Номенклатура><m:Код>");
							wcscat(wzData, code);
							wcscat(wzData, L"</m:Код><m:Наименование>");
							wcscat(wzData, tovar);	//кнопка
							wcscat(wzData, L"</m:Наименование><m:Количество>");
							c = _wtoi(kolvo);
							wsprintf(kolvo, L"%u", c);
							wcscat(wzData, kolvo);
							wcscat(wzData, L"</m:Количество></m:Номенклатура>");
						}
						wcscat(wzData, L"</m:Список></m:ОбменТСД></soap:Body></soap:Envelope>");
						sendws(wzData);
						MultiByteToWideChar(CODEPAGE, 0, szData, dwBytesRead, wzData, dwBytesRead);
						SetTimer(hwndDlg, WM_USER + 2, 1000, NULL);
						return resultList(hwndDlg, TRUE);
					}
					return TRUE;

				case combo:
					//editc = 0;
					i = SendMessage(GetDlgItem(hwndDlg, combo), CB_GETCURSEL, 0, 0);
					if (i != CB_ERR)
					{
//                  if(HIWORD(wParam)==CBN_CLOSEUP){
						SendMessage(GetDlgItem(hwndDlg, combo), CB_GETLBTEXT, i, (LPARAM)temp);
						ShowWindow(GetDlgItem(hwndDlg, combo), SW_HIDE);
						//editc = 0;
						SetWindowText(GetDlgItem(hwndDlg, combo), L"");
						SendMessage(GetDlgItem(hwndDlg, combo), CB_SETCURSEL, -1, 0);

						mSetFocus(GetDlgItem(hwndDlg, combo));
						SetTimer(hwndDlg, WM_USER + 2, 1000, NULL);
						wcscpy(wzData, temp);
						i = xmlgetw(0, L"code");
						wcsncpy(tovar, temp, i);
						wcscpy(code, wotvet);
						chkGoods(hwndDlg, code);
					}
					return TRUE;
				case IDOK:
					if (editc == 0)
					{

						if (UseMotorolla)
							return scaningList(hwndDlg, TRUE);

						GetWindowText(GetDlgItem(hwndDlg, combo), code, 100);
						ShowWindow(GetDlgItem(hwndDlg, combo), SW_HIDE);
						SetWindowText(GetDlgItem(hwndDlg, combo), L"");
						chkGoods(hwndDlg, code);
						SetTimer(hwndDlg, WM_USER + 2, 1000, NULL);
						mSetFocus(GetDlgItem(hwndDlg, combo));
					}
					return TRUE;
				case treelist:
					SendMessage(GetDlgItem(hwndDlg, list), LVM_DELETEALLITEMS, 0, 0);
					mSetFocus(GetDlgItem(hwndDlg, combo));
					SetTimer(hwndDlg, WM_USER + 2, 1000, NULL);
					return TRUE;
				case Userlist:
					KillTimer(hwndDlg, WM_USER + 2);
					mSetFocus(GetDlgItem(hwndDlg, list));
					i = SendMessage(GetDlgItem(hwndDlg, list), LVM_GETSELECTIONMARK, 0, 0);
					editc = 1;
					SendMessage(GetDlgItem(hwndDlg, list), LVM_EDITLABEL, i, 0);

					SipDown(hwndDlg);

				//editc = SendMessage(GetDlgItem(hwndDlg,list), LVM_GETEDITCONTROL,0,0);
					return TRUE;
				case ServerName:
					if (Goodslistmode != 441)
					{
						i = SendMessage(GetDlgItem(hwndDlg, list), LVM_GETSELECTIONMARK, 0, 0);
						SendMessage(GetDlgItem(hwndDlg, list), LVM_DELETEITEM, i, 0);
						mSetFocus(GetDlgItem(hwndDlg, combo));
						SetTimer(hwndDlg, WM_USER + 2, 1000, NULL);
					}
					return TRUE;
				case Namespace:
					//if (Goodslistmode > 2)
					chkGoods(hwndDlg, tovar);
				//KillTimer(hwndDlg, WM_USER + 2);
				//EndDialog(hwndDlg, 0);
					return TRUE;
			}




			break;
		case 0x53:	//help_context

			helpme(hwndDlg);

			break;

		case WM_CLOSE:
			KillTimer(hwndDlg, WM_USER + 2);
			EndDialog(hwndDlg, 0);
			return TRUE;
	}



	return FALSE;
}
//свернуть дерево
void ExpandAll(HWND hwndDlg, _Bool Expand)
{
	HTREEITEM htil = TreeView_GetRoot(hwndDlg);
	while (htil != NULL)
	{
		TreeView_Expand(hwndDlg, htil, Expand ? TVE_EXPAND : TVE_COLLAPSE);
		htil = TreeView_GetNextItem(hwndDlg, htil, TVGN_NEXT);
	}
}


HTREEITEM findtreeitem(HWND hwndDlg, wchar_t *text, wchar_t *guid, wchar_t *modify, int operation)
{
	TVINSERTSTRUCTW tvii;
	TVITEMW tvi;
	HTREEITEM htil;
	int d, c = 0, Param = _wtoi(text);
	wchar_t gettxt[255];
	for (; c < GUIDSCOUNT; c++)
		if (uuid[c].Param == Param || Param == 0)
//      if (wcscmp(uuid[c].guid, text) == 0)
			break;
	htil = uuid[c].hti;
	if (c == GUIDSCOUNT || htil == NULL || Param == 0)
	{
		for (c = 0; c < GUIDSCOUNT; c++)
			if (wcscmp(uuid[c].guid, guid) == 0)
				break;

		if (c == GUIDSCOUNT || htil == NULL)
		{
			wcscpy(gettxt, L"not found:");
			wcscat(gettxt, guid);
			//MyMessageWait(gettxt);
			_PlaySound(TEXT("SystemCritical"), NULL, SND_ALIAS);

			return NULL;
		}
	}


	if (operation == 0)
	{	//modify
		tvi.mask = TVIF_PARAM | TVIF_TEXT | TVIF_STATE;
		tvi.pszText = gettxt;
		tvi.cchTextMax = 255;
		tvi.hItem = htil;
		TreeView_GetItem(GetDlgItem(hwndDlg, treelist), &tvi);
		tvi.pszText = modify;
		tvi.cchTextMax = wcslen(modify);
		tvi.state = TVIS_SELECTED;
		TreeView_SetItem(GetDlgItem(hwndDlg, treelist), &tvi);
		TreeView_SelectItem(GetDlgItem(hwndDlg, list), htil);
		//uuid[c].kolvo --;

	}

	if (operation == 1)
	{	//delete

		TreeView_DeleteItem(GetDlgItem(hwndDlg, treelist), htil);
		//uuid[c].kolvo = 0;
	}


	if (operation == 2)
	{	//+
		ExpandAll(GetDlgItem(hwndDlg, treelist), FALSE);
		TreeView_EnsureVisible(GetDlgItem(hwndDlg, treelist), htil);
		TreeView_Expand(GetDlgItem(hwndDlg, treelist), htil, TVE_EXPAND);

	}
	if (operation == 3)
	{	//-
		TreeView_Expand(GetDlgItem(hwndDlg, treelist), htil, TVE_COLLAPSE);
	}

	if (operation == 4)
		{
		TreeView_SelectItem(GetDlgItem(hwndDlg, treelist),htil);
		}
	return htil;
}


HTREEITEM addtreeitem(HWND hwndDlg, wchar_t *text, long int lparam, _Bool ThisIsTheEnd, HTREEITEM htil)
{
	TVINSERTSTRUCTW tvii;
	TVITEMW tvi;
	tvi.mask = TVIF_CHILDREN | TVIF_TEXT | TVIF_PARAM;
	tvi.pszText = text;
	tvi.cchTextMax = wcslen(text);
	tvi.cChildren = ThisIsTheEnd ? 0 : 1;
	tvi.lParam = (LPARAM)lparam;
	//tvi.hItem = htil;
	tvii.hParent = htil;
	tvii.item = tvi;
	tvii.hInsertAfter = TVI_LAST;
	return TreeView_InsertItem(GetDlgItem(hwndDlg, treelist), &tvii);
}


//DWORD WINAPI filTreethread()
//{
	//int i, z;
	//HWND wnd = tekWnd;
	//wchar_t code[100], tovar[500], kolvo[100];
	//sendGoodslist(L"СписокФилиалов", serverWS, serviceWS, L"5000", TRUE);
	//memset(wzData, 0, BUFSIZE * 2);
	//MultiByteToWideChar(CODEPAGE, 0, szData, strlen(szData), wzData, BUFSIZE);
	//if (xmlgetw(0, L"detail") > 1)
		//MyMessageBox(wnd, wotvet, L"Error", 3);
	//else
	//{
		//i = 0;
		//z = 3;
		//int d, c = 0;
		//while (z > 1)
		//{
			//c++;
			//z = xmlgetw(i, L"<m:Номенклатур");
			//i = z;
			//if (z < 1)
				//continue;
			//if (xmlgetw(i, L"<m:Код") <= 0)
				//continue;
			//wcscpy(uuid[c].guid, wotvet);
			//if (xmlgetw(i, L"<m:Количеств") <= 0)
				//continue;
			//d = _wtoi(wotvet);
			//if (xmlgetw(i, L"<m:Наименов") <= 0)
				//continue;
			//hti[0] = NULL;
			//hti[d + 1] = addtreeitem(wnd, wotvet, c, d == 2, hti[d]);
		//}

		//GUIDSCOUNT = c + 1;
	//}
	//ExitThread(0);
	//return 0;

//}
//в режиме кеширования данных эта функция проверяет ответ сервера
_Bool resultprocess(HWND hwndDlg, _Bool event)
{

	wchar_t tovar[256], article[100], kolvo[100];
	int z, k = 0;

	int i = xmlgetw(0, L"return");
	if (i < 1)
	{
		SoundOK(FALSE);
		xmlgetw(0, L"detail");
		return FALSE;
	}


	z = 3;
	int c = 0;
	while (z > 1)
	{
		z = xmlgetw(i, L"<m:Номенклатур");
		i = z;
		if (i < 0)
			break;
		if (xmlgetw(i, L"<m:Наименов") <= 0)
			return FALSE;
		wcscpy(tovar, wotvet);

		if (xmlgetw(i, L"<m:Количество") < 1)
			return FALSE;
		wcscpy(kolvo, wotvet);


		if (xmlgetw(i, L"<m:Код") < 1)
			return FALSE;
		if (wcscmp(wotvet, L"Ошибка") == 0 || wcscmp(wotvet, L"Error") == 0)
		{
			wcscpy(wotvet, tovar);
			return FALSE;
		}
		if (event)
			SoundOK(TRUE);

		if (wcscmp(wotvet, L"Вопрос") == 0)
		{
			i = sendOtvetVopros(hwndDlg, L"СканДереваЗадания", L"Выбор товара", tovar, kolvo, i);
			z = 3;
			continue;

		}

		if (wcscmp(wotvet, L"Транспорт") == 0)
		{
			SetWindowText(GetDlgItem(hwndDlg, transport), tovar);
			continue;
		}

		if (wcscmp(wotvet, L"Раскрыть") == 0)
		{
			wcscpy(raskrit, tovar);
			findtreeitem(hwndDlg, kolvo, tovar, tovar, 2);
			continue;
		}

		if (wcscmp(wotvet, L"Свернуть") == 0)
		{
			findtreeitem(hwndDlg, kolvo, tovar, tovar, 3);
			continue;
		}

		if (wcscmp(wotvet, L"Удалить") == 0)
		{
			findtreeitem(hwndDlg, kolvo, tovar, tovar, 1);
			continue;
		}

		if (wcscmp(wotvet, L"Обновить") == 0)
		{
			refreshtree(hwndDlg);
			return TRUE;
			continue;
		}

		if (wcscmp(wotvet, L"Филиал") == 0)
		{
			SetWindowText(hwndDlg, tovar);
			continue;
		}

		if (wcscmp(wotvet, L"Завершить") == 0)
		{
			tomainmenu = TRUE;
			KillTimer(hwndDlg, WM_USER + 2);
			EndDialog(hwndDlg, 130);
			return TRUE;
		}


		if (wcscmp(wotvet, L"Выйти") == 0)
		{
			Goodslistmode = _wtoi(kolvo);
			tomainmenu = FALSE;
			KillTimer(hwndDlg, WM_USER + 2);
			EndDialog(hwndDlg, Goodslistmode);
			return TRUE;
		}
		wcscpy(article, wotvet);
		k = _wtoi(kolvo);	//ShowError(hwndDlg,wotvet);
		if (findtreeitem(hwndDlg, kolvo, article, tovar, 0) == NULL)
		{
			refreshtree(hwndDlg);
			return TRUE;
		};

		if (TreeView_GetVisibleCount(GetDlgItem(hwndDlg, treelist)) < 1)
		{
			tomainmenu = TRUE;
			KillTimer(hwndDlg, WM_USER + 2);
			EndDialog(hwndDlg, 130);
		}
	}
	return TRUE;

}

//обновление дерева на экране по запросу на сервер
static void refreshtree(HWND hwndDlg)
{
	wchar_t adress[100];
	wchar_t temp[500], code[100], tovar[500], kolvo[200];
	DWORD dwThreadId;
	int i, z;
	RECT rect;
	LV_COLUMN lvc;
	LV_ITEM lvi;
	LVFINDINFO lvf;
	LPNMHDR jj;
	int kk, k;
	LPMSG pMsg;
	_Bool zavershenie;
	TVITEMW tvi;
	GUIDSCOUNT = 0;
	TreeView_DeleteAllItems(GetDlgItem(hwndDlg, treelist));
	MyMessageWait(L"Подождите...");
	wsprintf(kolvo, L"%d", Goodslistmode);
	sendGoodslist(L"ТоварыПоЗаданию", zagolovokokna, kolvo, L"5000", TRUE);
	memset(wzData, 0, BUFSIZE);
	MultiByteToWideChar(CODEPAGE, 0, szData, strlen(szData), wzData, dwBytesRead);
	i = 0;
	hti[1] = NULL;
	z = 3;
	int c = 0, Param = 0;
	_Bool r = FALSE, d;
	wcscpy(raskrit, L"");
	wcscpy(adress, L"");
	while (z > 1)
	{
		memset((char *)tovar, 0, 10);
//      memset((char*)code,0,sizeof(code)*2);
		//memset((char*)kolvo,0,sizeof(kolvo)*2);
		z = xmlgetw(i, L"<m:Номенклатур");
		i = z;
		if (z < 1)
			continue;
		hti[0] = NULL;
		if (xmlgetw(i, L"<m:Наименов") <= 0)
			continue;
		wcscpy(tovar, wotvet);

		if (xmlgetw(i, L"<m:Количеств") <= 0)
			continue;
		Param = _wtoi(wotvet);

		if (xmlgetw(i, L"<m:Код") <= 0)
			continue;
		if (wcscmp(wotvet, L"Завершить") == 0)
		{
			tomainmenu = TRUE;
			KillTimer(hwndDlg, WM_USER + 2);
			EndDialog(hwndDlg, 130);
			return TRUE;
		}


		if (wcscmp(wotvet, L"Выйти") == 0)
		{
			Goodslistmode = _wtoi(kolvo);
			tomainmenu = FALSE;
			KillTimer(hwndDlg, WM_USER + 2);
			EndDialog(hwndDlg, Goodslistmode);
			return TRUE;
		}

		if (wcscmp(wotvet, L"Транспорт") == 0)
		{
			SetWindowText(GetDlgItem(hwndDlg, transport), tovar);
			continue;
		}
		if (wcscmp(wotvet, L"Выбрать") == 0)
		{
		TreeView_SelectItem(GetDlgItem(hwndDlg, treelist), hti[0]);
			continue;
		}

		if (wcscmp(wotvet, L"Раскрыть") == 0)
		{


			ExpandAll(GetDlgItem(hwndDlg, treelist), FALSE);
			TreeView_EnsureVisible(GetDlgItem(hwndDlg, treelist), hti[0]);
			TreeView_Expand(GetDlgItem(hwndDlg, treelist), hti[0], TVE_EXPAND);
			TreeView_EnsureVisible(GetDlgItem(hwndDlg, treelist), hti[1]);
			TreeView_Expand(GetDlgItem(hwndDlg, treelist), hti[1], TVE_EXPAND);
			wcscpy(raskrit, adress);
			continue;
		}
		if (wcscmp(wotvet, L"Филиал") == 0)
		{
			SetWindowText(hwndDlg, tovar);
			continue;
		}

		if (wcscmp(wotvet, L"Артикул") == 0)
		{
			uuid[c].kolvo = Param;
			wcscpy(uuid[c].Article, tovar);
			continue;
		}


		d = (wcscmp(wotvet, L"Адрес") == 0);
		if (d)
		{
			wcscpy(uuid[++c].guid, tovar);
			wcscpy(adress, tovar);
		}
		else
			wcscpy(uuid[++c].guid, wotvet);

		wcscpy(wotvet, tovar);
		wcscpy(kolvo, tovar);
		//wcscpy(   uuid[c].name,tovar);
		hti[0] = addtreeitem(hwndDlg, kolvo, Param, !d, d ? NULL : hti[1]);
		uuid[c].Param = Param;
		uuid[c].hti = hti[0];

		GUIDSCOUNT = c + 1;
		wcscpy(uuid[c].Adress, adress);
		hti[1] = d ? hti[0] : hti[1];
		//if(c<3)TreeView_EnsureVisible(GetDlgItem(hwndDlg,treelist),hti[0]);
	}
//  if(wcslen(raskrit)>0)findtreeitem(hwndDlg, raskrit, tovar, tovar, 2);
	if (z < 1)
		return;
	tekpos = z;
	tekWnd = hwndDlg;
	//  CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)fillcbthread,NULL,0,&dwThreadId);

}



_Bool scanTreeQuest(HWND hwndDlg, wchar_t *code, int kol)
{
	wchar_t tovar[200], article[100], kolvo[100];
	int z, k = 0;
	wcscpy(wotvet, L"Введен пустой штрихкод");
	if (wcslen(code) < 3)
		return TRUE;

	//if (cachemode && kol == 1)
	//return FindAndScan(hwndDlg, code);


	wsprintf(kolvo, L"%d", kol);


	sendGoodslist(L"СканДереваЗадания", code, code, kolvo, FALSE);
	MultiByteToWideChar(CODEPAGE, 0, szData, dwBytesRead, wzData, dwBytesRead);


	return resultprocess(hwndDlg, TRUE);
}

void findTreeItem(HWND hwndDlg)
{
	wchar_t code[100];
	LV_ITEM lvi;
	int c;
	if (Goodslistmode == 442)
	{
		GetWindowText(GetDlgItem(hwndDlg, combo), code, 100);

		if (wcslen(code) > 0)
		{
			int c = 0;
			for (; c < GUIDSCOUNT; c++)
				if (wcsncmp(code, uuid[c].Article, wcslen(code)) == 0 || wcsncmp(code, uuid[c].guid, wcslen(code)) == 0)
					break;
			if (uuid[c].hti != NULL && c < GUIDSCOUNT)
			{
				TreeView_SelectItem(GetDlgItem(hwndDlg, treelist), uuid[c].hti);
				TreeView_EnsureVisible(GetDlgItem(hwndDlg, treelist), uuid[c].hti);
			}
		}
	}


}

static LRESULT CALLBACK TreeGoodsProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	wchar_t temp[500], code[100], tovar[500], kolvo[200];
	int i, z;
	RECT rect;
	LV_COLUMN lvc;
	LV_ITEM lvi;
	LVFINDINFO lvf;
	LPNMHDR jj;
	int kk, k;
	LPMSG pMsg;
	_Bool zavershenie;
	_Bool SearchForm;
	TVITEMW tvi;
	HTREEITEM htil;






	switch (uMsg)
	{
		case WM_INITDIALOG:
		{
			SearchForm = (wcsncmp(zagolovokokna + wcslen(zagolovokokna) - 5, L"Поиск", 5) == 0);
			GUIDSCOUNT = 0;
			//memset((char*)uuid,0, GUIDSIZE * sizeof(struct guids));

			OpenClipboard(hwndDlg);
			EmptyClipboard();
			CloseClipboard();
			SHINITDLGINFO shidi;
			shidi.dwMask = SHIDIM_FLAGS;
			shidi.dwFlags = SHIDIF_SIPDOWN | SHIDIF_SIZEDLGFULLSCREEN;
			shidi.hDlg = hwndDlg;
			//  SHInitDialog(&shidi);
			int x = GetSystemMetrics(SM_CXSCREEN);
			int y = GetSystemMetrics(SM_CYSCREEN);
			ShowWindow(GetDlgItem(hwndDlg, combo), Goodslistmode == 442 ? SW_SHOW : SW_HIDE);
			SetWindowPos(hwndDlg, hwndDlg, 0, 0, x, y, SWP_NOZORDER);
			GetWindowRect(hwndDlg, &rect);
			x = rect.right;
			y = rect.bottom;
			int nl = 25 + 55 + 40, niz = 25 + 25;

			SetWindowPos(GetDlgItem(hwndDlg, treelist), GetDlgItem(hwndDlg, treelist), 0, 0, x, y - nl, SWP_NOZORDER);
			SetWindowPos(GetDlgItem(hwndDlg, 4011), GetDlgItem(hwndDlg, 4011), 0, y - nl, x, 50, SWP_NOZORDER);
			SetWindowPos(GetDlgItem(hwndDlg, 4012), GetDlgItem(hwndDlg, 4012), 3, y - niz, 20, 20, SWP_NOZORDER);
			SetWindowPos(GetDlgItem(hwndDlg, transport), GetDlgItem(hwndDlg, transport), 20, y - niz, x - 45, 20, SWP_NOZORDER);
			SetWindowPos(GetDlgItem(hwndDlg, zakonchit), GetDlgItem(hwndDlg, zakonchit), x - 23, y - niz, 20, 20, SWP_NOZORDER);
			fontialog(GetDlgItem(hwndDlg, 4011), 6, 16, 800, 0);
			refreshtree(hwndDlg);
			mSetFocus(GetDlgItem(hwndDlg, combo));
			if (UseMotorolla)
				mSetFocus(GetDlgItem(hwndDlg, treelist));
			SetTimer(hwndDlg, WM_USER + 2, 1000, NULL);
			sx = x;
			sy = y;
			tekWnd = hwndDlg;
			getform1c();
			return TRUE;
		}
		case 0x80b4:
			getscanline(hwndDlg, code);
			if (wcslen(code) > 3)
				if (!scanTreeQuest(hwndDlg, code, 1))
					ShowError(hwndDlg, wotvet);

		case WM_TIMER:
			if (wParam == WM_USER + 2)
			{
				findTreeItem(hwndDlg);	//    mSetFocus(GetDlgItem(hwndDlg, treelist));
				if (!UseMotorolla)
					mSetFocus(GetDlgItem(hwndDlg, combo));
				SendMessage(GetDlgItem(hwndDlg, combo), EM_SETSEL, -1, 0);
			}
			return TRUE;

		case WM_GETDLGCODE:
			pMsg = (LPMSG) (lParam);
			switch (pMsg->lParam)
			{
				case WM_KEYUP:




					break;
				case WM_KEYDOWN:
					if (LOWORD(pMsg->wParam) == VK_RIGHT)
					{
						TreeView_Expand(GetDlgItem(hwndDlg, treelist), TreeView_GetSelection(GetDlgItem(hwndDlg, treelist)), TVE_EXPAND);
					}

					if (LOWORD(pMsg->wParam) == VK_LEFT)
						TreeView_Expand(GetDlgItem(hwndDlg, treelist), TreeView_GetSelection(GetDlgItem(hwndDlg, treelist)), TVE_COLLAPSE);

					if (LOWORD(pMsg->wParam) == VK_UP)
					{
						mSetFocus(GetDlgItem(hwndDlg, treelist));
						//TreeView_SelectItem(GetDlgItem(hwndDlg, treelist), TreeView_GetPrevVisible(GetDlgItem(hwndDlg, treelist), TreeView_GetSelection(GetDlgItem(hwndDlg, treelist))));
					}
					if (LOWORD(pMsg->wParam) == VK_DOWN)
					{
						mSetFocus(GetDlgItem(hwndDlg, treelist));
						//TreeView_SelectItem(GetDlgItem(hwndDlg, treelist), TreeView_GetNextVisible(GetDlgItem(hwndDlg, treelist), TreeView_GetSelection(GetDlgItem(hwndDlg, treelist))));
					}


					if (LOWORD(pMsg->wParam) == VK_RETURN)
					{
						if (UseMotorolla)
						{
							getscanline(hwndDlg, code);
							if (wcslen(code) > 3)
							{
								if (!scanTreeQuest(hwndDlg, code, 1))
									ShowError(hwndDlg, wotvet);
								break;
							}
						}

						GetWindowText(GetDlgItem(hwndDlg, combo), code, 100);
						if (wcslen(code) < 3 && (Goodslistmode == 442 || Goodslistmode == 435 || Goodslistmode == 441))
						{

							tvi.mask = TVIF_HANDLE | TVIF_PARAM | TVIF_CHILDREN;
							tvi.hItem = TreeView_GetSelection(GetDlgItem(hwndDlg, treelist));
							KillTimer(hwndDlg, WM_USER + 1);
							KillTimer(hwndDlg, WM_USER + 2);
							int k = 1;
							//if (Goodslistmode != 330 && Goodslistmode != 435 && Goodslistmode != 441)
							//{
							//k = DialogBox(g_hInstance, MAKEINTRESOURCE(1013), hwndDlg, (DLGPROC)VvodKolvo);
							//if (k == 0)
							//return TRUE;
							//}
							//else
							//{
							//k = 1;
							//}
							int c = 0;
							mSetFocus(GetDlgItem(hwndDlg, combo));
							SetTimer(hwndDlg, WM_USER + 2, 1000, NULL);
							for (; c < GUIDSCOUNT; c++)
								if (uuid[c].hti == tvi.hItem)
									break;
							//ShowError(hwndDlg,uuid[c].guid);
							if (!scanTreeQuest(hwndDlg, uuid[c].guid, k))
								ShowError(hwndDlg, wotvet);
							return TRUE;


						}



						ShowWindow(GetDlgItem(hwndDlg, combo), Goodslistmode == 442 ? SW_SHOW : SW_HIDE);
						SetWindowText(GetDlgItem(hwndDlg, combo), L"");
						if (!scanTreeQuest(hwndDlg, code, 1))
							ShowError(hwndDlg, wotvet);
					}
					return TRUE;
			}
			if (!UseMotorolla)
				mSetFocus(GetDlgItem(hwndDlg, combo));
			SetTimer(hwndDlg, WM_USER + 2, 1000, NULL);

			SetWindowLong(hwndDlg, DWL_MSGRESULT, DLGC_WANTALLKEYS | DLGC_WANTCHARS);
			return DLGC_WANTARROWS | DLGC_WANTALLKEYS | DLGC_WANTCHARS;

		case WM_NOTIFY:

			if (UseMotorolla)
			{
				//mSetFocus(GetDlgItem(hwndDlg, treelist));
				if (GetAsyncKeyState(39) != 0)
				{
					TreeView_Expand(GetDlgItem(hwndDlg, treelist), TreeView_GetSelection(GetDlgItem(hwndDlg, treelist)), TVE_EXPAND);
				}

				if (GetAsyncKeyState(37) != 0)
				{
					TreeView_Expand(GetDlgItem(hwndDlg, treelist), TreeView_GetSelection(GetDlgItem(hwndDlg, treelist)), TVE_COLLAPSE);

				}

				if (GetAsyncKeyState(119) != 0)
				{

					tvi.mask = TVIF_HANDLE | TVIF_PARAM | TVIF_CHILDREN;
					tvi.hItem = TreeView_GetSelection(GetDlgItem(hwndDlg, treelist));
					KillTimer(hwndDlg, WM_USER + 1);
					KillTimer(hwndDlg, WM_USER + 2);
					int k = 0;
					if (Goodslistmode != 635 && Goodslistmode != 442 && Goodslistmode != 330 && Goodslistmode != 435)
					{
						k = DialogBox(g_hInstance, MAKEINTRESOURCE(1013), hwndDlg, (DLGPROC)VvodKolvo);
						if (k == 0)
							return TRUE;
					}
					else
					{
						k = 1;
					}
					int c = 0;
					for (; c < GUIDSCOUNT; c++)
						if (uuid[c].hti == tvi.hItem)
							break;
					//ShowError(hwndDlg,uuid[c].guid);
					if (!scanTreeQuest(hwndDlg, uuid[c].guid, k))
						ShowError(hwndDlg, wotvet);
				}

			}

			jj = (LPNMHDR)lParam;
		//tvi.mask = TVIF_HANDLE;
		//TreeView_GetItem(GetDlgItem(hwndDlg,treelist),&tvi);
			tvi.mask = TVIF_PARAM | TVIF_TEXT;
			wcscpy(temp, L"");
			tvi.pszText = temp;
			tvi.cchTextMax = 500;
			tvi.hItem = TreeView_GetSelection(GetDlgItem(hwndDlg, treelist));

			TreeView_GetItem(GetDlgItem(hwndDlg, treelist), &tvi);
			if (wcslen(tvi.pszText) > 10)
			{
				SetWindowText(GetDlgItem(hwndDlg, 4011), tvi.pszText);

				//hint(hwndDlg,100,tvi.pszText);
			}

			if (LOWORD(wParam) == treelist)
			{
				if (jj->code == NM_DBLCLK || jj->code == 0x3e8)
				{

					tvi.mask = TVIF_HANDLE | TVIF_PARAM | TVIF_CHILDREN;
					tvi.hItem = TreeView_GetSelection(GetDlgItem(hwndDlg, treelist));
					//TreeView_GetItem(GetDlgItem(hwndDlg,treelist),&tvi);
					KillTimer(hwndDlg, WM_USER + 1);
					KillTimer(hwndDlg, WM_USER + 2);
					int k = 0;
					if (Goodslistmode != 635 && Goodslistmode != 442 && Goodslistmode != 330 && Goodslistmode != 435)
					{
						k = DialogBox(g_hInstance, MAKEINTRESOURCE(1013), hwndDlg, (DLGPROC)VvodKolvo);
						if (k == 0)
							return TRUE;
					}
					else
					{
						k = 1;
					}
					int c = 0;
					if (!UseMotorolla)
						mSetFocus(GetDlgItem(hwndDlg, combo));
					SetTimer(hwndDlg, WM_USER + 2, 1000, NULL);
					for (; c < GUIDSCOUNT; c++)
						if (uuid[c].hti == tvi.hItem)
							break;
					//ShowError(hwndDlg,uuid[c].guid);
					if (!scanTreeQuest(hwndDlg, uuid[c].guid, k))
						ShowError(hwndDlg, wotvet);
				}
			}
			return TRUE;


		case WM_COMMAND:

			wsprintf(kolvo, L"%u", GET_WM_COMMAND_ID(wParam, lParam));
			GetWindowText(GetDlgItem(hwndDlg, GET_WM_COMMAND_ID(wParam, lParam)), tovar, 100);
		//if ((wcscmp(tovar, L"Заполнить все") == 0)||(wcscmp(tovar, L"Убрать все") == 0)||(wcscmp(tovar, L"Выбрать все") == 0)||(wcscmp(tovar, L"Удалить все") == 0)||(wcscmp(tovar, L"Набрать все") == 0)||(wcscmp(tovar, L"Отметить все") == 0))
		//{
		//tekWnd = hwndDlg;
		//wcscpy(zagolovokokna, L"СписокДЯ_");
		//wcscat(zagolovokokna, tovar);
		//getform1c();
		//return TRUE;
		//}
			switch (GET_WM_COMMAND_ID(wParam, lParam))
			{
				case 4501:
					KillTimer(hwndDlg, WM_USER + 2);
				//Goodslistmode = 333;

					tvi.mask = TVIF_HANDLE | TVIF_PARAM | TVIF_CHILDREN;
					tvi.hItem = TreeView_GetSelection(GetDlgItem(hwndDlg, treelist));
					int c = 0;
					for (; c < GUIDSCOUNT; c++)
						if (uuid[c].hti == tvi.hItem)
							break;
					if (wcslen(uuid[c].guid) < 3)
						return TRUE;
					if (!scanTreeQuest(hwndDlg, uuid[c].guid, Goodslistmode))
						ShowError(hwndDlg, wotvet);
					//tomainmenu = FALSE;
					//EndDialog(hwndDlg, 333);
					return TRUE;

				case 4013:
					//GetWindowText(GetDlgItem(hwndDlg, 4013), code, 100);
					if (!scanTreeQuest(hwndDlg, tovar, 0))
						ShowError(hwndDlg, wotvet);
					return TRUE;

				case IDOK:


					if (UseMotorolla)
					{
						getscanline(hwndDlg, code);
						if (wcslen(code) > 3)
						{
							if (!scanTreeQuest(hwndDlg, code, 1))
								ShowError(hwndDlg, wotvet);
							break;
						}
					}

					GetWindowText(GetDlgItem(hwndDlg, combo), code, 100);
					if (!scanTreeQuest(hwndDlg, code, 0))
						ShowError(hwndDlg, wotvet);
					SetWindowText(GetDlgItem(hwndDlg, combo), L"");
					return TRUE;

				case zakonchit:
					tomainmenu = TRUE;

					KillTimer(hwndDlg, WM_USER + 2);
					EndDialog(hwndDlg, 0);
					return TRUE;

			}
			break;
		case 0x53:	//help_context

			helpme(hwndDlg);

			break;

		case WM_CLOSE:
			EndDialog(hwndDlg, 0);
			return TRUE;
	}
	if (tomainmenu)
	{
		KillTimer(hwndDlg, WM_USER + 2);
		EndDialog(hwndDlg, 0);
	}

	//wsprintf(temp,L"%x",uMsg);if(uMsg < 0x300 && uMsg > 0x200 )MessageBox(hwndDlg,temp,0,0);

	return FALSE;
}



DWORD WINAPI loadUserList()
{
	HWND hwndDlg = tekWnd;
	int z, i;
	wcscpy(UserWS, L"WebConnection");
	wcscpy(PswWS, L"951");
	CloseConnection();
	sendGoodslist(L"СписокПользователей", serverWS, serverWS, L"500", FALSE);
	CloseConnection();
	MultiByteToWideChar(CODEPAGE, 0, szData, -1, wzData, BUFSIZE);
	if (dwBytesRead < 1)
		SetWindowText(GetDlgItem(hwndDlg, 4011), L"Ошибка доступа к серверу 1C");
	else if (xmlgetw(0, L"return") < 2)
	{

		ShowError(hwndDlg, L"Произошёл сбой. Необходимо перезапустить apache, обратитесь в отдел ИТ");
		SetWindowText(GetDlgItem(hwndDlg, 4011), L"Произошёл сбой. Необходимо перезапустить apache, обратитесь в отдел ИТ");
	}
	else if (xmlgetw(0, L"detail") > 1)
	{
		ShowError(hwndDlg, wotvet);

		SetWindowText(GetDlgItem(hwndDlg, 4011), L"Произошёл сбой. Необходимо перезапустить apache, обратитесь в отдел ИТ");
	}
	else
	{
		z = 3;
		int c = 0;
		i = xmlgetw(0, L"return");

		while (z > 1)
		{
			if (tomainmenu)
				break;
			c++;
			z = xmlgetw(i, L"<m:Номенклатур");
			if (z < 1)
				continue;
			i = z + 1;
			if (xmlgetw(i, L"<m:Наименов") < 1)
				continue;
			SendMessage(GetDlgItem(hwndDlg, Userlist), CB_ADDSTRING, 0, (LPARAM)wotvet);
		}
	}
	loadconfigxml();
	ExitThread(0);
	return 0;


}


static LRESULT CALLBACK Autorization(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	wchar_t temp[TempSize];
	DWORD dwThreadId;
	int i, z;
	RECT rect;
	SHINITDLGINFO shidi;


	if (UseMotorolla && (GetTickCount() != lastoperation))
	{

//(...) screen keys 
		int key = 117;
		if (GetAsyncKeyState(key) != 0)
		{
			if (Pressed[key] == 0)
			{
				lastoperation = GetTickCount();
				Pressed[key] = 1;
				DialogBox(g_hInstance, MAKEINTRESOURCE(options), NULL, (DLGPROC)OptionsProc);
				//SendMessage(GetDlgItem(hwndDlg, 4001), BM_CLICK, 0, 0);

			}
		}
		else
			Pressed[key] = 0;

//(...) screen keys 
		key = 118;
		if (GetAsyncKeyState(key) != 0)
		{
			if (Pressed[key] == 0)
			{
				lastoperation = GetTickCount();
				Pressed[key] = 1;
				EndDialog(hwndDlg, 333);
				//SendMessage(GetDlgItem(hwndDlg, 4005), BM_CLICK, 0, 0);

			}
		}
		else
			Pressed[key] = 0;


//green key
		key = 119;
		if (GetAsyncKeyState(key) != 0)
		{
			if (Pressed[key] == 0)
			{
				lastoperation = GetTickCount();
				Pressed[key] = 1;

				SendMessage(GetDlgItem(hwndDlg, IDOK), BM_CLICK, 0, 0);

			}
		}
		else
			Pressed[key] = 0;
	}

	switch (uMsg)
	{
		case WM_INITDIALOG:	//Функция ПриОткрытии
		{
			memset(&shidi, 0, sizeof(shidi));
			OpenClipboard(hwndDlg);
			EmptyClipboard();
			CloseClipboard();
			SetWindowText(GetDlgItem(hwndDlg, VersionNumber), VERSION);

			shidi.dwMask = SHIDIM_FLAGS;
			shidi.dwFlags = SHIDIF_SIPDOWN | SHIDIF_SIZEDLGFULLSCREEN;
			shidi.hDlg = hwndDlg;
			SHInitDialog(&shidi);
			SHMENUBARINFO mbi;
			memset(&mbi, 0, sizeof(mbi));
			mbi.cbSize = sizeof(mbi);
			mbi.hwndParent = hwndDlg;
			mbi.nToolBarId = popupmenu;
			mbi.hInstRes = g_hInstance;
			if (!SHCreateMenuBar(&mbi))
				return FALSE;
			//  autoscaledialog(hwndDlg,FALSE);
			int x = GetSystemMetrics(SM_CXSCREEN);
			int y = GetSystemMetrics(SM_CYSCREEN) - 20;
			GetWindowRect(GetDesktopWindow(), &rect);
			SetWindowPos(hwndDlg, hwndDlg, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) - 20, SWP_NOZORDER);
			SetWindowPos(GetDlgItem(hwndDlg, Userlist), GetDlgItem(hwndDlg, Userlist), 1, 20, x - 6, y - 20, SWP_NOZORDER);
			SetWindowPos(GetDlgItem(hwndDlg, password), GetDlgItem(hwndDlg, password), 1, y / 2 - 30, x - 6, 20, SWP_NOZORDER);
			SetWindowPos(GetDlgItem(hwndDlg, IDOK), GetDlgItem(hwndDlg, IDOK), (x / 2) - 40, y / 2, 80, 50, SWP_NOZORDER);
			SetWindowText(GetDlgItem(hwndDlg, Userlist), UserWS);
			char port[200];
			WideCharToMultiByte(CODEPAGE, 0, PortWS, -1, port, BUFSIZE, 0, 0);
			prt = atoi(port);
			if (prt == 0)
				prt = 52081;
			SetWindowLong(hwndDlg, DWL_MSGRESULT, DLGC_WANTALLKEYS | DLGC_WANTCHARS);
			SetTimer(hwndDlg, WM_USER + 334, 33, NULL);
			memset(wzData, 0, BUFSIZE);
			memset(szData, 0, BUFSIZE);
			tekWnd = hwndDlg;
			CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)loadUserList, NULL, 0, &dwThreadId);	//запускаем поток получения пользователей 1с
			mSetFocus(GetDlgItem(hwndDlg, password));
			return TRUE;
		}

		case WM_SIZE:
			mSetFocus(GetDlgItem(hwndDlg, password));
			return TRUE;


		case WM_TIMER:

			return TRUE;


		case WM_COMMAND:
			switch (GET_WM_COMMAND_ID(wParam, lParam))
			{

				case Userlist:
					//   i = SendMessage(GetDlgItem(hwndDlg,Userlist),CB_GETCURSEL, 0,0);
//                  if(i!=CB_ERR)SetFocus(GetDlgItem(hwndDlg,password));
					return TRUE;
				case 4501:
					//HideError(hwndDlg);
					return TRUE;

				case IDOK:
					localip();
					GetWindowText(GetDlgItem(hwndDlg, Userlist), temp, 500);
					wcscpy(UserWS, temp);
					GetWindowText(GetDlgItem(hwndDlg, password), temp, 500);
					wcscpy(PswWS, temp);
					strcpy(otvet, "Logon error");
					SetWindowText(GetDlgItem(hwndDlg, 4011), L"Авторизация...");
					char port[200];
					WideCharToMultiByte(CODEPAGE, 0, PortWS, -1, port, 200, 0, 0);
					prt = atoi(port);
					if (prt == 0)
						prt = 52081;
					CloseConnection();
				//wcscat(ipadr,);
					sndxdto(ipadr, L"Авторизация");
					xmlvalue(0, "return");
					if (dwBytesRead < 2)//||(xmlgetw(3, L"<m:Наименов") < 1))
					{
						SetWindowText(GetDlgItem(hwndDlg, 4011), L"Ошибка доступа к сети");
						if (MessageBox(hwndDlg, L"Произошла ошибка доступа к сети\nрекоммендуется выйти и запустить программу снова\nВыйти из программы ?", L"Ошибка сети", MB_YESNO | MB_ICONQUESTION) == IDYES)
							EndDialog(hwndDlg, 333);
						return TRUE;
					}
					wcscpy(temp, wotvet);
//					MultiByteToWideChar(CODEPAGE, 0, otvet, -1, temp, sizeof(temp));
					if (wcscmp(temp, L"Успех") == 0)
					{
						//MessageBox(hwndDlg,UserWS,PswWS,0);
						SaveParam();
						EndDialog(hwndDlg, 130);
					}
					else
						strcpy(otvet, "Logon error");
					if (xmlvalue(0, "detail") > 1 || xmlvalue(0, "faultstring") > 1)
					{
						MultiByteToWideChar(CODEPAGE, 0, otvet, -1, wotvet, sizeof(wotvet));

						GetWindowRect(GetDesktopWindow(), &rect);
						DrawText(GetDC(0), wotvet, wcslen(wotvet), &rect, DT_WORDBREAK);
						strcpy(otvet, "Необходимо перезапустить сервис apache, обратитесь в отдел ИТ");
					}
					else
						strcpy(otvet, "Logon error");
					MultiByteToWideChar(CODEPAGE, 0, otvet, -1, temp, sizeof(temp));
					SetWindowText(GetDlgItem(hwndDlg, 4011), temp);
					return TRUE;

				case menuitem1:
					DialogBox(g_hInstance, MAKEINTRESOURCE(options), NULL, (DLGPROC)OptionsProc);

					return TRUE;


			}
			break;

		case WM_CLOSE:
			//  KillTimer(hwndDlg,WM_USER+1);
			EndDialog(hwndDlg, 333);
			return TRUE;
	}
	//SetForegroundWindow(hwndDlg);
	return FALSE;
}

static LRESULT CALLBACK OptionsProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	wchar_t temp[TempSize];
	//char answer[BUFSIZE],srch[500];
	int i;
	RECT rect;
	switch (uMsg)
	{
		case WM_INITDIALOG:
		{
			SHINITDLGINFO shidi;
			OpenClipboard(hwndDlg);
			EmptyClipboard();
			CloseClipboard();
			SetFocus(GetDlgItem(hwndDlg, 4001));
			shidi.dwMask = SHIDIM_FLAGS;
			shidi.dwFlags = SHIDIF_SIPDOWN | SHIDIF_SIZEDLGFULLSCREEN;
			shidi.hDlg = hwndDlg;
			SHInitDialog(&shidi);
			int x = GetSystemMetrics(SM_CXSCREEN);
			int y = GetSystemMetrics(SM_CYSCREEN);
			GetWindowRect(GetDesktopWindow(), &rect);
			SetWindowPos(hwndDlg, hwndDlg, 0, 0, x, y, SWP_NOZORDER);
			SetWindowText(GetDlgItem(hwndDlg, ServerName), serverWS);
			SetWindowText(GetDlgItem(hwndDlg, PortEdit), PortWS);
			SetWindowText(GetDlgItem(hwndDlg, Namespace), serviceWS);
			CheckDlgButton(hwndDlg, 4013, cachemode ? BST_CHECKED : BST_UNCHECKED);
			CheckDlgButton(hwndDlg, 4014, ReconnectAllways ? BST_CHECKED : BST_UNCHECKED);
			return TRUE;
		}

		case WM_SIZE:
			SetFocus(GetDlgItem(hwndDlg, 4001));
			return TRUE;


		case WM_TIMER:

			return TRUE;


		case WM_COMMAND:
			switch (GET_WM_COMMAND_ID(wParam, lParam))
			{
				case 4501:
					/////HideError(hwndDlg);
					return TRUE;
				case 4012:
					debugmode = IsDlgButtonChecked(hwndDlg, 4011) == BST_CHECKED;
					GetWindowText(GetDlgItem(hwndDlg, PortEdit), temp, 500);
					wcscpy(PortWS, temp);
					char port[200];
					WideCharToMultiByte(CODEPAGE, 0, PortWS, -1, port, 200, 0, 0);
					prt = atoi(port);
					if (prt == 0)
						prt = 52081;
					GetWindowText(GetDlgItem(hwndDlg, Namespace), temp, 500);
					wcscpy(serviceWS, temp);
				//if (Scan(hwndDlg))
				//SetWindowText(GetDlgItem(hwndDlg, ServerName), serverWS);
				//else
				//{
				//GetWindowText(GetDlgItem(hwndDlg, ServerName), temp, 500);
				//wcscpy(serverWS, temp);
				//}
					return TRUE;

				case 4009:
					GetWindowText(GetDlgItem(hwndDlg, ServerName), temp, 500);
					wcscpy(serverWS, temp);
					GetWindowText(GetDlgItem(hwndDlg, PortEdit), temp, 500);
					wcscpy(PortWS, temp);
					GetWindowText(GetDlgItem(hwndDlg, Namespace), temp, 500);
					wcscpy(serviceWS, temp);
					debugmode = IsDlgButtonChecked(hwndDlg, 4011) == BST_CHECKED;
					cachemode = IsDlgButtonChecked(hwndDlg, 4013) == BST_CHECKED;
					ReconnectAllways = IsDlgButtonChecked(hwndDlg, 4014) == BST_CHECKED;
					SaveParam();
					EndDialog(hwndDlg, 130);
					return TRUE;
				case 4010:
					EndDialog(hwndDlg, 0);
					return TRUE;


			}
			break;

		case WM_CLOSE:
			EndDialog(hwndDlg, 0);
			return TRUE;
	}
	return FALSE;
}


void getinfo(HWND hwndDlg, wchar_t *code)
{
	//SetWindowText(GetDlgItem(hwndDlg, combo),code);
	sendGoodslist(L"Информация", code, code, L"0", TRUE);
	MultiByteToWideChar(CODEPAGE, 0, szData, dwBytesRead, wzData, dwBytesRead);


	int i = xmlgetw(0, L"return");
	if (i < 2)
	{
		xmlgetw(0, L"detail");
		SetWindowText(GetDlgItem(hwndDlg, list), wotvet);
		ShowError(hwndDlg, wotvet);
		return;
	}
	SoundOK(TRUE);
	//SetFocus(GetDlgItem(hwndDlg, combo));
	//  SendMessage(GetDlgItem(hwndDlg, combo), EM_SETSEL,0,-1);
	xmlgetw(i, L"<m:Код");
	if (wcscmp(wotvet, L"Информация") != 0)
	{
		tekWnd = hwndDlg;
		getformparse();
		return;
	}
	xmlgetw(i, L"<m:Наименов");
	SetWindowText(GetDlgItem(hwndDlg, list), wotvet);
}



static LRESULT CALLBACK InfoProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	wchar_t temp[500], kolvo[200], code[200];
	char barcode[200];
	int x, y, d, i, z;
	RECT rect;
	LPMSG pMsg;



	switch (uMsg)
	{
		case WM_INITDIALOG:
			OpenClipboard(hwndDlg);
			EmptyClipboard();
			CloseClipboard();
			SetWindowLong(hwndDlg, GWL_STYLE, GetWindowLong(hwndDlg, GWL_STYLE) | WS_CAPTION);
			SetWindowPos(hwndDlg, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_DRAWFRAME);
			SetWindowText(hwndDlg, L"Поиск товара");
			GetWindowRect(GetDesktopWindow(), &rect);
			x = rect.right;
			y = rect.bottom;
			x = GetSystemMetrics(SM_CXSCREEN);
			y = GetSystemMetrics(SM_CYSCREEN);
			SetWindowPos(hwndDlg, hwndDlg, 0, 0, x, y, SWP_NOZORDER);
			SetWindowPos(GetDlgItem(hwndDlg, combo), GetDlgItem(hwndDlg, combo), 0, 0, x, 25, SWP_NOZORDER);
			SetWindowPos(GetDlgItem(hwndDlg, 4016), GetDlgItem(hwndDlg, 4016), 0, 0, x, 25, SWP_NOZORDER);
			SetWindowPos(GetDlgItem(hwndDlg, 4017), GetDlgItem(hwndDlg, 4017), 0, 27, x, 27, SWP_NOZORDER);	//микроинструкция
			SetWindowPos(GetDlgItem(hwndDlg, list), GetDlgItem(hwndDlg, list), 0, 27 + 30, x, y / 2 + 33, SWP_NOZORDER);
			SetWindowPos(GetDlgItem(hwndDlg, zakonchit), GetDlgItem(hwndDlg, zakonchit), 0, y - (20 + 20 + 27), x / 3 - 5, 27, SWP_NOZORDER);
			SetWindowPos(GetDlgItem(hwndDlg, 4011), GetDlgItem(hwndDlg, 4011), x / 3 - 5, y - (20 + 20 + 27), x / 3 + 5, 27, SWP_NOZORDER);
			SetWindowPos(GetDlgItem(hwndDlg, 4013), GetDlgItem(hwndDlg, 4013), 1 + 2 * x / 3, y - (20 + 20 + 27), x / 3 + 3, 27, SWP_NOZORDER);

			autofontialog(hwndDlg, 6, 16);
			SetTimer(hwndDlg, WM_USER + 2, 1000, NULL);
			SetForegroundWindow(GetDlgItem(hwndDlg, combo));

			if (wcscmp(zagolovokokna, L"ОсновноеОкно") != 0)
			{
				tekWnd = hwndDlg;
				getform1c();
			}

			return TRUE;



		case WM_GETDLGCODE:
			pMsg = (LPMSG) (lParam);

			switch (pMsg->lParam)
			{

				case WM_KEYDOWN:
					if (LOWORD(pMsg->wParam) == VK_RETURN)
					{
						if (UseMotorolla)
						{
							getscanline(hwndDlg, code);
							if (wcslen(code) > 3)
							{
								getinfo(hwndDlg, code);
								break;
							}
						}


						GetWindowText(GetDlgItem(hwndDlg, combo), code, 100);
						//SetWindowText(GetDlgItem(hwndDlg, combo), L"");
						if (wcslen(code) < 4)
							break;
						getinfo(hwndDlg, code);
						//if(UseMotorolla){
						//SetForegroundWindow(GetDlgItem(hwndDlg, combo));
						//SetFocus(GetDlgItem(hwndDlg, combo));}
						//SendMessage(GetDlgItem(hwndDlg, combo), EM_SETSEL, -1, 0);

					}
					break;
				//case 1:
				//if((LOWORD(pMsg->wParam) == VK_RETURN)||(LOWORD(pMsg->wParam) == VK_RETURN))


				//break;

			}

			SetWindowLong(hwndDlg, DWL_MSGRESULT, DLGC_WANTALLKEYS | DLGC_WANTCHARS);

			return DLGC_WANTARROWS | DLGC_WANTALLKEYS | DLGC_WANTCHARS;

		case WM_TIMER:
			if (wParam == WM_USER + 2)
			{
				//mSetFocus(GetDlgItem(hwndDlg, combo));    //перевели фокус ввода в поле ввода  combo на форме
				//SendMessage(GetDlgItem(hwndDlg, combo), EM_SETSEL, -1, 0);    // переводим курсор в конец текста введенного в поле ввода
			}
			return TRUE;

		case WM_COMMAND:
			GetWindowText(GetDlgItem(hwndDlg, GET_WM_COMMAND_ID(wParam, lParam)), sklad, 100);
			switch (GET_WM_COMMAND_ID(wParam, lParam))
			{

				case zakonchit:	//подтвердить список
					KillTimer(hwndDlg, WM_USER + 2);
					EndDialog(hwndDlg, 130);
					return TRUE;

				case IDOK:
					if (UseMotorolla)
					{
						getscanline(hwndDlg, code);
						if (wcslen(code) > 3)
						{
							getinfo(hwndDlg, code);
							break;
						}
					}
					GetWindowText(GetDlgItem(hwndDlg, combo), code, 100);

				//SetWindowText(GetDlgItem(hwndDlg, combo), L"");
				//SetFocus(GetDlgItem(hwndDlg, combo));
					if (wcslen(code) < 4)
						break;
					getinfo(hwndDlg, code);
				//SetFocus(GetDlgItem(hwndDlg, combo));
				//SendMessage(GetDlgItem(hwndDlg, combo), EM_SETSEL, 0, -1);
					return TRUE;
				case 4011:
					KillTimer(hwndDlg, WM_USER + 2);
					EndDialog(hwndDlg, combo);

					return TRUE;

				case 4013:	// размещение товара с транзита
					KillTimer(hwndDlg, WM_USER + 2);
					EndDialog(hwndDlg, IDCANCEL);

					return TRUE;
				case 4014:	// размещение товара с транзита
					KillTimer(hwndDlg, WM_USER + 2);
					EndDialog(hwndDlg, GET_WM_COMMAND_ID(wParam, lParam));

					return TRUE;

			}
			break;

		case WM_CLOSE:
			KillTimer(hwndDlg, WM_USER + 2);
			EndDialog(hwndDlg, 0);
			return TRUE;
	}


	//if(pMsg->lParam==1)if(wParam == 0xd||wParam == 0x78||wParam == 0x79||wParam == 0x77){

	//getscanline(hwndDlg, code);
	//if (wcslen(code) > 3)
	//{
	//getinfo(hwndDlg, code);
	//return TRUE;
	//}
	//}

	if (uMsg == 0x80b4 || ((uMsg == 0x100) && (wParam == 0xd || wParam == 0x78 || wParam == 0x79 || wParam == 0x77)) || (uMsg == 0x111 && wParam == 1 && lParam == 0))	//полученный код эксперементальным путем оранжевая кнопка
	{
		getscanline(hwndDlg, code);
		if (wcslen(code) > 3)
		{
			getinfo(hwndDlg, code);
			return TRUE;
		}
	}

	return FALSE;
}


_Bool SelectFilQuest(HWND hwndDlg, int d)
{
	wchar_t tovar[100], kolvo[100];
	LV_ITEM lvi;
	HWND hwndlist = GetDlgItem(hwndDlg, list);
	LVFINDINFO lvf;
	int i = SendMessage(hwndlist, LVM_GETSELECTIONMARK, 0, 0);
	ListView_GetItemText(hwndlist, i, 2, tovar, 100);
	ListView_GetItemText(hwndlist, i, 1, wotvet, 100);
	lvi.mask = LVIF_PARAM;
	lvi.iItem = i;
	lvi.iSubItem = 0;	//0 обязательно
	ListView_GetItem(hwndlist, &lvi);
	wsprintf(kolvo, L"%d", d);
	sendGoodslist(L"ВыборЗадания", tovar, uuid[lvi.lParam].guid, kolvo, FALSE);
	MultiByteToWideChar(CODEPAGE, 0, szData, dwBytesRead, wzData, dwBytesRead);
	i = xmlgetw(0, L"return");
	if (i < 1)
	{
		xmlgetw(0, L"detail");
		//ShowError(hwndDlg,wotvet);
		//myMessageBox(0,wotvet,0,100);
		return FALSE;
	}
	xmlgetw(i, L"<m:Наименов");
	wcscpy(tovar, wotvet);
	//if(xmlgetw(i,L"<m:Номенклатур")<1)return FALSE;
	if (xmlgetw(i, L"<m:Код") < 1)
		return FALSE;
	if (wcscmp(wotvet, L"Ошибко") == 0 || wcscmp(wotvet, L"Error") == 0 || wcscmp(wotvet, L"Ошибка") == 0)
	{
		wcscpy(wotvet, tovar);
		return FALSE;
	}



	SoundOK(TRUE);
	return TRUE;
}

_Bool scanQuest(HWND hwndDlg, wchar_t *code)
{
	LV_ITEM lvi;
	wchar_t tovar[255], kolvo[100];
	wcscpy(wotvet, L"");
	if (wcslen(code) < 4)
		return FALSE;
	sendGoodslist(L"ВыборЗадания", code, code, L"1000", FALSE);

	MultiByteToWideChar(CODEPAGE, 0, szData, strlen(szData), wzData, -1);
	int i = xmlgetw(0, L"return");
	if (i < 1)
	{
		xmlgetw(0, L"detail");
		return FALSE;
	}
	xmlgetw(i, L"<m:Наименов");
	wcscpy(tovar, wotvet);
	if (xmlgetw(i, L"<m:Количество") < 1)
		return FALSE;
	wcscpy(kolvo, wotvet);
	if (xmlgetw(i, L"<m:Код") < 1)
		return FALSE;
	//wcscpy(kod, wotvet);
	SoundOK(TRUE);
	if (wcscmp(wotvet, L"Добавить") == 0)
	{
		lvi.mask = LVIF_TEXT | LVIF_PARAM;
		wcscpy(uuid[++GUIDSCOUNT].guid, tovar);
		uuid[GUIDSCOUNT].Param = _wtoi(kolvo);
		wcscpy(uuid[GUIDSCOUNT].Article, code);
		wcscpy(uuid[GUIDSCOUNT].Adress, code);

		lvi.pszText = tovar;
		lvi.cchTextMax = wcslen(tovar);
		lvi.iItem = 0;
		lvi.iSubItem = 0;	//0 обязательно
		lvi.lParam = _wtoi(kolvo);
		SendMessage(GetDlgItem(hwndDlg, list), LVM_INSERTITEM, 0, (LPARAM) & lvi);
		lvi.iSubItem = 1;
		SendMessage(GetDlgItem(hwndDlg, list), LVM_SETITEMTEXT, 0, (LPARAM) & lvi);

		lvi.pszText = tovar;
		lvi.iSubItem = 2;
		lvi.cchTextMax = wcslen(tovar);

		SendMessage(GetDlgItem(hwndDlg, list), LVM_SETITEMTEXT, 0, (LPARAM) & lvi);

		return TRUE;
	}

	if (wcscmp(wotvet, L"Ошибка") == 0 || wcscmp(wotvet, L"Ошибко") == 0 || wcscmp(wotvet, L"Error") == 0)
	{
		wcscpy(wotvet, tovar);
		return FALSE;
	}
	KillTimer(hwndDlg, WM_USER + 2);
	EndDialog(hwndDlg, 130);
	return TRUE;

}


void fillquests2(HWND hwndDlg, wchar_t *col1, wchar_t *col2, wchar_t *col3)
{
	LV_COLUMN lvc;	//LV_ITEM lvi;LVFINDINFO lvf; 
	ListView_SetExtendedListViewStyle(GetDlgItem(hwndDlg, list), LVS_EX_ONECLICKACTIVATE | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP | LVS_EX_FULLROWSELECT);
	lvc.mask = (LVCF_TEXT + LVCF_WIDTH + LVCF_FMT + LVCF_SUBITEM);
	lvc.fmt = LVCFMT_LEFT;
	lvc.iSubItem = 0;
	lvc.cx = 290;
	lvc.pszText = col1;
	lvc.cchTextMax = wcslen(col1);
	SendMessage(GetDlgItem(hwndDlg, list), LVM_INSERTCOLUMN, 0, (LPARAM) & lvc);
	lvc.iSubItem = 1;
	lvc.cx = 0;
	lvc.pszText = col2;
	lvc.cchTextMax = wcslen(col2);
	SendMessage(GetDlgItem(hwndDlg, list), LVM_INSERTCOLUMN, 0, (LPARAM) & lvc);
	lvc.iSubItem = 2;
	lvc.cx = 95;
	lvc.pszText = col3;
	lvc.cchTextMax = wcslen(col3);
	SendMessage(GetDlgItem(hwndDlg, list), LVM_INSERTCOLUMN, 0, (LPARAM) & lvc);
}


void fillquests(HWND hwndDlg, wchar_t *col1, wchar_t *col2, wchar_t *col3)
{
	LV_COLUMN lvc;	//LV_ITEM lvi;LVFINDINFO lvf; 
	ListView_SetExtendedListViewStyle(GetDlgItem(hwndDlg, list), LVS_EX_ONECLICKACTIVATE | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP | LVS_EX_FULLROWSELECT);
	lvc.mask = (LVCF_TEXT + LVCF_WIDTH + LVCF_FMT + LVCF_SUBITEM);
	lvc.fmt = LVCFMT_LEFT;
	lvc.iSubItem = 0;
	lvc.cx = 290;
	lvc.pszText = col1;
	lvc.cchTextMax = wcslen(col1);
	SendMessage(GetDlgItem(hwndDlg, list), LVM_INSERTCOLUMN, 0, (LPARAM) & lvc);
	lvc.iSubItem = 1;
	lvc.cx = 35;
	lvc.pszText = col2;
	lvc.cchTextMax = wcslen(col2);
	SendMessage(GetDlgItem(hwndDlg, list), LVM_INSERTCOLUMN, 0, (LPARAM) & lvc);
	lvc.iSubItem = 2;
	lvc.cx = 45;
	lvc.pszText = col3;
	lvc.cchTextMax = wcslen(col3);
	SendMessage(GetDlgItem(hwndDlg, list), LVM_INSERTCOLUMN, 0, (LPARAM) & lvc);
}

int filltab(HWND hwndDlg, wchar_t *col1, wchar_t *col2, wchar_t *col3, wchar_t *col4, int mode)
{
	wchar_t code[100], tovar[100], kolvo[100];
	int d, c, z, i, o, k;
	sendGoodslist(col1, col2, col3, col4, TRUE);
	MultiByteToWideChar(CODEPAGE, 0, szData, dwBytesRead, wzData, dwBytesRead);
	i = xmlgetw(0, L"return");
	if (i < 1)
	{
		xmlgetw(0, L"detail");
		ShowError(hwndDlg, wotvet);
		MyMessageBox(0, wotvet, 0, 100);
	}
	c = 1;
	z = 3;
	while (z > 1)
	{
		z = xmlgetw(i, L"<m:Номенклатур");
		i = z + 1;
		if (z < 1)
			break;
		if (xmlgetw(i, L"<m:Код") < 1)
			break;
		if (wcscmp(wotvet, L"Ошибко") == 0 || wcscmp(wotvet, L"Error") == 0)
		{
			xmlgetw(i, L"<m:Наименов");
			ShowError(hwndDlg, wotvet);
			MyMessageBox(0, wotvet, 0, 100);
			continue;
		}
		wcscpy(code, wotvet);
		if (xmlgetw(i, L"<m:Наименов") < 1)
			break;
		wcscpy(tovar, wotvet);

		if (xmlgetw(i, L"<m:Количество") < 1)
			break;
		if (mode == 0)
		{
			wcscpy(uuid[c].guid, code);
			d = _wtoi(wotvet);
			o = d >> 16;
			k = d & 0xffff;

			wsprintf(code, L"%d", k);
			wsprintf(wotvet, L"%d", o);
			addline(hwndDlg, wotvet, code, tovar, c);
		}
		if (mode == 1)
			addline(hwndDlg, code, wotvet, tovar, c);
		if (mode == 2)
			addline(hwndDlg, code, wotvet, tovar, c);
		if (mode > 2)
			addline(hwndDlg, code, wotvet, tovar, c);

		c++;
	}
	return --c;


}

int FindStringInTable(HWND hwndDlg, wchar_t *search, int kolonok)
{

	LV_ITEM lvi;
	int i, j, dlina, found;
	wchar_t temp[100];
	dlina = wcslen(search);
	found = -1;
	if (dlina == 0)
		return -1;
//Поиск по всем колонкам таблицы                    
	i = SendMessage(hwndDlg, LVM_GETITEMCOUNT, 0, 0);
	for (; i > 0;)
	{
		i--;
		for (j = 0; j <= kolonok; j++)
		{
			lvi.mask = LVIF_TEXT;
			lvi.pszText = temp;
			lvi.cchTextMax = 100;
			lvi.iSubItem = j;	//0 обязательно
			SendMessage(hwndDlg, LVM_GETITEMTEXT, i, (LPARAM) & lvi);
			if (wcsncmp(search, temp, dlina) == 0)
			{
				found = lvi.iItem;
				break;
			}

		}

		if (found != -1)
			break;
	}
	return found;
}


static LRESULT CALLBACK QuestSelect(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	wchar_t temp[TempSize], code[100], tovar[500], kolvo[100];
	int x, y, i, z;
	RECT rect;
	LV_COLUMN lvc;
	LV_ITEM lvi;
	LVFINDINFO lvf;
	LPNMHDR jj;
	int kk, k;
	LPMSG pMsg;
	HWND hwndList;
	_Bool zavershenie;
	//wsprintf(code,L"---r %x; l %x; ---",GetAsyncKeyState(39),GetAsyncKeyState(37));
	//MyMessageWait(code);


	switch (uMsg)
	{
		case WM_INITDIALOG:
		{
			OpenClipboard(hwndDlg);
			EmptyClipboard();
			CloseClipboard();
			mSetFocus(GetDlgItem(hwndDlg, combo));
			SHINITDLGINFO shidi;
			shidi.dwMask = SHIDIM_FLAGS;
			shidi.dwFlags = SHIDIF_SIPDOWN | SHIDIF_SIZEDLGFULLSCREEN;
			shidi.hDlg = hwndDlg;
			//SHInitDialog(&shidi);
			x = GetSystemMetrics(SM_CXSCREEN);
			y = GetSystemMetrics(SM_CYSCREEN);
			SetWindowPos(hwndDlg, hwndDlg, 0, 0, x, y, SWP_NOZORDER);
			SetWindowPos(GetDlgItem(hwndDlg, list), GetDlgItem(hwndDlg, list), 0, 10, x, y - 35, SWP_NOZORDER);
			SetWindowPos(GetDlgItem(hwndDlg, sledfilial), GetDlgItem(hwndDlg, sledfilial), 3, y - 22, x / 2 - 10, 20, SWP_NOZORDER);
			SetWindowPos(GetDlgItem(hwndDlg, zakonchit), GetDlgItem(hwndDlg, zakonchit), x / 2 - 5, y - 22, x / 2 - 10, 20, SWP_NOZORDER);
			if (Goodslistmode > 3 && Goodslistmode < 10 || Goodslistmode == 13)
			{
				fillquests(hwndDlg, L"Полка", L"Колич.", L"m3");
				filltab(hwndDlg, L"ЗаданияНаПолки", zagolovokokna, L"code", L"0", Goodslistmode);
			}
			if (Goodslistmode == 0)
			{
				fillquests(hwndDlg, L"Филиал", L"Колич.", L"m3");
				filltab(hwndDlg, L"ЗаданияНаФилиалы", zagolovokokna, L"code", L"0", Goodslistmode);
			}
			if (Goodslistmode == 1 || Goodslistmode == 10 || Goodslistmode == 300)
			{
				fillquests(hwndDlg, L"Ряд", L"Колич.", L"m3");
				filltab(hwndDlg, L"ЗаданияНаРяды", zagolovokokna, L"code", L"0", Goodslistmode);
			}
			if (Goodslistmode == 2 || Goodslistmode == 20 || Goodslistmode == 301 || Goodslistmode == 302)
			{
				fillquests(hwndDlg, L"Секция", L"Колич.", L"m3");
				filltab(hwndDlg, L"ЗаданияНаСекции", zagolovokokna, L"code", L"0", Goodslistmode);
			}
			autofontialog(hwndDlg, 10, 22);
			if (UseMotorolla)
				mSetFocus(GetDlgItem(hwndDlg, list));
			sx = x;
			sy = y;
			tekWnd = hwndDlg;
			editc = 0;
			SetTimer(hwndDlg, WM_USER + 2, 1000, NULL);
			getform1c();
			return TRUE;
		}

		case 0x53:	//help_context

			helpme(hwndDlg);

			break;

		case 0x80b4:



			getscanline(hwndDlg, code);
			if (wcslen(code) > 3)
				if (!scanQuest(hwndDlg, code))
					ShowError(hwndDlg, wotvet);
			return TRUE;



		case WM_NOTIFY:
			jj = (LPNMHDR)lParam;
			if (UseMotorolla)
			{
				hwndList = GetDlgItem(hwndDlg, list);
				i = ListView_GetSelectionMark(hwndList);

//(...) screen keys 
				int key = 117;
				if (GetAsyncKeyState(key) != 0)
				{
					if (Pressed[key] == 0)
					{
						lastoperation = GetTickCount();
						Pressed[key] = 1;

						SendMessage(GetDlgItem(hwndDlg, sledfilial), BM_CLICK, 0, 0);

					}
				}
				else
					Pressed[key] = 0;

//(...) screen keys 
				key = 118;
				if (GetAsyncKeyState(key) != 0)
				{
					if (Pressed[key] == 0)
					{
						lastoperation = GetTickCount();
						Pressed[key] = 1;

						SendMessage(GetDlgItem(hwndDlg, zakonchit), BM_CLICK, 0, 0);

					}
				}
				else
					Pressed[key] = 0;


				if (GetAsyncKeyState(39) != 0)
				{
					ListView_SetCheckState(hwndList, i, TRUE);
					//ListView_Update(hwndList,i);
					//editc = 0;
				}

				if (GetAsyncKeyState(37) != 0)
				{
					ListView_SetCheckState(hwndList, i, !TRUE);
					//ListView_Update(hwndList,i);
					//editc = 0;
				}
			}

			if (wParam == list)
				if (jj->code == 0x3e8 || (jj->code == NM_DBLCLK))
					if (SelectFilQuest(hwndDlg, Goodslistmode))
						EndDialog(hwndDlg, 130);
					else
						ShowError(hwndDlg, wotvet);
			return TRUE;

		case WM_GETDLGCODE:
			//hwndList = GetDlgItem(hwndDlg, list);
			pMsg = (LPMSG) (lParam);
		//i = ListView_GetSelectionMark(hwndList);
		//на chainway сюда не попадают нажатия
		//wsprintf(code,L"---%x;%x;%x;---",pMsg->wParam,pMsg->lParam,wParam);
		//MyMessageWait(code);

			hwndList = GetDlgItem(hwndDlg, list);
			i = ListView_GetSelectionMark(hwndList);
//if(GetAsyncKeyState(39)!=0){
		//ListView_SetCheckState (hwndList,i,TRUE);
		//ListView_Update(hwndList,i);
		//editc = 0;
		////return DLGC_WANTALLKEYS | DLGC_WANTCHARS | DLGC_WANTARROWS ;
		//}

//if(GetAsyncKeyState(37)!=0){
		//ListView_SetCheckState (hwndList,i,!TRUE);
		//ListView_Update(hwndList,i);
		//editc = 0;
		//SendMessage(hwndDlg
		//return DLGC_WANTALLKEYS | DLGC_WANTCHARS | DLGC_WANTARROWS ;
		//}
			switch (pMsg->lParam)
			{

				case WM_KEYDOWN:


					if (LOWORD(pMsg->wParam) == VK_UP)
					{
						mSetFocus(hwndList);
						ListView_SetSelectionMark(hwndList, i - 1);
					}
					if (LOWORD(pMsg->wParam) == VK_DOWN)
					{
						mSetFocus(hwndList);
						ListView_SetSelectionMark(hwndList, i + 1);
						editc = 0;
					}

					if (LOWORD(pMsg->wParam) == VK_RIGHT || LOWORD(pMsg->wParam) == VK_LEFT)
					{
						ListView_SetCheckState(hwndList, i, !ListView_GetCheckState(hwndList, i));
						ListView_Update(hwndList, i);
						editc = 0;
					}


					if (LOWORD(pMsg->wParam) == VK_RETURN)
					{



						GetWindowText(GetDlgItem(hwndDlg, combo), code, 100);
						if (Goodslistmode < 400 && Goodslistmode > 499)
							ShowWindow(GetDlgItem(hwndDlg, combo), SW_HIDE);
						SetWindowText(GetDlgItem(hwndDlg, combo), L"");

						if (wcslen(code) == 0 || Goodslistmode == 437)
							if (SelectFilQuest(hwndDlg, Goodslistmode))
								EndDialog(hwndDlg, 130);
							else
								ShowError(hwndDlg, wotvet);
						else if (!scanQuest(hwndDlg, code))
							ShowError(hwndDlg, wotvet);
					}

			}
		//if(!UseMotorolla)
			SetWindowLong(hwndDlg, DWL_MSGRESULT, DLGC_WANTALLKEYS | DLGC_WANTCHARS | DLGC_WANTARROWS);	//DLGC_WANTALLKEYS | DLGC_WANTCHARS);
			return DLGC_WANTALLKEYS | DLGC_WANTCHARS | DLGC_WANTARROWS;

			return DLGC_WANTARROWS | DLGC_WANTALLKEYS | DLGC_WANTCHARS;


		case WM_TIMER:
			//KillTimer(hwndDlg, WM_USER + 2);
			if (!UseMotorolla)
			{

				mSetFocus(GetDlgItem(hwndDlg, combo));
				SendMessage(GetDlgItem(hwndDlg, combo), EM_SETSEL, -1, 0);

			}
			if (wParam == WM_USER + 2)
			{
				if (Goodslistmode == 437)
				{
					GetWindowText(GetDlgItem(hwndDlg, combo), code, 100);
					if (wcslen(code) == 0)
						return TRUE;
					mSetFocus(GetDlgItem(hwndDlg, combo));
					SendMessage(GetDlgItem(hwndDlg, combo), EM_SETSEL, -1, 0);

					i = FindStringInTable(GetDlgItem(hwndDlg, list), code, 4);
					if (i < 0)
						return TRUE;
					ListView_SetSelectionMark(GetDlgItem(hwndDlg, list), i);
					ListView_SetItemState(GetDlgItem(hwndDlg, list), i, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
					ListView_EnsureVisible(GetDlgItem(hwndDlg, list), i, TRUE);
				}
			}
			return TRUE;

		case WM_COMMAND:

			GetWindowText(GetDlgItem(hwndDlg, GET_WM_COMMAND_ID(wParam, lParam)), code, 100);
			switch (GET_WM_COMMAND_ID(wParam, lParam))
			{

				case IDOK:

					if (UseMotorolla)
					{
						getscanline(hwndDlg, code);
						if (wcslen(code) > 3)
						{
							if (!scanQuest(hwndDlg, code))
								ShowError(hwndDlg, wotvet);
						}
						else
						{

							GetWindowText(GetDlgItem(hwndDlg, combo), code, 100);
							if (Goodslistmode < 400 && Goodslistmode > 499)
								ShowWindow(GetDlgItem(hwndDlg, combo), SW_HIDE);
							SetWindowText(GetDlgItem(hwndDlg, combo), L"");

							if (wcslen(code) == 0 || Goodslistmode == 437)
								if (SelectFilQuest(hwndDlg, Goodslistmode))
									EndDialog(hwndDlg, 130);
								else
									ShowError(hwndDlg, wotvet);
							else if (!scanQuest(hwndDlg, code))
								ShowError(hwndDlg, wotvet);

						}

					}
					return TRUE;

				case 4501:

					return TRUE;

				case zakonchit:

					KillTimer(hwndDlg, WM_USER + 2);
					wsprintf(kolvo, L"%u", Goodslistmode);
					sendGoodslist(L"ОтменаЗадания", code, L"ОтменаЗадания", kolvo, FALSE);
					xmlgetw(3, L"<Количе");
					EndDialog(hwndDlg, _wtoi(wotvet));
					return TRUE;

				case sledfilial:
					if(Goodslistmode == 537)if(SendListwithCommand(hwndDlg,L"",L"Список Флажков",TRUE)){
//								KillTimer(hwndDlg, WM_USER + 2);
//								EndDialog(hwndDlg, 130);
							return TRUE;}

					if (SelectFilQuest(hwndDlg, Goodslistmode))
					{
						KillTimer(hwndDlg, WM_USER + 2);
						EndDialog(hwndDlg, 130);
					}
					else
						ShowError(hwndDlg, wotvet);
					return TRUE;
			}
			break;

		case WM_CLOSE:
			KillTimer(hwndDlg, WM_USER + 2);
			EndDialog(hwndDlg, 0);
			return TRUE;
	}
	return FALSE;
}

static LRESULT CALLBACK VvodKolvo(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int i;
	RECT rect;
	LPMSG pMsg;
	switch (uMsg)
	{
		case WM_INITDIALOG:
		{
			OpenClipboard(hwndDlg);
			EmptyClipboard();
			CloseClipboard();
			//SetWindowText(hwndDlg,L"Введите количество");
			//HWND h = FindWindow(NULL,L"Введите количество");
			//if (h)
			//{
			//SetForegroundWindow(h);
			//EndDialog(h,0);
			//}
			//autoscaledialog(hwndDlg,FALSE);
			//autofontialog(hwndDlg, 8, 16);
			SetFocus(GetDlgItem(hwndDlg, combo));
			return TRUE;
		}

		case WM_COMMAND:

			switch (GET_WM_COMMAND_ID(wParam, lParam))
			{
				case 4501:
					//HideError(hwndDlg);
					return TRUE;


				case IDOK:
					GetWindowText(GetDlgItem(hwndDlg, combo), wotvet, 200);
					i = _wtoi(wotvet);
					if (i == 0)
						i = 1;
					EndDialog(hwndDlg, i);
					return TRUE;

				case IDCANCEL:
					EndDialog(hwndDlg, 0);
					return TRUE;

				case noscan:
					i = _wtoi(wotvet);
					if (i == 0)
						i = 1;
					EndDialog(hwndDlg, -i);
					return TRUE;


			}
			break;

		case WM_CLOSE:
			EndDialog(hwndDlg, 0);
			return TRUE;
	}
	if (tomainmenu)
		EndDialog(hwndDlg, 0);

	return FALSE;
}

static LRESULT CALLBACK Vopros(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int x, y, i, z, someval;
	RECT rect;
	LPMSG pMsg;
	LV_COLUMN lvc;
	LV_ITEM lvi;
	LVFINDINFO lvf;
	LPNMHDR jj;
	_Bool AskForCount;
	//static long starttime;
	wchar_t tovar[200], article[100], kolvo[100];

	switch (uMsg)
	{
		case WM_INITDIALOG:
		{
			AskForCount = TRUE;
			freeMessages(pMsg, hwndDlg);
			freeMessages(pMsg, hwndDlg);
			freeMessages(pMsg, hwndDlg);
			freeMessages(pMsg, hwndDlg);
			OpenClipboard(hwndDlg);
			EmptyClipboard();
			CloseClipboard();
			x = GetSystemMetrics(SM_CXSCREEN) - 1;
			y = GetSystemMetrics(SM_CYSCREEN) - 1;
			SetWindowPos(hwndDlg, hwndDlg, 0, 0, x, y, SWP_NOZORDER);
			SetWindowPos(GetDlgItem(hwndDlg, Userlist), GetDlgItem(hwndDlg, Userlist), 0, 0, x, y / 3 - 45, SWP_NOZORDER);
			SetWindowPos(GetDlgItem(hwndDlg, list), GetDlgItem(hwndDlg, list), 0, y / 3 - 35, x, y / 3 + 10, SWP_NOZORDER);
			SetWindowPos(GetDlgItem(hwndDlg, 4011), GetDlgItem(hwndDlg, 4011), 0, 2 * y / 3 - 30, x, y / 3, SWP_NOZORDER);
			SetWindowPos(GetDlgItem(hwndDlg, 4013), GetDlgItem(hwndDlg, 4013), x - 40, 2 * y / 3 - 30, 40, 20, SWP_NOZORDER);
			SetWindowPos(GetDlgItem(hwndDlg, IDOK), GetDlgItem(hwndDlg, IDOK), x / 3, y - 50, x / 3, 25, SWP_NOZORDER);
			SendMessage(GetDlgItem(hwndDlg, 4011), BM_CLICK, 0, 0);
			SetFocus(GetDlgItem(hwndDlg, Userlist));
			ShowWindow(GetDlgItem(hwndDlg, IDOK), SW_SHOW);
			SetWindowText(hwndDlg, zagolovokokna);
			SetWindowText(GetDlgItem(hwndDlg, Userlist), label1);	// текст вопроса
			autofontialog(hwndDlg, 9, 16);
			fontialog(GetDlgItem(hwndDlg, 4011), 6, 16, 500, 0);
			fillquests2(hwndDlg, L"Вариант", L" ", L"Код");	//для создания колонок
//          wcscpy(knopkadalee,kudaotvet);
			z = tekpos;
			i = z;
			//tekWnd = hwndDlg;
			int c = 0;
			while (z > 1)
			{
				z = xmlgetw(i, L"<m:Номенклатур");
				i = z;
				if (z < 1)
					break;
				if (xmlgetw(i, L"<m:Код") < 1)
					break;
				wcscpy(article, wotvet);

				if (xmlgetw(i, L"<m:Наименов") <= 0)
					break;
				wcscpy(tovar, wotvet);

				if (xmlgetw(i, L"<m:Количество") < 1)
					break;
				wcscpy(kolvo, wotvet);
				if (wcscmp(L"БезКоличества", article) == 0)
				{
					AskForCount = FALSE;
					continue;
				}
				if (wcscmp(L"КонецСписка", article) == 0)
					break;

				addline(hwndDlg, article, kolvo, tovar, c);


			}
			ListView_SetItemState(GetDlgItem(hwndDlg, list), 1, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
			memset(wzData, 0, sizeof(wzData));
			tekpos = 3;
			SoundOK(TRUE);
			SetForegroundWindow(hwndDlg);
			freeMessages(pMsg, hwndDlg);
			freeMessages(pMsg, hwndDlg);

			return TRUE;
		}





		case WM_GETDLGCODE:
			pMsg = (LPMSG) (lParam);
			switch (pMsg->lParam)
			{
				case WM_KEYDOWN:
					if (LOWORD(pMsg->wParam) == VK_UP)
					{
						mSetFocus(GetDlgItem(hwndDlg, list));
						ListView_SetSelectionMark(GetDlgItem(hwndDlg, list), (ListView_GetSelectionMark(GetDlgItem(hwndDlg, list)) - 1));
					}
					if (LOWORD(pMsg->wParam) == VK_DOWN)
					{
						mSetFocus(GetDlgItem(hwndDlg, list));
						ListView_SetSelectionMark(GetDlgItem(hwndDlg, list), ListView_GetSelectionMark(GetDlgItem(hwndDlg, list)) + 1);
					}

					if ((LOWORD(pMsg->wParam) == VK_RETURN))
					{
					}
			}
			SetWindowLong(hwndDlg, DWL_MSGRESULT, DLGC_WANTALLKEYS | DLGC_WANTCHARS);
			return DLGC_WANTARROWS | DLGC_WANTALLKEYS | DLGC_WANTCHARS;


		case WM_NOTIFY:
			jj = (LPNMHDR)lParam;
			if (wParam == list)
			{

				LPNMLVDISPINFOW ttt = (LPNMLVDISPINFOW)lParam;
				if (jj->code == 0x3e8 || (jj->code == NM_DBLCLK))
				{	//долгое удержание стилуса на таблице or dblclick
					SendMessage(GetDlgItem(hwndDlg, IDOK), BM_CLICK, 0, 0);
				}
				else
				{
					i = SendMessage(GetDlgItem(hwndDlg, list), LVM_GETSELECTIONMARK, 0, 0);
					if (i >= 0)
					{
						ListView_GetItemText(GetDlgItem(hwndDlg, list), i, 2, tovar, 200);
						SetWindowText(GetDlgItem(hwndDlg, 4011), tovar);
					}
				}
			}
			return TRUE;
		case WM_COMMAND:

			switch (GET_WM_COMMAND_ID(wParam, lParam))
			{
				case 4501:
					//HideError(hwndDlg);
					return TRUE;
				case IDOK:
					//if(someval == 12345){
					ListView_GetItemText(GetDlgItem(hwndDlg, list), ListView_GetSelectionMark(GetDlgItem(hwndDlg, list)), 0, article, 100);
					int k = 1;
				//GetWindowText(GetDlgItem(hwndDlg, 4013),
				//if(wcslen(article)>4)k = DialogBox(g_hInstance, MAKEINTRESOURCE(1013), hwndDlg, (DLGPROC)VvodKolvo);
					if (k == 0)
						k = 1;
					//wsprintf(kolvo, L"%d", k);
					if (wcscmp(knopkadalee, L"НайтиТовар") == 0)
					{
						//              k = (k * 16) + Goodslistmode;
						//                  wsprintf(kolvo, L"%u", k);
					}

					sendGoodslist(knopkadalee, article, article, L"1", TRUE);	//kolvo, TRUE);
					EndDialog(hwndDlg, 0);	//}
					return TRUE;



			}
			break;
		case 0x53:	//help_context|| нажали кнопку вопрос

			helpme(hwndDlg);

			break;


		case WM_CLOSE:
			//if((GetTickCount()- starttime)>1000||(GetTickCount()- starttime)<0 )
			EndDialog(hwndDlg, 0);
			return TRUE;
	}
	if (tomainmenu)
		EndDialog(hwndDlg, 0);

	return FALSE;
}



static LRESULT CALLBACK Erroring(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int i;
	RECT rect;
	LPMSG pMsg;
	int t, tm;

	switch (uMsg)
	{
		case WM_INITDIALOG:
		{
			//SetWindowText(hwndDlg,L"Error");
			//HWND h = FindWindow(NULL,L"Error");
			//if (h)
			//{
			//EndDialog(h,0);
			//}

			tm = GetTickCount();
			OpenClipboard(hwndDlg);
			EmptyClipboard();
			CloseClipboard();
			SetWindowText(GetDlgItem(hwndDlg, combo), label1);
			autofontialog(hwndDlg, 7, 18);
			int x = GetSystemMetrics(SM_CXSCREEN);
			int y = GetSystemMetrics(SM_CYSCREEN);
			//GetWindowRect(GetDesktopWindow(), &rect);
			SetWindowPos(hwndDlg, hwndDlg, 0, 0, x, y, SWP_NOZORDER);
			SetWindowPos(GetDlgItem(hwndDlg, combo), GetDlgItem(hwndDlg, combo), 3, 10, x - 9, 2 * y / 3, SWP_NOZORDER);
			SetWindowPos(GetDlgItem(hwndDlg, 4025), GetDlgItem(hwndDlg, 4025), x / 3, 2 * y / 3 + 10, (x / 3) - 6, y / 3 - 20, SWP_NOZORDER);
			SetFocus(GetDlgItem(hwndDlg, combo));
			//SendMessage(GetDlgItem(hwndDlg,combo), EM_SETSEL  ,-1,0);
			return TRUE;
		}
		case WM_GETDLGCODE:
			pMsg = (LPMSG) (lParam);
			switch (pMsg->lParam)
			{

				case WM_KEYDOWN:
					if (LOWORD(pMsg->wParam) == 0x30)
						SendMessage(GetDlgItem(hwndDlg, 4025), BM_CLICK, 0, 0);
					//if (LOWORD(pMsg->wParam) == VK_RETURN)SendMessage(GetDlgItem(hwndDlg,4025),BM_CLICK,0,0);

			}
			SetWindowLong(hwndDlg, DWL_MSGRESULT, DLGC_WANTALLKEYS | DLGC_WANTCHARS);
			return DLGC_WANTARROWS | DLGC_WANTALLKEYS | DLGC_WANTCHARS;

		case WM_COMMAND:

			switch (GET_WM_COMMAND_ID(wParam, lParam))
			{
				case 4025:
					//if((GetTickCount()-t)>2000)
					EndDialog(hwndDlg, 0);
				//Sleep(1000);

					return TRUE;




			}
			break;

		//case WM_CLOSE:
		//  EndDialog(hwndDlg, 0);
		// return TRUE;
	}
	//if((GetTickCount()-tm) > 30000)EndDialog(hwndDlg,0);
	if (tomainmenu)
		EndDialog(hwndDlg, 0);
	return FALSE;
}


static LRESULT CALLBACK ScanForm(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	wchar_t temp[500], kolvo[200], code[200];
	int x, y, d, i, z;
	RECT rect;
	LPMSG pMsg;
	//wsprintf(code,L"---%x;%x---",wParam,lParam);
	//MyMessageWait(code);
	switch (uMsg)
	{
		case WM_INITDIALOG:
			OpenClipboard(hwndDlg);
			EmptyClipboard();
			CloseClipboard();
			GetWindowRect(GetDesktopWindow(), &rect);
			x = rect.right;
			y = rect.bottom - 20;

		//x = GetSystemMetrics(SM_CXSCREEN);
		//y = GetSystemMetrics(SM_CYSCREEN);
			sx = x;
			sy = y;
			tekWnd = hwndDlg;
			SetWindowPos(hwndDlg, hwndDlg, 0, 0, x, y, SWP_NOZORDER);
			SetWindowPos(GetDlgItem(hwndDlg, list), GetDlgItem(hwndDlg, list), 0, 0, x, y / 3, SWP_NOZORDER);
			SetWindowPos(GetDlgItem(hwndDlg, 4011), GetDlgItem(hwndDlg, 4011), 0, y / 3, x, y / 3, SWP_NOZORDER);
		//SetWindowPos(GetDlgItem(hwndDlg, combo), GetDlgItem(hwndDlg, combo), 0, y - 50, x, 20, SWP_NOZORDER);
		//SetWindowPos(GetDlgItem(hwndDlg, IDOK), GetDlgItem(hwndDlg, IDOK), 0, y - 30, x / 2, 20, SWP_NOZORDER);
			SetWindowPos(GetDlgItem(hwndDlg, IDCANCEL), GetDlgItem(hwndDlg, IDCANCEL), x / 2, y - 30, x / 2, 20, SWP_NOZORDER);
			ShowWindow(GetDlgItem(hwndDlg, combo), SW_HIDE);
			ShowWindow(GetDlgItem(hwndDlg, IDOK), SW_HIDE);
			SetWindowText(GetDlgItem(hwndDlg, list), zagolovokokna);
			autofontialog(hwndDlg, 6, 16);
			getform1c();
			SetTimer(hwndDlg, WM_USER + 2, 1000, NULL);

			return TRUE;

		case 0x53:	//help_context

			helpme(hwndDlg);

			break;


		case WM_GETDLGCODE:
			pMsg = (LPMSG) (lParam);

			switch (pMsg->lParam)
			{
				case 1:
				case WM_KEYDOWN:
					if (LOWORD(pMsg->wParam) == VK_RETURN)
					{
						if (UseMotorolla)
						{
							getscanline(hwndDlg, code);
							if (wcslen(code) > 3)
								SetWindowText(GetDlgItem(hwndDlg, combo), code);
						}

						GetWindowText(GetDlgItem(hwndDlg, combo), code, 100);
						SetWindowText(GetDlgItem(hwndDlg, combo), L"");
						//SetFocus(GetDlgItem(hwndDlg, combo));
						if (wcslen(code) < 4)
							break;
						if (!scanQuest(hwndDlg, code))
							ShowError(hwndDlg, wotvet);

					}
			}

			SetWindowLong(hwndDlg, DWL_MSGRESULT, DLGC_WANTALLKEYS | DLGC_WANTCHARS);

			return DLGC_WANTARROWS | DLGC_WANTALLKEYS | DLGC_WANTCHARS;

		case WM_TIMER:
			if (wParam == WM_USER + 2)
			{
				//mSetFocus(GetDlgItem(hwndDlg, combo));

				SendMessage(GetDlgItem(hwndDlg, combo), EM_SETSEL, -1, 0);
			}
			return TRUE;

		case WM_COMMAND:

			switch (GET_WM_COMMAND_ID(wParam, lParam))
			{

				case IDCANCEL:	//Отмена список
					if (!scanQuest(hwndDlg, L"ОтменаЗадания"))
						ShowError(hwndDlg, wotvet);
					KillTimer(hwndDlg, WM_USER + 2);
					EndDialog(hwndDlg, 0);
					return TRUE;
				case IDOK:	//подтвердить список
					if (UseMotorolla)
					{
						getscanline(hwndDlg, code);
						if (wcslen(code) > 3)
							SetWindowText(GetDlgItem(hwndDlg, combo), code);
					}
					GetWindowText(GetDlgItem(hwndDlg, combo), code, 100);
					SetWindowText(GetDlgItem(hwndDlg, combo), L"");
				//SetFocus(GetDlgItem(hwndDlg, combo));
					if (wcslen(code) < 4)
						break;
					if (!scanQuest(hwndDlg, code))
						ShowError(hwndDlg, wotvet);

					return TRUE;

			}
			break;

		case WM_CLOSE:
			KillTimer(hwndDlg, WM_USER + 2);
			EndDialog(hwndDlg, 0);
			return TRUE;
	}
	if (uMsg == 0x80b4)
	{
		getscanline(hwndDlg, code);
		if (wcslen(code) > 3)
			if (!scanQuest(hwndDlg, code))
				ShowError(hwndDlg, wotvet);

	}
	if (tomainmenu)
		EndDialog(hwndDlg, 0);
	return FALSE;
}
