/*
 * Unit test of the IShellFolder functions.
 *
 * Copyright 2004 Vitaliy Margolen
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdarg.h>
#include <stdio.h>

#define COBJMACROS

#include "windef.h"
#include "winbase.h"
#include "wtypes.h"
#include "shellapi.h"


#include "shlguid.h"
#include "shlobj.h"
#include "shobjidl.h"


#include "wine/unicode.h"
#include "wine/test.h"


IMalloc *ppM;

/* creates a file with the specified name for tests */
void CreateTestFile(CHAR *name)
{
    HANDLE file;
    DWORD written;

    file = CreateFileA(name, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
    if (file != INVALID_HANDLE_VALUE)
    {
	WriteFile(file, name, strlen(name), &written, NULL);
	WriteFile(file, "\n", strlen("\n"), &written, NULL);
	CloseHandle(file);
    }
}


/* initializes the tests */
void CreateFilesFolders(void)
{
    CreateDirectoryA(".\\testdir", NULL);
    CreateDirectoryA(".\\testdir\\test.txt", NULL);
    CreateTestFile  (".\\testdir\\test1.txt ");
    CreateTestFile  (".\\testdir\\test2.txt ");
    CreateTestFile  (".\\testdir\\test3.txt ");
    CreateDirectoryA(".\\testdir\\testdir2 ", NULL);
}

/* cleans after tests */
void Cleanup(void)
{
    DeleteFileA(".\\testdir\\test1.txt");
    DeleteFileA(".\\testdir\\test2.txt");
    DeleteFileA(".\\testdir\\test3.txt");
    RemoveDirectoryA(".\\testdir\\test.txt");
    RemoveDirectoryA(".\\testdir\\testdir2");
    RemoveDirectoryA(".\\testdir");
}


/* perform test */
void test_EnumObjects(IShellFolder *iFolder)
{
    IEnumIDList *iEnumList;
    ITEMIDLIST *newPIDL, *(idlArr [5]);
    ULONG NumPIDLs;
    int i=0, j;
    HRESULT nResult;

    static const WORD iResults [5][5] =
    {
	{ 0,-1,-1,-1,-1},
	{ 1, 0,-1,-1, 1},
	{ 1, 1, 0,-1, 1},
	{ 1, 1, 1, 0, 1},
	{ 1,-1,-1,-1, 0}
    };

    if SUCCEEDED(IShellFolder_EnumObjects(iFolder, NULL, SHCONTF_FOLDERS | SHCONTF_NONFOLDERS | SHCONTF_INCLUDEHIDDEN, &iEnumList))
    {
	while (IEnumIDList_Next(iEnumList, 1, &newPIDL, &NumPIDLs) == S_OK)
	{
	    idlArr[i++] = newPIDL;
	}
	/* This fails on windows */
	/* IEnumIDList_Release(iEnumList); */
    
	for (i=0;i<5;i++) for (j=0;j<5;j++)
	{
	    nResult = IShellFolder_CompareIDs(iFolder, 0, idlArr[i], idlArr[j]);
	    ok(nResult == iResults[i][j], "Got %lx expected %x\n", nResult, iResults[i][j]);
	}

	for (i=0;i<5;i++)
	    IMalloc_Free(ppM, idlArr[i]);
    }
}

START_TEST(shlfolder)
{
    ITEMIDLIST *newPIDL;
    IShellFolder *IDesktopFolder, *testIShellFolder;
    WCHAR cCurrDirW [MAX_PATH];
    static const WCHAR cTestDirW[] = {'\\','t','e','s','t','d','i','r',0};

    
    GetCurrentDirectoryW(MAX_PATH, cCurrDirW);
    strcatW(cCurrDirW, cTestDirW);

    
    if(!SUCCEEDED(SHGetMalloc(&ppM)))
	return;

    CreateFilesFolders();
    SHGetDesktopFolder(&IDesktopFolder);

    if (SUCCEEDED(IShellFolder_ParseDisplayName(IDesktopFolder, NULL, NULL, cCurrDirW, NULL, &newPIDL, 0)))
    {
	if (SUCCEEDED(IShellFolder_BindToObject(IDesktopFolder, newPIDL, NULL, (REFIID)&IID_IShellFolder, (LPVOID *)&testIShellFolder)))
	{
	    test_EnumObjects(testIShellFolder);
	    
	    /* This fails on windows */
	    /* IShellFolder_Release(newIShellFolder); */

	    IMalloc_Free(ppM, newPIDL);
	}
    }

    Cleanup();
}
