// TI File $Revision: /main/2 $ 
// Checkin $Date: May 16, 2016 09:50:07 $
//###########################################################################

#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File
#include "includes.h"

// Prototype statements for functions found within this file.
interrupt void eCAN0_inta(void);
interrupt void cpu_timer0_isr(void);

Uint8 canSend(CAN_PORT notused,Message *m);
Uint8 canSendSet(UNS8 port,Message *m);
void canRevInit(void);
//void CANMessSet(UNS8 MBOX_PORT);

void InitLEDGPIO(void);   //定时器状态指示灯
void LEDOverTurn(void);
void delay_s(int i);
void setTimer(TIMEVAL value);
TIMEVAL getElapsedTime(void);

#ifndef NodeId
#define NodeId 0x01
#endif

// Global variable for this example
Uint32 Rec_l;
Uint32 Rec_h;
Uint32 Rec_ID;
Uint16 Tar_Pos_L = 0;
Uint16 Tar_Pos_H = 0;
Uint16 Pulse=100;
Uint16 counter=0;
extern Message Tm = Message_Initializer;
struct ECAN_REGS ECanaShadow;
extern TIMEVAL Last_Begin_Time;


void main(void)
{

	Message m;
	Uint16  i;

   InitSysCtrl();

   InitECanGpio();
   InitLEDGPIO();

// Step 3. Clear all interrupts and initialize PIE vector table:
// Disable CPU interrupts
   DINT;

// Disable CPU interrupts and clear all CPU interrupt flags:
   IER = 0x0000;
   IFR = 0x0000;

   InitPieVectTable();

     EALLOW;
     PieVectTable.ECAN0INTA = &eCAN0_inta;
     PieVectTable.TINT0 = &cpu_timer0_isr;
     EDIS;


    InitECana(); // Initialize eCAN-A module

    ECanaRegs.CANMD.all = 0xFFFF0000;
    ECanaRegs.CANME.all = 0x00000000;


    /*定时***********************/
    InitCpuTimers();
    ConfigCpuTimer(&CpuTimer0, 80, 2000);
    //ConfigCpuTimer(&CpuTimer0, 80 , 62500);
    CpuTimer0Regs.TCR.all = 0x4001; // Use write-only instruction to set TSS bit = 0

    //PieCtrlRegs.PIECTRL.bit.ENPIE = 1;          // Enable the PIE block   本来注释了，0604取消注释

    //EINT;   //开全局中断
    //ERTM;	//开实时中断
    /*定时***********************/

	canRevInit();  //接收初始化

	delay_s(1000);


	  setState(&ObjDict_Data, Initialisation);	// Init the state
	  setNodeId (&ObjDict_Data, 0x7F);
	  setState(&ObjDict_Data, Operational);		// Put the master in operational mode

	  /******配置从机*************/


	  delay_s(10);
	  masterSendNMTstateChange (&ObjDict_Data, 0x00, NMT_Reset_Comunication);

	  delay_s(300);

	  PDO_Onoff_Config(NodeId, 1 , 4);
	  delay_s(100);
	  PDO_Onoff_Config(NodeId, 0 , 1);
	  delay_s(100);
	  PDO_Onoff_Config(NodeId, 0 , 3);
	  delay_s(100);
	  PDO_Onoff_Config(NodeId, 0 , 4);
	  delay_s(100);


	  delay_s(1000);
      //Mapping RPDO1/RPDO2
	  //set huining mode
	  SDO_Mode_of_Operation(NodeId, 0x08);


	  //SDO_Velmode_Config(NodeId, 50000);

	  //change state

	  delay_s(100);
	  masterSendNMTstateChange (&ObjDict_Data, 0x01, NMT_Start_Node);

	  /******配置从机*************/

	  delay_s(1000);
	  //set PP mode statemachine
	  SDO_PPmode_Config_statemachine(NodeId);
		//masterSendNMTstateChange (&ObjDict_Data, 0x00, NMT_Start_Node);

//		delay_s(1000);
//		sendPDOevent (&ObjDict_Data);

	    IER |= M_INT1;
	    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;

		while(1)
		{

		}



}


//===========================================================================
// No more.
//===========================================================================


Uint8 canSend(CAN_PORT notused,Message *m)
{
   //hotpan 0521
	return canSendSet(0 , m);
}

