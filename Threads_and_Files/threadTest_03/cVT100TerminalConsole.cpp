#include "cVT100TerminalConsole.h"
#include <sstream>
#include <io.h>
#include <fcntl.h>

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


void cVT100TerminalConsole::moveCursorToXY(int column, int row)
{
    if (!this->m_bIsInVTMode)   { return; }

    // From: https://learn.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences
    std::stringstream ssTermSeq;
    ssTermSeq << this->m_VT100_CSI << (row + 1) << ';' << (column + 1) << 'H';
    printf(ssTermSeq.str().c_str());

    return;
}


void cVT100TerminalConsole::printCharAtXY(char theChar, int column, int row)
{
    if (!this->m_bIsInVTMode)   { return; }

    this->moveCursorToXY(column, row);
    printf("%c", theChar);

    return;
}




void cVT100TerminalConsole::setTextFormat_Default(void)
{
    if (!this->m_bIsInVTMode) { return; }

    // ESC [ <n> m
    // 0	Default	Returns all attributes to the default state prior to modification
    std::stringstream ssTermSeq;
    ssTermSeq << this->m_VT100_ESC << "[0m";
    printf(ssTermSeq.str().c_str());

    return;
}

void cVT100TerminalConsole::setTextFormat_Bold(bool bEnable)
{
    if (!this->m_bIsInVTMode) { return; }

    // ESC [ <n> m

    std::stringstream ssTermSeq;
    ssTermSeq << this->m_VT100_ESC << '[';
    // 1	Bold / Bright	Applies brightness / intensity flag to foreground color
    // 22	No bold / bright	Removes brightness / intensity flag from foreground color
    ssTermSeq << (bEnable ? '1' : '22');
    ssTermSeq << 'm';    
    printf(ssTermSeq.str().c_str());

    return;
}

void cVT100TerminalConsole::setTextForamt_Underline(bool bEnable)
{
    if (!this->m_bIsInVTMode) { return; }

    // ESC [ <n> m
    // ESC [ <n> m

    std::stringstream ssTermSeq;
    ssTermSeq << this->m_VT100_ESC << '[';
    //    4	Underline	Adds underline
    //    24	No underline	Removes underline
    ssTermSeq << (bEnable ? '4' : '24');
    ssTermSeq << 'm';
    printf(ssTermSeq.str().c_str());

    return;
}

void cVT100TerminalConsole::setTextFormat_Negative(bool bEnable)
{
    if (!this->m_bIsInVTMode) { return; }

    // ESC [ <n> m
    std::stringstream ssTermSeq;
    ssTermSeq << this->m_VT100_ESC << '[';
    //    7	Negative	Swaps foreground and background colors
    //    27	Positive(No negative)	Returns foreground / background to normal
    ssTermSeq << (bEnable ? '7' : '27');
    ssTermSeq << 'm';
    printf(ssTermSeq.str().c_str());


    return;
}

void cVT100TerminalConsole::setTextForeground(eStandardColours theColour, bool bIsBright)
{
    if (!this->m_bIsInVTMode) { return; }

    // ESC [ <n> m
    std::stringstream ssTermSeq;
    ssTermSeq << this->m_VT100_ESC << '[';

    if (bIsBright)
    {
        switch (theColour)  // eStandardColours
        {
        case BLACK:
            //    90	Bright Foreground Black	Applies bold / bright black to foreground
            ssTermSeq << "90";
            break;
        case WHITE:
            //    97	Bright Foreground White	Applies bold / bright white to foreground
            ssTermSeq << "97";
            break;
        case RED:
            //    91	Bright Foreground Red	Applies bold / bright red to foreground
            ssTermSeq << "91";
            break;
        case GREEN:
            //    92	Bright Foreground Green	Applies bold / bright green to foreground
            ssTermSeq << "92";
            break;
        case BLUE:
            //    94	Bright Foreground Blue	Applies bold / bright blue to foreground
            ssTermSeq << "94";
            break;
        case YELLOW:
            //    93	Bright Foreground Yellow	Applies bold / bright yellow to foreground
            ssTermSeq << "93";
            break;
        case CYAN:
            //    96	Bright Foreground Cyan	Applies bold / bright cyan to foreground
            ssTermSeq << "96";
            break;
        case MAGENTA:
            //    95	Bright Foreground Magenta	Applies bold / bright magenta to foreground
            ssTermSeq << "95";
            break;
        default:
            // Shouldn't happen, but an invalid code, anyway
            return;
            break;
        }//switch (theColour)
    }
    else
    {
        switch (theColour)  // eStandardColours
        {
        case BLACK:
            //   30	Foreground Black	Applies non - bold / bright black to foreground
            ssTermSeq << "30";
            break;
        case WHITE:
            //   37	Foreground White	Applies non - bold / bright white to foreground
            ssTermSeq << "37";
            break;
        case RED:
            //   31	Foreground Red	Applies non - bold / bright red to foreground
            ssTermSeq << "31";
            break;
        case GREEN:
            //   32	Foreground Green	Applies non - bold / bright green to foreground
            ssTermSeq << "32";
            break;
        case BLUE:
            //   34	Foreground Blue	Applies non - bold / bright blue to foreground
            ssTermSeq << "34";
            break;
        case YELLOW:
            //   33	Foreground Yellow	Applies non - bold / bright yellow to foreground
            ssTermSeq << "33";
            break;
        case CYAN:
            //   36	Foreground Cyan	Applies non - bold / bright cyan to foreground
            ssTermSeq << "36";
            break;
        case MAGENTA:
            //   35	Foreground Magenta	Applies non - bold / bright magenta to foreground
            ssTermSeq << "35";
            break;
        default:
            // Shouldn't happen, but an invalid code, anyway
            return;
            break;
        }//switch (theColour)
    }

    ssTermSeq << 'm';
    printf(ssTermSeq.str().c_str());

    return;
}

