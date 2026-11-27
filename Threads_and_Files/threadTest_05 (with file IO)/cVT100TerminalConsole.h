#ifndef _cVT100TerminalConsole_HG_
#define _cVT100TerminalConsole_HG_

// Written by: Michael Feeney mfeeney (at) fanshawec (dot) ca 
// November 2026
// 
// This is using Microsofts "virtual terminal" API
// https://learn.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences

#include <stdio.h>
#include <wchar.h>
#include <windows.h>
#include <string>

class cVT100TerminalConsole
{
public:
    cVT100TerminalConsole();
    ~cVT100TerminalConsole();

	bool enableVirtualTerminalMode(void);
    bool restoreTerminalMode(void);
    bool bIsVirtualTerminalModeEnabled(void);

    void moveCursorToXY(int column, int row);

    void printCharAtXY(char theChar, int column, int row);
    void printStringAtXY(std::string theString, int column, int row);
    // THESE ARE THREAD SAFE
    void printCharAtXY_MT(char theChar, int column, int row);
    void printStringAtXY_MT(std::string theString, int column, int row);

    void setTextFormat_Default(void);
    void setTextFormat_Bold(bool bEnable);
    void setTextForamt_Underline(bool bEnable);
    void setTextFormat_Negative(bool bEnable);

    void printHorizontalLine(unsigned int left, unsigned int top, unsigned int width);
    void printVerticalLine(unsigned int left, unsigned int top, unsigned int height);
    void printBox(unsigned int left, unsigned int width, 
                  unsigned int top, unsigned int height);

    enum eStandardColours
    {
        BLACK, WHITE,
        RED, GREEN, BLUE,
        YELLOW, CYAN, MAGENTA
    };
    void setTextForeground(eStandardColours theColour, bool bIsBright);
    void setTextForeground_Default(void);
    void setTextBackground(eStandardColours theColour, bool bIsBright);
    void setTextBackground_Default(void);
    // TODO: 38 Foreground Extended	Applies extended color value to the foreground
    // TODO: 48 Background Extended	Applies extended color value to the background

    enum eDECLineDrawingSymbols
    {
        //   Hex   ASCII  DEC Line Drawing
        //   0x6a    j     ┘   "Lower - right corner"
        LOWER_RIGHT_CORNER,
        //   0x6b    k     ┐   "Upper - right corner"
        UPPER_RIGHT_CORNER,
        //   0x6c    l     ┌   Upper - left corner
        UPPER_LEFT_CORNER,
        //   0x6d    m     └   Lower - left corner
        LOWER_LEFT_CORNER,
        //   0x6e    n     ┼   Crossing lines / plus
        CROSSING_LINES_PLUS,
        //   0x71    q     ─   Horizontal line
        HORIZONTAL_LINE,
        //   0x74    t     ├   Left tee(vertical and right)
        LEFT_TEE_VERTICAL_AND_RIGHT,
        //   0x75    u     ┤   Right tee(vertical and left)
        RIGHT_TEE_VERTICAL_AND_LEFT,
        //   0x76    v     ┴   Bottom tee(horizontal and up)
        BOTTOM_TEE_HORIZONTAL_AND_UP,
        //   0x77    w      ┬   Top tee(horizontal and down)
        TOP_TEE_HORIZONTAL_AND_DOWN,
        //   0x78    x     │   Vertical line
        VERTICAL_LINE,
        //  ` (0x60)	◆	Diamond
        DIAMOND,
        //   a(0x61)	▒	Checkerboard / stipple / shade
        CHECKERBOARD_STIPPLE_SHADE,
        //  b(0x62)	␉	Horizontal tab control picture(HT)
        HORIZONTAL_TAB_CONTROL_PICTURE,
        //  c(0x63)	␌	Form feed control picture(FF)
        FORM_FEED_CONTROL_PICTURE,
        //  d(0x64)	␍	Carriage return control picture(CR)
        CARRIAGE_RETURN_CONTROL_PICTURE,
        //  e(0x65)	␊	Line feed control picture(LF)
        LINE_FEED_CONTROL_PICTURE,
        //  f(0x66)	°	Degree symbol
        DEGREE_SYMBOL,
        //  g(0x67)	±	Plus - minus sign
        PLUS_MINUS_SIGN,
        //  h(0x68)	␤	New line control picture(NL)
        NEW_LINE_CONTROL_PICTURE,
        //  i(0x69)	␋	Vertical tab control picture(VT)
        VERTICAL_TAB_CONTROL_PICTURE
    };
    void printDECLineSymbol(eDECLineDrawingSymbols theSymbol);

    void printWhiteHappyFace(void);
    void printBlackHappyFace(void);


    DWORD getLastError(void);
    std::string getLastErrorString(DWORD theError);

private:
    DWORD m_originalTerminalMode = 0;
    bool m_bIsInVTMode = false;

    // From: https://learn.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences
    const std::string m_VT100_ESC = "\x1b";
    const std::string m_VT100_CSI = "\x1b[";

    void m_EnterCriticalSection(void);
    void m_LeaveCriticalSection(void);
};


#endif


