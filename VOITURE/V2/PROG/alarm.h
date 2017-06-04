#ifndef _ALARM_
#define _ALARM_

#include "global.h"

#define ALARM_DETECTION_STATE_NONE  0x00
#define ALARM_DETECTION_STATE_UTR2  0x01
#define ALARM_DETECTION_STATE_INT   0x02
#define ALARM_DETECTION_STATE_POWER 0x04

extern u08 alarm_detection_state;

void alarm_init(void);
void alarm_monitoring_on(void);
void alarm_monitoring_off(void);
u08 alarm_monitoring_is_on(void);
u08 alarm_monitoring_is_off(void);
void alarm_detection_utr2(void);
void alarm_detection_int(void);
void alarm_detection_power(void);
void alarm_cycle(void);

#endif
