/* Copyright (c) 2011-2012, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#define DBG_0 DBGDIDR
#define DBG_1 DBGDSCRint
#define DBG_2 DBGDCCINT
#define DBG_3 DBGDTRTXint
#define DBG_4 DBGDTRRXint
#define DBG_5 DBGWFAR
#define DBG_6 DBGVCR
#define DBG_7 DBGDTRRXext
#define DBG_8 DBGDSCRext
#define DBG_9 DBGDTRTXext
#define DBG_10 DBGOSECCR
#define DBG_11 DBGBVR0
#define DBG_12 DBGBVR1
#define DBG_13 DBGBVR2
#define DBG_14 DBGBVR3
#define DBG_15 DBGBVR4
#define DBG_16 DBGBVR5
#define DBG_17 DBGBCR0
#define DBG_18 DBGBCR1
#define DBG_19 DBGBCR2
#define DBG_20 DBGBCR3
#define DBG_21 DBGBCR4
#define DBG_22 DBGBCR5
#define DBG_23 DBGWVR0
#define DBG_24 DBGWVR1
#define DBG_25 DBGWVR2
#define DBG_26 DBGWVR3
#define DBG_27 DBGWCR0
#define DBG_28 DBGWCR1
#define DBG_29 DBGWCR2
#define DBG_30 DBGWCR3
#define DBG_31 DBGDRAR
#define DBG_32 DBGBXVR4
#define DBG_33 DBGBXVR5
#define DBG_34 DBGOSLAR
#define DBG_35 DBGOSLSR
#define DBG_36 DBGOSDLR
#define DBG_37 DBGPRCR
#define DBG_38 DBGDSAR
#define DBG_39 DBGDEVID2
#define DBG_40 DBGDEVID1
#define DBG_41 DBGDEVID
#define DBG_42 DBGCLAIMSET
#define DBG_43 DBGCLAIMCLR
#define DBG_44 DBGAUTHSTATUS
/* Dummy write accessor for read-only registers */

#define WCP14_DBGDIDR(x)		printk(KERN_INFO "DBGDIDR: read-only register\n");
#define WCP14_DBGDSCRint(x)		printk(KERN_INFO "DBGDSCRint: read-only register\n");
#define WCP14_DBGDTRRXint(x)	printk(KERN_INFO "DBGDTRRXint: read-only register\n");
#define WCP14_DBGDRAR(x)		printk(KERN_INFO "DBGDRAR: read-only register\n");
#define WCP14_DBGOSLSR(x)		printk(KERN_INFO "DBGOSLSR: read-only register\n");
#define WCP14_DBGDSAR(x)		printk(KERN_INFO "DBGDSAR: read-only register\n");
#define WCP14_DBGDEVID2(x)		printk(KERN_INFO "DBGDEVID2: read-only register\n");
#define WCP14_DBGDEVID1(x)		printk(KERN_INFO "DBGDEVID1: read-only register\n");
#define WCP14_DBGDEVID(x)		printk(KERN_INFO "DBGDEVID: read-only register\n");
#define WCP14_DBGAUTHSTATUS(x)	printk(KERN_INFO "DBGAUTHSTATUS: read-only register\n");

/* Dummy read accessor for write-only registers */

#define RCP14_DBGDTRTXint(x) 	printk(KERN_INFO "DBGDTRTXint: write-only register\n");

/* Accessors for CP14 registers */
#define dbgg_read(val, reg)			RCP14_##reg(val)
#define dbgg_write(val, reg)		WCP14_##reg(val)

#define dbg_read(val, reg)			dbgg_read(val, reg)	
#define dbg_write(val, reg)			dbgg_write(val, reg)

