// ***************************************************************************
// TITLE
//
// PROJECT
//
// ***************************************************************************
//
// FILE
//      $Id$
// HISTORY
//      $Log$
// ***************************************************************************

//#ifndef __GNUC__

#include <windows.h>
#include <DbgHelp.h>

#include <a7file.h>

#include <a7string.h>
#include <dreamix.h>
#include <AXApp.h>

// ---------------------------------------------------------------------------
// ---------------------------------- TYPES ----------------------------------
// -|-----------------------|-------------------------------------------------

typedef BOOL (*MINIDUMPWRITEDUMP)(HANDLE,UINT,HANDLE,UINT,
                                  _MINIDUMP_EXCEPTION_INFORMATION*,
                                  PVOID,PVOID);

// ---------------------------------------------------------------------------
// ---------------------------------- DATA -----------------------------------
// -----|-------------------|-------------------------------------------------

static  BOOL                showCrashInfo   = false;
static  AXApp*              that            = nil;
static  PCSTR               gBaseDir        = nil;
static  PCSTR               gRelease        = nil;

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// FUNCTION
//      TopLevelFilter
// PURPOSE
//
// PARAMETERS
//      struct _EXCEPTION_POINTERS *   pExceptionInfo --
// RESULT
//      LONG  --
// ***************************************************************************
LONG TopLevelFilter(struct _EXCEPTION_POINTERS *pExceptionInfo )
{
    INT                             result      = EXCEPTION_EXECUTE_HANDLER; // EXCEPTION_CONTINUE_SEARCH; //
    HMODULE                         dllHandle   = NULL;
    LPCTSTR                         szResult    = NULL;
    MINIDUMPWRITEDUMP               pDump;
    _MINIDUMP_EXCEPTION_INFORMATION ExInfo;
    HANDLE                          hFile;
    SYSTEMTIME                      systemTime;
    UINT                            count       = 1;
    CHAR                            baseDir     [ AXLPATH   ];
    CHAR                            path        [ AXLPATH   ];
    CHAR                            fname       [ AXLPATH   ];

    ENTER(TRUE);

    if (that)
        that->onCrash();

	if ((dllHandle = ::LoadLibrary("DBGHELP.DLL")) != NULL)
	{
		if ((pDump = (MINIDUMPWRITEDUMP)::GetProcAddress(dllHandle, "MiniDumpWriteDump" )) != NULL)
		{
            GetModuleFileName(NULL, path, AXLPATH);
            fname_get_name(fname, AXLPATH, path);
            fname_change_ext(fname, AXLPATH, "");

            if (gBaseDir)
            {
                strz_sformat(baseDir, AXLPATH, "%s%s%s",
                             gBaseDir,
                             gRelease ? "/" : "",
                             gRelease ? gRelease : "");
            }
            else
                fname_get_path(baseDir, AXLPATH, path);

            if (la6_path_create(baseDir))
            {
                GetLocalTime(&systemTime);

                do
                {
                    snprintf(path, AXLPATH, "%s/%s.%.4d%.2d%.2d%.2d%.2d%.2d-%.8X-%.2d.dmp",
                                 baseDir,
                                 fname,
                                 systemTime.wYear,
                                 systemTime.wMonth,
                                 systemTime.wDay,
                                 systemTime.wHour,
                                 systemTime.wMinute,
                                 systemTime.wSecond,
                                 pExceptionInfo->ExceptionRecord->ExceptionCode,
                                 count++);

                    if (!a7file_exist(path))
                    {
                        if ((hFile = ::CreateFile(path, GENERIC_WRITE, FILE_SHARE_WRITE, NULL,
                                        CREATE_ALWAYS,
                                        FILE_ATTRIBUTE_NORMAL, NULL )) !=INVALID_HANDLE_VALUE)
                        {
                            ExInfo.ThreadId             = ::GetCurrentThreadId();
                            ExInfo.ExceptionPointers    = pExceptionInfo;
                            ExInfo.ClientPointers       = NULL;

                            if (pDump(GetCurrentProcess(),
                                      GetCurrentProcessId(),
                                      hFile, MiniDumpNormal, &ExInfo, NULL, NULL ))
                            {
                                if (showCrashInfo)
                                {
                                    CHAR msg [ AXLTEXT ];

                                    GetModuleFileName(NULL, path, AXLPATH);

                                    strz_sformat(msg, AXLTEXT,
                                             "Please, send the problem description, binary executable file,\n"
                                             "configuration files, log files and all memory dump files\n"
                                             "with '.dmp' extension to software developer.\n"
                                             "Binary executable file: '%s'\n", path);

                                    ::MessageBox(NULL, msg, "Application fatal error",
                                        MB_SYSTEMMODAL | MB_SETFOREGROUND | MB_TOPMOST | MB_ICONHAND);
                                }
                            }

                            ::CloseHandle(hFile);

                            break;
                        }
                    }

                } while (true);
            }
        }

        ::FreeLibrary(dllHandle);
    }

    RETURN(result);
}
static BOOL PreventSetUnhandledExceptionFilter()
{
  HMODULE hKernel32 = LoadLibrary("kernel32.dll");
  if (hKernel32 == NULL) return FALSE;
  void *pOrgEntry = GetProcAddress(hKernel32, "SetUnhandledExceptionFilter");
  if (pOrgEntry == NULL) return FALSE;

#ifdef _M_IX86
  // Code for x86:
  // 33 C0                xor         eax,eax  
  // C2 04 00             ret         4 
  unsigned char szExecute[] = { 0x33, 0xC0, 0xC2, 0x04, 0x00 };
#elif _M_X64
  // 33 C0                xor         eax,eax 
  // C3                   ret  
  unsigned char szExecute[] = { 0x33, 0xC0, 0xC3 };
#else
#error "The following code only works for x86 and x64!"
#endif

  SIZE_T bytesWritten = 0;
  BOOL bRet = WriteProcessMemory(GetCurrentProcess(),
    pOrgEntry, szExecute, sizeof(szExecute), &bytesWritten);
  return bRet;
}
// ***************************************************************************
// FUNCTION
//      AXApp::setCrashHook
// PURPOSE
//
// PARAMETERS
//         void --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL AXApp::setCrashHook(AXApp *       t)
{
    BOOL            b_result        = FALSE;

    ENTER(TRUE);

    if (t != nil)
    {
        ::showCrashInfo = t->dbgShowCrashInfo;
        that            = t;
    }

    ::SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)TopLevelFilter);
    ::PreventSetUnhandledExceptionFilter();

    b_result = TRUE;

    RETURN(b_result);
}

// ***************************************************************************
// FUNCTION
//      AXApp::setCrashDumpDir
// PURPOSE
//
// PARAMETERS
//      PCSTR base        --
//      PCSTR releaseDesc --
// RESULT
//      void --
// ***************************************************************************
void AXApp::setCrashDumpDir(PCSTR   base,
                            PCSTR   releaseDesc,
                            PCSTR   binaryFname)
{
    gBaseDir = base;
    gRelease = releaseDesc;
}

//#endif

