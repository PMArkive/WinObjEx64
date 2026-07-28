/************************************************************************************
*
*  (C) COPYRIGHT AUTHORS, 2011 - 2026 UGN/HE
*
*  TITLE:       NTSUP.H
*
*  VERSION:     2.31
*
*  DATE:        26 Jul 2026
*
*  Common header file for the NT API support functions and definitions.
*
*  Depends on:    ntos.h
*
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
* ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED
* TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
* PARTICULAR PURPOSE.
*
************************************************************************************/

#define ENABLE_C_EXTERN

#if defined (_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef NTSUP_RTL
#define NTSUP_RTL

#pragma warning(push)
#pragma warning(disable: 4201) // nameless struct/union
#pragma warning(disable: 4214) // nonstandard extension used : bit field types other than int
#pragma warning(disable: 26812) // enum type % is unscoped

#ifndef _WINDOWS_
#include <Windows.h>
#endif

#pragma warning(push)
#pragma warning(disable: 4005) //macro redefinition
#include <ntstatus.h>
#pragma warning(pop)

#include "ntos.h"

#define _NTDEF_
#include <ntsecapi.h>
#undef _NTDEF_

#ifdef ENABLE_C_EXTERN
#if defined(__cplusplus)
extern "C" {
#endif
#endif

typedef NTSTATUS(NTAPI* PFN_NTQUERYROUTINE)(
   _In_opt_ HANDLE ObjectHandle,
   _In_ DWORD InformationClass,
   _Out_writes_bytes_(ObjectInformationLength) PVOID ObjectInformation,
   _In_ ULONG ObjectInformationLength,
   _Out_opt_ PULONG ReturnLength);

typedef PVOID(CALLBACK* PNTSUPMEMALLOC)(
    _In_ SIZE_T NumberOfBytes);

typedef BOOL(CALLBACK* PNTSUPMEMFREE)(
    _In_ PVOID Memory);

#define NTSUPHASH_SHA256_SIZE 32

#define MAX_NTSUP_BUFFER_SIZE (512 * 1024 * 1024) //512MB
#define MAX_NTSUP_ENV_SCAN 4096
#define MAX_NTSUP_PROCESS_ENUM_ITER (1024 * 1024)
#define MAX_NTSUP_WRITE_CHUNK 0x7FFFFFFF

typedef struct _OBJSCANPARAM {
    PCWSTR Buffer;
    ULONG BufferSize;
} OBJSCANPARAM, * POBJSCANPARAM;

typedef NTSTATUS(NTAPI* PENUMOBJECTSCALLBACK)(
    _In_ POBJECT_DIRECTORY_INFORMATION Entry, 
    _In_opt_ PVOID CallbackParam);

typedef BOOL(CALLBACK* pfnPatternSearchCallback)(
    _In_ PBYTE Buffer,
    _In_ ULONG PatternSize,
    _In_opt_ PVOID CallbackContext
    );

typedef struct _PATTERN_SEARCH_PARAMS {
    PBYTE Buffer;
    DWORD BufferSize;
    PBYTE Pattern;
    DWORD PatternSize;
    PBYTE Mask;
    pfnPatternSearchCallback Callback;
    PVOID CallbackContext;
} PATTERN_SEARCH_PARAMS, * PPATTERN_SEARCH_PARAMS;

typedef enum _NTSUP_IMAGE_TYPE {
    ImageTypeRaw,       // Raw file mapping (CreateFileMapping)
    ImageTypeLoaded     // Loaded module (PEB/LdrEntry)
} NTSUP_IMAGE_TYPE;

typedef struct _NTSUP_SHA256_CTX {
    ULONG State[8];
    ULONG64 BitCount;
    UCHAR Buffer[64];
} NTSUP_SHA256_CTX, * PNTSUP_SHA256_CTX;

VOID ntsupSha256Init(
    _Out_ PNTSUP_SHA256_CTX Ctx);

VOID ntsupSha256Update(
    _Inout_ PNTSUP_SHA256_CTX Ctx,
    _In_reads_bytes_(Length) const UCHAR* Data,
    _In_ SIZE_T Length);

VOID ntsupSha256Final(
    _Inout_ PNTSUP_SHA256_CTX Ctx,
    _Out_writes_bytes_all_(32) UCHAR Digest[32]);

//
// Ronova requires get rid of minirtl.
//

FORCEINLINE BOOLEAN ntsupIsDigitA(
    _In_ CHAR Ch
)
{
    return (BOOLEAN)(Ch >= '0' && Ch <= '9');
}

FORCEINLINE BOOLEAN ntsupIsDigitW(
    _In_ WCHAR Ch
)
{
    return (BOOLEAN)(Ch >= L'0' && Ch <= L'9');
}

//
// Minirtl section START
//
FORCEINLINE CHAR ntsupLowerCharA(
    _In_ CHAR c
)
{
    if ((c >= 'A') && (c <= 'Z'))
        return c + 0x20;
    else
        return c;
}

FORCEINLINE WCHAR ntsupLowerCharW(
    _In_ WCHAR c
)
{
    if ((c >= L'A') && (c <= L'Z'))
        return c + 0x20;
    else
        return c;
}

LPWSTR ntsupStrChrW(
    _In_z_ LPCWSTR String,
    _In_ WCHAR Character);
LPSTR ntsupStrChrA(
    _In_z_ LPCSTR String,
    _In_ CHAR Character);

SIZE_T ntsupStrLenA(
    _In_opt_ LPCSTR String);
SIZE_T ntsupStrLenW(
    _In_opt_ LPCWSTR String);

INT ntsupStrCmpA(
    _In_opt_ LPCSTR String1,
    _In_opt_ LPCSTR String2);
INT ntsupStrCmpW(
    _In_opt_ LPCWSTR String1,
    _In_opt_ LPCWSTR String2);

INT ntsupStrCmpIA(
    _In_opt_ LPCSTR String1,
    _In_opt_ LPCSTR String2);
INT ntsupStrCmpIW(
    _In_opt_ LPCWSTR String1,
    _In_opt_ LPCWSTR String2);

LPSTR ntsupStrCopyA(
    _Out_writes_z_(_String_length_(Source) + 1) LPSTR Destination,
    _In_z_ LPCSTR Source);
LPWSTR ntsupStrCopyW(
    _Out_writes_z_(_String_length_(Source) + 1) LPWSTR Destination,
    _In_z_ LPCWSTR Source);

LPSTR ntsupStrNCopyA(
    _Out_writes_z_(DestinationCount) LPSTR Destination,
    _In_ SIZE_T DestinationCount,
    _In_reads_(SourceCount) LPCSTR Source,
    _In_ SIZE_T SourceCount);
LPWSTR ntsupStrNCopyW(
    _Out_writes_z_(DestinationCount) LPWSTR Destination,
    _In_ SIZE_T DestinationCount,
    _In_reads_(SourceCount) LPCWSTR Source,
    _In_ SIZE_T SourceCount);

INT ntsupStrNCmpA(
    _In_opt_ LPCSTR String1,
    _In_opt_ LPCSTR String2,
    _In_ SIZE_T Count);
INT ntsupStrNCmpW(
    _In_opt_ LPCWSTR String1,
    _In_opt_ LPCWSTR String2,
    _In_ SIZE_T Count);

LPCSTR ntsupStrStrIA(
    _In_ LPCSTR String,
    _In_ LPCSTR SubString);
LPCWSTR ntsupStrStrIW(
    _In_ LPCWSTR String,
    _In_ LPCWSTR SubString);

LPSTR ntsupStrCatA(
    _Inout_ LPSTR Destination,
    _In_ LPCSTR Source);
LPWSTR ntsupStrCatW(
    _Inout_ LPWSTR Destination,
    _In_ LPCWSTR Source);

LPSTR ntsupStrCatExA(
    _Inout_updates_z_(DestinationCount) LPSTR Destination,
    _In_ SIZE_T DestinationCount,
    _In_ LPCSTR Source);
LPWSTR ntsupStrCatExW(
    _Inout_updates_z_(DestinationCount) LPWSTR Destination,
    _In_ SIZE_T DestinationCount,
    _In_ LPCWSTR Source);

BOOL ntsupStrToUInt64A(
    _In_ LPCSTR String,
    _Out_ PULONGLONG Value);
BOOL ntsupStrToUInt64W(
    _In_ LPCWSTR String,
    _Out_ PULONGLONG Value);

SIZE_T ntsupUInt64ToStrW(
    _In_ ULONGLONG Value,
    _Out_writes_z_(BufferCount) LPWSTR Buffer,
    _In_ SIZE_T BufferCount);
SIZE_T ntsupUInt64ToStrA(
    _In_ ULONGLONG Value,
    _Out_writes_z_(BufferCount) LPSTR Buffer,
    _In_ SIZE_T BufferCount);

#ifdef _UNICODE
#define ntsupLowerChar ntsupLowerCharW
#define ntsupStrChr ntsupStrChrW
#define ntsupStrLen ntsupStrLenW
#define ntsupStrCmp ntsupStrCmpW
#define ntsupStrCmpI ntsupStrCmpIW
#define ntsupStrCopy ntsupStrCopyW
#define ntsupStrNCopy ntsupStrNCopyW
#define ntsupStrNCmp ntsupStrNCmpW
#define ntsupStrStrI ntsupStrStrIW
#define ntsupStrCat ntsupStrCatW
#define ntsupStrCatEx ntsupStrCatExW
#define ntsupStrToUInt64 ntsupStrToUInt64W
#define ntsupUInt64ToStr ntsupUInt64ToStrW
#else
#define ntsupLowerChar ntsupLowerCharA
#define ntsupStrChr ntsupStrChrA
#define ntsupStrLen ntsupStrLenA
#define ntsupStrCmp ntsupStrCmpA
#define ntsupStrCmpI ntsupStrCmpIA
#define ntsupStrCopy ntsupStrCopyA
#define ntsupStrNCopy ntsupStrNCopyA
#define ntsupStrNCmp ntsupStrNCmpA
#define ntsupStrStrI ntsupStrStrIA
#define ntsupStrCat ntsupStrCatA
#define ntsupStrCatEx ntsupStrCatExA
#define ntsupStrToUInt64 ntsupStrToUInt64A
#define ntsupUInt64ToStr ntsupUInt64ToStrA
#endif

//
// Minirtl section END
//

PVOID ntsupHeapAlloc(
    _In_ SIZE_T Size);

PVOID ntsupHeapReAlloc(
    _In_ PVOID BaseAddress,
    _In_ SIZE_T Size);

BOOL ntsupHeapFree(
    _In_ PVOID BaseAddress);

SIZE_T ntsupHeapSize(
    _In_ PVOID BaseAddress);

BOOL ntsupHeapValidate(
    _In_ PVOID BaseAddress);

SIZE_T ntsupHeapCompact(
    VOID);

BOOL ntsupHeapLock(
    VOID);

BOOL ntsupHeapUnlock(
    VOID);

BOOLEAN ntsupIsAddressValid(
    _In_ PVOID Address,
    _In_ SIZE_T Size);

PVOID ntsupVirtualAllocEx(
    _In_ SIZE_T Size,
    _In_ ULONG AllocationType,
    _In_ ULONG Protect);

PVOID ntsupVirtualAlloc(
    _In_ SIZE_T Size);

BOOL ntsupVirtualFree(
    _In_ PVOID Memory);

BOOL ntsupVirtualLock(
    _In_ LPVOID lpAddress,
    _In_ SIZE_T dwSize);

BOOL ntsupVirtualUnlock(
    _In_ LPVOID lpAddress,
    _In_ SIZE_T dwSize);

SIZE_T ntsupWriteBufferToFile(
    _In_ PCWSTR FileName,
    _In_ PVOID Buffer,
    _In_ SIZE_T Size,
    _In_ BOOL Flush,
    _In_ BOOL Append,
    _Out_opt_ NTSTATUS* Result);

PVOID ntsupGetModuleEntryByAddress(
    _In_ PRTL_PROCESS_MODULES ModulesList,
    _In_ PVOID Address);

PVOID ntsupFindModuleEntryByName(
    _In_ PRTL_PROCESS_MODULES ModulesList,
    _In_ LPCSTR ModuleName);

PVOID ntsupFindModuleEntryByName_U(
    _In_ PRTL_PROCESS_MODULES ModulesList,
    _In_ LPCWSTR ModuleName);

BOOL ntsupFindModuleEntryByAddress(
    _In_ PRTL_PROCESS_MODULES ModulesList,
    _In_ PVOID Address,
    _Out_ PULONG ModuleIndex);

PVOID ntsupFindModuleNameByAddress(
    _In_ PRTL_PROCESS_MODULES ModulesList,
    _In_ PVOID Address,
    _Inout_	LPWSTR Buffer,
    _In_ DWORD ccBuffer);

NTSTATUS ntsupConvertToUnicode(
    _In_ LPCSTR AnsiString,
    _Inout_ PUNICODE_STRING UnicodeString);

NTSTATUS ntsupConvertToAnsi(
    _In_ LPCWSTR UnicodeString,
    _Inout_ PANSI_STRING AnsiString);

NTSTATUS ntsupSetPrivilege(
    _In_ HANDLE TokenHandle,
    _In_ DWORD Privilege,
    _In_ BOOLEAN Enable,
    _Out_opt_ PTOKEN_PRIVILEGES PreviousState,
    _Out_opt_ PULONG ReturnLength);

BOOLEAN ntsupEnablePrivilege(
    _In_ DWORD Privilege,
    _In_ BOOLEAN Enable);

HANDLE ntsupGetCurrentProcessToken(
    VOID);

ULONG_PTR ntsupQuerySystemRangeStart(
    VOID);

BOOLEAN ntsupQueryUserModeAccessibleRange(
    _Out_ PULONG_PTR MinimumUserModeAddress,
    _Out_ PULONG_PTR MaximumUserModeAddress);

BOOL ntsupIsProcess32bit(
    _In_ HANDLE hProcess);

PVOID ntsupGetLoadedModulesListEx(
    _In_ BOOL ExtendedOutput,
    _Out_opt_ PULONG ReturnLength,
    _In_ PNTSUPMEMALLOC AllocMem,
    _In_ PNTSUPMEMFREE FreeMem);

PVOID ntsupGetLoadedModulesList(
    _Out_opt_ PULONG ReturnLength);

PVOID ntsupGetLoadedModulesList2(
    _Out_opt_ PULONG ReturnLength);

PVOID ntsupGetSystemInfoEx(
    _In_ SYSTEM_INFORMATION_CLASS SystemInformationClass,
    _Out_opt_ PULONG ReturnLength,
    _In_ PNTSUPMEMALLOC AllocMem,
    _In_ PNTSUPMEMFREE FreeMem);

PVOID ntsupGetSystemInfo(
    _In_ SYSTEM_INFORMATION_CLASS SystemInformationClass,
    _Out_opt_ PULONG ReturnLength);

NTSTATUS NTAPI ntsupEnumSystemObjects(
    _In_opt_ LPCWSTR RootDirectory,
    _In_opt_ HANDLE RootDirectoryHandle,
    _In_ PENUMOBJECTSCALLBACK CallbackProc,
    _In_opt_ PVOID CallbackParam);

BOOL ntsupResolveSymbolicLink(
    _In_opt_ HANDLE RootDirectoryHandle,
    _In_ PUNICODE_STRING LinkName,
    _Inout_ LPWSTR Buffer,
    _In_ DWORD cbBuffer);

BOOL ntsupQueryThreadWin32StartAddress(
    _In_ HANDLE ThreadHandle,
    _Out_opt_ PULONG_PTR Win32StartAddress);

NTSTATUS ntsupQueryProcessCommandLine(
    _In_ HANDLE ProcessHandle,
    _Out_ PUNICODE_STRING CommandLine,
    _In_ PNTSUPMEMALLOC AllocMem,
    _In_ PNTSUPMEMFREE FreeMem);

_Success_(return)
NTSTATUS ntsupOpenDirectoryEx(
    _Out_ PHANDLE DirectoryHandle,
    _In_opt_ HANDLE RootDirectoryHandle,
    _In_ PUNICODE_STRING DirectoryName,
    _In_ ACCESS_MASK DesiredAccess);

NTSTATUS ntsupOpenDirectory(
    _Out_ PHANDLE DirectoryHandle,
    _In_opt_ HANDLE RootDirectoryHandle,
    _In_ LPCWSTR DirectoryName,
    _In_ ACCESS_MASK DesiredAccess);

BOOL ntsupQueryProcessName(
    _In_ ULONG_PTR dwProcessId,
    _In_ PVOID ProcessList,
    _Inout_ LPWSTR Buffer,
    _In_ DWORD ccBuffer);

BOOL ntsupQueryProcessEntryById(
    _In_ HANDLE UniqueProcessId,
    _In_ PVOID ProcessList,
    _Out_ PSYSTEM_PROCESS_INFORMATION* Entry);

NTSTATUS ntsupQueryProcessImageFileNameByProcessId(
    _In_ HANDLE UniqueProcessId,
    _Out_ PUNICODE_STRING ProcessImageFileName,
    _In_ PNTSUPMEMALLOC AllocMem,
    _In_ PNTSUPMEMFREE FreeMem);

NTSTATUS ntsupQuerySystemObjectInformationVariableSize(
    _In_ PFN_NTQUERYROUTINE QueryRoutine,
    _In_opt_ HANDLE ObjectHandle,
    _In_ DWORD InformationClass,
    _Out_ PVOID* Buffer,
    _Out_opt_ PULONG ReturnLength,
    _In_ PNTSUPMEMALLOC AllocMem,
    _In_ PNTSUPMEMFREE FreeMem);

NTSTATUS ntsupQuerySystemObjectInformationVariableSizeEx(
    _In_ PFN_NTQUERYROUTINE QueryRoutine,
    _In_opt_ HANDLE ObjectHandle,
    _In_ DWORD InformationClass,
    _Out_ PVOID * Buffer,
    _Out_opt_ PULONG ReturnLength,
    _In_ ULONG InitialBufferSize,
    _In_ PNTSUPMEMALLOC AllocMem,
    _In_ PNTSUPMEMFREE FreeMem);

BOOLEAN ntsupQueryVsmProtectionInformation(
    _Out_ PBOOLEAN pbDmaProtectionsAvailable,
    _Out_ PBOOLEAN pbDmaProtectionsInUse,
    _Out_ PBOOLEAN pbHardwareMbecAvailable,
    _Out_ PBOOLEAN pbApicVirtualizationAvailable);

BOOLEAN ntsupQueryVBSState(
    _Out_ PBOOLEAN pbVBSRunning,
    _Out_ PBOOLEAN pbHVCIEnabled,
    _Out_ PBOOLEAN pbHVCIStrictMode);

PVOID ntsupLookupImageSectionByNameEx(
    _In_ CHAR * SectionName,
    _In_ ULONG SectionNameLength,
    _In_ PVOID DllBase,
    _In_ SIZE_T ImageSize,
    _Out_opt_ PULONG SectionSize);

PVOID ntsupLookupImageSectionByName(
    _In_ CHAR* SectionName,
    _In_ ULONG SectionNameLength,
    _In_ PVOID DllBase,
    _Out_ PULONG SectionSize);

PVOID ntsupFindPattern(
    _In_ CONST PBYTE Buffer,
    _In_ SIZE_T BufferSize,
    _In_ CONST PBYTE Pattern,
    _In_ SIZE_T PatternSize);

DWORD ntsupFindPatternEx(
    _In_ PATTERN_SEARCH_PARAMS * SearchParams);

NTSTATUS ntsupOpenProcess(
    _In_ HANDLE UniqueProcessId,
    _In_ ACCESS_MASK DesiredAccess,
    _Out_ PHANDLE ProcessHandle);

NTSTATUS ntsupOpenThread(
    _In_ PCLIENT_ID ClientId,
    _In_ ACCESS_MASK DesiredAccess,
    _Out_ PHANDLE ThreadHandle);

NTSTATUS ntsupCICustomKernelSignersAllowed(
    _Out_ PBOOLEAN bAllowed);

NTSTATUS ntsupPrivilegeEnabled(
    _In_ HANDLE ClientToken,
    _In_ ULONG Privilege,
    _Out_ LPBOOL pfResult);

LPWSTR ntsupQueryEnvironmentVariableOffset(
    _In_ PUNICODE_STRING Value);

BOOLEAN ntsupSetEnvironmentVariable(
    _In_ LPCWSTR Name,
    _In_opt_ LPCWSTR Value);

DWORD ntsupExpandEnvironmentStrings(
    _In_ LPCWSTR lpSrc,
    _Out_writes_to_opt_(nSize, return) LPWSTR lpDst,
    _In_ DWORD nSize);

NTSTATUS ntsupIsLocalSystem(
    _Out_ PBOOL pbResult);

NTSTATUS ntsupIsUserHasInteractiveSid(
    _In_ HANDLE hToken,
    _Out_ PBOOL pbInteractiveSid);

BOOL ntsupGetProcessElevationType(
    _In_opt_ HANDLE ProcessHandle,
    _Out_ TOKEN_ELEVATION_TYPE * lpType);

NTSTATUS ntsupIsProcessElevated(
    _In_ ULONG ProcessId,
    _Out_ PBOOL Elevated);

VOID ntsupPurgeSystemCache(
    VOID);

PWSTR ntsupGetSystemRoot(
    VOID);

NTSTATUS ntsupGetProcessDebugObject(
    _In_ HANDLE ProcessHandle,
    _Out_ PHANDLE DebugObjectHandle);

PBYTE ntsupQueryResourceData(
    _In_ ULONG_PTR ResourceId,
    _In_ PVOID DllHandle,
    _In_ PULONG DataSize);

NTSTATUS ntsupEnableWow64Redirection(
    _In_ BOOLEAN bEnable);

BOOLEAN ntsupIsKdEnabled(
    _Out_opt_ PBOOLEAN DebuggerAllowed,
    _Out_opt_ PBOOLEAN DebuggerNotPresent);

BOOLEAN ntsupIsObjectExists(
    _In_ LPCWSTR RootDirectory,
    _In_ LPCWSTR ObjectName);

BOOLEAN ntsupUserIsFullAdmin(
    VOID);

NTSTATUS ntsupDuplicateUnicodeString(
    _In_ PCUNICODE_STRING SourceString,
    _Out_ PUNICODE_STRING DestinationString);

NTSTATUS ntsupDuplicateAnsiString(
    _In_ PCANSI_STRING SourceString,
    _Out_ PANSI_STRING DestinationString);

NTSTATUS ntsupHashImageSections(
    _In_ PVOID ImageBase,
    _In_ SIZE_T ImageSize,
    _Out_writes_bytes_(HashBufferSize) PBYTE HashBuffer,
    _In_ SIZE_T HashBufferSize,
    _In_ NTSUP_IMAGE_TYPE ImageType);

#define ntsupQuerySecurityInformation(\
     ObjectHandle, SecurityInformationClass, Buffer, ReturnLength, AllocMem, FreeMem) \
