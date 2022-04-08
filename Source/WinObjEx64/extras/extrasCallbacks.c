/*******************************************************************************
*
*  (C) COPYRIGHT AUTHORS, 2018 - 2022
*
*  TITLE:       EXTRASCALLBACKS.C
*
*  VERSION:     1.93
*
*  DATE:        01 Apr 2022
*
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
* ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED
* TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
* PARTICULAR PURPOSE.
*
*******************************************************************************/
#include "global.h"
#include "extras.h"
#include "extrasCallbacks.h"
#include "extras/extrasCallbacksPatterns.h"
#include "treelist/treelist.h"
#include "hde/hde64.h"
#include "ksymbols.h"

#define CBDLG_TRACKSIZE_MIN_X 640
#define CBDLG_TRACKSIZE_MIN_Y 480

ULONG g_CallbacksCount;

typedef struct _OBEX_CALLBACK_DISPATCH_ENTRY OBEX_CALLBACK_DISPATCH_ENTRY;

typedef ULONG_PTR(CALLBACK *POBEX_FINDCALLBACK_ROUTINE)(
    _In_opt_ ULONG_PTR QueryFlags);

typedef VOID(CALLBACK *POBEX_DISPLAYCALLBACK_ROUTINE)(
    _In_ HWND TreeList,
    _In_ LPWSTR CallbackType,
    _In_ ULONG_PTR KernelVariableAddress,
    _In_ PRTL_PROCESS_MODULES Modules);

typedef NTSTATUS(CALLBACK *POBEX_QUERYCALLBACK_ROUTINE)(
    _In_opt_ ULONG_PTR QueryFlags,
    _In_ POBEX_DISPLAYCALLBACK_ROUTINE DisplayRoutine,
    _In_opt_ POBEX_FINDCALLBACK_ROUTINE FindRoutine,
    _In_opt_ LPWSTR CallbackType,
    _In_ HWND TreeList,
    _In_ PRTL_PROCESS_MODULES Modules,
    _Inout_opt_ PULONG_PTR SystemCallbacksRef);

#define OBEX_FINDCALLBACK_ROUTINE(n) ULONG_PTR CALLBACK n(    \
    _In_opt_ ULONG_PTR QueryFlags)

#define OBEX_QUERYCALLBACK_ROUTINE(n) NTSTATUS CALLBACK n(    \
    _In_opt_ ULONG_PTR QueryFlags,                            \
    _In_ POBEX_DISPLAYCALLBACK_ROUTINE DisplayRoutine,        \
    _In_opt_ POBEX_FINDCALLBACK_ROUTINE FindRoutine,         \
    _In_opt_ LPWSTR CallbackType,                             \
    _In_ HWND TreeList,                                       \
    _In_ PRTL_PROCESS_MODULES Modules,                        \
    _Inout_opt_ PULONG_PTR SystemCallbacksRef)

#define OBEX_DISPLAYCALLBACK_ROUTINE(n) VOID CALLBACK n(     \
    _In_ HWND TreeList,                               \
    _In_ LPWSTR CallbackType,                         \
    _In_ ULONG_PTR KernelVariableAddress,             \
    _In_ PRTL_PROCESS_MODULES Modules)

typedef struct _OBEX_CALLBACK_DISPATCH_ENTRY {
    ULONG_PTR QueryFlags;
    LPWSTR CallbackType;
    POBEX_QUERYCALLBACK_ROUTINE QueryRoutine;
    POBEX_DISPLAYCALLBACK_ROUTINE DisplayRoutine;
    POBEX_FINDCALLBACK_ROUTINE FindRoutine;
    PULONG_PTR SystemCallbacksRef;
} OBEX_CALLBACK_DISPATCH_ENTRY, *POBEX_CALLBACK_DISPATCH_ENTRY;

OBEX_QUERYCALLBACK_ROUTINE(QueryIopFsListsCallbacks);
OBEX_QUERYCALLBACK_ROUTINE(QueryCallbackGeneric);

OBEX_DISPLAYCALLBACK_ROUTINE(DumpPsCallbacks);
OBEX_DISPLAYCALLBACK_ROUTINE(DumpKeBugCheckCallbacks);
OBEX_DISPLAYCALLBACK_ROUTINE(DumpKeBugCheckReasonCallbacks);
OBEX_DISPLAYCALLBACK_ROUTINE(DumpCmCallbacks);
OBEX_DISPLAYCALLBACK_ROUTINE(DumpIoCallbacks);
OBEX_DISPLAYCALLBACK_ROUTINE(DumpObCallbacks);
OBEX_DISPLAYCALLBACK_ROUTINE(DumpSeFileSystemCallbacks);
OBEX_DISPLAYCALLBACK_ROUTINE(DumpPoCallbacks);
OBEX_DISPLAYCALLBACK_ROUTINE(DumpDbgPrintCallbacks);
OBEX_DISPLAYCALLBACK_ROUTINE(DumpIoFsRegistrationCallbacks);
OBEX_DISPLAYCALLBACK_ROUTINE(DumpIoFileSystemCallbacks);
OBEX_DISPLAYCALLBACK_ROUTINE(DumpDbgkLCallbacks);
OBEX_DISPLAYCALLBACK_ROUTINE(DumpPsAltSystemCallHandlers);
OBEX_DISPLAYCALLBACK_ROUTINE(DumpCiCallbacks);
OBEX_DISPLAYCALLBACK_ROUTINE(DumpExHostCallbacks);
OBEX_DISPLAYCALLBACK_ROUTINE(DumpExpCallbackListCallbacks);
OBEX_DISPLAYCALLBACK_ROUTINE(DumpPoCoalescingCallbacks);

OBEX_FINDCALLBACK_ROUTINE(FindPspCreateProcessNotifyRoutine);
OBEX_FINDCALLBACK_ROUTINE(FindPspCreateThreadNotifyRoutine);
OBEX_FINDCALLBACK_ROUTINE(FindPspLoadImageNotifyRoutine);
OBEX_FINDCALLBACK_ROUTINE(FindKeBugCheckCallbackHead);
OBEX_FINDCALLBACK_ROUTINE(FindKeBugCheckReasonCallbackHead);
OBEX_FINDCALLBACK_ROUTINE(FindCmCallbackHead);
OBEX_FINDCALLBACK_ROUTINE(FindIopNotifyShutdownQueueHeadHead);
OBEX_FINDCALLBACK_ROUTINE(FindPopRegisteredPowerSettingCallbacks);
OBEX_FINDCALLBACK_ROUTINE(FindSeFileSystemNotifyRoutinesHead);
OBEX_FINDCALLBACK_ROUTINE(FindIopFsNotifyChangeQueueHead);
OBEX_FINDCALLBACK_ROUTINE(FindObjectTypeCallbackListHeadByType);
OBEX_FINDCALLBACK_ROUTINE(FindRtlpDebugPrintCallbackList);
OBEX_FINDCALLBACK_ROUTINE(FindDbgkLmdCallbacks);
OBEX_FINDCALLBACK_ROUTINE(FindPsAltSystemCallHandlers);
OBEX_FINDCALLBACK_ROUTINE(FindCiCallbacks);
OBEX_FINDCALLBACK_ROUTINE(FindExHostCallbacks);
OBEX_FINDCALLBACK_ROUTINE(FindExpCallbackListHead);
OBEX_FINDCALLBACK_ROUTINE(FindPoCoalescingCallbacks);

OBEX_CALLBACK_DISPATCH_ENTRY g_CallbacksDispatchTable[] = {
    {
        0, L"CreateProcess",
        QueryCallbackGeneric, DumpPsCallbacks, FindPspCreateProcessNotifyRoutine,
        &g_SystemCallbacks.PspCreateProcessNotifyRoutine
    },
    {
        0, L"CreateThread",
        QueryCallbackGeneric, DumpPsCallbacks, FindPspCreateThreadNotifyRoutine,
        &g_SystemCallbacks.PspCreateThreadNotifyRoutine
    },
    {
        0, L"LoadImage",
        QueryCallbackGeneric, DumpPsCallbacks, FindPspLoadImageNotifyRoutine,
        &g_SystemCallbacks.PspLoadImageNotifyRoutine
    },
    {
        0, L"KeBugCheck",
        QueryCallbackGeneric, DumpKeBugCheckCallbacks, FindKeBugCheckCallbackHead,
        &g_SystemCallbacks.KeBugCheckCallbackHead
    },
    {
        0, L"KeBugCheckReason",
        QueryCallbackGeneric, DumpKeBugCheckReasonCallbacks, FindKeBugCheckReasonCallbackHead,
        &g_SystemCallbacks.KeBugCheckReasonCallbackHead
    },
    {
        0, L"CmRegistry",
        QueryCallbackGeneric, DumpCmCallbacks, FindCmCallbackHead,
        &g_SystemCallbacks.CmCallbackListHead
    },
    {
        0, L"Shutdown",
        QueryCallbackGeneric, DumpIoCallbacks, FindIopNotifyShutdownQueueHeadHead,
        &g_SystemCallbacks.IopNotifyShutdownQueueHead
    },
    {
        1, L"LastChanceShutdown",
        QueryCallbackGeneric, DumpIoCallbacks, FindIopNotifyShutdownQueueHeadHead,
        &g_SystemCallbacks.IopNotifyLastChanceShutdownQueueHead
    },
    {
        0, L"ObProcess",
        QueryCallbackGeneric, DumpObCallbacks, FindObjectTypeCallbackListHeadByType,
        &g_SystemCallbacks.ObProcessCallbackHead },
    {
        1, L"ObThread",
        QueryCallbackGeneric, DumpObCallbacks, FindObjectTypeCallbackListHeadByType,
        &g_SystemCallbacks.ObThreadCallbackHead
    },
    {
        2, L"ObDesktop", 
        QueryCallbackGeneric, DumpObCallbacks, FindObjectTypeCallbackListHeadByType,
        &g_SystemCallbacks.ObDesktopCallbackHead
    },
    {
        0, L"SeFileSystem",
        QueryCallbackGeneric, DumpSeFileSystemCallbacks, FindSeFileSystemNotifyRoutinesHead,
        &g_SystemCallbacks.SeFileSystemNotifyRoutinesHead
    },
    {
        1, L"SeFileSystemEx",
        QueryCallbackGeneric, DumpSeFileSystemCallbacks, FindSeFileSystemNotifyRoutinesHead,
        &g_SystemCallbacks.SeFileSystemNotifyRoutinesExHead
    },
    {
        0, L"PowerSettings",
        QueryCallbackGeneric, DumpPoCallbacks, FindPopRegisteredPowerSettingCallbacks,
        &g_SystemCallbacks.PopRegisteredPowerSettingCallbacks
    },
    {
        0, L"DebugPrint",
        QueryCallbackGeneric, DumpDbgPrintCallbacks, FindRtlpDebugPrintCallbackList,
        &g_SystemCallbacks.RtlpDebugPrintCallbackList
    },
    {
        0, L"IoFsRegistration",
        QueryCallbackGeneric, DumpIoFsRegistrationCallbacks, FindIopFsNotifyChangeQueueHead,
        &g_SystemCallbacks.IopFsNotifyChangeQueueHead
    },
    {
        0, L"IoFileSystemType",
        QueryIopFsListsCallbacks, DumpIoFileSystemCallbacks, NULL,
        NULL
    },
    {
        0, L"DbgkLmd",
        QueryCallbackGeneric, DumpDbgkLCallbacks, FindDbgkLmdCallbacks,
        &g_SystemCallbacks.DbgkLmdCallbacks
    },
    {
        0, L"AltSystemCall",
        QueryCallbackGeneric, DumpPsAltSystemCallHandlers, FindPsAltSystemCallHandlers,
        &g_SystemCallbacks.PsAltSystemCallHandlers
    },
    {
        0, L"CiCallbacks",
        QueryCallbackGeneric, DumpCiCallbacks, FindCiCallbacks,
        &g_SystemCallbacks.CiCallbacks
    },
    {
        0, L"ExHostCallbacks",
        QueryCallbackGeneric, DumpExHostCallbacks, FindExHostCallbacks,
        &g_SystemCallbacks.ExpHostListHead
    },
    {
        0, L"ExpCallbackList",
        QueryCallbackGeneric, DumpExpCallbackListCallbacks, FindExpCallbackListHead,
        &g_SystemCallbacks.ExpCallbackListHead
    },
    {
        0, L"PowerCoalescing",
        QueryCallbackGeneric, DumpPoCoalescingCallbacks, FindPoCoalescingCallbacks,
        &g_SystemCallbacks.PoCoalescingCallbacks
    }
};

//
// All available names for CiCallbacks. Unknown is expected to be XBOX callback.
//
static const WCHAR *CiCallbackNames[] = {
    L"CiSetFileCache", //0
    L"CiGetFileCache", //1
    L"CiQueryInformation", //2
    L"CiValidateImageHeader", //3
    L"CiValidateImageData", //4
    L"CiHashMemory", //5
    L"KappxIsPackageFile", //6
    L"CiCompareSigningLevels", //7
    L"CiValidateFileAsImageType", //8
    L"CiRegisterSigningInformation", //9
    L"CiUnregisterSigningInformation",//10
    L"CiInitializePolicy",//11
    L"CiReleaseContext",//12
    L"XciUnknownCallback",//13 XBOX
    L"CiGetStrongImageReference", //14
    L"CiHvciSetImageBaseAddress", //15
    L"CipQueryPolicyInformation", //16
    L"CiValidateDynamicCodePages", //17
    L"CiQuerySecurityPolicy", //18
    L"CiRevalidateImage", //19
    L"CiSetInformation",//20
    L"CiSetInformationProcess", //21
    L"CiGetBuildExpiryTime", //22
    L"CiCheckProcessDebugAccessPolicy", //23
    L"SIPolicyQueryPolicyInformation",//24
    L"SIPolicyQuerySecurityPolicy",//25
    L"CiSetUnlockInformation",//26
    L"CiGetCodeIntegrityOriginClaimForFileObject",//27
    L"CiDeleteCodeIntegrityOriginClaimMembers", //28
    L"CiDeleteCodeIntegrityOriginClaimForFileObject",//29
    L"CiHvciReportMmIncompatibility",//30
    L"CiCompareExistingSePool",//31
    L"CiSetCachedOriginClaim"//32
};

typedef enum _CiNameIds {
    Id_CiSetFileCache = 0,
    Id_CiGetFileCache,
    Id_CiQueryInformation,
    Id_CiValidateImageHeader,
    Id_CiValidateImageData,
    Id_CiHashMemory,
    Id_KappxIsPackageFile,
    Id_CiCompareSigningLevels,
    Id_CiValidateFileAsImageType,
    Id_CiRegisterSigningInformation,
    Id_CiUnregisterSigningInformation,
    Id_CiInitializePolicy,
    Id_CiReleaseContext,
    Id_XciUnknownCallback,
    Id_CiGetStrongImageReference,
    Id_CiHvciSetImageBaseAddress,
    Id_CipQueryPolicyInformation,
    Id_CiValidateDynamicCodePages,
    Id_CiQuerySecurityPolicy,
    Id_CiRevalidateImage,
    Id_CiSetInformation,
    Id_CiSetInformationProcess,
    Id_CiGetBuildExpiryTime,
    Id_CiCheckProcessDebugAccessPolicy,
    Id_SIPolicyQueryPolicyInformation,
    Id_SIPolicyQuerySecurityPolicy,
    Id_CiSetUnlockInformation,
    Id_CiGetCodeIntegrityOriginClaimForFileObject,
    Id_CiDeleteCodeIntegrityOriginClaimMembers,
    Id_CiDeleteCodeIntegrityOriginClaimForFileObject,
    Id_CiHvciReportMmIncompatibility,
    Id_CiCompareExistingSePool,
    Id_CiSetCachedOriginClaim
} CiNameIds;

//
// Callback name index arrays
//

//
// Windows 7
//
static const BYTE CiCallbackIndexes_Win7[] = {
    Id_CiValidateImageHeader,
    Id_CiValidateImageData,
    Id_CiQueryInformation
};

//
// Windows 8
//
static const BYTE CiCallbackIndexes_Win8[] = {
    Id_CiSetFileCache,
    Id_CiGetFileCache,
    Id_CiQueryInformation,
    Id_CiValidateImageHeader,
    Id_CiValidateImageData,
    Id_CiHashMemory,
    Id_KappxIsPackageFile
};

//
// Windows 8.1
//
static const BYTE CiCallbackIndexes_Win81[] = {
    Id_CiSetFileCache,
    Id_CiGetFileCache,
    Id_CiQueryInformation,
    Id_CiValidateImageHeader,
    Id_CiValidateImageData,
    Id_CiHashMemory,
    Id_KappxIsPackageFile,
    Id_CiCompareSigningLevels,
    Id_CiValidateFileAsImageType,
    Id_CiRegisterSigningInformation,
    Id_CiUnregisterSigningInformation,
    Id_CiInitializePolicy
};

