/*
 * user.c
 *
 *  Created on: 2016Äê6ÔÂ11ÈÕ
 *      Author: HotPan
 */


#include "DSP28x_Project.h"
#include "canfestival.h"


/*************************************
 *Dis-enable a particular PDO
 * RorT:0 represent RPDO, 1 represent TPDO
 * Num: 1,2,3,4
 *hotpan 20160611 */
void SDO_DisEn_PDO(Uint8 NodeID, Uint8 RorT, Uint8 PDONum)
{
	Message m = {0x600+NodeID,0,8,{0x2F,PDONum-1,0,0,0,0,0,0}};

	if(RorT == 0)
		m.data[2] = 0x16;
	else
		m.data[2] = 0x1A;

	canSend(NULL,&m);

}

/*************************************
 *Enable a particular PDO
 * RorT:0 represent RPDO, 1 represent TPDO
 * Num: 1,2,3,4
 *hotpan 20160611 */
void SDO_En_PDO(Uint8 NodeID, Uint8 RorT, Uint8 PDONum, Uint8 ObjNum)
{
	Message m = {0x600+NodeID,0,8,{0x2F,PDONum-1,0,0,ObjNum,0,0,0}};

	if(RorT == 0)
		m.data[2] = 0x16;
	else
		m.data[2] = 0x1A;

	canSend(NULL,&m);

}

/*************************************
 *Config a particular PDO
 * RorT:0 represent RPDO, 1 represent TPDO
 * PDONum: 1,2,3,4   BitNum:Bits to be transfered OD_addr:mapping addr
 *hotpan 20160611 */
void SDO_Config_PDO(Uint8 NodeID, Uint8 RorT, Uint8 PDONum,Uint8 SubIndex, Uint8 BitNum, Uint16 OD_addr)
{
	Message m = {0x600+NodeID,0,8,{0x23,PDONum-1,0,SubIndex,BitNum,0,0,0}};

	if(RorT == 0)
		m.data[2] = 0x16;
	else
		m.data[2] = 0x1A;

	m.data[6] = OD_addr<<8>>8;
	m.data[7] = OD_addr>>8;

	canSend(NULL,&m);
}

/*************************************
 *Config Operation Mode
 * Mode: 0 NOP Mode 1:PPmode 3:Velocitiy mode 6:Homeing mode
 *hotpan 20160618*/
void SDO_Mode_of_Operation(Uint8 NodeID, Uint8 Mode)
{
	Message m = {0x600+NodeID,0,8,{0x2F,0x60,0x60,0x00,Mode,0,0,0}};

	canSend(NULL,&m);
}

/*************************************
 *Config PP Operation Mode StateMachine
 * Mode: 0 NOP Mode 1:PPmode 3:Velocitiy mode 6:Homeing mode
 *hotpan 20160618*/
void SDO_PPmode_Config_statemachine(Uint8 NodeID)
{
	Message m1 = {0x600+NodeID,0,8,{0x2B,0x40,0x60,0x00,0x80,0,0,0}};
	canSend(NULL,&m1);

	Message m2 = {0x600+NodeID,0,8,{0x2B,0x40,0x60,0x00,0x06,0,0,0}};
	canSend(NULL,&m2);

	Message m3 = {0x600+NodeID,0,8,{0x2B,0x40,0x60,0x00,0x0F,0,0,0}};
	canSend(NULL,&m3);
}


void SDO_Velmode_Config(Uint8 NodeID, Uint16 VelData)
{
	Message m = {0x600+NodeID,0,8,{0x23,0xFF,0x60,0,0,0,0,0}};

	m.data[4] = VelData;
	m.data[5] = VelData>>8;

	canSend(NULL,&m);
}


void RPDO_PPmode_PV(Uint8 NodeID, Uint16 PosData, Uint16 VelData)
{
	Message m1 = {0x300+NodeID,0,8,{0x50,0xC3,0,0,0,0,0,0}};

	/*
	m1.data[0] = PosData;
	m1.data[1] = PosData>>8;

	m1.data[2] = PosData>>16;
	m1.data[3] = PosData>>32; */

	m1.data[4] = VelData;
	m1.data[5] = VelData>>8;
	/*
	m1.data[6] = VelData>>16;
	m1.data[7] = VelData>>32;
*/
	canSend(NULL,&m1);

	Message m2 = {0x200+NodeID,0,2,{0x0F,0x00,0,0,0,0,0,0}};
	canSend(NULL,&m2);

	Message m3 = {0x200+NodeID,0,2,{0x1F,0x00,0,0,0,0,0,0}};
	canSend(NULL,&m3);

	Message m4 = {0x200+NodeID,0,2,{0x0F,0x00,0,0,0,0,0,0}};
	canSend(NULL,&m4);

	Message m5 = {0x200+NodeID,0,2,{0x5F,0x00,0,0,0,0,0,0}};
	canSend(NULL,&m5);

	Message m6 = {0x200+NodeID,0,2,{0x0F,0x00,0,0,0,0,0,0}};
	canSend(NULL,&m6);
}


void PDO_Onoff_Config(Uint8 NodeID, Uint8 RorT, Uint8 PDOnum)
{
	Message m = {0x600+NodeID,0,8,{0x23,PDOnum-1,0x14,1,0,0,0,0x80}};

	if(RorT == 0)
	{
		m.data[2] = 0x14;
		m.data[4]=0x01;
		m.data[5]=PDOnum+1;
	}
	else
	{
		m.data[2] = 0x18;
		m.data[4]=0x81;
		m.data[5]=PDOnum;
	}
	canSend(NULL,&m);
}

void PDO_send(Uint8 NodeID,Uint16 Pos_L,Uint16 Pos_H)
{
	Message m = {0x300+NodeID,0,8,{0,0,0,0,0,0,0,2}};

	m.data[0] = Pos_L<<8>>8;
	m.data[1] = Pos_L>>8;

	m.data[2] = Pos_H<<8>>8;
	m.data[3] = Pos_H>>8;

	canSend(NULL,&m);
}

void SYNC_send()
{
	Message m = {0x80,0,0,{0,0,0,0,0,0,0,0}};

	canSend(NULL,&m);
}
