/*
 * canfestival.h
 *
 *  Created on: 2016Äê5ÔÂ24ÈÕ
 *      Author: HotPan
 */

#ifndef OPENHEADER_CANFESTIVAL_H_
#define OPENHEADER_CANFESTIVAL_H_

#include "applicfg.h"
#include "data.h"
#include "objdict.h"

// ---------  to be called by user app ---------
void initTimer(void);
UNS8 canSend(CAN_PORT notused, Message *m);



#endif /* OPENHEADER_CANFESTIVAL_H_ */
