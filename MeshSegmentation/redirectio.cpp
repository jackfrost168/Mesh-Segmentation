// redirectio.cpp

#include "stdafx.h"     /* comment this line if do not use PCH on this file */
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <tchar.h>



/* Automatic unicode check when needed. */
#if defined (UNICODE) || defined (_UNICODE)

static int UnicodeSupportedInC(void);

#if defined (__cplusplus)
    class __CUnicodeSupported 
    {
    public:
        __CUnicodeSupported(void)
        {
            UnicodeSupportedInC();
        }
    };
    static __CUnicodeSupported __UnicodeSupportedObject;
#else
    static int __UnicodeSupportedObject = UnicodeSupportedInC();
#endif

int UnicodeSupportedInC(void)
{
    if (GetWindowsDirectoryW(NULL, 0) <= 0) {
        char filename[256];

        GetModuleFileNameA(NULL, filename, sizeof(filename)/sizeof(filename[0]));
        MessageBoxA(GetActiveWindow(), 
            "This Program requires an OS that supports Unicode.", 
            filename, 
            MB_OK|MB_ICONSTOP);

        ExitProcess(0);
    }

    return 1;
}

#endif  /* UNICODE || _UNICODE */


/* Automatic IO redirection when needed. */
#if !defined (_CONSOLE)

#if 1
//#if defined (DEBUG) || defined (_DEBUG)

static int _bConsoleAttached = 0;
static int _bConsoleCreated = 0;

static void RedirectIOToConsole(int nline, TCHAR* title);
static void FreeIOConsole(void);

#if defined (__cplusplus)
    class __CRedirectIOToConsole
    {
    public:
        __CRedirectIOToConsole(void)
        { 
            TCHAR filename[256];

            GetModuleFileName(NULL, filename, sizeof(filename)/sizeof(filename[0]));
            RedirectIOToConsole(1000, filename);
        }
        ~__CRedirectIOToConsole(void) {
            FreeIOConsole();
        }
    };
    static __CRedirectIOToConsole __RedirectIOToConsoleObject;
#else
    static int RedirectIOToConsoleInC(void);
    
    static int _RedirectIOToconsoleObject = RedirectIOToConsoleInC();
    
    int RedirectIOToConsoleInC(void)
    {
        TCHAR filename[256];

        GetModuleFileName(NULL, filename, sizeof(filename)/sizeof(filename[0]));
        RedirectIOToConsole(1000, filename);

        return atexit(FreeIOConsole);
    }
#endif



void RedirectIOToConsole(int nline, TCHAR* title)
{
    nline = nline < 25 ? 25 : nline;

	if (!_bConsoleCreated)
	    _bConsoleAttached = AllocConsole(); /* We don't care if it failed. */

	if (_bConsoleAttached) {
		HANDLE aHandle;
		CONSOLE_SCREEN_BUFFER_INFO coninfo;

		/* Set the number of screen buffer's lines. */
		aHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		GetConsoleScreenBufferInfo(aHandle, &coninfo);
		coninfo.dwSize.Y = nline;
		SetConsoleScreenBufferSize(aHandle, coninfo.dwSize);

#       pragma warning(push)
#       pragma warning(disable: 4311)   /* to avoid the (long) cast waring */
		/* Redirect full buffered STDOUT/STDERR/STDIN to the console. */
		*stdout = *_fdopen(_open_osfhandle((long)GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT), "w");
		setvbuf(stdout, NULL, _IONBF, 0);
		*stderr = *_fdopen(_open_osfhandle((long)GetStdHandle(STD_ERROR_HANDLE), _O_TEXT), "w");
		setvbuf(stderr, NULL, _IONBF, 0);
		*stdin = *_fdopen(_open_osfhandle((long)GetStdHandle(STD_INPUT_HANDLE), _O_TEXT), "rw");
		setvbuf(stdin, NULL, _IOFBF, 0);
#       pragma warning(pop)

		SetConsoleTitle(title);
	}

	_bConsoleCreated = 1;
}

void FreeIOConsole(void)
{
	if (_bConsoleAttached) {
		FreeConsole();
		_bConsoleAttached = 0;
	}
	_bConsoleCreated = 0;
}

#endif  /* DEBUG || _DEBUG */

#endif  /* _CONSOLE */