ntsupQuerySystemObjectInformationVariableSize((PFN_NTQUERYROUTINE)NtQuerySecurityObject, \
     ObjectHandle, SecurityInformationClass, (PVOID*)Buffer, ReturnLength,\
    (PNTSUPMEMALLOC)AllocMem, (PNTSUPMEMFREE)FreeMem)

#define ntsupQueryTokenInformation(\
     TokenHandle, TokenInformationClass, Buffer, ReturnLength, AllocMem, FreeMem) \
ntsupQuerySystemObjectInformationVariableSize((PFN_NTQUERYROUTINE)NtQueryInformationToken, \
     TokenHandle, TokenInformationClass, (PVOID*)Buffer, ReturnLength,\
    (PNTSUPMEMALLOC)AllocMem, (PNTSUPMEMFREE)FreeMem)

#define ntsupQueryObjectInformation(\
     ObjectHandle, ObjectInformationClass, Buffer, ReturnLength, AllocMem, FreeMem) \
ntsupQuerySystemObjectInformationVariableSize((PFN_NTQUERYROUTINE)NtQueryObject, \
    ObjectHandle, ObjectInformationClass, (PVOID*)Buffer, ReturnLength, \
    (PNTSUPMEMALLOC)AllocMem, (PNTSUPMEMFREE)FreeMem)