void cVT100TerminalConsole::setTextForeground_Default(void)
{
    if (!this->m_bIsInVTMode) { return; }

    // ESC [ <n> m
    // 39 Foreground Default (Applies only the foreground portion of the defaults)
    std::stringstream ssTermSeq;
    ssTermSeq << this->m_VT100_ESC << "[039m";
    printf(ssTermSeq.str().c_str());

    return;
}

void cVT100TerminalConsole::setTextBackground(eStandardColours theColour, bool bIsBright)
{
    if (!this->m_bIsInVTMode) { return; }

    // ESC [ <n> m
    std::stringstream ssTermSeq;
    ssTermSeq << this->m_VT100_ESC << '[';


    if (bIsBright)
    {
        switch (theColour)  // eStandardColours
        {
        case BLACK:
            //    100	Bright Background Black	Applies bold / bright black to background
            ssTermSeq << "100";
            break;
        case WHITE:
            //    107	Bright Background White	Applies bold / bright white to background
            ssTermSeq << "107";
            break;
        case RED:
            //    101	Bright Background Red	Applies bold / bright red to background
            ssTermSeq << "101";
            break;
        case GREEN:
            //    102	Bright Background Green	Applies bold / bright green to background
            ssTermSeq << "102";
            break;
        case BLUE:
            //    104	Bright Background Blue	Applies bold / bright blue to background
            ssTermSeq << "104";
            break;
        case YELLOW:
            //    103	Bright Background Yellow	Applies bold / bright yellow to background
            ssTermSeq << "103";
            break;
        case CYAN:
            //    106	Bright Background Cyan	Applies bold / bright cyan to background
            ssTermSeq << "106";
            break;
        case MAGENTA:
            //    105	Bright Background Magenta	Applies bold / bright magenta to background
            ssTermSeq << "105";
            break;
        default:
            // Shouldn't happen, but an invalid code, anyway
            return;
            break;
        }//switch (theColour)
    }
    else
    {
        switch (theColour)  // eStandardColours
        {
        case BLACK:
            //    40	Background Black	Applies non - bold / bright black to background
            ssTermSeq << "40";
            break;
        case WHITE:
        //    47	Background White	Applies non - bold / bright white to background
            ssTermSeq << "47";
            break;
        case RED:
            //    41	Background Red	Applies non - bold / bright red to background
            ssTermSeq << "41";
            break;
        case GREEN:
        //    42	Background Green	Applies non - bold / bright green to background
            ssTermSeq << "42";
            break;
        case BLUE:
        //    44	Background Blue	Applies non - bold / bright blue to background
            ssTermSeq << "44";
            break;
        case YELLOW:
        //    43	Background Yellow	Applies non - bold / bright yellow to background
            ssTermSeq << "43";
            break;
        case CYAN:
            //    46	Background Cyan	Applies non - bold / bright cyan to background
            ssTermSeq << "46";
            break;
        case MAGENTA:
            //    45	Background Magenta	Applies non - bold / bright magenta to background
            ssTermSeq << "45";
            break;
        default:
            // Shouldn't happen, but an invalid code, anyway
            return;
            break;
        }//switch (theColour)
    }

    ssTermSeq << 'm';
    printf(ssTermSeq.str().c_str());

    return;
}

void cVT100TerminalConsole::setTextBackground_Default(void)
{
    if (!this->m_bIsInVTMode) { return; }

    // ESC [ <n> m
    // 49 Background Default (Applies only the background portion of the defaults)

    std::stringstream ssTermSeq;
    ssTermSeq << this->m_VT100_ESC << "[049m";
    printf(ssTermSeq.str().c_str());

    return;
}