Uint8 canSendSet(UNS8 port,Message *m)
{
   //hotpan 0521
	struct MBOX *pMB;

    ECanaRegs.CANME.all = 0x00000000;
	pMB = &(ECanaMboxes.MBOX0)+port*8;

	(*pMB).MSGID.bit.IDE = 0;
	(*pMB).MSGID.bit.AAM = 0;
	(*pMB).MSGID.bit.AME = 0;
	(*pMB).MSGID.bit.STDMSGID = m->cob_id;
	(*pMB).MSGID.bit.EXTMSGID_L = 0;
	(*pMB).MSGID.bit.EXTMSGID_H = 0;

	(*pMB).MSGCTRL.bit.DLC = 8;

	/*
	(*pMB).MDL.all = 0x9555AAA0;
	(*pMB).MDH.all = 0x89ABCDEF;
    */

	(*pMB).MSGCTRL.bit.DLC    = m->len;
	(*pMB).MDH.byte.BYTE7     = m->data[7];
	(*pMB).MDH.byte.BYTE6     = m->data[6];
	(*pMB).MDH.byte.BYTE5     = m->data[5];
	(*pMB).MDH.byte.BYTE4     = m->data[4];
	(*pMB).MDL.byte.BYTE3     = m->data[3];
	(*pMB).MDL.byte.BYTE2     = m->data[2];
	(*pMB).MDL.byte.BYTE1     = m->data[1];
	(*pMB).MDL.byte.BYTE0     = m->data[0];

	ECanaRegs.CANME.all = 0xFFFFFFFF;

    EALLOW;
    ECanaRegs.CANMIM.all = 0xFFFFFFFF;
    EDIS;

    ECanaRegs.CANTRS.all = 0x00000001<<port;  // Set TRS for all transmit mailboxes
    while(ECanaRegs.CANTA.all != 0x00000001<<port ) {}  // Wait for all TAn bits to be set..
    ECanaRegs.CANTA.all = 0x00000001<<port;   // Clear all TAn
    return 0;
}


void delay_s(int i)
{
   int j = 0;
	while(i--)
	{
		j = 1000;
		while(j--)
		{
		}
	}
}



void canRevInit(void)
{
    /*接收************************/
	 ECanaRegs.CANME.all = 0x00000000;
	 ECanaMboxes.MBOX16.MSGID.all = 0x40000000;
	 ECanaMboxes.MBOX16.MSGID.bit.STDMSGID = 0x000;    //NMT


	 ECanaMboxes.MBOX17.MSGID.all = 0x40100000;
	 ECanaMboxes.MBOX17.MSGID.bit.STDMSGID = 0x080;    //SYNC

	 ECanaMboxes.MBOX18.MSGID.all = 0x40200000;
	 ECanaMboxes.MBOX18.MSGID.bit.STDMSGID = 0x280 + NodeId;    //RPDO2

	 ECanaMboxes.MBOX19.MSGID.all = 0x40300000;
	 ECanaMboxes.MBOX19.MSGID.bit.STDMSGID = 0x600 + NodeId;      //RSDO

	 ECanaMboxes.MBOX16.MSGCTRL.bit.DLC = 8;
	 ECanaMboxes.MBOX16.MSGCTRL.bit.RTR = 0;

	 ECanaMboxes.MBOX17.MSGCTRL.bit.DLC = 8;
	 ECanaMboxes.MBOX17.MSGCTRL.bit.RTR = 0;

	 ECanaMboxes.MBOX18.MSGCTRL.bit.DLC = 8;
	 ECanaMboxes.MBOX18.MSGCTRL.bit.RTR = 0;

	 ECanaMboxes.MBOX19.MSGCTRL.bit.DLC = 8;
	 ECanaMboxes.MBOX19.MSGCTRL.bit.RTR = 0;

	 ECanaShadow.CANME.all = ECanaRegs.CANME.all;
	 ECanaShadow.CANME.bit.ME16 =1;
	 ECanaShadow.CANME.bit.ME17 =1;
	 ECanaShadow.CANME.bit.ME18 =1;
	 ECanaShadow.CANME.bit.ME19 =1;
	 ECanaRegs.CANME.all = ECanaShadow.CANME.all;

         EALLOW;
         ECanaRegs.CANMIM.all = 0xFFFFFFFF;

         ECanaRegs.CANGAM.all   = 0xFFFFFFFF;
         ECanaLAMRegs.LAM16.all = 0x00000000;                        //本地屏蔽
         ECanaLAMRegs.LAM17.all = 0x00000000;                        //本地屏蔽
         ECanaLAMRegs.LAM18.all = 0x00000000;                        //本地屏蔽
         ECanaLAMRegs.LAM19.all = 0x00000000;                        //本地屏蔽

         ECanaRegs.CANMIL.all = 0; //邮箱中断将产生在ECAN0INT
         ECanaRegs.CANGIF0.all = 0xFFFFFFFF;
         ECanaRegs.CANGIM.bit.I0EN = 1; //ECAN0INT中断请求线被使能
         EDIS;

         delay_s(50);

         PieCtrlRegs.PIECTRL.bit.ENPIE = 1;          // Enable the PIE block
         PieCtrlRegs.PIEIER9.bit.INTx5 = 1; //使能PIE中断
         IER |= M_INT9; //使能CPU中断
         EINT;   //开全局中断
         ERTM;	//开实时中断
    /*接收************************/
}