#define ntsupQueryObjectInformationEx(\
     ObjectHandle, ObjectInformationClass, Buffer, ReturnLength, InitialBufferSize, AllocMem, FreeMem) \
ntsupQuerySystemObjectInformationVariableSizeEx((PFN_NTQUERYROUTINE)NtQueryObject, \
    ObjectHandle, ObjectInformationClass, (PVOID*)Buffer, ReturnLength, InitialBufferSize, \
    (PNTSUPMEMALLOC)AllocMem, (PNTSUPMEMFREE)FreeMem)

#define ntsupQueryThreadInformation(\
    ThreadHandle, ThreadInformationClass, Buffer, ReturnLength, AllocMem, FreeMem) \
ntsupQuerySystemObjectInformationVariableSize((PFN_NTQUERYROUTINE)NtQueryInformationThread, \
    ThreadHandle, ThreadInformationClass, (PVOID*)Buffer, ReturnLength, \
    (PNTSUPMEMALLOC)AllocMem, (PNTSUPMEMFREE)FreeMem)

#define ntsupQueryProcessInformation(\
    ProcessHandle, ProcessInformationClass, Buffer, ReturnLength, AllocMem, FreeMem)\
ntsupQuerySystemObjectInformationVariableSize((PFN_NTQUERYROUTINE)NtQueryInformationProcess, \
    ProcessHandle, ProcessInformationClass, (PVOID*)Buffer, ReturnLength, \
    (PNTSUPMEMALLOC)AllocMem, (PNTSUPMEMFREE)FreeMem)


#ifdef ENABLE_C_EXTERN
#ifdef __cplusplus
}
#endif
#endif

#pragma warning(pop)

#endif NTSUP_RTL
