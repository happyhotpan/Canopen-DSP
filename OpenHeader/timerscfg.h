/*
 * timerscfg.h
 *
 *  Created on: 2016Äê5ÔÂ24ÈÕ
 *      Author: HotPan
 */

#ifndef OPENHEADER_TIMERSCFG_H_
#define OPENHEADER_TIMERSCFG_H_

// Whatever your microcontroller, the timer wont work if
// TIMEVAL is not at least on 32 bits
#define TIMEVAL UNS32

// The timer of the AT91 counts from 0000 to 0xFFFF
#define TIMEVAL_MAX 0xFFFF

// The timer is incrementing every 2.66 us.
#define MS_TO_TIMEVAL(ms) ((ms) * 375)
#define US_TO_TIMEVAL(us) ((us) * 3 / 8)


#endif /* OPENHEADER_TIMERSCFG_H_ */