interrupt void eCAN0_inta(void)
{
	// Acknowledge this interrupt to get more from group 1
	unsigned long ulStatus=0;
	unsigned char i = 0;
	unsigned char len = 0;
	unsigned char rtr = 0;
    Message m = Message_Initializer;

    UNS32 IntNum = 0;
	struct MBOX *pMB;


    IntNum = ECanaRegs.CANRMP.all;

	DINT;
	while((IntNum != 0x00000000)&&(IntNum != 0x00010000)&&(IntNum != 0x00020000)&&(IntNum != 0x00040000)&&(IntNum != 0x00080000));

    //读取中断原因
	if(ECanaRegs.CANRMP.bit.RMP16 ==1) ulStatus = 7;
	if(ECanaRegs.CANRMP.bit.RMP17 ==1) ulStatus = 8;
	if(ECanaRegs.CANRMP.bit.RMP18 ==1) ulStatus = 9;
	if(ECanaRegs.CANRMP.bit.RMP19 ==1) ulStatus = 10;


	    switch (ulStatus)
	    {
	    	 case 7 :   //NMT
	    	 case 8 :   //SYNC
	    	 case 9 :   //RPDO2
	    	 case 10 :
	    		        pMB = &(ECanaMboxes.MBOX16)+(ulStatus-7);
	    		        len = (*pMB).MSGCTRL.bit.DLC;
	    		        rtr = (*pMB).MSGCTRL.bit.RTR;
	    		        if(len<5)
	    		        {
							for (i = len; i > 0; i--)
		    	 	        {
								m.data[i-1] = (*pMB).MDL.all >> i*8 ;
								m.data[i-1] = m.data[i] << 8 >> 8;
							}
	    		        }
	    		        else
	    		        {
	    		        	for (i = 4; i > 0; i--)
	    		        	{
								m.data[i-1] = (*pMB).MDL.all >> (4-i)*8 ;
								m.data[i-1] = m.data[i-1] << 8 >> 8;
	    		        	}

	    		        	for (i = len; i > 4; i--)
	    		        	{
	    		        		m.data[i-1] = (*pMB).MDH.all >> (8-i)*8 ;
								m.data[i-1] = m.data[i-1] << 8 >> 8;
	    		        	}

	    		        }

	    		        Tar_Pos_L = m.data[0]+m.data[1]*256;
	    		        Tar_Pos_H = m.data[2]+m.data[3]*256;

	    		        if(Tar_Pos_L!=0)
	    		        {
	    		     	   counter++;
	    		     	   if(counter==10)
	    		     	   {
	    		     		   if(Pulse<2000)
	    		     			   Pulse+=20;
	    		     		   else
	    		     			   Pulse=2000;
	    		     		   counter=0;
	    		     	   }

	    		        if(Tar_Pos_L>(65535-Pulse))  Tar_Pos_H++;
	    		        PDO_send(NodeId,Tar_Pos_L+Pulse,Tar_Pos_H);
	    		        }

						m.len = len;
						m.rtr = rtr;
						m.cob_id = (*pMB).MSGID.bit.STDMSGID;
	    	            //canDispatch( &ObjDict_Data,  &m);
						break;
	    	 default :  ;//CANStatusGet(CAN0_BASE, CAN_STS_CONTROL);

	    }

	ECanaRegs.CANRMP.all = 0x000F0000; //复位RMP标志，同时也复位中断标志

	PieCtrlRegs.PIEACK.bit.ACK9 = 1;
    EINT;
}

void InitLEDGPIO(void)
{
   EALLOW;
   GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 0;
   GpioCtrlRegs.GPADIR.bit.GPIO0 = 1;
   GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 0;
   GpioCtrlRegs.GPADIR.bit.GPIO1 = 1;
   GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 0;
   GpioCtrlRegs.GPADIR.bit.GPIO2 = 1;
   GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 0;
   GpioCtrlRegs.GPADIR.bit.GPIO3 = 1;
   EDIS;
}

void LEDOverTurn(void)
{
   GpioDataRegs.GPATOGGLE.bit.GPIO0 = 1;
   GpioDataRegs.GPATOGGLE.bit.GPIO1 = 1;
   GpioDataRegs.GPATOGGLE.bit.GPIO2 = 1;
   GpioDataRegs.GPATOGGLE.bit.GPIO3 = 1;
}

interrupt void cpu_timer0_isr(void)
{
   DINT;
   CpuTimer0.InterruptCount++;

   // Acknowledge this interrupt to receive more interrupts from group 1
   PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
   //LEDOverTurn();

  //TimeDispatch();

   SYNC_send();

   EINT;
}

void setTimer(TIMEVAL value)
{
	//TimerLoadSet(TIMER0_BASE, TIMER_A, value);  //硬件相关，需重写
    ConfigCpuTimer(&CpuTimer0, 80 , value);
	   CpuTimer0Regs.TCR.all = 0x4001;
}


TIMEVAL getElapsedTime(void)
{

	/********************硬件相关，需重写*******************
	TIMEVAL current = TimerValueGet(TIMER0_BASE, TIMER_A);
	//return (current - Last_Begin_Time);
	return (Last_Begin_Time - current); //因为是向下计数的
	 *******************硬件相关，需重写********************/

	TIMEVAL current = (CpuTimer0Regs.TIM.all)/80;
	//TIMEVAL current = (CpuTimer0Regs.TIM.all)/1280;
	return (Last_Begin_Time - current); //因为是向下计数的

}
