/*
 *  Copyright 2001-2009 Texas Instruments - http://www.ti.com/
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
/*============================================================================
 *  @file   SlpmTransportApp.c
 *
 *  @brief  Sample application for SlpmTransport module between MPU & AppM3
 *  using messageQ
 *
 *  ============================================================================
 */


/* Standard headers */
#include <sys/types.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include <Std.h>

/* OSAL & Utils headers */
#include <Trace.h>
#include <OsalPrint.h>
#include <Memory.h>
#include <String.h>

/* Module level headers */
#include <SysMgr.h>

/* Application header */
#include "slpmtransportApp_config.h"


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  Macros and types
 *  ============================================================================
 */
/*!
 *  @brief  Number of transfers to be tested.
 */
//#define  MESSAGEQAPP_NUM_TRANSFERS  1000
#define  MESSAGEQAPP_NUM_TRANSFERS  3
#define  PM_SUSPEND                 0
#define  PM_RESUME                  1


/** ============================================================================
 *  Globals
 *  ============================================================================
 */
MessageQ_Handle                MessageQApp_messageQ;
MessageQ_QueueId               MessageQApp_queueId;
UInt16                         MessageQApp_procId;
UInt32                         MessageQApp_shAddrBase;
UInt32                         MessageQApp_curShAddr;
ProcMgr_Handle                 MessageQApp_procMgrHandle;

/** ============================================================================
 *  Functions
 *  ============================================================================
 */
/*!
 *  @brief  Function to execute the startup for SlpmTransport sample application
 *
 *  @sa
 */
Int
SlpmTransport_startup (UInt32 notifyAddr, UInt32 sharedAddr)
{
    Int32                          status  = 0;
    SysMgr_Config                  config;
#if !defined (SYSLINK_USE_DAEMON)
    UInt32                         entry_point = 0;
    ProcMgr_StartParams            start_params;
#if defined(SYSLINK_USE_LOADER)
    Char *                         image_name;
    UInt32                         fileId;
#endif /* if defined(SYSLINK_USE_LOADER) */
#endif /* if !defined(SYSLINK_USE_DAEMON) */


    SysMgr_getConfig (&config);
    status = SysMgr_setup (&config);
    if (status < 0) {
        Osal_printf ("Error in SysMgr_setup [0x%x]\n", status);
    }

    MessageQApp_procId = MultiProc_getId ("SysM3");
    /* Open a handle to the ProcMgr instance. */
    status = ProcMgr_open (&MessageQApp_procMgrHandle,
                           MessageQApp_procId);
    if (status < 0) {
        Osal_printf ("Error in ProcMgr_open [0x%x]\n", status);
    }
    else {
        Osal_printf ("ProcMgr_open Status [0x%x]\n", status);
        /* Get the address of the shared region in kernel space. */
        status = ProcMgr_translateAddr (MessageQApp_procMgrHandle,
                                        (Ptr) &notifyAddr,
                                        ProcMgr_AddrType_MasterUsrVirt,
                                        (Ptr) SHAREDMEM0,
                                        ProcMgr_AddrType_SlaveVirt);

        if (status < 0) {
            Osal_printf ("Error in ProcMgr_translateAddr [0x%x]\n", status);
        }
        else {
            Osal_printf ("Virt address of shared address base:"
                         " [0x%x]\n", notifyAddr);
        }
    }

    if (status >= 0) {
        /* Get the address of the shared region in kernel space. */
        status = ProcMgr_translateAddr (MessageQApp_procMgrHandle,
                                        (Ptr) &sharedAddr,
                                        ProcMgr_AddrType_MasterUsrVirt,
                                        (Ptr) SHAREDMEM,
                                        ProcMgr_AddrType_SlaveVirt);

        if (status < 0) {
            Osal_printf ("Error in ProcMgr_translateAddr [0x%x]\n", status);
        }
        else {
            MessageQApp_shAddrBase = sharedAddr;
            Osal_printf ("Virt address of shared address base: [0x%x]\n",
                            sharedAddr);
        }
    }


    MessageQApp_procId = MultiProc_getId ("AppM3");

    if (status >= 0) {
        MessageQApp_curShAddr = notifyAddr;
        /* Add the region to SharedRegion module. */
        status = SharedRegion_add (0,
                                   (Ptr) MessageQApp_curShAddr,
                                   SHAREDMEMSIZE0);
        if (status < 0) {
            Osal_printf ("Error in SharedRegion_add0 [0x%x]\n", status);
        }
        else {
            Osal_printf ("SharedRegion_add0 [0x%x]\n", status);
        }
    }

    if (status >= 0) {
        MessageQApp_curShAddr = sharedAddr;
        /* Add the region to SharedRegion module. */
        status = SharedRegion_add (1,
                                   (Ptr) MessageQApp_shAddrBase,
                                   SHAREDMEMSIZE);
        if (status < 0) {
            Osal_printf ("Error in SharedRegion_add1 [0x%x]\n", status);
        }
        else {
            Osal_printf ("SharedRegion_add1 [0x%x]\n", status);
        }
    }



#if !defined (SYSLINK_USE_DAEMON)
    start_params.proc_id = MultiProc_getId("SysM3");
#ifdef SYSLINK_USE_LOADER
    image_name = "./Transport_MPUSYS_Test_PM_Core0.xem3";
    Osal_printf ("Loading image (%s) onto Ducati with ProcId %d\n", image_name,
                start_params.proc_id);
    status = ProcMgr_load (MessageQApp_procMgrHandle, image_name, 2,
                            (String *)image_name, &entry_point, &fileId,
                            start_params.proc_id);
    Osal_printf ("ProcMgr_load SysM3 Status [0x%x]\n", status);
#endif /* SYSLINK_USE_LOADER */
    status = ProcMgr_start (MessageQApp_procMgrHandle, entry_point,
                            &start_params);
    Osal_printf ("ProcMgr_start SysM3 Status [0x%x]\n", status);

    start_params.proc_id = MessageQApp_procId;
#ifdef SYSLINK_USE_LOADER
    image_name = "./Transport_MPUAPP_Test_PM_Core1.xem3";
    Osal_printf ("Loading image (%s) onto Ducati with ProcId %d\n", image_name,
            start_params.proc_id);
    status = ProcMgr_load (MessageQApp_procMgrHandle, image_name, 2,
                            (String *)image_name, &entry_point, &fileId,
                            start_params.proc_id);
    Osal_printf ("ProcMgr_load AppM3 Status [0x%x]\n", status);
#endif /* SYSLINK_USE_LOADER */
    status = ProcMgr_start (MessageQApp_procMgrHandle, entry_point,
                            &start_params);
    Osal_printf ("ProcMgr_start AppM3 Status [0x%x]\n", status);
#endif /* !SYSLINK_USE_DAEMON */


    Osal_printf ("Leaving SlpmTransport_startup\n");

    return (status);
}


