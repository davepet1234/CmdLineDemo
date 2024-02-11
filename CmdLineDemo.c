/***********************************************************************

 CmdLineDemo.c
 
 Author: David Petrovic
 GitHub: https://github.com/davepet1234/CmdLineDemo

 Demo application to test and demonstate the command line parser.

 Run "CmdLineDemo.efi -h" to get help 

***********************************************************************/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/ShellCEntryLib.h>
#include <Library/MemoryAllocationLib.h>
#include "CmdLineLib/CmdLine.h"

// Parameter variables
#define STR_MAXSIZE 20
CHAR16  Param1[STR_MAXSIZE] = L"default string";
UINT8   Param2  = 0;
UINTN   Param3  = 0;

// Switch variables
typedef enum { ENUM_BLACK, ENUM_RED, ENUM_GREEN, ENUM_BLUE, ENUM_WHITE } ENUM_COLOUR;
typedef enum { ENUM_STR, ENUM_DEC, ENUM_HEX, ENUM_INT } ENUM_INPUT_TYPE;
BOOLEAN     Flag        = FALSE;
UINTN       Flag2       = 0;
ENUM_COLOUR Colour      = ENUM_BLACK;
UINT8       DecValue    = 0;
UINTN       HexValue    = 0;
UINT16      IntValue    = 0;
CHAR16      StringValue[STR_MAXSIZE] = L"not initialised";
UINT16      Lines       = 0;
ENUM_INPUT_TYPE InputType = ENUM_STR;
CHAR16      KeyPressList[STR_MAXSIZE] = L"";

// Flags set when switch specified
// Note that for switches with a value this is option by using macros with the '_FLGD' suffix
BOOLEAN ColourPresent;
BOOLEAN DecValPresent;
BOOLEAN HexValPresent;
BOOLEAN IntValPresent;
BOOLEAN StrValPresent;
BOOLEAN LinesPresent;
BOOLEAN InputPresent;
BOOLEAN KeyPressPresent;

// Main program help
CHAR16 ProgName[]       = L"CmdLine";
CHAR16 ProgHelpStr[]    = L"Application to test command line parser";

// Parameter table defines 3 arguments
PARAMTABLE_START(ParamTable)
PARAMTABLE_STR(Param1, STR_MAXSIZE, L"[str]string parameter")
PARAMTABLE_HEX8(&Param2,            L"[num1]hexidecimal parameter")
PARAMTABLE_DEC(&Param3,             L"[num2]decimal parameter")
PARAMTABLE_END

// String definitions for enum switch
ENUMSTR_START(EnumColourStrs)
ENUMSTR_ENTRY(ENUM_BLACK,  L"black")
ENUMSTR_ENTRY(ENUM_RED,    L"red")
ENUMSTR_ENTRY(ENUM_GREEN,  L"green")
ENUMSTR_ENTRY(ENUM_BLUE,   L"blue")
ENUMSTR_ENTRY(ENUM_WHITE,  L"white")
ENUMSTR_END

// String definitions for input switch
ENUMSTR_START(InputTypeStrs)
ENUMSTR_ENTRY(ENUM_BLACK,  L"str")
ENUMSTR_ENTRY(ENUM_RED,    L"dec")
ENUMSTR_ENTRY(ENUM_GREEN,  L"hex")
ENUMSTR_ENTRY(ENUM_BLUE,   L"int")
ENUMSTR_END

// Switch table defines 7 switches
SWTABLE_START(SwitchTable)
// test switches
SWTABLE_OPT_FLAG(       L"-f",  NULL,           &Flag,                                          L"boolean flag")
SWTABLE_OPT_FLGVAL(     NULL,   L"-flag2",      &Flag2, 12345678,                               L"flag with default value assigned")
SWTABLE_OPT_ENUM_FLGD(  L"-c",  L"-colour",     &ColourPresent,     &Colour, EnumColourStrs,    L"[val]named option")
SWTABLE_MAN_DEC8_FLGD(  L"-d",  L"-dec",        &DecValPresent,     &DecValue,                  L"[num]decimal value (0-255)")
SWTABLE_OPT_HEX_FLGD(   L"-x",  L"-hex",        &HexValPresent,     &HexValue,                  L"[num]hexidecimal value")
SWTABLE_OPT_INT16_FLGD( L"-i",  NULL,           &IntValPresent,     &IntValue,                  L"[num]integer value")
SWTABLE_OPT_STR_FLGD(   L"-s",  L"-string",     &StrValPresent,     StringValue, STR_MAXSIZE,   L"[str]string value")
// switches to test user interaction
SWTABLE_OPT_INT16_FLGD( L"-l",  L"-lines",      &LinesPresent,      &Lines,                     L"[num]display 'num' lines - test break/abort")
SWTABLE_OPT_ENUM_FLGD(  NULL,   L"-input",      &InputPresent,      &InputType, InputTypeStrs,  L"[type]test User input")
SWTABLE_OPT_STR_FLGD(   NULL,   L"-key",        &KeyPressPresent,   KeyPressList, STR_MAXSIZE,  L"[keylist]test key press")
SWTABLE_END

//---------------------------
// Main entry point
//---------------------------