//
// Windows 10 TH1/TH2
//
static const BYTE CiCallbackIndexes_Win10Threshold[] = {
    Id_CiSetFileCache,
    Id_CiGetFileCache,
    Id_CiQueryInformation,
    Id_CiValidateImageHeader,
    Id_CiValidateImageData,
    Id_CiHashMemory,
    Id_KappxIsPackageFile,
    Id_CiCompareSigningLevels,
    Id_CiValidateFileAsImageType,
    Id_CiRegisterSigningInformation,
    Id_CiUnregisterSigningInformation,
    Id_CiInitializePolicy,
    Id_CiReleaseContext,
    Id_XciUnknownCallback,
    Id_CiGetStrongImageReference,
    Id_CiHvciSetImageBaseAddress,
    Id_SIPolicyQueryPolicyInformation,
    Id_CiValidateDynamicCodePages
};

//
// Windows 10 RS1
//
static const BYTE CiCallbackIndexes_Win10RS1[] = {
    Id_CiSetFileCache,
    Id_CiGetFileCache,
    Id_CiQueryInformation,
    Id_CiValidateImageHeader,
    Id_CiValidateImageData,
    Id_CiHashMemory,
    Id_KappxIsPackageFile,
    Id_CiCompareSigningLevels,
    Id_CiValidateFileAsImageType,
    Id_CiRegisterSigningInformation,
    Id_CiUnregisterSigningInformation,
    Id_CiInitializePolicy,
    Id_CiReleaseContext,
    Id_XciUnknownCallback,
    Id_CiGetStrongImageReference,
    Id_CiHvciSetImageBaseAddress,
    Id_SIPolicyQueryPolicyInformation,
    Id_CiValidateDynamicCodePages,
    Id_SIPolicyQuerySecurityPolicy,
    Id_CiRevalidateImage
};

//
// Windows 10 RS2
//
static const BYTE CiCallbackIndexes_Win10RS2[] = {
    Id_CiSetFileCache,
    Id_CiGetFileCache,
    Id_CiQueryInformation,
    Id_CiValidateImageHeader,
    Id_CiValidateImageData,
    Id_CiHashMemory,
    Id_KappxIsPackageFile,
    Id_CiCompareSigningLevels,
    Id_CiValidateFileAsImageType,
    Id_CiRegisterSigningInformation,
    Id_CiUnregisterSigningInformation,
    Id_CiInitializePolicy,
    Id_CiReleaseContext,
    Id_XciUnknownCallback,
    Id_CiGetStrongImageReference,
    Id_CiHvciSetImageBaseAddress,
    Id_CipQueryPolicyInformation,
    Id_CiValidateDynamicCodePages,
    Id_SIPolicyQuerySecurityPolicy,
    Id_CiRevalidateImage,
    Id_CiSetUnlockInformation,
    Id_CiGetBuildExpiryTime
};

//
// Windows 10 RS3
//
static const BYTE CiCallbackIndexes_Win10RS3[] = {
    Id_CiSetFileCache,
    Id_CiGetFileCache,
    Id_CiQueryInformation,
    Id_CiValidateImageHeader,
    Id_CiValidateImageData,
    Id_CiHashMemory,
    Id_KappxIsPackageFile,
    Id_CiCompareSigningLevels,
    Id_CiValidateFileAsImageType,
    Id_CiRegisterSigningInformation,
    Id_CiUnregisterSigningInformation,
    Id_CiInitializePolicy,
    Id_CiReleaseContext,
    Id_XciUnknownCallback,
    Id_CiGetStrongImageReference,
    Id_CiHvciSetImageBaseAddress,
    Id_CipQueryPolicyInformation,
    Id_CiValidateDynamicCodePages,
    Id_CiQuerySecurityPolicy,
    Id_CiRevalidateImage,
    Id_CiSetInformation,
    Id_CiGetBuildExpiryTime
};

//
// Windows 10 RS4-21H2
//
static const BYTE CiCallbackIndexes_Win10RS4_21H2[] = {
    Id_CiSetFileCache,
    Id_CiGetFileCache,
    Id_CiQueryInformation,
    Id_CiValidateImageHeader,
    Id_CiValidateImageData,
    Id_CiHashMemory,
    Id_KappxIsPackageFile,
    Id_CiCompareSigningLevels,
    Id_CiValidateFileAsImageType,
    Id_CiRegisterSigningInformation,
    Id_CiUnregisterSigningInformation,
    Id_CiInitializePolicy,
    Id_CiReleaseContext,
    Id_XciUnknownCallback,
    Id_CiGetStrongImageReference,
    Id_CiHvciSetImageBaseAddress,
    Id_CipQueryPolicyInformation,
    Id_CiValidateDynamicCodePages,
    Id_CiQuerySecurityPolicy,
    Id_CiRevalidateImage,
    Id_CiSetInformation,
    Id_CiSetInformationProcess,
    Id_CiGetBuildExpiryTime,
    Id_CiCheckProcessDebugAccessPolicy
};

//
// Windows 11 21H2
//
static const BYTE CiCallbackIndexes_Win11[] = {
    Id_CiSetFileCache,
    Id_CiGetFileCache,
    Id_CiQueryInformation,
    Id_CiValidateImageHeader,
    Id_CiValidateImageData,
    Id_CiHashMemory,
    Id_KappxIsPackageFile,
    Id_CiCompareSigningLevels,
    Id_CiValidateFileAsImageType,
    Id_CiRegisterSigningInformation,
    Id_CiUnregisterSigningInformation,
    Id_CiInitializePolicy,
    Id_CiReleaseContext,
    Id_XciUnknownCallback,
    Id_CiGetStrongImageReference,
    Id_CiHvciSetImageBaseAddress,
    Id_CipQueryPolicyInformation,
    Id_CiValidateDynamicCodePages,
    Id_CiQuerySecurityPolicy,
    Id_CiRevalidateImage,
    Id_CiSetInformation,
    Id_CiSetInformationProcess,
    Id_CiGetBuildExpiryTime,
    Id_CiCheckProcessDebugAccessPolicy,
    Id_CiGetCodeIntegrityOriginClaimForFileObject,
    Id_CiDeleteCodeIntegrityOriginClaimMembers,
    Id_CiDeleteCodeIntegrityOriginClaimForFileObject,
    Id_CiHvciReportMmIncompatibility,
    Id_CiCompareExistingSePool,
    Id_CiSetCachedOriginClaim
};

//
// Windows 11 Next
//
static const BYTE CiCallbackIndexes_Win11_Next[] = {
    Id_CiSetFileCache,
    Id_CiGetFileCache,
    Id_CiQueryInformation,
    Id_CiValidateImageHeader,
    Id_CiValidateImageData,
    Id_CiHashMemory,
    Id_KappxIsPackageFile,
    Id_CiCompareSigningLevels,
    Id_CiValidateFileAsImageType,
    Id_CiRegisterSigningInformation,
    Id_CiUnregisterSigningInformation,
    Id_CiInitializePolicy,
    Id_CiReleaseContext,
    Id_XciUnknownCallback,
    Id_CiGetStrongImageReference,
    Id_CiHvciSetImageBaseAddress,
    Id_CipQueryPolicyInformation,
    Id_CiQuerySecurityPolicy,
    Id_CiRevalidateImage,
    Id_CiSetInformation,
    Id_CiSetInformationProcess,
    Id_CiGetBuildExpiryTime,
    Id_CiCheckProcessDebugAccessPolicy,
    Id_CiGetCodeIntegrityOriginClaimForFileObject,
    Id_CiDeleteCodeIntegrityOriginClaimMembers,
    Id_CiDeleteCodeIntegrityOriginClaimForFileObject,
    Id_CiHvciReportMmIncompatibility,
    Id_CiCompareExistingSePool,
    Id_CiSetCachedOriginClaim
};

/*
* GetCiRoutineNameFromIndex
*
* Purpose:
*
* Return CiCallback name by index
*
*/
LPWSTR GetCiRoutineNameFromIndex(
    _In_ ULONG Index)
{
    ULONG ArrayCount = 0, index;
    CONST BYTE *Indexes;

    switch (g_NtBuildNumber) {

    case NT_WIN7_RTM:
    case NT_WIN7_SP1:
        Indexes = CiCallbackIndexes_Win7;
        ArrayCount = RTL_NUMBER_OF(CiCallbackIndexes_Win7);
        break;

    case NT_WIN8_RTM:
        Indexes = CiCallbackIndexes_Win8;
        ArrayCount = RTL_NUMBER_OF(CiCallbackIndexes_Win8);
        break;

    case NT_WIN8_BLUE:
        Indexes = CiCallbackIndexes_Win81;
        ArrayCount = RTL_NUMBER_OF(CiCallbackIndexes_Win81);
        break;

    case NT_WIN10_THRESHOLD1:
    case NT_WIN10_THRESHOLD2:
        Indexes = CiCallbackIndexes_Win10Threshold;
        ArrayCount = RTL_NUMBER_OF(CiCallbackIndexes_Win10Threshold);
        break;

    case NT_WIN10_REDSTONE1:
        Indexes = CiCallbackIndexes_Win10RS1;
        ArrayCount = RTL_NUMBER_OF(CiCallbackIndexes_Win10RS1);
        break;

    case NT_WIN10_REDSTONE2:
        Indexes = CiCallbackIndexes_Win10RS2;
        ArrayCount = RTL_NUMBER_OF(CiCallbackIndexes_Win10RS2);
        break;

    case NT_WIN10_REDSTONE3:
        Indexes = CiCallbackIndexes_Win10RS3;
        ArrayCount = RTL_NUMBER_OF(CiCallbackIndexes_Win10RS3);
        break;

    case NT_WIN10_REDSTONE4:
    case NT_WIN10_REDSTONE5:
    case NT_WIN10_19H1:
    case NT_WIN10_19H2:
    case NT_WIN10_20H1:
    case NT_WIN10_20H2:
    case NT_WIN10_21H1:
    case NT_WIN10_21H2:
        Indexes = CiCallbackIndexes_Win10RS4_21H2;
        ArrayCount = RTL_NUMBER_OF(CiCallbackIndexes_Win10RS4_21H2);
        break;
    case NT_WIN11_21H2:
        Indexes = CiCallbackIndexes_Win11;
        ArrayCount = RTL_NUMBER_OF(CiCallbackIndexes_Win11);
        break;
    case NTX_WIN11_ADB:
    default:
        Indexes = CiCallbackIndexes_Win11_Next;
        ArrayCount = RTL_NUMBER_OF(CiCallbackIndexes_Win11_Next);
        break;
    }

    if (Index >= ArrayCount)
        return T_CannotQuery;

    index = Indexes[Index];
    if (index >= RTL_NUMBER_OF(CiCallbackNames))
        return T_CannotQuery;

    return (LPWSTR)CiCallbackNames[index];
}

/*
* ComputeAddressInsideNtOs
*
* Purpose:
*
* Returns kernel variable computed address within ntoskrnl image or zero in case of error.
*
*/
ULONG_PTR ComputeAddressInsideNtOs(
    _In_ ULONG_PTR CodeBase,
    _In_ ULONG_PTR Offset,
    _In_ ULONG InstructionLength,
    _In_ LONG Relative
)
{
    ULONG_PTR address;

    if (Relative == 0)
        return 0;

    address = kdAdjustAddressToNtOsBase(CodeBase, Offset, InstructionLength, Relative);

    if (!IN_REGION(address,
        g_kdctx.NtOsBase,
        g_kdctx.NtOsSize))
    {
        return 0;
    }

    return address;
}

/*
* FindCiCallbacks
*
* Purpose:
*
* Locate address of ntoskrnl g_CiCallbacks/SeCiCallbacks structure.
*
*/
OBEX_FINDCALLBACK_ROUTINE(FindCiCallbacks)
{
    ULONG_PTR kvarAddress = 0;

    PBYTE   Signature = NULL, ptrCode = NULL, InstructionMatchPattern = NULL;
    ULONG   SignatureSize = 0, InstructionMatchLength;
    ULONG   InstructionExactMatchLength;

    PVOID   SectionBase;
    ULONG   SectionSize = 0, Index;
    LPCWSTR KVARName;

    LONG    Rel = 0;
    hde64s  hs;

    UNREFERENCED_PARAMETER(QueryFlags);

    do {

        //
        // Symbols query.
        //
        if (kdIsSymAvailable((PSYMCONTEXT)g_kdctx.NtOsSymContext)) {

            if (g_NtBuildNumber < NT_WIN8_RTM) {

                KVARName = (LPCWSTR)KVAR_g_CiCallbacks;

            }
            else {

                KVARName = (LPCWSTR)KVAR_SeCiCallbacks;

            }

            kdGetAddressFromSymbol(&g_kdctx,
                KVARName,
                &kvarAddress);

        }

        //
        // Pattern searching.
        //
        if (kvarAddress == 0) {

            //
            // Locate PAGE image section as required variable is always in PAGE.
            //
            SectionBase = supLookupImageSectionByName(
                PAGE_SECTION,
                PAGE_SECTION_LEGNTH,
                g_kdctx.NtOsImageMap,
                &SectionSize);

            if ((SectionBase == 0) || (SectionSize == 0))
                break;

            InstructionMatchPattern = SeCiCallbacksMatchingPattern; //default matching pattern
            InstructionMatchLength = 7; //lea
            InstructionExactMatchLength = RTL_NUMBER_OF(SeCiCallbacksMatchingPattern);

            switch (g_NtBuildNumber) {

            case NT_WIN7_SP1:
                Signature = g_CiCallbacksPattern_7601;
                SignatureSize = sizeof(g_CiCallbacksPattern_7601);
                InstructionMatchPattern = g_CiCallbacksMatchingPattern;
                InstructionExactMatchLength = RTL_NUMBER_OF(g_CiCallbacksMatchingPattern);
                break;

            case NT_WIN8_RTM:
            case NT_WIN8_BLUE:
                Signature = SeCiCallbacksPattern_9200_9600;
                SignatureSize = sizeof(SeCiCallbacksPattern_9200_9600);
                break;

            case NT_WIN10_THRESHOLD1:
            case NT_WIN10_THRESHOLD2:
                Signature = SeCiCallbacksPattern_10240_10586;
                SignatureSize = sizeof(SeCiCallbacksPattern_10240_10586);
                break;

            case NT_WIN10_REDSTONE1:
                Signature = SeCiCallbacksPattern_14393;
                SignatureSize = sizeof(SeCiCallbacksPattern_14393);
                break;

            case NT_WIN10_REDSTONE2:
            case NT_WIN10_REDSTONE3:
                Signature = SeCiCallbacksPattern_15063_16299;
                SignatureSize = sizeof(SeCiCallbacksPattern_15063_16299);
                break;

            case NT_WIN10_REDSTONE4:
            case NT_WIN10_REDSTONE5:
                Signature = SeCiCallbacksPattern_17134_17763;
                SignatureSize = sizeof(SeCiCallbacksPattern_17134_17763);
                break;

            case NT_WIN10_19H1:
            case NT_WIN10_19H2:
            case NT_WIN10_20H1:
            case NT_WIN10_20H2:
            case NT_WIN10_21H1:
            case NT_WIN10_21H2:
                Signature = SeCiCallbacksPattern_19H1_21H2;
                SignatureSize = sizeof(SeCiCallbacksPattern_19H1_21H2);
                InstructionMatchPattern = SeCiCallbacksMatchingPattern_19H1_21H2;
                InstructionMatchLength = 10; //mov
                InstructionExactMatchLength = RTL_NUMBER_OF(SeCiCallbacksMatchingPattern_19H1_21H2);
                break;

            case NT_WIN11_21H2:
            default:
                Signature = SeCiCallbacksPattern_W11_21H2;
                if (g_NtBuildNumber > NT_WIN11_21H2) {
                    SeCiCallbacksPattern_W11_21H2[2] = 0xEC; //test only, remove with proper find pattern mask support
                }
                SignatureSize = sizeof(SeCiCallbacksPattern_W11_21H2);
                InstructionMatchPattern = SeCiCallbacksMatchingPattern_19H1_21H2;
                InstructionMatchLength = 10; //mov
                InstructionExactMatchLength = RTL_NUMBER_OF(SeCiCallbacksMatchingPattern_19H1_21H2);
                break;
            }

            ptrCode = (PBYTE)supFindPattern(
                (PBYTE)SectionBase,
                SectionSize,
                Signature,
                SignatureSize);

            if (ptrCode == NULL)
                break;

            if (g_NtBuildNumber <= NT_WIN7_SP1) {

                //
                // Find reference to g_CiCallbacks in code.
                //

                Index = 0; //pattern search include target instruction, do not skip

            }
            else {

                //
                // Find reference to SeCiCallbacks/g_CiCallbacks in code.
                //

                Index = SignatureSize; //skip signature instructions

            }

            do {
                hde64_disasm((void*)(ptrCode + Index), &hs);
                if (hs.flags & F_ERROR)
                    break;
                //
                // mov cs:g_CiCallbacks, rax (for Windows 7)
                // lea rcx, SeCiCallbacks (for 8/10 TH/RS)
                // mov cs:SeCiCallbacks (19H1-21H1)
                //
                if (hs.len == InstructionMatchLength) {

                    //
                    // Match block found.
                    //
                    if (RtlCompareMemory((VOID*)&ptrCode[Index], (VOID*)InstructionMatchPattern,
                        InstructionExactMatchLength) == InstructionExactMatchLength)
                    {
                        Rel = *(PLONG)(ptrCode + Index + InstructionExactMatchLength);
                        break;
                    }
                }
                Index += hs.len;

            } while (Index < 64);

            kvarAddress = ComputeAddressInsideNtOs((ULONG_PTR)ptrCode, Index, hs.len, Rel);

        }

    } while (FALSE);

    if (kvarAddress == 0)
        logAdd(WOBJ_LOG_ENTRY_WARNING, TEXT("Could not locate CiCallbacks"));

    return kvarAddress;
}

