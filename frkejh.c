#define  CODEPAGE  CP_OEMCP	//1— работает только с кодировкой UTF8 а windows только с 1251 нужно посто€нно конвертировать

int main(int argc, char *argv[])
{
	wchar_t Login[255],password[255],domen[255],cmdline[1000];
    printf("Hello %s, in domain %s\n",argv[1],argv[2]);
	STARTUPINFOW si = { sizeof(STARTUPINFOW) };
	si.dwFlags = 0;
	si.cb = sizeof(STARTUPINFOW); 
	si.dwFlags = CREATE_NEW_CONSOLE;
	si.wShowWindow = SW_HIDE;
	si.lpDesktop = NULL;
	//SHELLEXECUTEINFO se = { sizeof(SHELLEXECUTEINFO) };
	//se.lpFile = selfname;
	//se.nShow = SW_SHOW;
	//se.fMask = SEE_MASK_FLAG_NO_UI;
	PROCESS_INFORMATION pi;
  //ZeroMemory(@si,sizeof(TSTARTUPINFO));
  //si.cb:= sizeof(TSTARTUPINFO);
  //si.lpDesktop:=nil;
			MultiByteToWideChar(CODEPAGE, 0, argv[1], strlen(argv[1])+1, Login, 255);
			MultiByteToWideChar(CODEPAGE, 0, argv[2], strlen(argv[2])+1, domen, 255);
			MultiByteToWideChar(CODEPAGE, 0, argv[3], strlen(argv[3])+1, password, 255);
			MultiByteToWideChar(CODEPAGE, 0, argv[4], strlen(argv[4])+1, cmdline, 1000);
	CreateProcessWithLogonW(Login,domen,password,LOGON_WITH_PROFILE,NULL,cmdline,CREATE_NEW_CONSOLE,NULL,NULL,&si,&pi);
    return 0;