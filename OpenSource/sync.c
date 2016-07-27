/*
 * sync.c
 *
 *  Created on: 2016Äê5ÔÂ28ÈÕ
 *      Author: HotPan
 */


#include "data.h"
#include "sync.h"
#include "canfestival.h"
#include "sysdep.h"

/* Prototypes for internals functions */

/*!
**
**
** @param d
** @param id
**/
void SyncAlarm(CO_Data* d, UNS32 id);
UNS32 OnCOB_ID_SyncUpdate(CO_Data* d, const indextable * unsused_indextable,
	UNS8 unsused_bSubindex);

/*!
**
**
** @param d
** @param id
**/
void SyncAlarm(CO_Data* d, UNS32 id)
{
	sendSYNC(d) ;
}

/*!
** This is called when Index 0x1005 is updated.
**
** @param d
** @param unsused_indextable
** @param unsused_bSubindex
**
** @return
**/
UNS32 OnCOB_ID_SyncUpdate(CO_Data* d, const indextable * unsused_indextable, UNS8 unsused_bSubindex)
{
	startSYNC(d);
	return 0;
}

/*!
**
**
** @param d
**/
void startSYNC(CO_Data* d)
{
	if(d->syncTimer != TIMER_NONE){
		stopSYNC(d);
	}

	RegisterSetODentryCallBack(d, 0x1005, 0, &OnCOB_ID_SyncUpdate);
	RegisterSetODentryCallBack(d, 0x1006, 0, &OnCOB_ID_SyncUpdate);

	if(*d->COB_ID_Sync & 0x40000000ul && *d->Sync_Cycle_Period)
	{
		d->syncTimer = SetAlarm(
				d,
				0 /*No id needed*/,
				&SyncAlarm,
				US_TO_TIMEVAL(*d->Sync_Cycle_Period),
				US_TO_TIMEVAL(*d->Sync_Cycle_Period));
	}
}

/*!
**
**
** @param d
**/
void stopSYNC(CO_Data* d)
{
    RegisterSetODentryCallBack(d, 0x1005, 0, NULL);
    RegisterSetODentryCallBack(d, 0x1006, 0, NULL);
	d->syncTimer = DelAlarm(d->syncTimer);
}


/*!
**
**
** @param d
** @param cob_id
**
** @return
**/
UNS8 sendSYNCMessage(CO_Data* d)
{
  Message m;

  MSG_WAR(0x3001, "sendSYNC ", 0);

  m.cob_id = (UNS16)UNS16_LE(*d->COB_ID_Sync);
  m.rtr = NOT_A_REQUEST;
  m.len = 0;

  return canSend(d->canHandle,&m);
}


/*!
**
**
** @param d
** @param cob_id
**
** @return
**/
UNS8 sendSYNC(CO_Data* d)
{
  UNS8 res;
  res = sendSYNCMessage(d);
  proceedSYNC(d) ;
  return res ;
}

/*!
**
**
** @param d
** @param m
**
** @return
**/
UNS8 proceedSYNC(CO_Data* d)
{

  UNS8 res;

  MSG_WAR(0x3002, "SYNC received. Proceed. ", 0);

  (*d->post_sync)(d);

  /* only operational state allows PDO transmission */
  if(! d->CurrentCommunicationState.csPDO)
    return 0;

  res = _sendPDOevent(d, 1 /*isSyncEvent*/ );

  /*Call user app callback*/
  (*d->post_TPDO)(d);

  return res;

}


void _post_sync(CO_Data* d){}
void _post_TPDO(CO_Data* d){}