BOOL IopFileSystemIsValidPattern(
    _In_ PBYTE Buffer,
    _In_ ULONG Offset,
    _In_ ULONG InstructionSize
)
{
    BOOL bResult = FALSE;

    if (g_NtBuildNumber <= NT_WIN11_21H2) {

        //
        // lea  rdx, xxx                
        //
        if ((Buffer[Offset] == 0x48) &&
            (Buffer[Offset + 1] == 0x8D) &&
            (Buffer[Offset + 2] == 0x0D) &&
            ((Buffer[Offset + InstructionSize] == 0x48) || (Buffer[Offset + InstructionSize] == 0xE9)))
        {
            bResult = TRUE;
        }

    }
    else { //win11 22h1+

        //
        // mov  rcx, xxx                
        //
        if ((Buffer[Offset] == 0x48) &&
            (Buffer[Offset + 1] == 0x8B) &&
            (Buffer[Offset + 2] == 0x0D) &&
            (
                (Buffer[Offset + InstructionSize] == 0x48) ||
                (Buffer[Offset + InstructionSize] == 0xE9) ||
                (Buffer[Offset + InstructionSize] == 0x8B))
            )
        {
            bResult = TRUE;
        }

    }

    return bResult;
}

/*
* FindIopFileSystemQueueHeads
*
* Purpose:
*
* Return addresses of list heads for callbacks registered with:
*
*   IoRegisterFileSystem
*
*/
BOOL FindIopFileSystemQueueHeads(
    _Out_ ULONG_PTR *IopCdRomFileSystemQueueHead,
    _Out_ ULONG_PTR *IopDiskFileSystemQueueHead,
    _Out_ ULONG_PTR *IopTapeFileSystemQueueHead,
    _Out_ ULONG_PTR *IopNetworkFileSystemQueueHead
)
{
    BOOL bSymQuerySuccess = FALSE;
    ULONG Index, Count;
    LONG Rel = 0;
    ULONG_PTR kvarAddress = 0;
    PBYTE ptrCode;
    hde64s hs;

    //
    // Assume failure.
    //
    *IopCdRomFileSystemQueueHead = 0;
    *IopDiskFileSystemQueueHead = 0;
    *IopTapeFileSystemQueueHead = 0;
    *IopNetworkFileSystemQueueHead = 0;

    if (kdIsSymAvailable((PSYMCONTEXT)g_kdctx.NtOsSymContext)) {

        do {

            if (!kdGetAddressFromSymbol(&g_kdctx,
                KVAR_IopCdRomFileSystemQueueHead,
                &kvarAddress))
            {
                break;
            }

            *IopCdRomFileSystemQueueHead = kvarAddress;

            if (!kdGetAddressFromSymbol(&g_kdctx,
                KVAR_IopDiskFileSystemQueueHead,
                &kvarAddress))
            {
                break;
            }

            *IopDiskFileSystemQueueHead = kvarAddress;

            if (!kdGetAddressFromSymbol(&g_kdctx,
                KVAR_IopTapeFileSystemQueueHead,
                &kvarAddress))
            {
                break;
            }

            *IopTapeFileSystemQueueHead = kvarAddress;

            if (!kdGetAddressFromSymbol(&g_kdctx,
                KVAR_IopNetworkFileSystemQueueHead,
                &kvarAddress))
            {
                break;
            }

            *IopNetworkFileSystemQueueHead = kvarAddress;

            bSymQuerySuccess = TRUE;

        } while (FALSE);

    }

    if (bSymQuerySuccess)
        return TRUE;

    ptrCode = (PBYTE)GetProcAddress((HMODULE)g_kdctx.NtOsImageMap, 
        "IoRegisterFileSystem");

    if (ptrCode == NULL)
        return 0;

    Index = 0;
    Rel = 0;
    Count = 0;

    if (g_NtBuildNumber < NT_WIN8_RTM) {

        do {
            hde64_disasm(ptrCode + Index, &hs);
            if (hs.flags & F_ERROR)
                break;

            if (hs.len == 7) {
                //
                // lea  rdx, xxx                
                //
                if ((ptrCode[Index] == 0x48) &&
                    (ptrCode[Index + 1] == 0x8D) &&
                    (ptrCode[Index + 2] == 0x15))
                {
                    Rel = *(PLONG)(ptrCode + Index + 3);
                    if (Rel) {

                        kvarAddress = kdAdjustAddressToNtOsBase((ULONG_PTR)ptrCode, Index, hs.len, Rel);

                        if (kdAddressInNtOsImage((PVOID)kvarAddress)) {

                            switch (Count) {
                            case 0:
                                *IopNetworkFileSystemQueueHead = kvarAddress;
                                break;

                            case 1:
                                *IopCdRomFileSystemQueueHead = kvarAddress;
                                break;

                            case 2:
                                *IopDiskFileSystemQueueHead = kvarAddress;
                                break;

                            case 3:
                                *IopTapeFileSystemQueueHead = kvarAddress;
                                break;

                            default:
                                break;
                            }
                            Count += 1;
                            if (Count == 4)
                                break;
                        }
                    }
                }

            }

            Index += hs.len;

        } while (Index < 512);

    }
    else {

        do {
            hde64_disasm(ptrCode + Index, &hs);
            if (hs.flags & F_ERROR)
                break;

            if (hs.len == 7) {

                if (IopFileSystemIsValidPattern(ptrCode, Index, hs.len)) {
                    Rel = *(PLONG)(ptrCode + Index + 3);
                    if (Rel) {

                        kvarAddress = kdAdjustAddressToNtOsBase((ULONG_PTR)ptrCode, Index, hs.len, Rel);

                        if (kdAddressInNtOsImage((PVOID)kvarAddress)) {

                            switch (Count) {

                            case 0:
                                *IopDiskFileSystemQueueHead = kvarAddress;
                                break;

                            case 1:
                                *IopCdRomFileSystemQueueHead = kvarAddress;
                                break;

                            case 2:
                                *IopNetworkFileSystemQueueHead = kvarAddress;
                                break;

                            case 3:
                                *IopTapeFileSystemQueueHead = kvarAddress;
                                break;

                            default:
                                break;
                            }
                            Count += 1;
                            if (Count == 4)
                                break;
                        }
                    }
                }

            }

            Index += hs.len;

        } while (Index < 512);

    }

    return (Count == 4);
}

/*
* FindIopFsNotifyChangeQueueHead
*
* Purpose:
*
* Return address of list head for callbacks registered with:
*
*   IoRegisterFsRegistrationChange
*
*/
OBEX_FINDCALLBACK_ROUTINE(FindIopFsNotifyChangeQueueHead)
{
    ULONG Index;
    LONG Rel = 0;
    ULONG_PTR kvarAddress = 0;
    PBYTE ptrCode;
    hde64s hs;

    UNREFERENCED_PARAMETER(QueryFlags);

    if (kdIsSymAvailable((PSYMCONTEXT)g_kdctx.NtOsSymContext)) {

        kdGetAddressFromSymbol(&g_kdctx,
            KVAR_IopFsNotifyChangeQueueHead,
            &kvarAddress);

    }

    if (kvarAddress == 0) {

        ptrCode = (PBYTE)GetProcAddress((HMODULE)g_kdctx.NtOsImageMap, 
            "IoUnregisterFsRegistrationChange");

        if (ptrCode == NULL)
            return 0;

        Index = 0;
        Rel = 0;

        do {
            hde64_disasm(ptrCode + Index, &hs);
            if (hs.flags & F_ERROR)
                break;

            if (hs.len == 7) {
                //
                // lea  rax, IopFsNotifyChangeQueueHead
                // jmp  short
                //
                if ((ptrCode[Index] == 0x48) &&
                    (ptrCode[Index + 1] == 0x8D) &&
                    (ptrCode[Index + 2] == 0x05) &&
                    (ptrCode[Index + 7] == 0xEB))
                {
                    Rel = *(PLONG)(ptrCode + Index + 3);
                    break;
                }

            }

            Index += hs.len;

        } while (Index < 256);

        kvarAddress = ComputeAddressInsideNtOs((ULONG_PTR)ptrCode, Index, hs.len, Rel);

    }

    return kvarAddress;
}

/*
* FindRtlpDebugPrintCallbackList
*
* Purpose:
*
* Return address of list head for callbacks registered with:
*
*   DbgSetDebugPrintCallback
*
*/
OBEX_FINDCALLBACK_ROUTINE(FindRtlpDebugPrintCallbackList)
{
    ULONG Index;
    LONG Rel = 0;
    ULONG_PTR kvarAddress = 0;
    PBYTE ptrCode;
    hde64s hs;

    UNREFERENCED_PARAMETER(QueryFlags);

    if (kdIsSymAvailable((PSYMCONTEXT)g_kdctx.NtOsSymContext)) {

        kdGetAddressFromSymbol(&g_kdctx,
            KVAR_RtlpDebugPrintCallbackList,
            &kvarAddress);

    } 

    if (kvarAddress == 0) {

        ptrCode = (PBYTE)GetProcAddress((HMODULE)g_kdctx.NtOsImageMap, "DbgSetDebugPrintCallback");
        if (ptrCode == NULL)
            return 0;

        //
        // Find DbgpInsertDebugPrintCallback pointer.
        //
        Index = 0;
        do {

            hde64_disasm(ptrCode + Index, &hs);
            if (hs.flags & F_ERROR)
                break;

            //jmp/call DbgpInsertDebugPrintCallback
            if (hs.len == 5) {

                if ((ptrCode[Index] == 0xE9) ||
                    (ptrCode[Index] == 0xE8))
                {
                    Rel = *(PLONG)(ptrCode + Index + 1);
                    break;
                }
            }
            //jz
            if (hs.len == 6) {

                if (ptrCode[Index] == 0x0F) {
                    Rel = *(PLONG)(ptrCode + Index + 2);
                    break;
                }
            }
            Index += hs.len;

        } while (Index < 64);

        if (Rel == 0)
            return 0;

        ptrCode = ptrCode + Index + (hs.len) + Rel;
        Index = 0;
        Rel = 0;

        //
        // Complicated search. Not unique search patterns.
        //

        do {
            hde64_disasm(ptrCode + Index, &hs);
            if (hs.flags & F_ERROR)
                break;

            //
            // lea  reg, RtlpDebugPrintCallbackList
            //
            if (hs.len == 7) {
                if ((ptrCode[Index] == 0x48) &&
                    (ptrCode[Index + 1] == 0x8D) &&
                    ((ptrCode[Index + 2] == 0x15) || (ptrCode[Index + 2] == 0x0D)) &&
                    (ptrCode[Index + hs.len] == 0x48))
                {
                    Rel = *(PLONG)(ptrCode + Index + 3);
                    break;
                }
            }

            Index += hs.len;

        } while (Index < 512);

        kvarAddress = ComputeAddressInsideNtOs((ULONG_PTR)ptrCode, Index, hs.len, Rel);

    }

    return kvarAddress;
}

/*
* FindPopRegisteredPowerSettingCallbacks
*
* Purpose:
*
* Return address of list head for callbacks registered with:
*
*   PoRegisterPowerSettingCallback
*
*/
OBEX_FINDCALLBACK_ROUTINE(FindPopRegisteredPowerSettingCallbacks)
{
    ULONG Index;
    LONG Rel = 0;
    ULONG_PTR kvarAddress = 0;
    PBYTE ptrCode;
    hde64s hs;

    UNREFERENCED_PARAMETER(QueryFlags);

    if (kdIsSymAvailable((PSYMCONTEXT)g_kdctx.NtOsSymContext)) {

        kdGetAddressFromSymbol(&g_kdctx,
            KVAR_PopRegisteredPowerSettingCallbacks,
            &kvarAddress);

    }

    if (kvarAddress == 0) {

        ptrCode = (PBYTE)GetProcAddress((HMODULE)g_kdctx.NtOsImageMap, 
            "PoRegisterPowerSettingCallback");

        if (ptrCode == NULL)
            return 0;

        Index = 0;
        Rel = 0;

        do {
            hde64_disasm(ptrCode + Index, &hs);
            if (hs.flags & F_ERROR)
                break;

            if (hs.len == 7) {
                //
                // lea      rcx, PopRegisteredPowerSettingCallbacks
                // mov      [rbx + 8], rax |
                // cmp      [rax], rcx
                //
                if ((ptrCode[Index] == 0x48) &&
                    (ptrCode[Index + 1] == 0x8D) &&
                    (ptrCode[Index + 2] == 0x0D) &&
                    (ptrCode[Index + 7] == 0x48))
                {
                    Rel = *(PLONG)(ptrCode + Index + 3);
                    break;
                }

            }

            Index += hs.len;

        } while (Index < 512);

        kvarAddress = ComputeAddressInsideNtOs((ULONG_PTR)ptrCode, Index, hs.len, Rel);

    }

    return kvarAddress;
}

/*
* FindSeFileSystemNotifyRoutinesHead
*
* Purpose:
*
* Return address of list head for callbacks registered with:
*
*   SeRegisterLogonSessionTerminatedRoutine
*   SeRegisterLogonSessionTerminatedRoutineEx
*
*/
OBEX_FINDCALLBACK_ROUTINE(FindSeFileSystemNotifyRoutinesHead)
{
    BOOL Extended = (BOOL)(ULONG)QueryFlags;
    ULONG Index;
    LONG Rel = 0;
    ULONG_PTR kvarAddress = 0;
    LPCWSTR lpVarName;
    LPSTR lpCallbackName;
    PBYTE ptrCode;
    hde64s hs;


    if (kdIsSymAvailable((PSYMCONTEXT)g_kdctx.NtOsSymContext)) {

        if (Extended)
            lpVarName = KVAR_SeFileSystemNotifyRoutinesExHead;
        else
            lpVarName = KVAR_SeFileSystemNotifyRoutinesHead;

        kdGetAddressFromSymbol(&g_kdctx,
            lpVarName,
            &kvarAddress);

    }

    if (kvarAddress == 0) {

        //
        // Routines have similar design.
        //
        if (Extended) {
            lpCallbackName = "SeRegisterLogonSessionTerminatedRoutineEx";
        }
        else {
            lpCallbackName = "SeRegisterLogonSessionTerminatedRoutine";
        }

        ptrCode = (PBYTE)GetProcAddress((HMODULE)g_kdctx.NtOsImageMap, lpCallbackName);
        if (ptrCode == NULL)
            return 0;

        Index = 0;
        Rel = 0;

        do {
            hde64_disasm(ptrCode + Index, &hs);
            if (hs.flags & F_ERROR)
                break;

            if (hs.len == 7) {

                //
                // mov     rax, cs:SeFileSystemNotifyRoutines(Ex)Head
                //

                if ((ptrCode[Index] == 0x48) &&
                    (ptrCode[Index + 1] == 0x8B) &&
                    (ptrCode[Index + 2] == 0x05))
                {
                    Rel = *(PLONG)(ptrCode + Index + 3);
                    break;
                }

            }

            Index += hs.len;

        } while (Index < 128);

        kvarAddress = ComputeAddressInsideNtOs((ULONG_PTR)ptrCode, Index, hs.len, Rel);

    }

    return kvarAddress;
}

/*
* FindObjectTypeCallbackListHeadByType
*
* Purpose:
*
* Return address of list head for callbacks registered with:
*
*   ObRegisterCallbacks
*
*/
OBEX_FINDCALLBACK_ROUTINE(FindObjectTypeCallbackListHeadByType)
{
    ULONG Type = (ULONG)QueryFlags;
    ULONG_PTR ListHead = 0;
    ULONG ObjectSize, ObjectVersion = 0, CallbackListOffset = 0;
    LPWSTR TypeName = NULL;
    POBJINFO CurrentObject = NULL;
    PVOID ObjectTypeInformation = NULL;

    union {
        union {
            OBJECT_TYPE_7 *ObjectType_7;
            OBJECT_TYPE_8 *ObjectType_8;
            OBJECT_TYPE_RS1 *ObjectType_RS1;
            OBJECT_TYPE_RS2 *ObjectType_RS2;
        } Versions;
        PVOID Ref;
    } ObjectType;

    switch (Type) {
    case 0: //PsProcessType
        TypeName = TEXT("Process");
        break;
    case 1: //PsThreadType
        TypeName = TEXT("Thread");
        break;
    case 2:
        //ExDesktopObjectType
        TypeName = TEXT("Desktop");
        break;
    default:
        return 0;
    }

    //
    // Get the reference to the object.
    //
    CurrentObject = ObQueryObject(T_OBJECTTYPES, TypeName);
    if (CurrentObject == NULL)
        return 0;

    //
    // Dump object information version aware.
    //
    ObjectTypeInformation = ObDumpObjectTypeVersionAware(
        CurrentObject->ObjectAddress,
        &ObjectSize,
        &ObjectVersion);

    if (ObjectTypeInformation == NULL) {
        supHeapFree(CurrentObject);
        return 0;
    }

    ObjectType.Ref = ObjectTypeInformation;

    //
    // Flags in structure offset compatible fields.
    //
    if (ObjectType.Versions.ObjectType_7->TypeInfo.SupportsObjectCallbacks) {

        //
        // Calculate offset to structure field.
        //
        switch (ObjectVersion) {
        case OBVERSION_OBJECT_TYPE_V1:
            CallbackListOffset = FIELD_OFFSET(OBJECT_TYPE_7, CallbackList);
            break;

        case OBVERSION_OBJECT_TYPE_V2:
            CallbackListOffset = FIELD_OFFSET(OBJECT_TYPE_8, CallbackList);
            break;

        case OBVERSION_OBJECT_TYPE_V3:
            CallbackListOffset = FIELD_OFFSET(OBJECT_TYPE_RS1, CallbackList);
            break;

        default:
            CallbackListOffset = FIELD_OFFSET(OBJECT_TYPE_RS2, CallbackList);
            break;
        }

        ListHead = CurrentObject->ObjectAddress + CallbackListOffset;
    }

    supHeapFree(CurrentObject);
    supVirtualFree(ObjectTypeInformation);
    return ListHead;
}

