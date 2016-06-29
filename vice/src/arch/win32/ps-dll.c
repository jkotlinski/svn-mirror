/*
 * ps-dll.c - DLL based PARallel port SID support for WIN32.
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
 *
 * This file is part of VICE, the Versatile Commodore Emulator.
 * See README for copyright notice.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA.
 *
 */

/* Tested and confirmed working on:
 */

#include "vice.h"

#ifdef HAVE_PARSID
#include <windows.h>
#include <stdio.h>

#include "log.h"
#include "parsid.h"
#include "ps.h"
#include "types.h"

#define MAXSID 3

static int sids_found = -1;
static int pssids[MAXSID] = {-1, -1, -1};
static int psctrl[MAXSID] = {-1, -1, -1};
static int parsid_port_address[MAXSID] = {0, 0, 0};

#ifndef MSVC_RC
typedef short _stdcall (*inpfuncPtr)(short portaddr);
typedef void _stdcall (*oupfuncPtr)(short portaddr, short datum);

static inpfuncPtr inp32fp;
static oupfuncPtr oup32fp;
#else
typedef short (CALLBACK* Inp32_t)(short);
typedef void (CALLBACK* Out32_t)(short, short);

static Inp32_t Inp32;
static Out32_t Out32;
#endif

/* input/output functions */
static void parsid_outb(unsigned int addrint, BYTE value)
{
    WORD addr = (WORD)addrint;

#ifndef MSVC_RC
   (oup32fp)(addr, value);
#else
    Out32(addr, value);
#endif
}

static BYTE parsid_inb(unsigned int addrint)
{
    WORD addr = (WORD)addrint;

#ifndef MSVC_RC
    return (BYTE)(inp32fp)(addr);
#else
    return (BYTE)Inp32(addr);
#endif
}

void ps_dll_out_ctr(BYTE parsid_ctrport, int chipno)
{
    if (chipno < MAXSID && pssids[chipno] != -1) {
        parsid_outb(pssids[chipno] + 2, parsid_ctrport);
        psctrl[chipno] = parsid_ctrport;
    }
}

BYTE ps_dll_in_ctr(int chipno)
{
    if (chipno < MAXSID && pssids[chipno] != -1) {
        if (psctrl[chipno] == -1) {
            parsid_outb(pssids[chipno] + 2, 0);
            psctrl[chipno] = 0;
        } else {
            return psctrl[chipno];
        }
    }
    return 0;
}

void ps_dll_out_data(BYTE outval, int chipno)
{
    if (chipno < MAXSID && pssids[chipno] != -1) {
        parsid_outb(pssids[chipno], outval);
    }
}

BYTE ps_dll_in_data(int chipno)
{
    if (chipno < MAXSID && pssids[chipno] != -1) {
        return parsid_inb(pssids[chipno]);
    }
    return 0;
}

/*----------------------------------------------------------------------*/

/* original code by Peter Burke, modified by me. */
static int parsid_GetParallelControllerKey(char *parKey)
{
    HKEY hKey, hKey1, hKey2;
    char myData[255];
    LONG res;
    DWORD mySize, dwIndex1, dwIndex2, dwIndex3;
    FILETIME ftLastWriteTime;
    char myKey[255], myKey1[255], myKey2[255];

    if (NULL == parKey) {
        return -1;
    }

    *parKey = 0;

    sprintf(myKey, "HARDWARE\\DESCRIPTION\\System");

    res = RegOpenKeyEx(HKEY_LOCAL_MACHINE, myKey, 0, KEY_READ, &hKey);

    if (res != ERROR_SUCCESS) {
        return -1;
    }

    for (dwIndex1 = 0; dwIndex1 <= 10; dwIndex1++) {
        mySize = sizeof(myData);
        res = RegEnumKeyEx(hKey, dwIndex1, myData, &mySize, NULL, NULL, NULL, &ftLastWriteTime);

        if (res == ERROR_SUCCESS) {
            strcpy(myKey1, myKey);
            strcat(myKey1, "\\");
            strcat(myKey1, myData);

            res = RegOpenKeyEx(HKEY_LOCAL_MACHINE, myKey1, 0, KEY_READ, &hKey1);

            if (res != ERROR_SUCCESS) {
                return -1;
            }

            for (dwIndex2 = 0; dwIndex2 <= 10; dwIndex2++) {
                mySize = sizeof(myData);
                res = RegEnumKeyEx(hKey1, dwIndex2, myData, &mySize, NULL, NULL, NULL, &ftLastWriteTime);

                if (res == ERROR_SUCCESS) {
                    strcpy(myKey2, myKey1);
                    strcat(myKey2, "\\");
                    strcat(myKey2, myData);

                    res = RegOpenKeyEx(HKEY_LOCAL_MACHINE, myKey2, 0, KEY_READ, &hKey2);

                    if (res != ERROR_SUCCESS) {
                        return -1;
                    }

                    for (dwIndex3 = 0; dwIndex3 <= 10; dwIndex3++) {
                        mySize = sizeof(myData);
                        res = RegEnumKeyEx(hKey2, dwIndex3, myData, &mySize, NULL, NULL, NULL, &ftLastWriteTime);

                        if (res == ERROR_SUCCESS) {
                            if (0 == strcmp(myData, "ParallelController")) {
                                strcpy(parKey, myKey2);
                                strcat(parKey, "\\");
                                strcat(parKey, myData);
                                return 0;
                            }
                        }
                    }
                }
            }
        }
    }
    return -1;
}

