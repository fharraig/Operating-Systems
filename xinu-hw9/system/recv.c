/**
 * @file recv.c
 * @provides recv.
 *
 * $Id: receive.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <xinu.h>

/**
 * recv - wait for a mesage and return it
 * @return message
 */

message recv(void)
{
	register pcb *ppcb;
	int senderpid = -1;
	message msg;
	ppcb = &proctab[currpid[getcpuid()]];

	/* TODO:
 	* - Remember to acquire lock and release lock when interacitng with the msg structure
 	* - Check for Message. If no message, put in blocking state and reschedule
 	*   			If there is message, retrive message
 	* - Now, check queue for wating send processes,
 	*   If stuff in queue  collect message for next time recv is called and ready send process
 	*   else, reset message flag
 	*   return collected message
 	*/

	register pcb *sender; 
	
	lock_acquire(ppcb -> msg_var.core_com_lock);

	if (ppcb -> msg_var.hasMessage == FALSE) {
		ppcb -> state = PRRECV;

		lock_release(ppcb -> msg_var.core_com_lock);
		resched(); 
		
		msg = NULL;
	} else {

		msg = ppcb -> msg_var.msgin;
	
		if (!(isempty(ppcb -> msg_var.msgqueue))) {
			senderpid = dequeue(ppcb -> msg_var.msgqueue);
			sender = &proctab[senderpid]; 
			
			ppcb -> msg_var.msgin = sender -> msg_var.msgout;
			sender -> msg_var.msgout = NULL;

			ready(senderpid, RESCHED_NO, sender -> core_affinity);
		} else {
			ppcb -> msg_var.hasMessage = FALSE;
		}
	
		lock_release(ppcb -> msg_var.core_com_lock);
	}

	return msg;
}