/*
* FindIopNotifyShutdownQueueHeadHead
*
* Purpose:
*
* Return address of list head for callbacks registered with:
*
*   IoRegisterShutdownNotification
*   IoRegisterLastChanceShutdownNotification
*
*/
OBEX_FINDCALLBACK_ROUTINE(FindIopNotifyShutdownQueueHeadHead)
{
    BOOL bLastChance = (BOOL)(ULONG)QueryFlags;
    ULONG Index;
    LONG Rel = 0;
    ULONG_PTR kvarAddress = 0;
    LPCWSTR lpVarName;
    LPSTR lpCallbackName;
    PBYTE ptrCode;
    hde64s hs;

    if (kdIsSymAvailable((PSYMCONTEXT)g_kdctx.NtOsSymContext)) {

        if (bLastChance)
            lpVarName = KVAR_IopNotifyLastChanceShutdownQueueHead;
        else
            lpVarName = KVAR_IopNotifyShutdownQueueHead;

        kdGetAddressFromSymbol(&g_kdctx,
            lpVarName,
            &kvarAddress);

    }

    if (kvarAddress == 0) {

        //
        // Routines have similar design.
        //
        if (bLastChance) {
            lpCallbackName = "IoRegisterLastChanceShutdownNotification";
        }
        else {
            lpCallbackName = "IoRegisterShutdownNotification";
        }

        ptrCode = (PBYTE)GetProcAddress((HMODULE)g_kdctx.NtOsImageMap, lpCallbackName);
        if (ptrCode == NULL)
            return 0;

        Index = 0;
        Rel = 0;

        do {
            hde64_disasm(ptrCode + Index, &hs);
            if (hs.flags & F_ERROR)
                break;

            if (hs.len == 7) { //check if lea

                if (((ptrCode[Index] == 0x48) || (ptrCode[Index] == 0x4C)) &&
                    (ptrCode[Index + 1] == 0x8D))
                {
                    Rel = *(PLONG)(ptrCode + Index + 3);
                    break;
                }

            }

            Index += hs.len;

        } while (Index < 128);

        kvarAddress = ComputeAddressInsideNtOs((ULONG_PTR)ptrCode, Index, hs.len, Rel);

    }

    return kvarAddress;
}

/*
* FindCmCallbackHead
*
* Purpose:
*
* Return address of list head for callbacks registered with:
*
*   CmRegisterCallback
*   CmRegisterCallbackEx
*
*/
OBEX_FINDCALLBACK_ROUTINE(FindCmCallbackHead)
{
    ULONG Index, resultOffset;
    LONG Rel = 0, FirstInstructionLength;
    ULONG_PTR kvarAddress = 0;
    PBYTE ptrCode;
    hde64s hs, hs_next;

    UNREFERENCED_PARAMETER(QueryFlags);

    if (kdIsSymAvailable((PSYMCONTEXT)g_kdctx.NtOsSymContext)) {

        kdGetAddressFromSymbol(&g_kdctx,
            KVAR_CallbackListHead,
            &kvarAddress);

    }

    if (kvarAddress == 0) {

        ptrCode = (PBYTE)GetProcAddress((HMODULE)g_kdctx.NtOsImageMap, "CmUnRegisterCallback");
        if (ptrCode == NULL)
            return 0;

        Index = 0;
        Rel = 0;
        resultOffset = 0;

        do {
            hde64_disasm(ptrCode + Index, &hs);
            if (hs.flags & F_ERROR)
                break;

            if (hs.len == 5) {
                /*
                ** lea     rdx, [rsp+20h] <-
                ** lea     rcx, CallbackListHead
                */
                if ((ptrCode[Index] == 0x48) &&
                    (ptrCode[Index + 1] == 0x8D) &&
                    (ptrCode[Index + 2] == 0x54))
                {
                    hde64_disasm(ptrCode + Index + hs.len, &hs_next);
                    if (hs_next.flags & F_ERROR)
                        break;
                    if (hs_next.len == 7) {

                        /*
                        ** lea     rdx, [rsp+20h]
                        ** lea     rcx, CallbackListHead <-
                        */
                        FirstInstructionLength = hs.len;

                        if ((ptrCode[Index + FirstInstructionLength] == 0x48) &&
                            (ptrCode[Index + FirstInstructionLength + 1] == 0x8D) &&
                            (ptrCode[Index + FirstInstructionLength + 2] == 0x0D))
                        {
                            resultOffset = Index + FirstInstructionLength + hs_next.len;
                            Rel = *(PLONG)(ptrCode + Index + FirstInstructionLength + 3);
                            break;
                        }
                    }
                }
            }

            Index += hs.len;

        } while (Index < 256);

        kvarAddress = ComputeAddressInsideNtOs((ULONG_PTR)ptrCode, resultOffset, 0, Rel);

    }

    return kvarAddress;
}

/*
* FindKeBugCheckReasonCallbackHead
*
* Purpose:
*
* Return address of list head for callbacks registered with:
*
*   KeRegisterBugCheckReasonCallback
*
*/
OBEX_FINDCALLBACK_ROUTINE(FindKeBugCheckReasonCallbackHead)
{
    ULONG Index;
    LONG Rel = 0;
    ULONG_PTR kvarAddress = 0;
    PBYTE ptrCode;
    hde64s hs;

    UNREFERENCED_PARAMETER(QueryFlags);

    if (kdIsSymAvailable((PSYMCONTEXT)g_kdctx.NtOsSymContext)) {

        kdGetAddressFromSymbol(&g_kdctx,
            KVAR_KeBugCheckReasonCallbackListHead,
            &kvarAddress);

    }

    if (kvarAddress == 0) {

        ptrCode = (PBYTE)GetProcAddress((HMODULE)g_kdctx.NtOsImageMap, 
            "KeRegisterBugCheckReasonCallback");

        if (ptrCode == NULL)
            return 0;

        Index = 0;
        Rel = 0;

        do {
            hde64_disasm(ptrCode + Index, &hs);
            if (hs.flags & F_ERROR)
                break;

            if (hs.len == 7) { //check if lea

                if (((ptrCode[Index] == 0x48) || (ptrCode[Index] == 0x4C)) &&
                    (ptrCode[Index + 1] == 0x8D) &&
                    ((ptrCode[Index + hs.len] == 0x48) || (ptrCode[Index + hs.len] == 0x83)))
                {
                    Rel = *(PLONG)(ptrCode + Index + 3);
                    break;
                }

            }

            Index += hs.len;

        } while (Index < 512);

        kvarAddress = ComputeAddressInsideNtOs((ULONG_PTR)ptrCode, Index, hs.len, Rel);

    }

    return kvarAddress;
}

/*
* FindKeBugCheckCallbackHead
*
* Purpose:
*
* Return address of list head for callbacks registered with:
*
*   KeRegisterBugCheckCallback
*
*/
OBEX_FINDCALLBACK_ROUTINE(FindKeBugCheckCallbackHead)
{
    ULONG Index;
    LONG Rel = 0;
    ULONG_PTR kvarAddress = 0;
    PBYTE ptrCode;
    hde64s hs;

    UNREFERENCED_PARAMETER(QueryFlags);

    if (kdIsSymAvailable((PSYMCONTEXT)g_kdctx.NtOsSymContext)) {

        kdGetAddressFromSymbol(&g_kdctx,
            KVAR_KeBugCheckCallbackListHead,
            &kvarAddress);

    }

    if (kvarAddress == 0) {

        ptrCode = (PBYTE)GetProcAddress((HMODULE)g_kdctx.NtOsImageMap,
            "KeRegisterBugCheckCallback");

        if (ptrCode == NULL)
            return 0;

        Index = 0;
        Rel = 0;

        do {
            hde64_disasm(ptrCode + Index, &hs);
            if (hs.flags & F_ERROR)
                break;

            if (hs.len == 7) { //check if lea + mov

                if (((ptrCode[Index] == 0x48) || (ptrCode[Index] == 0x4C)) &&
                    (ptrCode[Index + 1] == 0x8D) &&
                    (ptrCode[Index + hs.len] == 0x48))
                {
                    Rel = *(PLONG)(ptrCode + Index + 3);
                    break;
                }

            }

            Index += hs.len;

        } while (Index < 512);

        kvarAddress = ComputeAddressInsideNtOs((ULONG_PTR)ptrCode, Index, hs.len, Rel);

    }

    return kvarAddress;
}

/*
* FindPspLoadImageNotifyRoutine
*
* Purpose:
*
* Return array address of callbacks registered with:
*
*   PsSetLoadImageNotifyRoutine
*   PsSetLoadImageNotifyRoutineEx
*
*/
OBEX_FINDCALLBACK_ROUTINE(FindPspLoadImageNotifyRoutine)
{
    ULONG Index;
    LONG Rel = 0;
    ULONG_PTR kvarAddress = 0;
    PBYTE ptrCode;
    hde64s hs;

    UNREFERENCED_PARAMETER(QueryFlags);

    if (kdIsSymAvailable((PSYMCONTEXT)g_kdctx.NtOsSymContext)) {

        kdGetAddressFromSymbol(&g_kdctx,
            KVAR_PspLoadImageNotifyRoutine,
            &kvarAddress);

    }

    if (kvarAddress == 0) {

        ptrCode = (PBYTE)GetProcAddress((HMODULE)g_kdctx.NtOsImageMap,
            "PsRemoveLoadImageNotifyRoutine");

        if (ptrCode == NULL)
            return 0;


        Index = 0;
        Rel = 0;

        do {
            hde64_disasm(ptrCode + Index, &hs);
            if (hs.flags & F_ERROR)
                break;

            if (hs.len == 7) { //check if lea

                if (((ptrCode[Index] == 0x48) || (ptrCode[Index] == 0x4C)) &&
                    (ptrCode[Index + 1] == 0x8D))
                {
                    Rel = *(PLONG)(ptrCode + Index + 3);
                    break;
                }

            }

            Index += hs.len;

        } while (Index < 128);

        kvarAddress = ComputeAddressInsideNtOs((ULONG_PTR)ptrCode, Index, hs.len, Rel);

    }

    return kvarAddress;
}

/*
* FindPspCreateThreadNotifyRoutine
*
* Purpose:
*
* Return array address of callbacks registered with:
*
*   PsSetCreateThreadNotifyRoutine
*   PsSetCreateThreadNotifyRoutineEx
*
*/
OBEX_FINDCALLBACK_ROUTINE(FindPspCreateThreadNotifyRoutine)
{
    ULONG Index;
    LONG Rel = 0;
    ULONG_PTR kvarAddress = 0;
    PBYTE ptrCode;
    hde64s hs;

    UNREFERENCED_PARAMETER(QueryFlags);

    if (kdIsSymAvailable((PSYMCONTEXT)g_kdctx.NtOsSymContext)) {

        kdGetAddressFromSymbol(&g_kdctx,
            KVAR_PspCreateThreadNotifyRoutine,
            &kvarAddress);

    }

    if (kvarAddress == 0) {

        ptrCode = (PBYTE)GetProcAddress((HMODULE)g_kdctx.NtOsImageMap,
            "PsRemoveCreateThreadNotifyRoutine");

        if (ptrCode == NULL)
            return 0;

        Index = 0;
        Rel = 0;

        do {
            hde64_disasm(ptrCode + Index, &hs);
            if (hs.flags & F_ERROR)
                break;

            if (hs.len == 7) { //check if lea

                if (((ptrCode[Index] == 0x48) || (ptrCode[Index] == 0x4C)) &&
                    (ptrCode[Index + 1] == 0x8D))
                {
                    Rel = *(PLONG)(ptrCode + Index + 3);
                    break;
                }

            }

            Index += hs.len;

        } while (Index < 128);

        kvarAddress = ComputeAddressInsideNtOs((ULONG_PTR)ptrCode, Index, hs.len, Rel);

    }

    return kvarAddress;
}

/*
* FindDbgkLmdCallbacks
*
* Purpose:
*
* Return array address of callbacks registered with:
*
*   DbgkLkmdRegisterCallback
*
*/
OBEX_FINDCALLBACK_ROUTINE(FindDbgkLmdCallbacks)
{
    ULONG Index;
    LONG Rel = 0;
    ULONG_PTR kvarAddress = 0;
    PBYTE ptrCode;
    hde64s hs;

    UNREFERENCED_PARAMETER(QueryFlags);

    if (kdIsSymAvailable((PSYMCONTEXT)g_kdctx.NtOsSymContext)) {

        kdGetAddressFromSymbol(&g_kdctx,
            KVAR_DbgkLmdCallbacks,
            &kvarAddress);

    }

    if (kvarAddress == 0) {

        ptrCode = (PBYTE)GetProcAddress((HMODULE)g_kdctx.NtOsImageMap,
            "DbgkLkmdUnregisterCallback");

        if (ptrCode == NULL)
            return 0;

        Index = 0;
        Rel = 0;

        //
        // Find DbgkLmdCallbacks pointer
        //
        do {
            hde64_disasm(ptrCode + Index, &hs);
            if (hs.flags & F_ERROR)
                break;

            if (hs.len == 7) { //check if lea

                //
                // lea     rcx, DbgkLmdCallbacks
                //

                if (((ptrCode[Index] == 0x4C) || (ptrCode[Index] == 0x48)) &&
                    (ptrCode[Index + 1] == 0x8D))
                {
                    Rel = *(PLONG)(ptrCode + Index + 3);
                    break;
                }

            }

            Index += hs.len;

        } while (Index < 64);

        kvarAddress = ComputeAddressInsideNtOs((ULONG_PTR)ptrCode, Index, hs.len, Rel);

    }

    return kvarAddress;
}

/*
* FindPspCreateProcessNotifyRoutine
*
* Purpose:
*
* Return array address of callbacks registered with:
*
*   PsSetCreateProcessNotifyRoutine
*   PsSetCreateProcessNotifyRoutineEx
*   PsSetCreateProcessNotifyRoutineEx2
*
*/
OBEX_FINDCALLBACK_ROUTINE(FindPspCreateProcessNotifyRoutine)
{
    ULONG Index;
    LONG Rel = 0;
    ULONG_PTR kvarAddress = 0;
    PBYTE ptrCode;
    hde64s hs;

    UNREFERENCED_PARAMETER(QueryFlags);

    if (kdIsSymAvailable((PSYMCONTEXT)g_kdctx.NtOsSymContext)) {

        kdGetAddressFromSymbol(&g_kdctx,
            KVAR_PspCreateProcessNotifyRoutine,
            &kvarAddress);

    }

    if (kvarAddress == 0) {

        ptrCode = (PBYTE)GetProcAddress((HMODULE)g_kdctx.NtOsImageMap, 
            "PsSetCreateProcessNotifyRoutine");

        if (ptrCode == NULL)
            return 0;

        //
        // Find PspSetCreateProcessNotifyRoutine pointer.
        //
        Index = 0;
        do {

            hde64_disasm(ptrCode + Index, &hs);
            if (hs.flags & F_ERROR)
                break;

            //jmp/call PspSetCreateProcessNotifyRoutine
            if ((ptrCode[Index] == 0xE9) ||
                (ptrCode[Index] == 0xE8) ||
                (ptrCode[Index] == 0xEB))
            {
                Rel = *(PLONG)(ptrCode + Index + 1);
                break;
            }

            Index += hs.len;

        } while (Index < 64);

        if (Rel == 0)
            return 0;

        ptrCode = ptrCode + Index + (hs.len) + Rel;
        Index = 0;
        Rel = 0;

        do {
            hde64_disasm(ptrCode + Index, &hs);
            if (hs.flags & F_ERROR)
                break;

            if (hs.len == 7) { //check if lea

                if ((ptrCode[Index] == 0x4C) &&
                    (ptrCode[Index + 1] == 0x8D))
                {
                    Rel = *(PLONG)(ptrCode + Index + 3);
                    break;
                }

            }

            Index += hs.len;

        } while (Index < 128);

        kvarAddress = ComputeAddressInsideNtOs((ULONG_PTR)ptrCode, Index, hs.len, Rel);

    }

    return kvarAddress;
}

