#ifndef _cVT100TerminalConsole_HG_
#define _cVT100TerminalConsole_HG_

// This is using Microsofts "virtual terminal" API
// https://learn.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences

#include <stdio.h>
#include <wchar.h>
#include <windows.h>
#include <string>

class cVT100TerminalConsole
{
public:
	bool enableVirtualTerminalMode(void);
    bool restoreTerminalMode(void);
    bool bIsVirtualTerminalModeEnabled(void);

    DWORD getLastError(void);
    std::string getLastErrorString(DWORD theError);

private:
    DWORD m_originalTerminalMode = 0;
    bool m_bIsInVTMode = false;
};


#endif


