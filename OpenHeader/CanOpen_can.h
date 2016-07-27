/*
 * CanOpen_can.h
 *
 *  Created on: 2016Äê5ÔÂ23ÈÕ
 *      Author: HotPan
 */

#ifndef OPENHEADER_CANOPEN_CAN_H_
#define OPENHEADER_CANOPEN_CAN_H_

#include "applicfg.h"

typedef struct {
  UNS16 cob_id;	/**< message's ID */
  UNS8 rtr;		/**< remote transmission request. (0 if not rtr message, 1 if rtr message) */
  UNS8 len;		/**< message's length (0 to 8) */
  UNS8 data[8]; /**< message's datas */
} Message;

#define Message_Initializer {0,0,0,{0,0,0,0,0,0,0,0}}

#endif /* OPENHEADER_CANOPEN_CAN_H_ */