/*
* FindPsAltSystemCallHandlers
*
* Purpose:
*
* Return array address of callbacks registered with:
*
*   PsRegisterAltSystemCallHandler
*
*/
OBEX_FINDCALLBACK_ROUTINE(FindPsAltSystemCallHandlers)
{
    ULONG_PTR kvarAddress = 0;

    ULONG   Index, InstructionExactMatchLength;
    PBYTE   ptrCode;
    LONG    Rel = 0;
    hde64s  hs;

    UNREFERENCED_PARAMETER(QueryFlags);

    if (kdIsSymAvailable((PSYMCONTEXT)g_kdctx.NtOsSymContext)) {

        kdGetAddressFromSymbol(&g_kdctx,
            KVAR_PsAltSystemCallHandlers,
            &kvarAddress);

    }

    if (kvarAddress == 0) {

        ptrCode = (PBYTE)GetProcAddress((HMODULE)g_kdctx.NtOsImageMap,
            "PsRegisterAltSystemCallHandler");

        if (ptrCode == NULL)
            return 0;

        InstructionExactMatchLength = sizeof(PsAltSystemCallHandlersPattern);

        Index = 0;

        do {
            hde64_disasm((void*)(ptrCode + Index), &hs);
            if (hs.flags & F_ERROR)
                break;
            //
            // lea reg, PsAltSystemCallHandlers
            //
            if (hs.len == 7) {

                //
                // Match block found.
                //
                if (RtlCompareMemory((VOID*)&ptrCode[Index],
                    (VOID*)PsAltSystemCallHandlersPattern,
                    InstructionExactMatchLength) == InstructionExactMatchLength)
                {
                    Rel = *(PLONG)(ptrCode + Index + 3);
                    break;
                }
            }
            Index += hs.len;

        } while (Index < 128);

        kvarAddress = ComputeAddressInsideNtOs((ULONG_PTR)ptrCode, Index, hs.len, Rel);

    }

    return kvarAddress;
}

/*
* FindExHostListCallbacks
*
* Purpose:
*
* Return address of list head for callbacks registered with:
*
*   ExRegisterExtension
*
*/
OBEX_FINDCALLBACK_ROUTINE(FindExHostCallbacks)
{
    ULONG_PTR kvarAddress = 0;
    PBYTE   ptrCode;
    LONG    Rel = 0;
    ULONG   Index, c;
    hde64s  hs;

    UNREFERENCED_PARAMETER(QueryFlags);

    if (kdIsSymAvailable((PSYMCONTEXT)g_kdctx.NtOsSymContext)) {

        kdGetAddressFromSymbol(&g_kdctx,
            KVAR_ExpHostList,
            &kvarAddress);

    }

    if (kvarAddress == 0) {

        ptrCode = (PBYTE)GetProcAddress((HMODULE)g_kdctx.NtOsImageMap,
            "ExRegisterExtension");

        if (ptrCode == NULL)
            return 0;

        c = 0;
        Index = 0;

        //
        // Find ExpFindHost
        //

        do {

            hde64_disasm(ptrCode + Index, &hs);
            if (hs.flags & F_ERROR)
                break;

            //
            // Find call instruction.
            //
            if (hs.len != 5) {
                Index += hs.len;
                continue;
            }

            if (ptrCode[Index] == 0xE8)
                c++;

            if (c > 1) {
                Rel = *(PLONG)(ptrCode + Index + 1);
                break;
            }

            Index += hs.len;

        } while (Index < 256);

        if (Rel == 0)
            return 0;

        //
        // Examine ExpFindHost
        //
        ptrCode = ptrCode + Index + 5 + Rel;

        hde64_disasm(ptrCode, &hs);
        if (hs.flags & F_ERROR)
            return 0;

        if (hs.len == 7) {
            //
            // mov     rax, cs:ExpHostList
            //
            if (ptrCode[1] == 0x8B) {
                Rel = *(PLONG)(ptrCode + 3);
                kvarAddress = kdAdjustAddressToNtOsBase((ULONG_PTR)ptrCode,
                    0,
                    hs.len,
                    Rel);
            }

        }
    
    }

    return kvarAddress;
}

/*
* FindExpCallbackListHead
*
* Purpose:
*
* Returns the address of ExpCallbackListHead for callbacks registered with:
*
*   ExCreateCallback
*
*/
OBEX_FINDCALLBACK_ROUTINE(FindExpCallbackListHead)
{
    ULONG Index;
    LONG Rel;
    PBYTE ptrCode;
    hde64s hs;

    UNREFERENCED_PARAMETER(QueryFlags);

    if (g_NtBuildNumber < NT_WIN8_BLUE)
        return 0;

    ptrCode = (PBYTE)GetProcAddress((HMODULE)g_kdctx.NtOsImageMap,
        "ExCreateCallback");

    if (ptrCode == NULL)
        return 0;

    Index = 0;
    Rel = 0;

    do {
        hde64_disasm(ptrCode + Index, &hs);
        if (hs.flags & F_ERROR)
            break;

        if (hs.len == 7) { //check if lea

            if (((ptrCode[Index] == 0x48) || (ptrCode[Index] == 0x4C)) &&
                (ptrCode[Index + 1] == 0x8D) &&
                (ptrCode[Index + hs.len + 3] == 0x28)) // add/lea with +0x28 = offset of object's ExpCallbackList
            {
                Rel = *(PLONG)(ptrCode + Index + 3);
                break;
            }
        }

        Index += hs.len;

    } while (Index < 512);

    return ComputeAddressInsideNtOs((ULONG_PTR)ptrCode, Index, hs.len, Rel);
}

/*
* FindPoCoalescingCallbacks
*
* Purpose:
*
* Returns the address of PopCoalescingCallbackRoutine array or
* PopCoalRegistrationList list head for callbacks registered with:
*
*   PoRegisterCoalescingCallback
*
*/
OBEX_FINDCALLBACK_ROUTINE(FindPoCoalescingCallbacks)
{
    ULONG Index;
    LONG Rel;
    PBYTE ptrCode;
    hde64s hs;
    LPCWSTR lpSymbolName;
    BYTE checkByte;
    ULONG_PTR kvarAddress = 0;

    UNREFERENCED_PARAMETER(QueryFlags);

    //
    // Not available before Windows 8.
    //
    if (g_NtBuildNumber < NT_WIN8_BLUE)
        return 0;

    if (g_NtBuildNumber < NT_WIN10_REDSTONE4) {
        lpSymbolName = KVAR_PopCoalescingCallbackRoutine;
        checkByte = 0x0D;
    }
    else {
        lpSymbolName = KVAR_PopCoalRegistrationList;
        checkByte = 0x15;
    }


    if (kdIsSymAvailable((PSYMCONTEXT)g_kdctx.NtOsSymContext)) {

        kdGetAddressFromSymbol(&g_kdctx,
            lpSymbolName,
            &kvarAddress);

    }

    if (kvarAddress == 0) {

        ptrCode = (PBYTE)GetProcAddress((HMODULE)g_kdctx.NtOsImageMap,
            "PoRegisterCoalescingCallback");

        if (ptrCode == NULL)
            return 0;

        Index = 0;
        Rel = 0;

        do {

            hde64_disasm(ptrCode + Index, &hs);
            if (hs.flags & F_ERROR)
                break;

            if (hs.len == 7) { //check if lea

                if ((ptrCode[Index] == 0x48) &&
                    (ptrCode[Index + 1] == 0x8D) &&
                    (ptrCode[Index + 2] == checkByte)) //universal for both types of implementation
                {
                    Rel = *(PLONG)(ptrCode + Index + 3);
                    break;
                }
            }

            Index += hs.len;

        } while (Index < 256);

        kvarAddress = ComputeAddressInsideNtOs((ULONG_PTR)ptrCode, Index, hs.len, Rel);
    }

    return kvarAddress;
}

/*
* AddRootEntryToList
*
* Purpose:
*
* Adds callback root entry to the treelist.
*
*/
HTREEITEM AddRootEntryToList(
    _In_ HWND TreeList,
    _In_ LPWSTR lpCallbackType
)
{
    return supTreeListAddItem(
        TreeList,
        NULL,
        TVIF_TEXT | TVIF_STATE,
        (UINT)0,
        (UINT)0,
        lpCallbackType,
        NULL);
}

/*
* AddParentEntryToList
*
* Purpose:
*
* Adds a parent entry for callbacks to the treelist.
*
*/
HTREEITEM AddParentEntryToList(
    _In_ HWND TreeList,
    _In_ HTREEITEM RootItem,
    _In_ ULONG_PTR CallbackObjectAddress,
    _In_ LPWSTR lpCallbackObjectType
)
{
    TL_SUBITEMS_FIXED TreeListSubItems;
    WCHAR szAddress[32];

    RtlSecureZeroMemory(&TreeListSubItems, sizeof(TreeListSubItems));
    TreeListSubItems.Count = 2;

    szAddress[0] = L'0';
    szAddress[1] = L'x';
    szAddress[2] = 0;
    u64tohex(CallbackObjectAddress, &szAddress[2]);
    TreeListSubItems.Text[0] = T_EmptyString;
    TreeListSubItems.Text[1] = lpCallbackObjectType;

    return supTreeListAddItem(
        TreeList,
        RootItem,
        TVIF_TEXT | TVIF_STATE,
        (UINT)0,
        (UINT)0,
        szAddress,
        &TreeListSubItems);
}

/*
* AddEntryToList
*
* Purpose:
*
* Adds callback entry to the treelist.
*
*/
VOID AddEntryToList(
    _In_ HWND TreeList,
    _In_ HTREEITEM RootItem,
    _In_ ULONG_PTR Function,
    _In_opt_ LPWSTR lpAdditionalInfo,
    _In_ PRTL_PROCESS_MODULES Modules
)
{
    ULONG moduleIndex = 0;
    TL_SUBITEMS_FIXED TreeListSubItems;
    WCHAR szAddress[32];
    WCHAR szBuffer[MAX_PATH + 1];

    RtlSecureZeroMemory(&TreeListSubItems, sizeof(TreeListSubItems));
    TreeListSubItems.Count = 2;

    szAddress[0] = L'0';
    szAddress[1] = L'x';
    szAddress[2] = 0;
    u64tohex(Function, &szAddress[2]);
    TreeListSubItems.Text[0] = szAddress;

    RtlSecureZeroMemory(szBuffer, sizeof(szBuffer));

    if (ntsupFindModuleEntryByAddress(
        Modules, 
        (PVOID)Function, 
        &moduleIndex))
    {
        MultiByteToWideChar(
            CP_ACP,
            0,
            (LPCSTR)&Modules->Modules[moduleIndex].FullPathName,
            (INT)_strlen_a((char*)Modules->Modules[moduleIndex].FullPathName),
            szBuffer,
            MAX_PATH);
    }
    else {
        _strcpy(szBuffer, TEXT("Unknown Module"));
    }

    TreeListSubItems.Text[0] = szBuffer;
    if (lpAdditionalInfo) {
        TreeListSubItems.Text[1] = lpAdditionalInfo;
    }
    else {
        TreeListSubItems.Text[1] = T_EmptyString;
    }

    supTreeListAddItem(
        TreeList,
        RootItem,
        TVIF_TEXT | TVIF_STATE,
        (UINT)0,
        (UINT)0,
        szAddress,
        &TreeListSubItems);

    g_CallbacksCount += 1;
}

/*
* AddZeroEntryToList
*
* Purpose:
*
* Adds emptry callback entry to the treelist.
*
*/
VOID AddZeroEntryToList(
    _In_ HWND TreeList,
    _In_ HTREEITEM RootItem,
    _In_ ULONG_PTR Function,
    _In_opt_ LPWSTR lpAdditionalInfo
)
{
    TL_SUBITEMS_FIXED TreeListSubItems;
    WCHAR szAddress[32];
    WCHAR szBuffer[MAX_PATH + 1];

    RtlSecureZeroMemory(&TreeListSubItems, sizeof(TreeListSubItems));
    TreeListSubItems.Count = 2;

    szAddress[0] = TEXT('0');
    szAddress[1] = TEXT('x');
    szAddress[2] = 0;
    u64tohex(Function, &szAddress[2]);
    TreeListSubItems.Text[0] = szAddress;

    _strcpy(szBuffer, TEXT("Nothing"));

    TreeListSubItems.Text[0] = szBuffer;

    if (Function == 0) {
        TreeListSubItems.Text[1] = T_CannotQuery;
    }
    else {
        if (lpAdditionalInfo) {
            TreeListSubItems.Text[1] = lpAdditionalInfo;
        }
        else {
            TreeListSubItems.Text[1] = T_EmptyString;
        }
    }

    supTreeListAddItem(
        TreeList,
        RootItem,
        TVIF_TEXT | TVIF_STATE,
        (UINT)0,
        (UINT)0,
        szAddress,
        &TreeListSubItems);
}

/*
* DumpPsCallbacks
*
* Purpose:
*
* Read Psp* callback data from kernel and send it to output window.
*
*/
OBEX_DISPLAYCALLBACK_ROUTINE(DumpPsCallbacks)
{
    ULONG c;
    ULONG_PTR Address, Function;
    EX_FAST_REF Callbacks[PspNotifyRoutinesLimit];

    HTREEITEM RootItem;

    //
    // Add callback root entry to the treelist.
    //
    RootItem = AddRootEntryToList(TreeList, CallbackType);
    if (RootItem == 0)
        return;

    RtlSecureZeroMemory(Callbacks, sizeof(Callbacks));
    if (kdReadSystemMemory(KernelVariableAddress,
        &Callbacks, sizeof(Callbacks)))
    {

        for (c = 0; c < PspNotifyRoutinesLimit; c++) {

            if (Callbacks[c].Value) {

                Address = (ULONG_PTR)ObGetObjectFastReference(Callbacks[c]);
                Function = (ULONG_PTR)ObGetCallbackBlockRoutine((PVOID)Address);
                if (Function < g_kdctx.SystemRangeStart)
                    continue;

                AddEntryToList(TreeList,
                    RootItem,
                    Function,
                    NULL,
                    Modules);
            }
        }
    }

}

/*
* DumpDbgkLCallbacks
*
* Purpose:
*
* Read DbgkL* callback data from kernel and send it to output window.
*
*/
OBEX_DISPLAYCALLBACK_ROUTINE(DumpDbgkLCallbacks)
{
    ULONG c;
    ULONG_PTR Address, Function;
    EX_FAST_REF Callbacks[DbgkLmdCount];

    HTREEITEM RootItem;

    //
    // Add callback root entry to the treelist.
    //
    RootItem = AddRootEntryToList(TreeList, CallbackType);
    if (RootItem == 0)
        return;

    RtlSecureZeroMemory(Callbacks, sizeof(Callbacks));
    if (kdReadSystemMemory(KernelVariableAddress,
        &Callbacks, sizeof(Callbacks)))
    {

        for (c = 0; c < DbgkLmdCount; c++) {

            if (Callbacks[c].Value > g_kdctx.SystemRangeStart) {

                Address = (ULONG_PTR)ObGetObjectFastReference(Callbacks[c]);
                Function = (ULONG_PTR)ObGetCallbackBlockRoutine((PVOID)Address);
                if (Function < g_kdctx.SystemRangeStart)
                    continue;

                AddEntryToList(TreeList,
                    RootItem,
                    Function,
                    NULL,
                    Modules);
            }
        }
    }

}

/*
* DumpPsAltSystemCallHandlers
*
* Purpose:
*
* Read PsAltSystemCallHandlers data from kernel and send it to output window.
*
*/
OBEX_DISPLAYCALLBACK_ROUTINE(DumpPsAltSystemCallHandlers)
{
    ULONG i;
    ULONG_PTR AltSystemCallHandlers[MAX_ALT_SYSTEM_CALL_HANDLERS];

    HTREEITEM RootItem;

    //
    // Add callback root entry to the treelist.
    //
    RootItem = AddRootEntryToList(TreeList, CallbackType);
    if (RootItem == 0)
        return;

    RtlSecureZeroMemory(AltSystemCallHandlers, sizeof(AltSystemCallHandlers));
    if (kdReadSystemMemory(KernelVariableAddress,
        &AltSystemCallHandlers, sizeof(AltSystemCallHandlers)))
    {

        for (i = 0; i < MAX_ALT_SYSTEM_CALL_HANDLERS; i++) {

            if (AltSystemCallHandlers[i]) {

                if (AltSystemCallHandlers[i] < g_kdctx.SystemRangeStart)
                    continue;

                AddEntryToList(TreeList,
                    RootItem,
                    AltSystemCallHandlers[i],
                    NULL,
                    Modules);
            }
        }
    }

}

/*
* DumpKeBugCheckCallbacks
*
* Purpose:
*
* Read KeBugCheck callback data from kernel and send it to output window.
*
*/
OBEX_DISPLAYCALLBACK_ROUTINE(DumpKeBugCheckCallbacks)
{
    ULONG_PTR ListHead = KernelVariableAddress;
    LIST_ENTRY ListEntry;

    KBUGCHECK_CALLBACK_RECORD CallbackRecord;

    HTREEITEM RootItem;

    //
    // Add callback root entry to the treelist.
    //
    RootItem = AddRootEntryToList(TreeList, CallbackType);
    if (RootItem == 0)
        return;

    ListEntry.Flink = ListEntry.Blink = NULL;

    //
    // Read head.
    //
    if (!kdReadSystemMemory(
        ListHead,
        &ListEntry,
        sizeof(LIST_ENTRY)))
    {
        return;
    }

    //
    // Walk list entries.
    //
    while ((ULONG_PTR)ListEntry.Flink != ListHead) {

        RtlSecureZeroMemory(&CallbackRecord, sizeof(CallbackRecord));

        if (!kdReadSystemMemory((ULONG_PTR)ListEntry.Flink,
            &CallbackRecord,
            sizeof(CallbackRecord)))
        {
            break;
        }

        AddEntryToList(TreeList,
            RootItem,
            (ULONG_PTR)CallbackRecord.CallbackRoutine,
            NULL,
            Modules);

        ListEntry.Flink = CallbackRecord.Entry.Flink;
    }

}