INTN EFIAPI ShellAppMain(IN UINTN Argc, IN CHAR16 **Argv)
{
    SHELL_STATUS ShellStatus = SHELL_SUCCESS;
    UINTN ParamCount;

    // variable used for user input testing
    EFI_STATUS InputStatus;
    CONST UINTN BUFF_LEN=128;
    CHAR16 UserString[BUFF_LEN];
    UserString[0] = '\0';
    UINTN UserNumber = 0;
    EFI_STATUS KeyStatus;
    CHAR16 KeyPressed = 0;

    // Parse the command line

    //SetProgName(L"cmdlinetest");
    ShellStatus = ParseCmdLine(ParamTable, 1, SwitchTable, ProgHelpStr, NO_OPT, &ParamCount);
    if (ShellStatus == SHELL_ABORTED){
        goto Error_exit;
    }

    // No command line errors so handle additioanl tests
    if (ShellStatus == SHELL_SUCCESS) {

        // break switch '-b' test
        if (LinesPresent) {
            for (UINT16 i=0; i<Lines; i++) {
                ShellPrintEx(-1, -1, L"Line %u\n", i);
                if (CheckProgAbort(TRUE)) break;
            }
        }

        // User input test
        if (InputPresent) {
            switch (InputType) {
            case ENUM_STR:
                InputStatus = StringInput(UserString, BUFF_LEN, L"Enter string: ");
                break;
            case ENUM_DEC:
                InputStatus = DecimalInput(&UserNumber, L"Enter decimal number: ");
                break;
            case ENUM_HEX:
                InputStatus = HexidecimalInput(&UserNumber, L"Enter hexidecimal number: ");
                break;
            case ENUM_INT:
                InputStatus = IntegerInput(&UserNumber, L"Enter integer number: ");
                break;
            default:
                break;
            }
        }

        // Key press test
        if (KeyPressPresent) {
            if (StrLen(KeyPressList) == 0) {
                KeyStatus = WaitKeyPress(&KeyPressed, KeyPressList, L"Press any key", KEY_NOOPT);
            } else {
                KeyStatus = WaitKeyPress(&KeyPressed, KeyPressList, L"Press key in list", KEY_LIST | KEY_ECHO);
            }
        }
    }

    // Display parmaeter and switch status
    ShellPrintEx(-1, -1, L"========================================\n");
    if (ShellStatus == SHELL_SUCCESS) {
        ShellPrintEx(-1, -1, L"Parameters (%u):\n", ParamCount);
        ShellPrintEx(-1, -1, L"%c Param1      = '%s'\n",        ParamCount >= 1 ? '*':' ',  Param1);
        ShellPrintEx(-1, -1, L"%c Param2      = %d, 0x%02x\n",  ParamCount >= 2 ? '*':' ',  Param2, Param2);
        ShellPrintEx(-1, -1, L"%c Param3      = %d, 0x%02x\n",  ParamCount >= 3 ? '*':' ',  Param3, Param3);
        ShellPrintEx(-1, -1, L"Options:\n");
        ShellPrintEx(-1, -1, L"%c Flag        = %u\n",          Flag ? '*':' ',             Flag);
        ShellPrintEx(-1, -1, L"%c Flag2       = %u\n",          (Flag2 != 0)  ? '*':' ',    Flag2);
        ShellPrintEx(-1, -1, L"%c Colour      = %u\n",          ColourPresent ? '*':' ',    Colour);
        ShellPrintEx(-1, -1, L"%c DecValue    = %u\n",          DecValPresent ? '*':' ',    DecValue);
        ShellPrintEx(-1, -1, L"%c HexValue    = %u, 0x%02x\n",  HexValPresent ? '*':' ',    HexValue, HexValue);
        ShellPrintEx(-1, -1, L"%c IntValue    = %u, 0x%02x\n",  IntValPresent ? '*':' ',    IntValue, IntValue);
        ShellPrintEx(-1, -1, L"%c StringValue = '%s'\n",        StrValPresent ? '*':' ',    StringValue);
        ShellPrintEx(-1, -1, L"%c Lines       = %u\n",          LinesPresent  ? '*':' ',    Lines);
        ShellPrintEx(-1, -1, L"%c Input       = %u\n",          InputPresent  ? '*':' ',    InputType);
        ShellPrintEx(-1, -1, L"%c Key         = '%s'\n",        KeyPressPresent  ? '*':' ', KeyPressList);
    }
    ShellPrintEx(-1, -1, L"ShellStatus   = %d\n", ShellStatus);
    ShellPrintEx(-1, -1, L"========================================\n");

    // Display user input test results
    if (InputPresent || KeyPressPresent) {
        if (InputPresent) {
            switch (InputType) {
            case ENUM_STR:
                ShellPrintEx(-1, -1, L"String Input: '%s' - %r\n", UserString, InputStatus);
                break;
            case ENUM_DEC:
            case ENUM_HEX:
            case ENUM_INT:
                ShellPrintEx(-1, -1, L"Number Input: %d, 0x%02X - %r\n", UserNumber, UserNumber, InputStatus);
                break;
            default:
                break;
            }
        }
        if (KeyPressPresent) {
            ShellPrintEx(-1, -1, L"Key Pressed: %d:'%c' - %r\n", KeyPressed, KeyPressed, KeyStatus);
        }
        ShellPrintEx(-1, -1, L"========================================\n");
    }

Error_exit:
    return ShellStatus;
}