/*!
 *  @brief  Function to execute the SlpmTransport sample application
 *
 *  @sa
 */
Int
SlpmTransport_execute (Void)
{
    Int32                    status = 0;
    MessageQ_Msg             msg    = NULL;
    MessageQ_Params          msgParams;
    UInt16                   i;

    Osal_printf ("Entered SlpmTransport_execute\n");

    /* Create the Message Queue. */
    MessageQ_Params_init (NULL, &msgParams);
    MessageQApp_messageQ = MessageQ_create (ARM_MESSAGEQNAME, &msgParams);
    if (MessageQApp_messageQ == NULL) {
        Osal_printf ("Error in MessageQ_create\n");
    }
    else {
        Osal_printf ("MessageQ_create handle [0x%x]\n",
                     MessageQApp_messageQ);
    }

    if (status >=0) {
        do {
            status = MessageQ_open (DUCATI_CORE1_MESSAGEQNAME,
                                    &MessageQApp_queueId);
        } while (status == MESSAGEQ_E_NOTFOUND);
        if (status < 0) {
            Osal_printf ("Error in MessageQ_open [0x%x]\n", status);
        }
        else {
            Osal_printf ("MessageQ_open Status [0x%x]\n", status);
            Osal_printf ("MessageQApp_queueId  [0x%x]\n", MessageQApp_queueId);
        }
    }

    if (status > 0) {
        Osal_printf ("\nExchanging messages with remote processor\n");
        for (i = 0 ; i < MESSAGEQAPP_NUM_TRANSFERS ; i++) {
            /* Allocate message. */
            msg = MessageQ_alloc (HEAPID, MSGSIZE);
            if (msg == NULL) {
                Osal_printf ("Error in MessageQ_alloc\n");
                break;
            }
            else {
                Osal_printf ("MessageQ_alloc msg [0x%x]\n", msg);
            }

            MessageQ_setMsgId (msg, (i % 16));

            /* Have the DSP reply to this message queue */
            MessageQ_setReplyQueue (MessageQApp_messageQ, msg);

            status = MessageQ_put (MessageQApp_queueId, msg);
            if (status < 0) {
                Osal_printf ("Error in MessageQ_put [0x%x]\n",
                             status);
                break;
            }
            else {
                Osal_printf ("MessageQ_put #%d Status [0x%x]\n", i, status);
            }

            status = MessageQ_get(MessageQApp_messageQ, &msg, MESSAGEQ_FOREVER);
            if (status < 0) {
                Osal_printf ("Error in MessageQ_get\n");
                break;
            }
            else {
                /* Validate the returned message. */
                if (msg != NULL) {
                    if (MessageQ_getMsgId (msg) != ((i % 16) + 1)) {
                        Osal_printf ("Data integrity failure!\n"
                                     "    Expected %d\n"
                                     "    Received %d\n",
                                     ((i % 16) + 1),
                                     MessageQ_getMsgId (msg));
                        break;
                    }
                }

                status = MessageQ_free (msg);
                Osal_printf ("MessageQ_free status [0x%x]\n", status);
            }

            if ((i % 2) == 0) {
                Osal_printf ("Exchanged %d messages with remote processor\n",
                             i);
            }
        }
    }

    /* Keep the Ducati application running. */
#if !defined (SYSLINK_USE_DAEMON)
    /* Send die message */
    msg = MessageQ_alloc (HEAPID, MSGSIZE);
    if (msg == NULL) {
        Osal_printf ("MessageQ_alloc (die message) failed\n");
    }
    else {
        Osal_printf ("MessageQ_alloc (die message) msg = [0x%x]\n", msg);

        /* Send a die message */
        MessageQ_setMsgId(msg, DIEMESSAGE);

        /* Have the DSP reply to this message queue */
        MessageQ_setReplyQueue (MessageQApp_messageQ, msg);

        /* Send the message off */
        status = MessageQ_put (MessageQApp_queueId, msg);
        if (status < 0) {
            Osal_printf ("Error in MessageQ_put (die message) [0x%x]\n",
                         status);
        }
        else {
            Osal_printf ("MessageQ_put (die message) Status [0x%x]\n", status);
        }

        /* Wait for the final message. */
        status = MessageQ_get(MessageQApp_messageQ, &msg, MESSAGEQ_FOREVER);
        if (status < 0) {
            Osal_printf ("\nError in MessageQ_get (die message)!\n");
        }
        else {
            /* Validate the returned message. */
            if (msg != NULL) {
                if (MessageQ_getMsgId (msg) == DIEMESSAGE) {
                    Osal_printf ("\nSuccessfully received die response from the"
                                 " remote processor\n");
                    Osal_printf ("Sample application successfully completed\n");
                }
                else {
                    Osal_printf("\nUnsuccessful run of the sample "
                                "application!\n");
                }
            }
            else {
                Osal_printf ("\nUnsuccessful run of the sample application "
                                "msg == NULL!\n");
            }
        }
        MessageQ_free(msg);
    }
#else
    Osal_printf ("Sample application successfully completed!\n");
#endif /* !SYSLINK_USE_DAEMON */

    /* Clean-up */
    if (MessageQApp_messageQ != NULL) {
        status = MessageQ_delete (&MessageQApp_messageQ);
        if (status < 0) {
            Osal_printf ("Error in MessageQ_delete [0x%x]\n",
                         status);
        }
        else {
            Osal_printf ("MessageQ_delete Status [0x%x]\n", status);
        }
    }

    if (MessageQApp_messageQ != NULL) {
        MessageQ_close (&MessageQApp_queueId);
    }

    Osal_printf ("Leaving SlpmTransport_execute\n");

    return (status);
}