/*
* KeBugCheckReasonToString
*
* Purpose:
*
* Return Reason as text constant.
*
*/
LPWSTR KeBugCheckReasonToString(
    _In_ KBUGCHECK_CALLBACK_REASON Reason)
{
    switch (Reason) {
    case KbCallbackInvalid:
        return TEXT("KbCallbackInvalid");

    case KbCallbackReserved1:
        return TEXT("KbCallbackReserved1");

    case KbCallbackSecondaryDumpData:
        return TEXT("KbCallbackSecondaryDumpData");

    case KbCallbackDumpIo:
        return TEXT("KbCallbackDumpIo");

    case KbCallbackAddPages:
        return TEXT("KbCallbackAddPages");

    case KbCallbackSecondaryMultiPartDumpData:
        return TEXT("KbCallbackSecondaryMultiPartDumpData");

    case KbCallbackRemovePages:
        return TEXT("KbCallbackRemovePages");
    case KbCallbackTriageDumpData:
        return TEXT("KbCallbackTriageDumpData");

    }
    return NULL;
}

/*
* DumpKeBugCheckReasonCallbacks
*
* Purpose:
*
* Read KeBugCheckReason callback data from kernel and send it to output window.
*
*/
OBEX_DISPLAYCALLBACK_ROUTINE(DumpKeBugCheckReasonCallbacks)
{
    ULONG_PTR ListHead = KernelVariableAddress;

    LIST_ENTRY ListEntry;

    KBUGCHECK_REASON_CALLBACK_RECORD CallbackRecord;

    HTREEITEM RootItem;

    //
    // Add callback root entry to the treelist.
    //
    RootItem = AddRootEntryToList(TreeList, CallbackType);
    if (RootItem == 0)
        return;

    ListEntry.Flink = ListEntry.Blink = NULL;

    //
    // Read head.
    //
    if (!kdReadSystemMemory(
        ListHead,
        &ListEntry,
        sizeof(LIST_ENTRY)))
    {
        return;
    }

    //
    // Walk list entries.
    //
    while ((ULONG_PTR)ListEntry.Flink != ListHead) {

        RtlSecureZeroMemory(&CallbackRecord, sizeof(CallbackRecord));

        if (!kdReadSystemMemory((ULONG_PTR)ListEntry.Flink,
            &CallbackRecord,
            sizeof(CallbackRecord)))
        {
            break;
        }

        AddEntryToList(TreeList,
            RootItem,
            (ULONG_PTR)CallbackRecord.CallbackRoutine,
            KeBugCheckReasonToString(CallbackRecord.Reason),
            Modules);

        ListEntry.Flink = CallbackRecord.Entry.Flink;
    }

}

/*
* DumpCmCallbacks
*
* Purpose:
*
* Read Cm Registry callback data from kernel and send it to output window.
*
*/
OBEX_DISPLAYCALLBACK_ROUTINE(DumpCmCallbacks)
{
    ULONG_PTR ListHead = KernelVariableAddress;

    LIST_ENTRY ListEntry;

    CM_CALLBACK_CONTEXT_BLOCK CallbackRecord;

    HTREEITEM RootItem;

    //
    // Add callback root entry to the treelist.
    //
    RootItem = AddRootEntryToList(TreeList, CallbackType);
    if (RootItem == 0)
        return;

    ListEntry.Flink = ListEntry.Blink = NULL;

    //
    // Read head.
    //
    if (!kdReadSystemMemory(
        ListHead,
        &ListEntry,
        sizeof(LIST_ENTRY)))
    {
        return;
    }

    //
    // Walk list entries.
    //
    while ((ULONG_PTR)ListEntry.Flink != ListHead) {

        RtlSecureZeroMemory(&CallbackRecord, sizeof(CallbackRecord));

        if (!kdReadSystemMemory((ULONG_PTR)ListEntry.Flink,
            &CallbackRecord,
            sizeof(CallbackRecord)))
        {
            break;
        }

        AddEntryToList(TreeList,
            RootItem,
            (ULONG_PTR)CallbackRecord.Function,
            NULL,
            Modules);

        ListEntry.Flink = CallbackRecord.CallbackListEntry.Flink;
    }

}

/*
* DumpIoCallbacks
*
* Purpose:
*
* Read Io related callback data from kernel and send it to output window.
*
*/
OBEX_DISPLAYCALLBACK_ROUTINE(DumpIoCallbacks)
{
    ULONG_PTR ListHead = KernelVariableAddress;

    LIST_ENTRY ListEntry;

    SHUTDOWN_PACKET EntryPacket;

    DEVICE_OBJECT DeviceObject;

    DRIVER_OBJECT DriverObject;

    PVOID Routine;
    LPWSTR lpDescription;

    HTREEITEM RootItem;

    //
    // Add callback root entry to the treelist.
    //
    RootItem = AddRootEntryToList(TreeList, CallbackType);
    if (RootItem == 0)
        return;

    ListEntry.Flink = ListEntry.Blink = NULL;

    //
    // Read head.
    //
    if (!kdReadSystemMemory(
        ListHead,
        &ListEntry,
        sizeof(LIST_ENTRY)))
    {
        return;
    }

    //
    // Walk list entries.
    //
    while ((ULONG_PTR)ListEntry.Flink != ListHead) {

        RtlSecureZeroMemory(&EntryPacket, sizeof(EntryPacket));

        if (!kdReadSystemMemory(
            (ULONG_PTR)ListEntry.Flink,
            &EntryPacket,
            sizeof(EntryPacket)))
        {
            break;
        }

        Routine = EntryPacket.DeviceObject;
        lpDescription = TEXT("PDEVICE_OBJECT");

        //
        // Attempt to query owner of the device object.
        //
        if ((ULONG_PTR)EntryPacket.DeviceObject > g_kdctx.SystemRangeStart) {

            //
            // Read DEVICE_OBJECT.
            //
            RtlSecureZeroMemory(&DeviceObject, sizeof(DeviceObject));

            if (kdReadSystemMemory((ULONG_PTR)EntryPacket.DeviceObject,
                (PVOID)&DeviceObject,
                sizeof(DeviceObject)))
            {
                //
                // Read DRIVER_OBJECT.
                //
                RtlSecureZeroMemory(&DriverObject, sizeof(DriverObject));
                if (kdReadSystemMemory((ULONG_PTR)DeviceObject.DriverObject,
                    (PVOID)&DriverObject,
                    sizeof(DriverObject)))
                {
                    Routine = DriverObject.MajorFunction[IRP_MJ_SHUTDOWN];
                    lpDescription = TEXT("IRP_MJ_SHUTDOWN");
                }
            }

        }

        AddEntryToList(TreeList,
            RootItem,
            (ULONG_PTR)Routine,
            lpDescription,
            Modules);

        ListEntry.Flink = EntryPacket.ListEntry.Flink;
    }

}

/*
* DumpObCallbacks
*
* Purpose:
*
* Read Ob callback data from kernel and send it to output window.
*
*/
OBEX_DISPLAYCALLBACK_ROUTINE(DumpObCallbacks)
{
    BOOL bAltitudeRead, bNeedFree;

    ULONG_PTR ListHead = KernelVariableAddress;

    LPWSTR lpInfoBuffer = NULL, lpType;

    SIZE_T Size, AltitudeSize = 0;

    LIST_ENTRY ListEntry;

    OB_CALLBACK_CONTEXT_BLOCK CallbackRecord;

    OB_CALLBACK_REGISTRATION Registration;

    HTREEITEM RootItem;

    //
    // Add callback root entry to the treelist.
    //
    RootItem = AddRootEntryToList(TreeList, CallbackType);
    if (RootItem == 0)
        return;

    ListEntry.Flink = ListEntry.Blink = NULL;

    //
    // Read head.
    //
    if (!kdReadSystemMemory(
        ListHead,
        &ListEntry,
        sizeof(LIST_ENTRY)))
    {
        return;
    }

    //
    // Walk list entries.
    //
    while ((ULONG_PTR)ListEntry.Flink != ListHead) {

        RtlSecureZeroMemory(&CallbackRecord, sizeof(CallbackRecord));

        if (!kdReadSystemMemory((ULONG_PTR)ListEntry.Flink,
            &CallbackRecord,
            sizeof(CallbackRecord)))
        {
            break;
        }

        //
        // Read Altitude.
        //
        bAltitudeRead = FALSE;

        RtlSecureZeroMemory(&Registration, sizeof(Registration));
        if (kdReadSystemMemory((ULONG_PTR)CallbackRecord.Registration,
            (PVOID)&Registration,
            sizeof(Registration)))
        {
            AltitudeSize = 8 + (SIZE_T)Registration.Altitude.Length;
            lpInfoBuffer = (LPWSTR)supHeapAlloc(AltitudeSize);
            if (lpInfoBuffer) {

                bAltitudeRead = kdReadSystemMemory((ULONG_PTR)Registration.Altitude.Buffer,
                    (PVOID)lpInfoBuffer,
                    Registration.Altitude.Length);
            }
        }

        //
        // Output PreCallback.
        //
        if ((ULONG_PTR)CallbackRecord.PreCallback > g_kdctx.SystemRangeStart) {

            bNeedFree = FALSE;

            if (bAltitudeRead) {
                Size = AltitudeSize + MAX_PATH;
                lpType = (LPWSTR)supHeapAlloc(Size);
                if (lpType) {
                    _strcpy(lpType, TEXT("PreCallback, Altitude: "));
                    _strcat(lpType, lpInfoBuffer);
                    bNeedFree = TRUE;
                }
            }
            else
                lpType = TEXT("PreCallback");

            AddEntryToList(TreeList,
                RootItem,
                (ULONG_PTR)CallbackRecord.PreCallback,
                lpType,
                Modules);

            if (bNeedFree) supHeapFree(lpType);
        }

        //
        // Output PostCallback.
        //
        if ((ULONG_PTR)CallbackRecord.PostCallback > g_kdctx.SystemRangeStart) {

            bNeedFree = FALSE;

            if (bAltitudeRead) {
                Size = AltitudeSize + MAX_PATH;
                lpType = (LPWSTR)supHeapAlloc(Size);
                if (lpType) {
                    _strcpy(lpType, TEXT("PostCallback, Altitude: "));
                    _strcat(lpType, lpInfoBuffer);
                    bNeedFree = TRUE;
                }
            }
            else
                lpType = TEXT("PostCallback");

            AddEntryToList(TreeList,
                RootItem,
                (ULONG_PTR)CallbackRecord.PostCallback,
                lpType,
                Modules);

            if (bNeedFree) supHeapFree(lpType);
        }
        ListEntry.Flink = CallbackRecord.CallbackListEntry.Flink;

        if (lpInfoBuffer) supHeapFree(lpInfoBuffer);
    }

}

/*
* DumpSeFileSystemCallbacks
*
* Purpose:
*
* Read Se related callback data from kernel and send it to output window.
*
*/
OBEX_DISPLAYCALLBACK_ROUTINE(DumpSeFileSystemCallbacks)
{
    ULONG_PTR Next;

    SEP_LOGON_SESSION_TERMINATED_NOTIFICATION SeEntry; // This structure is different for Ex variant but 
                                                       // key callback function field is on the same offset.

    HTREEITEM RootItem;

    //
    // Add callback root entry to the treelist.
    //
    RootItem = AddRootEntryToList(TreeList, CallbackType);
    if (RootItem == 0)
        return;

    //
    // Read head.
    //
    RtlSecureZeroMemory(&SeEntry, sizeof(SeEntry));

    if (!kdReadSystemMemory(KernelVariableAddress,
        (PVOID)&SeEntry,
        sizeof(SeEntry)))
    {
        return;
    }

    //
    // Walk each entry in single linked list.
    //
    Next = (ULONG_PTR)SeEntry.Next;
    while (Next) {

        RtlSecureZeroMemory(&SeEntry, sizeof(SeEntry));

        if (!kdReadSystemMemory(Next,
            (PVOID)&SeEntry,
            sizeof(SeEntry)))
        {
            break;
        }

        AddEntryToList(TreeList,
            RootItem,
            (ULONG_PTR)SeEntry.CallbackRoutine,
            NULL,
            Modules);

        Next = (ULONG_PTR)SeEntry.Next;

    }

}

/*
* DumpPoCallbacks
*
* Purpose:
*
* Read Po callback data from kernel and send it to output window.
*
*/
OBEX_DISPLAYCALLBACK_ROUTINE(DumpPoCallbacks)
{
    LIST_ENTRY ListEntry;

    union {
        union {
            POP_POWER_SETTING_REGISTRATION_V1 *v1;
            POP_POWER_SETTING_REGISTRATION_V2 *v2;
        } Versions;
        PBYTE Ref;
    } CallbackData;

    ULONG ReadSize;
    ULONG_PTR ListHead = KernelVariableAddress;
    SIZE_T BufferSize;
    LPWSTR GuidString;
    PVOID Buffer = NULL;
    PVOID CallbackRoutine = NULL;

    GUID EntryGuid;
    UNICODE_STRING ConvertedGuid;

    HTREEITEM RootItem;

    //
    // Add callback root entry to the treelist.
    //
    RootItem = AddRootEntryToList(TreeList, CallbackType);
    if (RootItem == 0)
        return;

    ListEntry.Flink = ListEntry.Blink = NULL;

    //
    // Determinate size of structure to read.
    //
    ReadSize = sizeof(POP_POWER_SETTING_REGISTRATION_V1);
    if (g_NtBuildNumber >= NT_WIN10_REDSTONE1)
        ReadSize = sizeof(POP_POWER_SETTING_REGISTRATION_V2);

    __try {

        //
        // Allocate read buffer with enough size.
        // 

        BufferSize = sizeof(POP_POWER_SETTING_REGISTRATION_V1) + sizeof(POP_POWER_SETTING_REGISTRATION_V2);
        Buffer = supHeapAlloc(BufferSize);
        if (Buffer == NULL)
            __leave;

        CallbackData.Ref = (PBYTE)Buffer;

        //
        // Read head.
        //
        if (!kdReadSystemMemory(
            ListHead,
            &ListEntry,
            sizeof(LIST_ENTRY)))
        {
            __leave;
        }

        //
        // Walk list entries.
        //
        while ((ULONG_PTR)ListEntry.Flink != ListHead) {

            RtlSecureZeroMemory(Buffer, BufferSize);

            if (!kdReadSystemMemory((ULONG_PTR)ListEntry.Flink,
                Buffer,
                ReadSize))
            {
                break;
            }

            //
            // Is valid registration entry?
            //
            if (CallbackData.Versions.v1->Tag != PO_POWER_SETTINGS_REGISTRATION_TAG)
                break;

            if (ReadSize == sizeof(POP_POWER_SETTING_REGISTRATION_V2)) {
                CallbackRoutine = CallbackData.Versions.v2->Callback;
                EntryGuid = CallbackData.Versions.v2->Guid;
            }
            else {
                CallbackRoutine = CallbackData.Versions.v1->Callback;
                EntryGuid = CallbackData.Versions.v1->Guid;
            }

            if (CallbackRoutine) {

                if (NT_SUCCESS(RtlStringFromGUID(&EntryGuid, &ConvertedGuid)))
                    GuidString = ConvertedGuid.Buffer;
                else
                    GuidString = NULL;

                AddEntryToList(TreeList,
                    RootItem,
                    (ULONG_PTR)CallbackRoutine,
                    GuidString,
                    Modules);

                if (GuidString)
                    RtlFreeUnicodeString(&ConvertedGuid);

            }

            //
            // Next item address, ListEntry offset version independent.
            //
            ListEntry.Flink = CallbackData.Versions.v1->Link.Flink;
        }

    }
    __finally {

        if (AbnormalTermination())
            supReportAbnormalTermination(__FUNCTIONW__);

        if (Buffer) supHeapFree(Buffer);
    }
}