static int parsid_GetAddressLptPortInTheRegistry(int myPort)
{
    HKEY phkResult;
    char myKey[255], myData[255];
    LONG res;
    DWORD mySize, myType;

    res = parsid_GetParallelControllerKey(myKey);
    if (res < 0) {
        return -1;
    }

    sprintf(myData, "%s\\%d", myKey, myPort - 1);

    res = RegOpenKeyEx(HKEY_LOCAL_MACHINE, myData, 0, KEY_READ, &phkResult);
    if (res != ERROR_SUCCESS) {
        return -1;
    }

    mySize = sizeof(myData);
    myType = REG_BINARY;

    res = RegQueryValueEx(phkResult, "Configuration Data", NULL, &myType, (unsigned char *)myData, &mySize);
    if (res != ERROR_SUCCESS) {
        return -1;
    }

    return (myData[0x14] | myData[0x15] << 8);
}
         
typedef BOOL (CALLBACK * PROCTYPE_Toolhelp32ReadProcessMemory)(DWORD, LPCVOID, LPVOID, DWORD, LPDWORD);

static int parsid_GetAddressLptPortInTheMemory(int myPort)
{
    HINSTANCE hDLL = NULL;
    PROCTYPE_Toolhelp32ReadProcessMemory myProcPointer = NULL;
    int portAddresses[] = { 0, 0, 0, 0, 0 };
    BOOL rtn = 0;

    hDLL = LoadLibrary("kernel32");
    if (hDLL == NULL) {
        return -1;
    }

    myProcPointer = (PROCTYPE_Toolhelp32ReadProcessMemory)GetProcAddress(hDLL, "Toolhelp32ReadProcessMemory");
    if (myProcPointer == NULL) {
        FreeLibrary(hDLL);
        return -1;
    }

    rtn = myProcPointer(0, (LPCVOID *)0x408, portAddresses, 8, NULL);
    FreeLibrary(hDLL);

    if (rtn == 0) {
        return -1;
    }

    if (portAddresses[myPort - 1] <= 0 || portAddresses[myPort - 1] >= 0x1000) {
        return -1;
    }

    return portAddresses[myPort - 1];
}

static int parsid_GetAddressLptPort(int myPort)
{
    int retval;

    if (myPort < 1 || myPort > 3) {
        return -1;
    }

    if (!(GetVersion() & 0x80000000)) {
        retval = parsid_GetAddressLptPortInTheRegistry(myPort);
    } else {
        retval = parsid_GetAddressLptPortInTheMemory(myPort);
    }
    log_message(LOG_DEFAULT, "Address of parallel port %d is $%X.", myPort, retval);
    return retval;
}

/*----------------------------------------------------------------------*/

static HINSTANCE hLib = NULL;

#ifdef _MSC_VER
#  ifdef _WIN64
#    define INPOUTDLLNAME "inpoutx64.dll"
#  else
#    define INPOUTDLLNAME "inpout32.dll"
#  endif
#else
#  if defined(__amd64__) || defined(__x86_64__)
#    define INPOUTDLLNAME "inpoutx64.dll"
#  else
#    define INPOUTDLLNAME "inpout32.dll"
#  endif
#endif

