/*******************************************************************************
*
*  (C) COPYRIGHT AUTHORS, 2016 - 2021
*
*  TITLE:       EXTRASDRIVERS.H
*
*  VERSION:     1.88
*
*  DATE:        02 May 2021
*
*  Common header file for Drivers dialog.
*
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
* ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED
* TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
* PARTICULAR PURPOSE.
*
*******************************************************************************/
#pragma once

typedef enum _DRIVERS_DLG_MODE {
    DDM_Normal = 0,
    DDM_Unloaded = 1,
    DDM_Max
} DRIVERS_DLG_MODE;

VOID extrasCreateDriversDialog(
    _In_ HWND hwndParent,
    _In_ DRIVERS_DLG_MODE dialogMode);