/*
* DumpDbgPrintCallbacks
*
* Purpose:
*
* Read Dbg callback data from kernel and send it to output window.
*
*/
OBEX_DISPLAYCALLBACK_ROUTINE(DumpDbgPrintCallbacks)
{
    ULONG_PTR ListHead = KernelVariableAddress;
    ULONG_PTR RecordAddress;

    LIST_ENTRY ListEntry;

    RTL_CALLBACK_REGISTER CallbackRecord;

    HTREEITEM RootItem;

    //
    // Add callback root entry to the treelist.
    //
    RootItem = AddRootEntryToList(TreeList, CallbackType);
    if (RootItem == 0)
        return;

    ListEntry.Flink = ListEntry.Blink = NULL;

    //
    // Read head.
    //
    if (!kdReadSystemMemory(
        ListHead,
        &ListEntry,
        sizeof(ListEntry)))
    {
        return;
    }

    //
    // Walk list entries.
    //
    while ((ULONG_PTR)ListEntry.Flink != ListHead) {

        RtlSecureZeroMemory(&CallbackRecord, sizeof(CallbackRecord));

        RecordAddress = (ULONG_PTR)ListEntry.Flink - FIELD_OFFSET(RTL_CALLBACK_REGISTER, ListEntry);

        if (!kdReadSystemMemory((ULONG_PTR)RecordAddress,
            &CallbackRecord,
            sizeof(CallbackRecord)))
        {
            break;
        }

        if (CallbackRecord.DebugPrintCallback) {

            AddEntryToList(TreeList,
                RootItem,
                (ULONG_PTR)CallbackRecord.DebugPrintCallback,
                NULL,
                Modules);

        }
        ListEntry.Flink = CallbackRecord.ListEntry.Flink;
    }

}

/*
* DumpIoFsRegistrationCallbacks
*
* Purpose:
*
* Read Io File System registration related callback data from kernel and send it to output window.
*
*/
OBEX_DISPLAYCALLBACK_ROUTINE(DumpIoFsRegistrationCallbacks)
{
    LIST_ENTRY ListEntry;

    NOTIFICATION_PACKET CallbackRecord;

    ULONG_PTR ListHead = KernelVariableAddress;

    HTREEITEM RootItem;

    //
    // Add callback root entry to the treelist.
    //
    RootItem = AddRootEntryToList(TreeList, CallbackType);
    if (RootItem == 0)
        return;

    ListEntry.Flink = ListEntry.Blink = NULL;

    //
    // Read head.
    //
    if (!kdReadSystemMemory(
        ListHead,
        &ListEntry,
        sizeof(LIST_ENTRY)))
    {
        return;
    }

    //
    // Walk list entries.
    //
    while ((ULONG_PTR)ListEntry.Flink != ListHead) {

        RtlSecureZeroMemory(&CallbackRecord, sizeof(CallbackRecord));

        if (!kdReadSystemMemory((ULONG_PTR)ListEntry.Flink,
            &CallbackRecord,
            sizeof(CallbackRecord)))
        {
            break;
        }

        if (CallbackRecord.NotificationRoutine) {

            AddEntryToList(TreeList,
                RootItem,
                (ULONG_PTR)CallbackRecord.NotificationRoutine,
                NULL,
                Modules);

        }

        ListEntry.Flink = CallbackRecord.ListEntry.Flink;
    }

}

/*
* DumpIoFileSystemCallbacks
*
* Purpose:
*
* Read Io File System related callback data from kernel and send it to output window.
*
*/
OBEX_DISPLAYCALLBACK_ROUTINE(DumpIoFileSystemCallbacks)
{
    BOOL bNeedFree;

    LIST_ENTRY ListEntry, NextEntry;

    ULONG_PTR ListHead = KernelVariableAddress;

    ULONG_PTR DeviceObjectAddress = 0, BaseAddress = 0;

    DEVICE_OBJECT DeviceObject;

    DRIVER_OBJECT DriverObject;

    LPWSTR lpType;

    HTREEITEM RootItem;

    //
    // Add callback root entry to the treelist.
    //
    RootItem = AddRootEntryToList(TreeList, CallbackType);
    if (RootItem == 0)
        return;

    ListEntry.Flink = ListEntry.Blink = NULL;

    //
    // Read head.
    //
    if (!kdReadSystemMemory(
        ListHead,
        &ListEntry,
        sizeof(LIST_ENTRY)))
    {
        return;
    }

    //
    // Walk list entries.
    //
    while ((ULONG_PTR)ListEntry.Flink != ListHead) {

        RtlSecureZeroMemory(&DeviceObject, sizeof(DeviceObject));

        DeviceObjectAddress = (ULONG_PTR)ListEntry.Flink - FIELD_OFFSET(DEVICE_OBJECT, Queue);

        //
        // Read DEVICE_OBJECT.
        //
        if (!kdReadSystemMemory(DeviceObjectAddress,
            &DeviceObject,
            sizeof(DeviceObject)))
        {
            break;
        }

        //
        // Additional info column default text.
        //
        lpType = TEXT("PDEVICE_OBJECT");
        BaseAddress = DeviceObjectAddress;
        bNeedFree = FALSE;

        //
        // Read DRIVER_OBJECT.
        //
        RtlSecureZeroMemory(&DriverObject, sizeof(DriverObject));
        if (kdReadSystemMemory((ULONG_PTR)DeviceObject.DriverObject,
            &DriverObject,
            sizeof(DriverObject)))
        {
            //
            // Determinate address to display.
            //
            BaseAddress = (ULONG_PTR)DriverObject.DriverInit;
            if (BaseAddress == 0) {
                BaseAddress = (ULONG_PTR)DriverObject.DriverStart;
            }

            lpType = NULL;

            //
            // Read DRIVER_OBJECT name.
            //
            if (DriverObject.DriverName.Length &&
                DriverObject.DriverName.MaximumLength &&
                DriverObject.DriverName.Buffer)
            {
                lpType = (LPWSTR)supHeapAlloc((SIZE_T)DriverObject.DriverName.Length + sizeof(UNICODE_NULL));
                if (lpType) {
                    bNeedFree = TRUE;
                    if (!kdReadSystemMemory((ULONG_PTR)DriverObject.DriverName.Buffer,
                        lpType,
                        (ULONG)DriverObject.DriverName.Length))
                    {
                        supHeapFree(lpType);
                        lpType = NULL;
                        bNeedFree = FALSE;
                    }
                }
            }
        }

        AddEntryToList(TreeList,
            RootItem,
            BaseAddress,
            lpType, //PDEVICE_OBJECT or DRIVER_OBJECT.DriverName
            Modules);

        if (bNeedFree)
            supHeapFree(lpType);

        //
        // Next ListEntry.
        //
        NextEntry.Blink = NextEntry.Flink = NULL;

        if (!kdReadSystemMemory(
            (ULONG_PTR)ListEntry.Flink,
            &NextEntry,
            sizeof(LIST_ENTRY)))
        {
            break;
        }

        if (NextEntry.Flink == NULL)
            break;

        ListEntry.Flink = NextEntry.Flink;
    }

}

/*
* DumpCiCallbacks
*
* Purpose:
*
* Read SeCiCallbacks/g_CiCallbacks related callback data from kernel and send it to output window.
*
*/
OBEX_DISPLAYCALLBACK_ROUTINE(DumpCiCallbacks)
{
    HTREEITEM RootItem;

    ULONG_PTR *CallbacksData;

    LPWSTR CallbackName;

    ULONG_PTR SizeOfCiCallbacks = 0;

    ULONG i, c;

    BOOL bRevisionMarker;

    //
    // Add callback root entry to the treelist.
    //
    RootItem = AddRootEntryToList(TreeList, CallbackType);
    if (RootItem == 0)
        return;

    if (g_NtBuildNumber <= NT_WIN7_SP1) {
        SizeOfCiCallbacks = 3 * sizeof(ULONG_PTR);

        CallbacksData = (PULONG_PTR)supVirtualAlloc((SIZE_T)SizeOfCiCallbacks);
        if (CallbacksData) {

            if (kdReadSystemMemory(KernelVariableAddress,
                CallbacksData,
                (ULONG)SizeOfCiCallbacks))
            {
                c = (ULONG)(SizeOfCiCallbacks / sizeof(ULONG_PTR));
                for (i = 0; i < c; i++) {

                    CallbackName = GetCiRoutineNameFromIndex(i);

                    if (CallbacksData[i]) {

                        AddEntryToList(TreeList,
                            RootItem,
                            CallbacksData[i],
                            CallbackName,
                            Modules);

                    }
                    else {

                        AddZeroEntryToList(TreeList,
                            RootItem,
                            CallbacksData[i],
                            CallbackName);

                    }
                }
            }
            supVirtualFree(CallbacksData);
        }
    }
    else {

        //
        // Probe size element.
        //
        if (!kdReadSystemMemory(KernelVariableAddress,
            &SizeOfCiCallbacks,
            sizeof(ULONG_PTR)))
        {
            return;
        }

        //
        // Check size.
        //
        if ((SizeOfCiCallbacks == 0) || (SizeOfCiCallbacks > PAGE_SIZE))
            return;

        CallbacksData = (PULONG_PTR)supVirtualAlloc((SIZE_T)SizeOfCiCallbacks);
        if (CallbacksData) {

            if (kdReadSystemMemory(KernelVariableAddress,
                CallbacksData,
                (ULONG)SizeOfCiCallbacks))
            {
                SizeOfCiCallbacks -= sizeof(ULONG_PTR); //exclude structure sizeof
                bRevisionMarker = (g_NtBuildNumber >= NT_WIN10_REDSTONE1); //there is a revision marker at the end of this structure.
                if (bRevisionMarker) SizeOfCiCallbacks -= sizeof(ULONG_PTR); //exclude marker (windows 10 + revision)

                c = (ULONG)(SizeOfCiCallbacks / sizeof(ULONG_PTR));

                for (i = 1; i <= c; i++) {

                    CallbackName = GetCiRoutineNameFromIndex(i - 1);

                    if (CallbacksData[i]) {

                        AddEntryToList(TreeList,
                            RootItem,
                            CallbacksData[i],
                            CallbackName,
                            Modules);

                    }
                    else {

                        AddZeroEntryToList(TreeList,
                            RootItem,
                            CallbacksData[i],
                            CallbackName);

                    }

                }
            }

            supVirtualFree(CallbacksData);
        }
    }
}

/*
* DumpExHostCallbacks
*
* Purpose:
*
* Read ExHostList related callback data from kernel and send it to output window.
*
*/
OBEX_DISPLAYCALLBACK_ROUTINE(DumpExHostCallbacks)
{
    LIST_ENTRY ListEntry;

    EX_HOST_ENTRY HostEntry;

    ULONG_PTR ListHead = KernelVariableAddress;
    ULONG_PTR* HostTableDump;
    ULONG NumberOfCallbacks, i;

    HTREEITEM RootItem;

    //
    // Add callback root entry to the treelist.
    //
    RootItem = AddRootEntryToList(TreeList, CallbackType);
    if (RootItem == 0)
        return;

    ListEntry.Flink = ListEntry.Blink = NULL;

    //
    // Read head.
    //
    if (!kdReadSystemMemory(
        ListHead,
        &ListEntry,
        sizeof(LIST_ENTRY)))
    {
        return;
    }

    //
    // Walk list entries.
    //
    while ((ULONG_PTR)ListEntry.Flink != ListHead) {

        RtlSecureZeroMemory(&HostEntry, sizeof(HostEntry));

        if (!kdReadSystemMemory((ULONG_PTR)ListEntry.Flink,
            &HostEntry,
            sizeof(HostEntry)))
        {
            break;
        }

        //
        // Find not an empty host table.
        //
        NumberOfCallbacks = HostEntry.HostParameters.HostInformation.FunctionCount;

        if (NumberOfCallbacks) {

            if (HostEntry.HostParameters.NotificationRoutine) {
                AddEntryToList(TreeList,
                    RootItem,
                    (ULONG_PTR)HostEntry.HostParameters.NotificationRoutine,
                    L"NotificationRoutine",
                    Modules);

            }

            //
            // Read function table.
            //
            if (HostEntry.FunctionTable) {
                HostTableDump = (ULONG_PTR*)supHeapAlloc(NumberOfCallbacks * sizeof(PVOID));
                if (HostTableDump) {

                    if (kdReadSystemMemory(
                        (ULONG_PTR)HostEntry.FunctionTable,
                        HostTableDump,
                        NumberOfCallbacks * sizeof(PVOID)))
                    {

                        for (i = 0; i < NumberOfCallbacks; i++) {
                            if (HostTableDump[i]) {
                                AddEntryToList(TreeList,
                                    RootItem,
                                    (ULONG_PTR)HostTableDump[i],
                                    L"Callback",
                                    Modules);
                            }
                        }

                    }

                    supHeapFree(HostTableDump);
                }
            }
        }

        ListEntry.Flink = HostEntry.ListEntry.Flink;
    }
}

/*
* DumpExpCallbackListCallbacks
*
* Purpose:
*
* Read ExCreateCallback created objects from kernel and send them to output window.
*
*/
OBEX_DISPLAYCALLBACK_ROUTINE(DumpExpCallbackListCallbacks)
{
    LIST_ENTRY ListEntry, NextEntry, RegistrationsListEntry;

    CALLBACK_OBJECT_V2 CallbackObject;
    CALLBACK_REGISTRATION CallbackRegistration;

    ULONG_PTR ListHead = KernelVariableAddress, RegistrationsListHead;

    ULONG_PTR CallbackObjectAddress;

    HTREEITEM RootItem, SubItem;

    //
    // Add callback root entry to the treelist.
    //
    RootItem = AddRootEntryToList(TreeList, CallbackType);
    if (RootItem == 0)
        return;

    ListEntry.Flink = ListEntry.Blink = NULL;

    //
    // Read head.
    //
    if (!kdReadSystemMemory(
        ListHead,
        &ListEntry,
        sizeof(LIST_ENTRY)))
    {
        return;
    }

    //
    // Walk list entries.
    //
    while ((ULONG_PTR)ListEntry.Flink != ListHead) {

        RtlSecureZeroMemory(&CallbackObject, sizeof(CallbackObject));

        CallbackObjectAddress = (ULONG_PTR)ListEntry.Flink - FIELD_OFFSET(CALLBACK_OBJECT_V2, ExpCallbackList);

        if (!kdReadSystemMemory(CallbackObjectAddress,
            &CallbackObject,
            sizeof(CallbackObject))
            ||
            CallbackObject.Signature != EX_CALLBACK_SIGNATURE)
        {
            break;
        }

        SubItem = AddParentEntryToList(
            TreeList,
            RootItem,
            CallbackObjectAddress,
            TEXT("Callback object"));
        if (SubItem == 0)
            break;

        //
        // Walk RegisteredCallbacks list entry.
        //
        RegistrationsListHead = CallbackObjectAddress + FIELD_OFFSET(CALLBACK_OBJECT_V2, RegisteredCallbacks);
        RegistrationsListEntry.Flink = CallbackObject.RegisteredCallbacks.Flink;
        while ((ULONG_PTR)RegistrationsListEntry.Flink != RegistrationsListHead) {

            //
            // Read callback registration data.
            //
            RtlSecureZeroMemory(&CallbackRegistration, sizeof(CallbackRegistration));
            if (!kdReadSystemMemory((ULONG_PTR)RegistrationsListEntry.Flink,
                (PVOID)&CallbackRegistration,
                sizeof(CallbackRegistration)))
            {
                break;
            }

            RegistrationsListEntry.Flink = CallbackRegistration.Link.Flink;

            AddEntryToList(TreeList,
                SubItem,
                (ULONG_PTR)CallbackRegistration.CallbackFunction,
                TEXT("Callback registration"),
                Modules);
        }

        //
        // Next ListEntry.
        //
        NextEntry.Blink = NextEntry.Flink = NULL;

        if (!kdReadSystemMemory(
            (ULONG_PTR)ListEntry.Flink,
            &NextEntry,
            sizeof(LIST_ENTRY)))
        {
            break;
        }

        if (NextEntry.Flink == NULL)
            break;

        ListEntry.Flink = NextEntry.Flink;
    }
}

/*
* DumpPoCoalescingCallbacks
*
* Purpose:
*
* Read PoRegisterCoalescingCallback created objects from kernel and send them to output window.
*
*/
OBEX_DISPLAYCALLBACK_ROUTINE(DumpPoCoalescingCallbacks)
{
    ULONG CallbacksCount, i;

    LIST_ENTRY ListEntry;

    ULONG_PTR ListHead = KernelVariableAddress;
    ULONG_PTR objectFastRef, callbackAddress;

    union {
        PO_COALESCING_CALLBACK_V1 v1;
        PO_COALESCING_CALLBACK_V2 v2;
    } callbackObject;

    EX_FAST_REF Callbacks[PopCoalescingCallbackRoutineCount_V2];

    HTREEITEM RootItem;

    //
    // Add callback root entry to the treelist.
    //
    RootItem = AddRootEntryToList(TreeList, CallbackType);
    if (RootItem == 0)
        return;

    //
    // Before Win10 RS4 this list implemented as the array of the fixed size.
    //
    if (g_NtBuildNumber < NT_WIN10_REDSTONE4) {

        RtlSecureZeroMemory(Callbacks, sizeof(Callbacks));

        //
        // Before Win10 RS3 this list is limited to 8 callbacks.
        // In Win10 RS3 this list increased up to 32 callbacks.
        //
        if (g_NtBuildNumber < NT_WIN10_REDSTONE3)
            CallbacksCount = PopCoalescingCallbackRoutineCount_V1;
        else
            CallbacksCount = PopCoalescingCallbackRoutineCount_V2;

        if (kdReadSystemMemory(KernelVariableAddress,
            &Callbacks, 
            CallbacksCount * sizeof(EX_FAST_REF)))
        {

            for (i = 0; i < CallbacksCount; i++) {

                if (Callbacks[i].Value) {

                    objectFastRef = (ULONG_PTR)ObGetObjectFastReference(Callbacks[i]);
                    RtlSecureZeroMemory(&callbackObject, sizeof(callbackObject));

                    if (kdReadSystemMemory(objectFastRef,
                        &callbackObject.v1,
                        sizeof(callbackObject.v1)))
                    {
                        AddEntryToList(TreeList,
                            RootItem,
                            (ULONG_PTR)callbackObject.v1.Callback,
                            L"CoalescingCallback",
                            Modules);
                    }


                }
            }
        }
    }
    else
    {
        ListEntry.Flink = ListEntry.Blink = NULL;

        //
        // Read head.
        //
        if (!kdReadSystemMemory(
            ListHead,
            &ListEntry,
            sizeof(LIST_ENTRY)))
        {
            return;
        }

        //
        // Walk list entries.
        //
        while ((ULONG_PTR)ListEntry.Flink != ListHead) {

            RtlSecureZeroMemory(&callbackObject, sizeof(callbackObject));

            callbackAddress = (ULONG_PTR)ListEntry.Flink - FIELD_OFFSET(PO_COALESCING_CALLBACK_V2, Link);

            if (!kdReadSystemMemory(callbackAddress,
                &callbackObject.v2,
                sizeof(callbackObject.v2)))
            {
                break;
            }

            AddEntryToList(TreeList,
                RootItem,
                (ULONG_PTR)callbackObject.v2.Callback,
                L"CoalescingCallback",
                Modules);

            ListEntry.Flink = callbackObject.v2.Link.Flink;
        }

    }
}

