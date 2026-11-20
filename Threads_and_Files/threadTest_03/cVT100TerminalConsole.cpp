#include "cVT100TerminalConsole.h"

bool cVT100TerminalConsole::enableVirtualTerminalMode(void)
{
    // Set output mode to handle virtual terminal sequences
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    this->m_originalTerminalMode = 0;
    if (!GetConsoleMode(hOut, &(this->m_originalTerminalMode)))
    {
        return false;
    }

    DWORD newTerminalMode = this->m_originalTerminalMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING;

    if (!SetConsoleMode(hOut, newTerminalMode))
    {
        return false;
    }

    this->m_bIsInVTMode = true;
    return true;
}

bool cVT100TerminalConsole::restoreTerminalMode(void)
{
    // Set output mode to handle virtual terminal sequences
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    if (!SetConsoleMode(hOut, this->m_originalTerminalMode))
    {
        return false;
    }

    return true;
}

bool cVT100TerminalConsole::bIsVirtualTerminalModeEnabled(void)
{
    return this->m_bIsInVTMode;
}


DWORD cVT100TerminalConsole::getLastError(void)
{
	// TODO: 
	return 0;
}

std::string cVT100TerminalConsole::getLastErrorString(DWORD theError)
{
	// TODO:
	return "";
}
