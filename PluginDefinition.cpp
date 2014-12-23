//this file is part of notepad++
//Copyright (C)2003 Don HO <donho@altern.org>
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#include "PluginDefinition.h"
#include "menuCmdID.h"
#include "CommCtrl.h"

//
// The plugin data that Notepad++ needs
//
FuncItem funcItem[nbFunc];

//
// The data of Notepad++ that you can use in your plugin commands
//
NppData nppData;

//
// Initialize your plugin data here
// It will be called while plugin loading   
void pluginInit(HANDLE hModule)
{
}

//
// Here you can do the clean up, save the parameters (if any) for the next session
//
void pluginCleanUp()
{
}

//
// Initialization of your plugin commands
// You should fill your plugins commands here
void commandMenuInit()
{

    //--------------------------------------------//
    //-- STEP 3. CUSTOMIZE YOUR PLUGIN COMMANDS --//
    //--------------------------------------------//
    // with function :
    // setCommand(int index,                      // zero based number to indicate the order of command
    //            TCHAR *commandName,             // the command name that you want to see in plugin menu
    //            PFUNCPLUGINCMD functionPointer, // the symbol of function (function pointer) associated with this command. The body should be defined below. See Step 4.
    //            ShortcutKey *shortcut,          // optional. Define a shortcut to trigger this command
    //            bool check0nInit                // optional. Make this menu item be checked visually
    //            );
    setCommand(0, TEXT("Swap With Previous Tab"), SwapPrev, NULL, false);
    setCommand(1, TEXT("Swap With Next Tab"), SwapNext, NULL, false);
}

//
// Here you can do the clean up (especially for the shortcut)
//
void commandMenuCleanUp()
{
	// Don't forget to deallocate your shortcut here
}


//
// This function help you to initialize your plugin commands
//
bool setCommand(size_t index, TCHAR *cmdName, PFUNCPLUGINCMD pFunc, ShortcutKey *sk, bool check0nInit) 
{
    if (index >= nbFunc)
        return false;

    if (!pFunc)
        return false;

    lstrcpy(funcItem[index]._itemName, cmdName);
    funcItem[index]._pFunc = pFunc;
    funcItem[index]._init2Check = check0nInit;
    funcItem[index]._pShKey = sk;

    return true;
}

//----------------------------------------------//
//-- STEP 4. DEFINE YOUR ASSOCIATED FUNCTIONS --//
//----------------------------------------------//
#define NOTEPADPLUS_USER_INTERNAL     (WM_USER + 0000)
#define NPPM_INTERNAL_DOCORDERCHANGED			(NOTEPADPLUS_USER_INTERNAL + 32)
BOOL CALLBACK SwapTab(HWND hwnd, LPARAM param)
{
	wchar_t name[256];
	GetClassNameW(hwnd, name, sizeof(name)/sizeof(*name));
	if (wcscmp(name, L"SysTabControl32") != 0)
		return TRUE;
	if (!IsWindowVisible(hwnd))
		return TRUE;
	//found

	int count = TabCtrl_GetItemCount(hwnd);
	if (count <= 1)
		return FALSE;

	int src_id = TabCtrl_GetCurSel(hwnd);
	int dst_id = src_id + (int)param;
	if (dst_id < 0)
		dst_id = count - 1;
	if (dst_id >= count)
		dst_id = 0;

	//exchangeItemData
	TabCtrl_SetCurSel(hwnd, dst_id);

	TCITEMW item1, item2;
	item1.mask = item2.mask = TCIF_IMAGE | TCIF_TEXT | TCIF_PARAM;
	
	const int len = 256;
	wchar_t str1[len];
	wchar_t str2[len];
	item1.pszText = str1;
	item1.cchTextMax = len;
	item2.pszText = str2;
	item2.cchTextMax = len;
	TabCtrl_GetItem(hwnd, src_id, &item1);
	TabCtrl_GetItem(hwnd, dst_id, &item2);
	TabCtrl_SetItem(hwnd, src_id, &item2);
	TabCtrl_SetItem(hwnd, dst_id, &item1);
	// Tell Notepad_plus to notifiy plugins that a D&D operation was done (so doc index has been changed)
	::SendMessage(nppData._nppHandle, NPPM_INTERNAL_DOCORDERCHANGED, 0, src_id);

	return FALSE;
}

void SwapPrev()
{
	EnumChildWindows(nppData._nppHandle, SwapTab, -1);
}

void SwapNext()
{
	EnumChildWindows(nppData._nppHandle, SwapTab, 1);
}