void cVT100TerminalConsole::printDECLineSymbol(eDECLineDrawingSymbols theSymbol)
{
    if (!this->m_bIsInVTMode) { return; }

    // https://learn.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences

    std::stringstream ssTermSeq;
    //    ESC(0     Designate Character Set – DEC Line Drawing	Enables DEC Line Drawing Mode
    ssTermSeq << this->m_VT100_ESC << "(0";

    //  Hex   ASCII  DEC Line Drawing
    switch (theSymbol)
    {
    case LOWER_RIGHT_CORNER:
        //   0x6a    j     ┘   "Lower - right corner"
        ssTermSeq << 'j';   // 0x6a;
        break;
    case UPPER_RIGHT_CORNER:
        //   0x6b    k     ┐   "Upper - right corner"
        ssTermSeq << 'k';   // 0x6b;
        break;
    case UPPER_LEFT_CORNER:
        //   0x6c    l     ┌   Upper - left corner
        ssTermSeq << 'l';   // 0x6c;
        break;
    case LOWER_LEFT_CORNER:
        //   0x6d    m     └   Lower - left corner
        ssTermSeq << 'm';   // 0x6d;
        break;
    case CROSSING_LINES_PLUS:
        //   0x6e    n     ┼   Crossing lines / plus
        ssTermSeq << 'n';   // 0x6e;
        break;
    case HORIZONTAL_LINE:
        //   0x71    q     ─   Horizontal line
        ssTermSeq << 'q';   // 0x71;
        break;
    case LEFT_TEE_VERTICAL_AND_RIGHT:
        //   0x74    t     ├   Left tee(vertical and right)
        ssTermSeq << 't';   // 0x74;
        break;
    case RIGHT_TEE_VERTICAL_AND_LEFT:
        //   0x75    u     ┤   Right tee(vertical and left)
        ssTermSeq << 'u';   // 0x75;
        break;
    case BOTTOM_TEE_HORIZONTAL_AND_UP:
        //   0x76    v     ┴   Bottom tee(horizontal and up)
        ssTermSeq << 'v';   // 0x76;
        break;
    case TOP_TEE_HORIZONTAL_AND_DOWN:
        //   0x77    w      ┬   Top tee(horizontal and down)
        ssTermSeq << 'w';   // 0x77;
        break;
    case VERTICAL_LINE:
        //   0x78    x     │   Vertical line
        ssTermSeq << 'x';   // 0x78;
        break;

    case DIAMOND:
        //  ` (0x60)	◆	Diamond
        ssTermSeq << '`';   // 0x60;
        break;
    case CHECKERBOARD_STIPPLE_SHADE:
        //   a  (0x61)	▒	Checkerboard / stipple / shade
        ssTermSeq << 'a';   // 0x61;
        break;
    case HORIZONTAL_TAB_CONTROL_PICTURE:
        //  b   (0x62)	␉	Horizontal tab control picture(HT)
        ssTermSeq << 'b';   // 0x62;
        break;
    case FORM_FEED_CONTROL_PICTURE:
        //  c   (0x63)	␌	Form feed control picture(FF)
        ssTermSeq << 'c';   // 0x63;
        break;
    case CARRIAGE_RETURN_CONTROL_PICTURE:
        //  d   (0x64)	␍	Carriage return control picture(CR)
        ssTermSeq << 'd';   // 0x64;
        break;
    case LINE_FEED_CONTROL_PICTURE:
        //  e   (0x65)	␊	Line feed control picture(LF)
        ssTermSeq << 'e';   // 0x65;
        break;
    case DEGREE_SYMBOL:
        //  f   (0x66)	°	Degree symbol
        ssTermSeq << 'f';   // 0x66;
        break;
    case PLUS_MINUS_SIGN:
        //  g   (0x67)	±	Plus - minus sign
        ssTermSeq << 'g';   // 0x67;
        break;
    case NEW_LINE_CONTROL_PICTURE:
        //  h   (0x68)	␤	New line control picture(NL)
        ssTermSeq << 'h';   // 0x68;
        break;
    case VERTICAL_TAB_CONTROL_PICTURE:
        //  i   (0x69)	␋	Vertical tab control picture(VT)
        ssTermSeq << 'i';   // 0x69;
        break;

    default:
        // Shouldn't happen, but an invalid code, anyway
        return;
        break;
    };//switch (theSymbol)

    //    ESC(B     Designate Character Set – US ASCII	Enables ASCII Mode(Default)
    ssTermSeq << this->m_VT100_ESC << "(B";
    printf(ssTermSeq.str().c_str());

    return;
}

void cVT100TerminalConsole::printWhiteHappyFace(void)
{
    fflush(stdout);
    int prevMode = _setmode(_fileno(stdout), _O_U16TEXT);
    wprintf(L"\u263A"); 
    fflush(stdout);
    if (prevMode != -1) {
        prevMode = _setmode(_fileno(stdout), prevMode);
    }
    return;
}

void cVT100TerminalConsole::printBlackHappyFace(void)
{
    fflush(stdout);
    int prevMode = _setmode(_fileno(stdout), _O_U16TEXT);
    wprintf(L"\u263B");
    fflush(stdout);
    if (prevMode != -1) {
        prevMode = _setmode(_fileno(stdout), prevMode);
    }
    return;
}