/* MRC14 and MCR14 */
#define MRC14(val, op1, crn, crm, op2) \
({ \
asm volatile("mrc p14, "#op1", %0, "#crn", "#crm", "#op2 : "=r" (val));	\
})

#define MCR14(val, op1, crn, crm, op2) \
({ \
asm volatile("mcr p14, "#op1", %0, "#crn", "#crm", "#op2 : : "r" (val)); \
})

/* Debug Registers
 *
 * Available only in DBGv7
 * DBGECR, DBGDSCCR, DBGDSMCR, DBGDRCR
 *
 * Available only in DBGv7.1
 * DBGBXVRm, DBGOSDLR, DBGDEVID2, DBGDEVID1
 *
 * Read only
 * DBGDIDR, DBGDSCRint, DBGDTRRXint, DBGDRAR, DBGOSLSR, DBGOSSRR, DBGPRSR,
 * DBGPRSR, DBGDSAR, DBGAUTHSTATUS, DBGDEVID2, DBGDEVID1, DBGDEVID
 *
 * Write only
 * DBGDTRTXint, DBGOSLAR
 */
#define RCP14_DBGDIDR(val)			MRC14(val, 0, c0, c0, 0)
#define RCP14_DBGDSCRint(val)		MRC14(val, 0, c0, c1, 0)
#define RCP14_DBGDTRRXint(val)		MRC14(val, 0, c0, c5, 0)
#define RCP14_DBGWFAR(val)			MRC14(val, 0, c0, c6, 0)
#define RCP14_DBGVCR(val)			MRC14(val, 0, c0, c7, 0)
#define RCP14_DBGECR(val)			MRC14(val, 0, c0, c9, 0)
#define RCP14_DBGDCCINT(val)			MRC14(val, 0, c0, c2, 0)
#define RCP14_DBGDSCCR(val)		MRC14(val, 0, c0, c10, 0)
#define RCP14_DBGDSMCR(val)		MRC14(val, 0, c0, c11, 0)
#define RCP14_DBGDTRRXext(val)		MRC14(val, 0, c0, c0, 2)
#define RCP14_DBGOSECCR(val)		MRC14(val, 0, c0, c6, 2)
#define RCP14_DBGDSCRext(val)		MRC14(val, 0, c0, c2, 2)
#define RCP14_DBGDTRTXext(val)		MRC14(val, 0, c0, c3, 2)
#define RCP14_DBGDRCR(val)			MRC14(val, 0, c0, c4, 2)
#define RCP14_DBGBVR0(val)			MRC14(val, 0, c0, c0, 4)
#define RCP14_DBGBVR1(val)			MRC14(val, 0, c0, c1, 4)
#define RCP14_DBGBVR2(val)			MRC14(val, 0, c0, c2, 4)
#define RCP14_DBGBVR3(val)			MRC14(val, 0, c0, c3, 4)
#define RCP14_DBGBVR4(val)			MRC14(val, 0, c0, c4, 4)
#define RCP14_DBGBVR5(val)			MRC14(val, 0, c0, c5, 4)
#define RCP14_DBGBVR6(val)			MRC14(val, 0, c0, c6, 4)
#define RCP14_DBGBVR7(val)			MRC14(val, 0, c0, c7, 4)
#define RCP14_DBGBVR8(val)			MRC14(val, 0, c0, c8, 4)
#define RCP14_DBGBVR9(val)			MRC14(val, 0, c0, c9, 4)
#define RCP14_DBGBVR10(val)		MRC14(val, 0, c0, c10, 4)
#define RCP14_DBGBVR11(val)		MRC14(val, 0, c0, c11, 4)
#define RCP14_DBGBVR12(val)		MRC14(val, 0, c0, c12, 4)
#define RCP14_DBGBVR13(val)		MRC14(val, 0, c0, c13, 4)
#define RCP14_DBGBVR14(val)		MRC14(val, 0, c0, c14, 4)
#define RCP14_DBGBVR15(val)		MRC14(val, 0, c0, c15, 4)
#define RCP14_DBGBCR0(val)			MRC14(val, 0, c0, c0, 5)
#define RCP14_DBGBCR1(val)			MRC14(val, 0, c0, c1, 5)
#define RCP14_DBGBCR2(val)			MRC14(val, 0, c0, c2, 5)
#define RCP14_DBGBCR3(val)			MRC14(val, 0, c0, c3, 5)
#define RCP14_DBGBCR4(val)			MRC14(val, 0, c0, c4, 5)
#define RCP14_DBGBCR5(val)			MRC14(val, 0, c0, c5, 5)
#define RCP14_DBGBCR6(val)			MRC14(val, 0, c0, c6, 5)
#define RCP14_DBGBCR7(val)			MRC14(val, 0, c0, c7, 5)
#define RCP14_DBGBCR8(val)			MRC14(val, 0, c0, c8, 5)
#define RCP14_DBGBCR9(val)			MRC14(val, 0, c0, c9, 5)
#define RCP14_DBGBCR10(val)		MRC14(val, 0, c0, c10, 5)
#define RCP14_DBGBCR11(val)		MRC14(val, 0, c0, c11, 5)
#define RCP14_DBGBCR12(val)		MRC14(val, 0, c0, c12, 5)
#define RCP14_DBGBCR13(val)		MRC14(val, 0, c0, c13, 5)
#define RCP14_DBGBCR14(val)		MRC14(val, 0, c0, c14, 5)
#define RCP14_DBGBCR15(val)		MRC14(val, 0, c0, c15, 5)
#define RCP14_DBGWVR0(val)			MRC14(val, 0, c0, c0, 6)
#define RCP14_DBGWVR1(val)			MRC14(val, 0, c0, c1, 6)
#define RCP14_DBGWVR2(val)			MRC14(val, 0, c0, c2, 6)
#define RCP14_DBGWVR3(val)			MRC14(val, 0, c0, c3, 6)
#define RCP14_DBGWVR4(val)			MRC14(val, 0, c0, c4, 6)
#define RCP14_DBGWVR5(val)			MRC14(val, 0, c0, c5, 6)
#define RCP14_DBGWVR6(val)			MRC14(val, 0, c0, c6, 6)
#define RCP14_DBGWVR7(val)			MRC14(val, 0, c0, c7, 6)
#define RCP14_DBGWVR8(val)			MRC14(val, 0, c0, c8, 6)
#define RCP14_DBGWVR9(val)			MRC14(val, 0, c0, c9, 6)
#define RCP14_DBGWVR10(val)		MRC14(val, 0, c0, c10, 6)
#define RCP14_DBGWVR11(val)		MRC14(val, 0, c0, c11, 6)
#define RCP14_DBGWVR12(val)		MRC14(val, 0, c0, c12, 6)
#define RCP14_DBGWVR13(val)		MRC14(val, 0, c0, c13, 6)
#define RCP14_DBGWVR14(val)		MRC14(val, 0, c0, c14, 6)
#define RCP14_DBGWVR15(val)		MRC14(val, 0, c0, c15, 6)
#define RCP14_DBGWCR0(val)			MRC14(val, 0, c0, c0, 7)
#define RCP14_DBGWCR1(val)			MRC14(val, 0, c0, c1, 7)
#define RCP14_DBGWCR2(val)			MRC14(val, 0, c0, c2, 7)
#define RCP14_DBGWCR3(val)			MRC14(val, 0, c0, c3, 7)
#define RCP14_DBGWCR4(val)			MRC14(val, 0, c0, c4, 7)
#define RCP14_DBGWCR5(val)			MRC14(val, 0, c0, c5, 7)
#define RCP14_DBGWCR6(val)			MRC14(val, 0, c0, c6, 7)
#define RCP14_DBGWCR7(val)			MRC14(val, 0, c0, c7, 7)
#define RCP14_DBGWCR8(val)			MRC14(val, 0, c0, c8, 7)
#define RCP14_DBGWCR9(val)			MRC14(val, 0, c0, c9, 7)
#define RCP14_DBGWCR10(val)		MRC14(val, 0, c0, c10, 7)
#define RCP14_DBGWCR11(val)		MRC14(val, 0, c0, c11, 7)
#define RCP14_DBGWCR12(val)		MRC14(val, 0, c0, c12, 7)
#define RCP14_DBGWCR13(val)		MRC14(val, 0, c0, c13, 7)
#define RCP14_DBGWCR14(val)		MRC14(val, 0, c0, c14, 7)
#define RCP14_DBGWCR15(val)		MRC14(val, 0, c0, c15, 7)
#define RCP14_DBGDRAR(val)			MRC14(val, 0, c1, c0, 0)
#define RCP14_DBGBXVR0(val)		MRC14(val, 0, c1, c0, 1)
#define RCP14_DBGBXVR1(val)		MRC14(val, 0, c1, c1, 1)
#define RCP14_DBGBXVR2(val)		MRC14(val, 0, c1, c2, 1)
#define RCP14_DBGBXVR3(val)		MRC14(val, 0, c1, c3, 1)
#define RCP14_DBGBXVR4(val)		MRC14(val, 0, c1, c4, 1)
#define RCP14_DBGBXVR5(val)		MRC14(val, 0, c1, c5, 1)
#define RCP14_DBGBXVR6(val)		MRC14(val, 0, c1, c6, 1)
#define RCP14_DBGBXVR7(val)		MRC14(val, 0, c1, c7, 1)
#define RCP14_DBGBXVR8(val)		MRC14(val, 0, c1, c8, 1)
#define RCP14_DBGBXVR9(val)		MRC14(val, 0, c1, c9, 1)
#define RCP14_DBGBXVR10(val)		MRC14(val, 0, c1, c10, 1)
#define RCP14_DBGBXVR11(val)		MRC14(val, 0, c1, c11, 1)
#define RCP14_DBGBXVR12(val)		MRC14(val, 0, c1, c12, 1)
#define RCP14_DBGBXVR13(val)		MRC14(val, 0, c1, c13, 1)
#define RCP14_DBGBXVR14(val)		MRC14(val, 0, c1, c14, 1)
#define RCP14_DBGBXVR15(val)		MRC14(val, 0, c1, c15, 1)
#define RCP14_DBGOSLAR(val)		MRC14(val, 0, c1, c0, 4)
#define RCP14_DBGOSLSR(val)		MRC14(val, 0, c1, c1, 4)
#define RCP14_DBGOSSRR(val)		MRC14(val, 0, c1, c2, 4)
#define RCP14_DBGOSDLR(val)		MRC14(val, 0, c1, c3, 4)
#define RCP14_DBGPRCR(val)			MRC14(val, 0, c1, c4, 4)
#define RCP14_DBGPRSR(val)			MRC14(val, 0, c1, c5, 4)
#define RCP14_DBGDSAR(val)			MRC14(val, 0, c2, c0, 0)
#define RCP14_DBGITCTRL(val)		MRC14(val, 0, c7, c0, 4)
#define RCP14_DBGCLAIMSET(val)		MRC14(val, 0, c7, c8, 6)
#define RCP14_DBGCLAIMCLR(val)		MRC14(val, 0, c7, c9, 6)
#define RCP14_DBGAUTHSTATUS(val)		MRC14(val, 0, c7, c14, 6)
#define RCP14_DBGDEVID2(val)		MRC14(val, 0, c7, c0, 7)
#define RCP14_DBGDEVID1(val)		MRC14(val, 0, c7, c1, 7)
#define RCP14_DBGDEVID(val)		MRC14(val, 0, c7, c2, 7)

#define WCP14_DBGDTRTXint(val)		MCR14(val, 0, c0, c5, 0)
#define WCP14_DBGWFAR(val)		MCR14(val, 0, c0, c6, 0)
#define WCP14_DBGVCR(val)		MCR14(val, 0, c0, c7, 0)
#define WCP14_DBGECR(val)		MCR14(val, 0, c0, c9, 0)
#define WCP14_DBGDCCINT(val)		MCR14(val, 0, c0, c2, 0)
#define WCP14_DBGDSCCR(val)		MCR14(val, 0, c0, c10, 0)
#define WCP14_DBGDSMCR(val)		MCR14(val, 0, c0, c11, 0)
#define WCP14_DBGDTRRXext(val)		MCR14(val, 0, c0, c0, 2)
#define WCP14_DBGOSECCR(val)		MCR14(val, 0, c0, c6, 2)
#define WCP14_DBGDSCRext(val)		MCR14(val, 0, c0, c2, 2)
#define WCP14_DBGDTRTXext(val)		MCR14(val, 0, c0, c3, 2)
#define WCP14_DBGDRCR(val)		MCR14(val, 0, c0, c4, 2)
#define WCP14_DBGBVR0(val)		MCR14(val, 0, c0, c0, 4)
#define WCP14_DBGBVR1(val)		MCR14(val, 0, c0, c1, 4)
#define WCP14_DBGBVR2(val)		MCR14(val, 0, c0, c2, 4)
#define WCP14_DBGBVR3(val)		MCR14(val, 0, c0, c3, 4)
#define WCP14_DBGBVR4(val)		MCR14(val, 0, c0, c4, 4)
#define WCP14_DBGBVR5(val)		MCR14(val, 0, c0, c5, 4)
#define WCP14_DBGBVR6(val)		MCR14(val, 0, c0, c6, 4)
#define WCP14_DBGBVR7(val)		MCR14(val, 0, c0, c7, 4)
#define WCP14_DBGBVR8(val)		MCR14(val, 0, c0, c8, 4)
#define WCP14_DBGBVR9(val)		MCR14(val, 0, c0, c9, 4)
#define WCP14_DBGBVR10(val)		MCR14(val, 0, c0, c10, 4)
#define WCP14_DBGBVR11(val)		MCR14(val, 0, c0, c11, 4)
#define WCP14_DBGBVR12(val)		MCR14(val, 0, c0, c12, 4)
#define WCP14_DBGBVR13(val)		MCR14(val, 0, c0, c13, 4)
#define WCP14_DBGBVR14(val)		MCR14(val, 0, c0, c14, 4)
#define WCP14_DBGBVR15(val)		MCR14(val, 0, c0, c15, 4)
#define WCP14_DBGBCR0(val)		MCR14(val, 0, c0, c0, 5)
#define WCP14_DBGBCR1(val)		MCR14(val, 0, c0, c1, 5)
#define WCP14_DBGBCR2(val)		MCR14(val, 0, c0, c2, 5)
#define WCP14_DBGBCR3(val)		MCR14(val, 0, c0, c3, 5)
#define WCP14_DBGBCR4(val)		MCR14(val, 0, c0, c4, 5)
#define WCP14_DBGBCR5(val)		MCR14(val, 0, c0, c5, 5)
#define WCP14_DBGBCR6(val)		MCR14(val, 0, c0, c6, 5)
#define WCP14_DBGBCR7(val)		MCR14(val, 0, c0, c7, 5)
#define WCP14_DBGBCR8(val)		MCR14(val, 0, c0, c8, 5)
#define WCP14_DBGBCR9(val)		MCR14(val, 0, c0, c9, 5)
#define WCP14_DBGBCR10(val)		MCR14(val, 0, c0, c10, 5)
#define WCP14_DBGBCR11(val)		MCR14(val, 0, c0, c11, 5)
#define WCP14_DBGBCR12(val)		MCR14(val, 0, c0, c12, 5)
#define WCP14_DBGBCR13(val)		MCR14(val, 0, c0, c13, 5)
#define WCP14_DBGBCR14(val)		MCR14(val, 0, c0, c14, 5)
#define WCP14_DBGBCR15(val)		MCR14(val, 0, c0, c15, 5)
#define WCP14_DBGWVR0(val)		MCR14(val, 0, c0, c0, 6)
#define WCP14_DBGWVR1(val)		MCR14(val, 0, c0, c1, 6)
#define WCP14_DBGWVR2(val)		MCR14(val, 0, c0, c2, 6)
#define WCP14_DBGWVR3(val)		MCR14(val, 0, c0, c3, 6)
#define WCP14_DBGWVR4(val)		MCR14(val, 0, c0, c4, 6)
#define WCP14_DBGWVR5(val)		MCR14(val, 0, c0, c5, 6)
#define WCP14_DBGWVR6(val)		MCR14(val, 0, c0, c6, 6)
#define WCP14_DBGWVR7(val)		MCR14(val, 0, c0, c7, 6)
#define WCP14_DBGWVR8(val)		MCR14(val, 0, c0, c8, 6)
#define WCP14_DBGWVR9(val)		MCR14(val, 0, c0, c9, 6)
#define WCP14_DBGWVR10(val)		MCR14(val, 0, c0, c10, 6)
#define WCP14_DBGWVR11(val)		MCR14(val, 0, c0, c11, 6)
#define WCP14_DBGWVR12(val)		MCR14(val, 0, c0, c12, 6)
#define WCP14_DBGWVR13(val)		MCR14(val, 0, c0, c13, 6)
#define WCP14_DBGWVR14(val)		MCR14(val, 0, c0, c14, 6)
#define WCP14_DBGWVR15(val)		MCR14(val, 0, c0, c15, 6)
#define WCP14_DBGWCR0(val)		MCR14(val, 0, c0, c0, 7)
#define WCP14_DBGWCR1(val)		MCR14(val, 0, c0, c1, 7)
#define WCP14_DBGWCR2(val)		MCR14(val, 0, c0, c2, 7)
#define WCP14_DBGWCR3(val)		MCR14(val, 0, c0, c3, 7)
#define WCP14_DBGWCR4(val)		MCR14(val, 0, c0, c4, 7)
#define WCP14_DBGWCR5(val)		MCR14(val, 0, c0, c5, 7)
#define WCP14_DBGWCR6(val)		MCR14(val, 0, c0, c6, 7)
#define WCP14_DBGWCR7(val)		MCR14(val, 0, c0, c7, 7)
#define WCP14_DBGWCR8(val)		MCR14(val, 0, c0, c8, 7)
#define WCP14_DBGWCR9(val)		MCR14(val, 0, c0, c9, 7)
#define WCP14_DBGWCR10(val)		MCR14(val, 0, c0, c10, 7)
#define WCP14_DBGWCR11(val)		MCR14(val, 0, c0, c11, 7)
#define WCP14_DBGWCR12(val)		MCR14(val, 0, c0, c12, 7)
#define WCP14_DBGWCR13(val)		MCR14(val, 0, c0, c13, 7)
#define WCP14_DBGWCR14(val)		MCR14(val, 0, c0, c14, 7)
#define WCP14_DBGWCR15(val)		MCR14(val, 0, c0, c15, 7)
#define WCP14_DBGBXVR0(val)		MCR14(val, 0, c1, c0, 1)
#define WCP14_DBGBXVR1(val)		MCR14(val, 0, c1, c1, 1)
#define WCP14_DBGBXVR2(val)		MCR14(val, 0, c1, c2, 1)
#define WCP14_DBGBXVR3(val)		MCR14(val, 0, c1, c3, 1)
#define WCP14_DBGBXVR4(val)		MCR14(val, 0, c1, c4, 1)
#define WCP14_DBGBXVR5(val)		MCR14(val, 0, c1, c5, 1)
#define WCP14_DBGBXVR6(val)		MCR14(val, 0, c1, c6, 1)
#define WCP14_DBGBXVR7(val)		MCR14(val, 0, c1, c7, 1)
#define WCP14_DBGBXVR8(val)		MCR14(val, 0, c1, c8, 1)
#define WCP14_DBGBXVR9(val)		MCR14(val, 0, c1, c9, 1)
#define WCP14_DBGBXVR10(val)		MCR14(val, 0, c1, c10, 1)
#define WCP14_DBGBXVR11(val)		MCR14(val, 0, c1, c11, 1)
#define WCP14_DBGBXVR12(val)		MCR14(val, 0, c1, c12, 1)
#define WCP14_DBGBXVR13(val)		MCR14(val, 0, c1, c13, 1)
#define WCP14_DBGBXVR14(val)		MCR14(val, 0, c1, c14, 1)
#define WCP14_DBGBXVR15(val)		MCR14(val, 0, c1, c15, 1)
#define WCP14_DBGOSLAR(val)		MCR14(val, 0, c1, c0, 4)
#define WCP14_DBGOSSRR(val)		MCR14(val, 0, c1, c2, 4)
#define WCP14_DBGOSDLR(val)		MCR14(val, 0, c1, c3, 4)
#define WCP14_DBGPRCR(val)		MCR14(val, 0, c1, c4, 4)
#define WCP14_DBGITCTRL(val)		MCR14(val, 0, c7, c0, 4)
#define WCP14_DBGCLAIMSET(val)		MCR14(val, 0, c7, c8, 6)
#define WCP14_DBGCLAIMCLR(val)		MCR14(val, 0, c7, c9, 6)