static BYTE detect_sid_read(int chipno, BYTE addr)
{
    BYTE value = 0;
    BYTE ctl = ps_dll_in_ctr(chipno);

    ps_dll_out_data((BYTE)(addr & 0x1f), chipno);
    
    ctl &= ~parsid_AUTOFEED;
    ps_dll_out_ctr(ctl, chipno);

    ctl |= parsid_AUTOFEED;
    ps_dll_out_ctr(ctl, chipno);

    ctl |= parsid_PCD;
    ps_dll_out_ctr(ctl, chipno);

    ctl |= parsid_nINIT;
    ps_dll_out_ctr(ctl, chipno);

    ctl |= parsid_STROBE;
    ps_dll_out_ctr(ctl, chipno);

    value = ps_dll_in_data(chipno);

    ctl &= ~parsid_STROBE;
    ps_dll_out_ctr(ctl, chipno);

    ctl &= ~parsid_PCD;
    ps_dll_out_ctr(ctl, chipno);

    ctl &= ~parsid_nINIT;
    ps_dll_out_ctr(ctl, chipno);

    return value;
}

static void detect_sid_store(int chipno, BYTE addr, BYTE outval)
{
    BYTE ctl = ps_dll_in_ctr(chipno);

    ps_dll_out_data((BYTE)(addr & 0x1f), chipno);

    ctl &= ~parsid_AUTOFEED;
    ps_dll_out_ctr(ctl, chipno);

    ctl |= parsid_AUTOFEED;
    ps_dll_out_ctr(ctl, chipno);

    ps_dll_out_data(outval, chipno);

    ctl |= parsid_STROBE;
    ps_dll_out_ctr(ctl, chipno);

    ctl &= ~parsid_STROBE;
    ps_dll_out_ctr(ctl, chipno);
}

static int detect_sid(int port)
{
    int i;

    psctrl[port] = -1;

    for (i = 0x18; i >= 0; --i) {
        detect_sid_store(port, (BYTE)i, 0);
    }

    detect_sid_store(port, 0x12, 0xff);

    for (i = 0; i < 100; ++i) {
        if (detect_sid_read(port, 0x1b)) {
            return 0;
        }
    }

    detect_sid_store(port, 0x0e, 0xff);
    detect_sid_store(port, 0x0f, 0xff);
    detect_sid_store(port, 0x12, 0x20);

    for (i = 0; i < 100; ++i) {
        if (detect_sid_read(port, 0x1b)) {
            return 1;
        }
    }
    return 0;
}

int ps_dll_open(void)
{
    int j;

    if (!sids_found) {
        return -1;
    }

    if (sids_found > 0) {
        return 0;
    }

    sids_found = 0;

    log_message(LOG_DEFAULT, "Detecting dll assisted PardSIDs.");

    hLib = LoadLibrary(INPOUTDLLNAME);

    if (hLib == NULL) {
        log_message(LOG_DEFAULT, "Cannot open %s.", INPOUTDLLNAME);
        return -1;
    }

#ifndef MSVC_RC
    inp32fp = (inpfuncPtr)GetProcAddress(hLib, "Inp32");
    if (inp32fp != NULL) {
        oup32fp = (oupfuncPtr)GetProcAddress(hLib, "Out32");
        if (oup32fp == NULL) {
            log_message(LOG_DEFAULT, "I/O functions not found in %s.", INPOUTDLLNAME);
            return -1;
        }
    } else {
        log_message(LOG_DEFAULT, "I/O functions not found in %s.", INPOUTDLLNAME);
        return -1;
    }
#else
    Inp32 = (Inp32_t)GetProcAddress(hLib, "Inp32");
    if (Inp32 != NULL) {
        Out32 = (Out32_t)GetProcAddress(hLib, "Out32");
        if (Out32 == NULL) {
            log_message(LOG_DEFAULT, "I/O functions not found in %s.", INPOUTDLLNAME);
            return -1;
        }
    } else {
        log_message(LOG_DEFAULT, "I/O functions not found in %s.", INPOUTDLLNAME);
        return -1;
    }
#endif

    for (j = 0; j < 3; j++) {
        pssids[sids_found] = parsid_GetAddressLptPort(j + 1);
        if (parsid_port_address[sids_found] > 0) {
            if (detect_sid(sids_found)) {
                sids_found++;
            }
        }
    }

    if (!sids_found) {
        log_message(LOG_DEFAULT, "No dll assisted ParSIDs found.");
        return -1;
    }

    log_message(LOG_DEFAULT, "Dll assisted ParSID: opened, found %d SIDs.", sids_found);

    return 0;
}

int ps_dll_close(void)
{
    int i;

    for (i = 0; i < 3; ++i) {
        pssids[i] = -1;
    }
    FreeLibrary(hLib);
    hLib = NULL;

    log_message(LOG_DEFAULT, "Dll assisted ParSID: closed");

    return 0;
}

int ps_dll_available(void)
{
    return sids_found;
}
#endif
