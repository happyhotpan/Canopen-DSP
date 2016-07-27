/*
 * user.h
 *
 *  Created on: 2016Äê6ÔÂ11ÈÕ
 *      Author: HotPan
 */

#ifndef INCLUDE_USER_H_
#define INCLUDE_USER_H_


#define RPDO1_adr 0x1600
#define RPDO2_adr 0x1601
#define RPDO3_adr 0x1602
#define RPDO4_adr 0x1603

#define TPDO1_adr 0x1A00
#define TPDO2_adr 0x1A01
#define TPDO3_adr 0x1A02
#define TPDO4_adr 0x1A03


void SDO_DisEn_PDO(Uint8 NodeID, Uint8 RorT, Uint8 PDONum);

void SDO_Mode_of_Operation(Uint8 NodeID, Uint8 Mode);

void SDO_PPmode_Config_statemachine(Uint8 NodeID);

void SDO_Velmode_Config(Uint8 NodeID, Uint16 VelData);

void SDO_En_PDO(Uint8 NodeID, Uint8 RorT, Uint8 PDONum, Uint8 ObjNum);

void SDO_Config_PDO(Uint8 NodeID, Uint8 RorT, Uint8 PDONum,Uint8 SubIndex, Uint8 BitNum, Uint16 OD_addr);

void RPDO_PPmode_PV(Uint8 NodeID, Uint16 PosData, Uint16 VelData);

void PDO_Onoff_Config(Uint8 NodeID, Uint8 RorT, Uint8 PDOnum);

void PDO_send(Uint8 NodeID,Uint16 Pos_L,Uint16 Pos_H);

void SYNC_send();

#endif /* INCLUDE_USER_H_ */
