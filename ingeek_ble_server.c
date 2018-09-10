/*********************************************************************
 * @fn      
 *
 * @brief   Process a pending Simple Profile characteristic value change
 *          event.
 *
 * @param   
 * @version 0.1
 * @return  None.
 */
 
 /*Header        Address Operate UUID    Datalength      dataload        FCS
 * 1Byte        1Byte   2Byte   2Byte   1Byte           xxxByte         2Byte
 * 0x7E
 * Address      ble->mcu 0x01
 * */
#define UART_HEADER     0x7E
#define UART_BLETOMCU   0x01
#define UART_MCUTOBLE   0x10
#define UART_OPERATE_BLE_BROADCAST      0x0101
#define UART_OPERATE_BLE_CONNECT        0x0201
#define UART_OPERATE_BLE_UNCONNECT      0x0301
#define UART_OPERATE_CHARACTER_NOTIFY   0x1002
#define INFO_UUID       0xFFF2
#define AUTH_UUID       0xFFF3
#define SESSION_UUID    0xFFF4
#define CMD_UUID        0xFFF5

 typedef struct{
	uint8_t	Header;
	uint8_t	Address;
	uint16_t Operate;
	uint16_t UUID;
	uint8_t	Datalength;
	uint8_t	dataload[Datalength];
	uint16_t	FCS;
}T_UartFrame;

typedef enum {
    e_Header,
    e_Address,
    e_Operate,
    e_UUID,
	e_Datelength,
}Frame_status;

uint8_t gDataload[200];

 
#include "ingeek_ble_server.h"
#include "rscan.h"

uint8_t gSendata[200] = {0};
uint8_t gmemorySendata[200] = {0};
uint8_t ret_ingeek[10];
/* Create Can_FrameType for send and receive data */
    const Can_FrameType CANOPenDoor={
    //CiTBpA
    0x18,
    0,
    0,
    0,        
    
    //CiTBpB
    0x0000,                            
    0x000,                            
    0x8,    
    
    {
    0x12,                            //DB0
    0x34,                            //DB1
    0x56,                            //DB2
    0x78,                            //DB3
    //CiTBpD
    0x87,                            //DB4
    0x65,                            //DB5
    0x43,                            //DB6
    0x21                             //DB7
    }
    };
const Can_FrameType CANCloseDoor={
    //CiTBpA
    0x18,
    0,
    0,
    0,        
    
    //CiTBpB
    0x0000,                            
    0x000,                            
    0x8,    
    
    {
    0x12,                            //DB0
    0x34,                            //DB1
    0x56,                            //DB2
    0x78,                            //DB3
    //CiTBpD
    0x87,                            //DB4
    0x65,                            //DB5
    0x43,                            //DB6
    0x21                             //DB7
    }
    };
		
/*
* Function:    Handle_Character_A
* Description:        
* Parameter:   Data  frame_len
* Return:      uint8_t
* auther: lifei 
* change time£º2018/9/10
*/
uint8_t Handle_Character_A( uint8_t *Data, uint32_t frame_len)
{
	T_UartFrame *Framedata;
	Framedata->Datalength = 1;
	Framedata = (T_UartFrame *) Data;

	if(Framedata->Header == UART_HEADER){
		if(Framedata->Address == UART_BLETOMCU){
			switch (Framedata->Operate){
				case UART_OPERATE_BLE_BROADCAST:{
					 Handle_broadcast();
					 break;
				}
				case UART_OPERATE_BLE_UNCONNECT:{
					Handle_disconnect();
					break;
				}
				case UART_OPERATE_CHARACTER_NOTIFY:{
					switch (Framedata->UUID){
						case INFO_UUID:
						{
							T_UartFrame *Framedata;
							Framedata->Datalength = 0x48;
							Framedata = (T_UartFrame *) Data;
							Handle_info(Framedata->dataload,Framedata->Datalength);
							break;
						}
							case AUTH_UUID:
						{
							T_UartFrame *Framedata;
							Framedata->Datalength = 55;
							Framedata = (T_UartFrame *) Data;
							Handle_auth(Framedata->dataload,Framedata->Datalength);
							break;
						}
						case SESSION_UUID:
						{
							T_UartFrame *Framedata;
							Framedata->Datalength = 112;
							Framedata = (T_UartFrame *) Data;
							Handle_session(Framedata->dataload,Framedata->Datalength);
							break;
						}
						case CMD_UUID:
						{
							T_UartFrame *Framedata;
							Framedata->Datalength = 16;
							Framedata = (T_UartFrame *) Data;
							Handle_cmd(Framedata->dataload,Framedata->Datalength);
							break;
						}
						default:
						break;
						}
					break;
					}
			}
		}
	}
return ;	
}

