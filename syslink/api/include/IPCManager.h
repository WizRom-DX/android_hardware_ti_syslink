/*
 *  Syslink-IPC for TI OMAP Processors
 *
 *  Copyright (c) 2008-2010, Texas Instruments Incorporated
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *  *  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *  *  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 *  *  Neither the name of Texas Instruments Incorporated nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 *  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/** ============================================================================
 *  @file   IPCManager.h
 *
 *  @brief      Defines the IPC driver wrapper for Syslink modules.
 *
 *  ============================================================================
 */

#ifndef IPCMANAGER_
#define IPCMANAGER_

#include <Std.h>

#undef open
#define open  IPCManager_open

#undef close
#define close IPCManager_close

#undef ioctl
#define ioctl IPCManager_ioctl

#undef fcntl
#define fcntl IPCManager_fcntl


/*
 *  ======== IPCManager_open ========
 *      Open handle to the IPC driver
 */
Int IPCManager_open(const char *name, int flags);

/*
 *  ======== IPCManager_close ========
 *      Close handle to the IPC driver
 */
Int IPCManager_close(int fd);

/*
 * ======== IPCManager_ioctl ========
 */
Int IPCManager_ioctl(int fd, UInt32 cmd, Ptr args);

/*
 * ======== IPCManager_fcntl========
 */
Int IPCManager_fcntl(int fd, int cmd, long arg);

/*
 * ======== IPCManager_getModuleStatus========
 *  This function converts the os specific (Linux) error code to
 *  module specific error code
 */
Int IPCManager_getModuleStatus(Int moduleId, Ptr args);

#endif
