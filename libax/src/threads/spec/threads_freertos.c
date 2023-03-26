// ***************************************************************************
// TITLE
//      Threads on UNIX platform
// PROJECT
//      Arsenal Library
// ***************************************************************************
// FILE
//      $Id: threads.c,v 1.2 2003/04/24 10:07:28 A.Kozhukhar Exp $
// AUTHOR
//      $Author: A.Kozhukhar $
// ***************************************************************************

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <arsenal.h>
#include <axthreads.h>
#include <pvt_threads.h>

#include <FreeRTOS.h>
#include <task.h>
#include <projdefs.h>

// ---------------------------------------------------------------------------
// ------------------------------ FUNCTIONS ----------------------------------
// ---------------------------------------------------------------------------

// ***************************************************************************
// FUNCTION
//      axthread_exit
// PURPOSE
//
// PARAMETERS
//      UINT   u_retcode --
// RESULT
//      void  --
// ***************************************************************************
void axthread_exit(UINT u_retcode)
{
    // TODO: Find out how exectly should be returned retcode
    //pthread_exit(&u_retcode);
}
// ***************************************************************************
// FUNCTION
//      axthread_destroy
// PURPOSE
//
// PARAMETERS
//      HAXTHREAD h_thread --
// RESULT
//      HAXTHREAD  --
// ***************************************************************************
HAXTHREAD axthread_destroy(HAXTHREAD  h_thread)
{
    ENTER(h_thread);

    //pthread_detach((pthread_t)h_thread);

    RETURN(NULL);
}
// ***************************************************************************
// FUNCTION
//      axthread_create
// PURPOSE
//      Create thread
// PARAMETERS
//      PLA6THREAD          pst_thread -- Pointer to Thread control structure
//      DWORD               d_flags    -- Creation flags
//      PFNAXTHREAD pf_entry   -- Pointer to thread function
//      PVOID               p_param    -- User's parameter for thread function
// RESULT
//      BOOL  -- true if all is ok or false if error has occured
// ***************************************************************************
BOOL axthread_create(HAXTHREAD *        ph_thread,
                     UINT               d_flags,
                     UINT               stackSize,
                     UINT               priority,
                     PFNAXTHREAD        pf_entry,
                     PVOID              p_param)
{
    BOOL            b_result    = false;

    ENTER(pf_entry);

    b_result = (pdPASS == xTaskCreate((pdTASK_CODE)pf_entry, 
                                      nil, 
                                      stackSize ? stackSize : (configMINIMAL_STACK_SIZE * 2), 
                                      p_param, 
                                      priority  ? priority  : (tskIDLE_PRIORITY + 1UL),
                                      (xTaskHandle *)ph_thread));


//    TaskHandle_t handle;
//
//
//    if (xTaskCreate((TaskFunction_t)thread_def->pthread,(const portCHAR *)thread_def->name,
//                thread_def->stacksize, argument, makeFreeRtosPriority(thread_def->tpriority),
//                &handle) != pdPASS)  {
//      return NULL;
//    }


//        pthread_attr_init(&v_attr);

//        pthread_attr_setdetachstate(&v_attr, PTHREAD_CREATE_DETACHED);

                                   // &v_attr

//    if (!pthread_create(&v_thread, NULL, (void*(*)(void*))pf_entry, p_param))
//    {
//        if (ph_thread)
//        {
//            *ph_thread = (HAXTHREAD)v_thread;
//        }
//
//        b_result = true;
//    }

//        pthread_attr_destroy(&v_attr);

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      axthread_priority_set
// PURPOSE
//
// PARAMETERS
//      HAXTHREAD   h_thread   --
//      INT         i_priority --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL axthread_priority_set(HAXTHREAD        h_thread,
                           INT              i_priority)
{
    BOOL                    b_result    = false;
//    int                     i_policy;
//    struct sched_param      st_param;

    ENTER(h_thread);

//    if (!pthread_getschedparam((pthread_t)h_thread, &i_policy, &st_param))
//    {
//        i_policy = SCHED_FIFO;
//        st_param.sched_priority = i_priority * 10;
//
//        if (!pthread_setschedparam((pthread_t)h_thread, i_policy, &st_param))
//        {
//            b_result = true;
//        }
//        else
//        {
//            AXTRACE("  priority %d NOT set for %p\n", i_priority, h_thread);
//            perror("  priority setting error: ");
//        }
//    }
//    else
//        AXTRACE("  cannot get priorit yof thread\n");

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      axthread_get_id
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      UINT --
// ***************************************************************************
UINT axthread_get_id()
{
    return 0;
}

