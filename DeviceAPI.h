#define DEVICEAPI_API(type) EXTERN_C type STDAPICALLTYPE
//#pragma comment( lib, "DeviceAPI" )
//#define DECLSPEC_IMPORT __declspec(dllimport)
//void WINAPI StartShake(int);
typedef int (WINAPI *_HardwareVersion_Ex)(UINT8 *pszData);
typedef void (WINAPI *_SerialPortSwitch_Ex)(UINT8 ComID);
typedef void (WINAPI *_SerialPortControl_Ex)(UINT8 uPortID, UINT8 uValue);
typedef int (WINAPI *_SerialPortSetBaudRate_Ex)(int iBaudRate);
typedef int (WINAPI *_SerialPortFunctionSwitch_Ex)(int iModule);
typedef _Bool (WINAPI *_Barcode1D_init)(void);
typedef void (WINAPI *_Barcode1D_free)(void);
typedef int (WINAPI *_Barcode1D_scan)(UINT8 *pszData);
typedef void (WINAPI *_STARTSHAKE)(int);
_STARTSHAKE StartShake;
_HardwareVersion_Ex HardwareVersion_Ex;
_SerialPortSwitch_Ex SerialPortSwitch_Ex;//(UINT8 ComID);
_SerialPortControl_Ex SerialPortControl_Ex;//(UINT8 uPortID, UINT8 uValue);
_SerialPortSetBaudRate_Ex SerialPortSetBaudRate_Ex;//(int iBaudRate);
_SerialPortFunctionSwitch_Ex SerialPortFunctionSwitch_Ex;//(int iModule);
_Barcode1D_init Barcode1D_init;
_Barcode1D_free Barcode1D_free;
_Barcode1D_scan Barcode1D_scan;//(UINT8 *pszData);