/*!
 *  @brief Function to execute the shutdown for SlpmTransport sample app
 *
 *  @sa
 */

Int
SlpmTransport_shutdown (Void)
{
    Int32               status = 0;
#if !defined (SYSLINK_USE_DAEMON)
    ProcMgr_StopParams  stop_params;
#endif /* !SYSLINK_USE_DAEMON */

    Osal_printf ("Entered SlpmTransport_shutdown()\n");

    SharedRegion_remove (0);
    SharedRegion_remove (1);

#if !defined (SYSLINK_USE_DAEMON)
    stop_params.proc_id = MessageQApp_procId;
    status = ProcMgr_stop (MessageQApp_procMgrHandle, &stop_params);
    Osal_printf ("ProcMgr_stop status for proc_id %d : [0x%x]\n",
    stop_params.proc_id, status);
    MessageQApp_procId = MultiProc_getId ("SysM3");

    stop_params.proc_id = MessageQApp_procId;
    status = ProcMgr_stop (MessageQApp_procMgrHandle, &stop_params);
    Osal_printf ("ProcMgr_stop status for proc_id %d : [0x%x]\n",
    stop_params.proc_id, status);
#endif /* !SYSLINK_USE_DAEMON */

    status = ProcMgr_close (&MessageQApp_procMgrHandle);
    Osal_printf ("ProcMgr_close status: [0x%x]\n", status);

    status = SysMgr_destroy ();
    Osal_printf ("SysMgr_destroy status: [0x%x]\n", status);


    Osal_printf ("Leave SlpmTransport_shutdown()\n");

    return (status);
}