#ifndef __S5PV210_REG_KEYPAD_H__
#define __S5PV210_REG_KEYPAD_H__

#include <xboot.h>

#define S5PV210_KEYPAD_CON	 	 			(0xe1600000)
#define S5PV210_KEYPAD_STSCLR 	 			(0xe1600004)
#define S5PV210_KEYPAD_COL	 	 			(0xe1600008)
#define S5PV210_KEYPAD_ROW	 	 			(0xe160000c)
#define S5PV210_KEYPAD_FC	 	 			(0xe1600010)

#define S5PV210_KEYPAD_CON_INT_F_EN			(1 << 0)
#define S5PV210_KEYPAD_CON_INT_R_EN			(1 << 1)
#define S5PV210_KEYPAD_CON_DF_EN			(1 << 2)
#define S5PV210_KEYPAD_CON_FC_EN			(1 << 3)
#define S5PV210_KEYPAD_CON_WAKEUP_EN		(1 << 4)

#endif /* __S5PV210_REG_KEYPAD_H__ */