/*
* QueryIopFsListsCallbacks
*
* Purpose:
*
* Query and list Io Fs lists callbacks.
*
*/
OBEX_QUERYCALLBACK_ROUTINE(QueryIopFsListsCallbacks)
{
    UNREFERENCED_PARAMETER(QueryFlags);
    UNREFERENCED_PARAMETER(CallbackType);
    UNREFERENCED_PARAMETER(FindRoutine);
    UNREFERENCED_PARAMETER(SystemCallbacksRef);

    __try {

        if ((g_SystemCallbacks.IopCdRomFileSystemQueueHead == 0) ||
            (g_SystemCallbacks.IopDiskFileSystemQueueHead == 0) ||
            (g_SystemCallbacks.IopTapeFileSystemQueueHead == 0) ||
            (g_SystemCallbacks.IopNetworkFileSystemQueueHead == 0))
        {
            if (!FindIopFileSystemQueueHeads(&g_SystemCallbacks.IopCdRomFileSystemQueueHead,
                &g_SystemCallbacks.IopDiskFileSystemQueueHead,
                &g_SystemCallbacks.IopTapeFileSystemQueueHead,
                &g_SystemCallbacks.IopNetworkFileSystemQueueHead))
            {
                kdReportErrorByFunction(__FUNCTIONW__, TEXT("Could not locate all Iop ListHeads"));
                return STATUS_NOT_FOUND;
            }
        }

    }
    __except (WOBJ_EXCEPTION_FILTER_LOG) {
        return GetExceptionCode();
    }

    __try {

        if (g_SystemCallbacks.IopDiskFileSystemQueueHead) {

            DisplayRoutine(TreeList,
                TEXT("IoDiskFs"),
                g_SystemCallbacks.IopDiskFileSystemQueueHead,
                Modules);
        }
        if (g_SystemCallbacks.IopCdRomFileSystemQueueHead) {

            DisplayRoutine(TreeList,
                TEXT("IoCdRomFs"),
                g_SystemCallbacks.IopCdRomFileSystemQueueHead,
                Modules);
        }
        if (g_SystemCallbacks.IopNetworkFileSystemQueueHead) {

            DisplayRoutine(TreeList,
                TEXT("IoNetworkFs"),
                g_SystemCallbacks.IopNetworkFileSystemQueueHead,
                Modules);
        }
        if (g_SystemCallbacks.IopTapeFileSystemQueueHead) {

            DisplayRoutine(TreeList,
                TEXT("IoTapeFs"),
                g_SystemCallbacks.IopTapeFileSystemQueueHead,
                Modules);
        }

    }
    __except (WOBJ_EXCEPTION_FILTER_LOG) {
        return GetExceptionCode();
    }

    return STATUS_SUCCESS;
}

/*
* QueryCallbackGeneric
*
* Purpose:
*
* Query and list kernel mode data for most types of callbacks/notifies.
*
*/
OBEX_QUERYCALLBACK_ROUTINE(QueryCallbackGeneric)
{
    ULONG_PTR QueryAddress = 0;

    //
    // All parameters must be valid for this variant of Query callback.
    //
    if ((DisplayRoutine == NULL) ||
        (FindRoutine == NULL) ||
        (SystemCallbacksRef == NULL) ||
        (CallbackType == NULL))
    {
        return STATUS_INVALID_PARAMETER;
    }

    __try {

        QueryAddress = *SystemCallbacksRef;

        if (QueryAddress == 0)
            QueryAddress = FindRoutine(QueryFlags);

        *SystemCallbacksRef = QueryAddress;


    }
    __except (WOBJ_EXCEPTION_FILTER_LOG) {
        return GetExceptionCode();
    }

    __try {
        if (QueryAddress) {
            DisplayRoutine(
                TreeList,
                CallbackType,
                QueryAddress,
                Modules);
        }
        else
            return STATUS_NOT_FOUND;
    }
    __except (WOBJ_EXCEPTION_FILTER_LOG) {
        return GetExceptionCode();
    }

    return STATUS_SUCCESS;
}

/*
* DisplayCallbacksList
*
* Purpose:
*
* Find callbacks pointers and list them to output window.
*
*/
VOID DisplayCallbacksList(
    _In_ HWND TreeList,
    _In_ HWND StatusBar)
{
    NTSTATUS QueryStatus;
    ULONG i;
    PRTL_PROCESS_MODULES Modules = NULL;

    PWSTR lpStatusMsg;

    WCHAR szText[200];

    __try {

        Modules = (PRTL_PROCESS_MODULES)supGetLoadedModulesList(NULL);
        if (Modules == NULL) {
            lpStatusMsg = TEXT("Could not allocate memory for modules list!");
            supStatusBarSetText(StatusBar, 1, lpStatusMsg);
            __leave;
        }

        if (g_kdctx.NtOsImageMap == NULL) {
            lpStatusMsg = TEXT("Error, ntoskrnl image is not mapped!");
            supStatusBarSetText(StatusBar, 1, lpStatusMsg);
            __leave;
        }

        //
        // List callbacks.
        //

        for (i = 0; i < RTL_NUMBER_OF(g_CallbacksDispatchTable); i++) {
            QueryStatus = g_CallbacksDispatchTable[i].QueryRoutine(
                g_CallbacksDispatchTable[i].QueryFlags,
                g_CallbacksDispatchTable[i].DisplayRoutine,
                g_CallbacksDispatchTable[i].FindRoutine,
                g_CallbacksDispatchTable[i].CallbackType,
                TreeList,
                Modules,
                g_CallbacksDispatchTable[i].SystemCallbacksRef);

            if (!NT_SUCCESS(QueryStatus)) {

                if (QueryStatus == STATUS_NOT_FOUND) {
#ifdef _DEBUG
                    RtlStringCchPrintfSecure(szText, 
                        RTL_NUMBER_OF(szText),
                        TEXT("Callback type %ws was not found"),
                        g_CallbacksDispatchTable[i].CallbackType);

                    logAdd(WOBJ_LOG_ENTRY_WARNING, szText);

#endif
                }
                else {

                    RtlStringCchPrintfSecure(szText,
                        RTL_NUMBER_OF(szText),
                        TEXT("Callback type %ws, error 0x%lX"),
                        g_CallbacksDispatchTable[i].CallbackType,
                        QueryStatus);

                    logAdd(WOBJ_LOG_ENTRY_ERROR, szText);
                    supStatusBarSetText(StatusBar, 1, (LPWSTR)&szText);
                }
            }
        }

        //
        // Show total number of callbacks.
        //
        _strcpy(szText, TEXT("Total listed callbacks: "));
        ultostr(g_CallbacksCount, _strend(szText));
        supStatusBarSetText(StatusBar, 0, (LPWSTR)&szText);

    }
    __finally {

        if (AbnormalTermination())
            supReportAbnormalTermination(__FUNCTIONW__);

        if (Modules) supHeapFree(Modules);
    }

    SetFocus(TreeList);
}

/*
* CallbacksDialogHandlePopupMenu
*
* Purpose:
*
* Treelist popup construction
*
*/
VOID CallbacksDialogHandlePopupMenu(
    _In_ HWND hwndDlg,
    _In_ EXTRASCONTEXT* pDlgContext, 
    _In_ LPARAM lParam
)
{
    UINT uPos = 0;
    HMENU hMenu;
    POINT pt1;

    if (GetCursorPos(&pt1) == FALSE)
        return;

    hMenu = CreatePopupMenu();
    if (hMenu) {
        
        if (supTreeListAddCopyValueItem(hMenu, 
            pDlgContext->TreeList, 
            ID_OBJECT_COPY, 
            uPos++, 
            lParam, 
            &pDlgContext->tlSubItemHit)) 
        {
            InsertMenu(hMenu, uPos++, MF_BYPOSITION | MF_SEPARATOR, 0, NULL);
        }

        InsertMenu(hMenu, uPos++, MF_BYCOMMAND, ID_VIEW_REFRESH, T_VIEW_REFRESH);

        TrackPopupMenu(hMenu, TPM_RIGHTBUTTON | TPM_LEFTALIGN, pt1.x, pt1.y, 0, hwndDlg, NULL);
        DestroyMenu(hMenu);
    }
}

/*
* CallbacksDialogResize
*
* Purpose:
*
* WM_SIZE handler.
*
*/
INT_PTR CallbacksDialogResize(
    _In_ HWND hwndDlg,
    _In_ HWND hwndStatusBar,
    _In_ HWND hwndTreeList
)
{
    RECT r, szr;

    RtlSecureZeroMemory(&r, sizeof(RECT));
    RtlSecureZeroMemory(&szr, sizeof(RECT));

    GetClientRect(hwndDlg, &r);
    GetClientRect(hwndStatusBar, &szr);

    SendMessage(hwndStatusBar, WM_SIZE, 0, 0);

    SetWindowPos(hwndTreeList, 0, 0, 0,
        r.right,
        r.bottom - szr.bottom,
        SWP_NOZORDER);

    return 1;
}

/*
* CallbackDialogContentRefresh
*
* Purpose:
*
* Refresh callback list handler.
*
*/
VOID CallbackDialogContentRefresh(
    _In_  HWND hwndDlg,
    _In_ EXTRASCONTEXT *pDlgContext,
    _In_ BOOL fResetContent
)
{
#ifndef _DEBUG
    HWND hwndBanner = supDisplayLoadBanner(
        hwndDlg,
        TEXT("Processing callbacks list, please wait"),
        NULL,
        FALSE);
#else
    UNREFERENCED_PARAMETER(hwndDlg);
#endif

    __try {

        if (fResetContent) TreeList_ClearTree(pDlgContext->TreeList);

        g_CallbacksCount = 0;

        supTreeListEnableRedraw(pDlgContext->TreeList, FALSE);

        DisplayCallbacksList(pDlgContext->TreeList, pDlgContext->StatusBar);

    }
    __finally {

        supTreeListEnableRedraw(pDlgContext->TreeList, TRUE);

#ifndef _DEBUG
        supCloseLoadBanner(hwndBanner);
#endif
    }
}

/*
* CallbacksDialogProc
*
* Purpose:
*
* Callbacks Dialog window procedure.
*
*/
INT_PTR CALLBACK CallbacksDialogProc(
    _In_  HWND hwndDlg,
    _In_  UINT uMsg,
    _In_  WPARAM wParam,
    _In_  LPARAM lParam
)
{
    EXTRASCONTEXT *pDlgContext;

    switch (uMsg) {

    case WM_INITDIALOG:
        SetProp(hwndDlg, T_DLGCONTEXT, (HANDLE)lParam);
        supCenterWindow(hwndDlg);
        break;

    case WM_GETMINMAXINFO:
        if (lParam) {
            supSetMinMaxTrackSize((PMINMAXINFO)lParam,
                CBDLG_TRACKSIZE_MIN_X,
                CBDLG_TRACKSIZE_MIN_Y,
                TRUE);
        }
        break;

    case WM_SIZE:
        pDlgContext = (EXTRASCONTEXT*)GetProp(hwndDlg, T_DLGCONTEXT);
        if (pDlgContext) {
            CallbacksDialogResize(hwndDlg, pDlgContext->StatusBar, pDlgContext->TreeList);
        }
        break;

    case WM_CLOSE:
        pDlgContext = (EXTRASCONTEXT*)RemoveProp(hwndDlg, T_DLGCONTEXT);
        if (pDlgContext) {
            extrasRemoveDlgIcon(pDlgContext);
            g_WinObj.AuxDialogs[wobjCallbacksDlgId] = NULL;
            supHeapFree(pDlgContext);
        }
        return DestroyWindow(hwndDlg);

    case WM_COMMAND:

        switch (GET_WM_COMMAND_ID(wParam, lParam)) {
        case IDCANCEL:
            SendMessage(hwndDlg, WM_CLOSE, 0, 0);
            return TRUE;
        case ID_OBJECT_COPY:
            pDlgContext = (EXTRASCONTEXT*)GetProp(hwndDlg, T_DLGCONTEXT);
            if (pDlgContext) {

                supTreeListCopyItemValueToClipboard(pDlgContext->TreeList, 
                    pDlgContext->tlSubItemHit);

            }
            break;
        case ID_VIEW_REFRESH:
            pDlgContext = (EXTRASCONTEXT*)GetProp(hwndDlg, T_DLGCONTEXT);
            if (pDlgContext) {
                CallbackDialogContentRefresh(hwndDlg, pDlgContext, TRUE);
            }
            break;
        default:
            break;
        }
        break;

    case WM_CONTEXTMENU:
        pDlgContext = (EXTRASCONTEXT*)GetProp(hwndDlg, T_DLGCONTEXT);
        if (pDlgContext) {
            CallbacksDialogHandlePopupMenu(hwndDlg, pDlgContext, lParam);
        }
        break;

    }

    return FALSE;
}

/*
* extrasCreateCallbacksDialog
*
* Purpose:
*
* Create and initialize Callbacks Dialog.
*
*/
VOID extrasCreateCallbacksDialog(
    _In_ HWND hwndParent
)
{
    HWND        hwndDlg;

    HDITEM      hdritem;
    RECT        rc;

    INT SbParts[] = { 200, -1 };

    EXTRASCONTEXT  *pDlgContext;

    //
    // Allow only one dialog.
    //
    ENSURE_DIALOG_UNIQUE_WITH_RESTORE(g_WinObj.AuxDialogs[wobjCallbacksDlgId]);

    pDlgContext = (EXTRASCONTEXT*)supHeapAlloc(sizeof(EXTRASCONTEXT));
    if (pDlgContext == NULL)
        return;

    pDlgContext->tlSubItemHit = -1;

    hwndDlg = CreateDialogParam(
        g_WinObj.hInstance,
        MAKEINTRESOURCE(IDD_DIALOG_CALLBACKS),
        hwndParent,
        &CallbacksDialogProc,
        (LPARAM)pDlgContext);

    if (hwndDlg == NULL) {
        supHeapFree(pDlgContext);
        return;
    }

    pDlgContext->hwndDlg = hwndDlg;
    g_WinObj.AuxDialogs[wobjCallbacksDlgId] = hwndDlg;
    pDlgContext->StatusBar = GetDlgItem(hwndDlg, ID_EXTRASLIST_STATUSBAR);
    SendMessage(pDlgContext->StatusBar, SB_SETPARTS, 2, (LPARAM)&SbParts);

    extrasSetDlgIcon(pDlgContext);
    SetWindowText(hwndDlg, TEXT("System Callbacks"));

    GetClientRect(hwndParent, &rc);
    pDlgContext->TreeList = CreateWindowEx(WS_EX_STATICEDGE, WC_TREELIST, NULL,
        WS_VISIBLE | WS_CHILD | WS_TABSTOP | TLSTYLE_COLAUTOEXPAND | TLSTYLE_LINKLINES, 12, 14,
        rc.right - 24, rc.bottom - 24, hwndDlg, NULL, NULL, NULL);

    if (pDlgContext->TreeList) {
        RtlSecureZeroMemory(&hdritem, sizeof(hdritem));
        hdritem.mask = HDI_FORMAT | HDI_TEXT | HDI_WIDTH;
        hdritem.fmt = HDF_LEFT | HDF_BITMAP_ON_RIGHT | HDF_STRING;
        hdritem.cxy = 160;
        hdritem.pszText = TEXT("Routine Address");
        TreeList_InsertHeaderItem(pDlgContext->TreeList, 0, &hdritem);

        hdritem.cxy = 300;
        hdritem.pszText = TEXT("Module");
        TreeList_InsertHeaderItem(pDlgContext->TreeList, 1, &hdritem);

        hdritem.cxy = 200;
        hdritem.pszText = TEXT("Additional Information");
        TreeList_InsertHeaderItem(pDlgContext->TreeList, 2, &hdritem);

        CallbackDialogContentRefresh(hwndDlg, pDlgContext, FALSE);
    }

    SendMessage(hwndDlg, WM_SIZE, 0, 0);
}