uint8_t Handle_info(uint8_t *data, uint32_t data_len)
{
	T_UartFrame Framedatainfo;
	
	uint32_t outlen;
	uint8_t *preply_data;
	
	preply_data = gDataload;
	
	if(ingeek_push_info(data, data_len) != SUCCESS){
		return;
	  }
	  else{
		  if(ingeek_get_sec_status() == CARINFO_VALID){
			if(ingeek_pull_info(preply_data, &outlen) != SUCCESS){
				return;
			}
			else{
				if((outlen != 29) ){
					return;
				}
				else{
					Framedatainfo.Header = 0x7E;
					Framedatainfo.Address = 0x10;
					Framedatainfo.Operate = 0x1002;
					Framedatainfo.UUID = 0xFFF2;
					Framedatainfo.dataload[0] = 0x00;
					memcpy(&(Framedatainfo.dataload[1]),preply_data,outlen);
					Framedatainfo.Datalength = (uint8_t)outlen+1;
					Framedatainfo.FCS = 0xFFFF;
				}
			}
		}
		if(ingeek_get_sec_status() != READ_INFO){
			return;
		}
		else{
			Uart3Sent(Framedatainfo,(Framedatainfo.Datalength)+9);	
		}
	  }
}
uint8_t Handle_auth(uint8_t *data, uint32_t data_len)
{
	T_UartFrame Framedataauth;
	if(ingeek_get_sec_status() == READ_INFO){
		if(ingeek_push_auth(data, data_len, (unsigned char*)1, (unsigned int*)1) != 0x0000){
		return;
		}	
		else{
		Framedataauth.Header = 0x7E;
		Framedataauth.Address = 0x10;
		Framedataauth.Operate = 0x1002;
		Framedataauth.UUID = 0xFFF1;
		Framedataauth.Datalength = 0x01;
		Framedataauth.dataload[0] = 0x02;
		Framedataauth.FCS = 0xFFFF;	
		Uart3Sent(Framedataauth,(Framedataauth.Datalength)+9);
		}
	}
}
uint8_t Handle_session(uint8_t *data, uint32_t data_len)
{
	T_UartFrame FramedataSession;
	uint32_t outlen;
	uint8_t *preply_data;
	preply_data = gDataload;
	
	if(ingeek_get_sec_status() == WRITE_AUTH){
	ingeek_push_session(data, data_len, preply_data, &outlen);
	if(ingeek_get_sec_status() != WRITE_SESSION){
	 return;
	 }
	 else{
		 if(outlen != 112){
			 return;
		 }
		 else{
			 FramedataSession.Header = 0x7E;
			 FramedataSession.Address = 0x10;
			 FramedataSession.Operate = 0x1002;
			 FramedataSession.UUID = 0xFFF4;
			 FramedataSession.Datalength = (uint8_t)outlen+1;
			 FramedataSession.dataload[0] = WRITE_SESSION;
			 memcpy(&(FramedataSession.dataload[1]),preply_data,outlen);
			 FramedataSession.FCS = 0xFFFF;
			 Uart3Sent(FramedataSession,FramedataSession.Datalength+9);
		 }
	 }
	 
}
uint8_t Handle_cmd(uint8_t *data, uint32_t data_len)
{
	T_UartFrame FramedataCmd;
	uint8_t *preply_data;
	preply_data = gDataload;

	DK_Cmd_Meg struct_cmd;
	uint8_t cmd;

	if(ingeek_command_input_action(data, data_len, &struct_cmd) == 0x0000){
	cmd = (uint8_t)(struct_cmd.command);
	Uart3Sent(&cmd,1);  
	MslCANSentFromSDK(cmd);	
	
	if(ingeek_command_output_action(&struct_cmd,preply_data, &outlen) != SUCCESS){
		return;
	}
	else{
		if(outlen != 16){
			return;
		}
		else{
			FramedataCmd.Header = 0x7E;
			FramedataCmd.Address = 0x10;
			FramedataCmd.Operate = 0x1002;
			FramedataCmd.UUID = 0xFFF5;
			FramedataCmd.Datalength = outlen;
			FramedataCmd.FCS = 0xFFFF;
			memcpy(FramedataCmd.dataload,preply_data,outlen);
			Uart3Sent(FramedataCmd,FramedataCmd.Datalength+9); 
		}
	}			
}

uint8_t Handle_active()
{
	T_UartFrame FramedataActive;
	
	uint8_t preply_data;
	uint32_t outlen;
	preply_data = gDataload;
	
	ingeek_se_final();
	ingeek_se_init();
	
	if(ingeek_pull_info(preply_data, &outlen) != SUCCESS){
		return;
	}
	else{
		if(outlen != 29){
			return;
		}
		else{
			FramedataActive.Header = 0x7E;
			FramedataActive.Address = 0x10;
			FramedataActive.Operate = 0x1002;
			FramedataActive.UUID = 0xFFF2;
			FramedataActive.Datalength = (uint8_t)outlen+1;
			FramedataActive.dataload[0] = 0x00;
			memcpy(&(FramedataActive.dataload[1]),preply_data,outlen);
			FramedataActive->FCS = 0xFFFF;
			Uart3Sent(FramedataActive,(FramedataActive.Datalength)+9);
		}
	}
}
			 
}
uint8_t Handle_disconnect()
{
	ingeek_se_final();
	ingeek_se_init();
	return;
}
uint8_t Handle_broadcast()
{
	uint32_t ret;
	ret = ingeek_get_sec_status();
	if(ret == 0x00FF){
		return 21;
	}
	if(((ret > 0)&&(ret < 0x00FF))||(ret == 0)){
		Handle_active();
	}
}