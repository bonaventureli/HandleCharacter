/*********************************************************************
 * @fn      
 *
 * @brief   Process a pending Simple Profile characteristic value change
 *          event.
 *
 * @param   
 * @version 0.2
 * @return  None.
 */
 
 /*Header        Address Operate UUID    Datalength      dataload        FCS
 * 1Byte        1Byte   2Byte   2Byte   1Byte           xxxByte         2Byte
 * 0x7E
 * Address      ble->mcu 0x01
 * */
#include "ingeek_ble_server.h"
#include "rscan.h"

#define UART_HEADER     0x7E
#define UART_BLETOMCU   0x01
#define UART_MCUTOBLE   0x10

#define UART_OPERATE_BLE_BROADCAST      0x0101
#define UART_OPERATE_BLE_CONNECT        0x0102
#define UART_OPERATE_BLE_UNCONNECT      0x0103
#define UART_OPERATE_CHARACTER_NOTIFY   0x0210

#define UART_SEND_OPERATE_CHARACTER_READ		0x0104
#define UART_RECEIVE_OPERATE_CHARACTER_READ		0x0204

#define UART_SEND_OPERATE_CHARACTER_WRITE		0x0108
#define UART_RECEIVE_OPERATE_CHARACTER_WRITE		0x0208

#define UART_SEND_OPERATE_CHARACTER_NOTIFY		0x0110
#define UART_RECEIVE_OPERATE_CHARACTER_NOTIFY		0x0210


#define INFO_UUID       0xF2FF
#define AUTH_UUID       0xF3FF
#define SESSION_UUID    0xF4FF
#define CMD_UUID        0xF5FF
#define CARSTATE_UUID        0xF6FF

//#define Datalength 1

uint8_t gDataload[200];

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
* change time 2018/9/11
*/
uint8_t Handle_Character_A( uint8_t *Data, uint32_t frame_len)
{
	T_UartFrame *Framedata;
	
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
							Handle_info(&Data[7],0x48);
							break;
						}
							case AUTH_UUID:
						{
							Handle_auth(&Data[7],0x37);
							break;
						}
						case SESSION_UUID:
						{
							Handle_session(&Data[7],0x70);
							break;
						}
						case CMD_UUID:
						{
							Handle_cmd(&Data[7],0x10);
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
return 0;	
}

void Handle_info(uint8_t *data, uint32_t data_len)
{
	T_UartSendInfo Framedatainfo;
	
	
	unsigned int outlen;
	uint8_t *preply_data;
	uint8_t tmpdata[150];
	
	preply_data = tmpdata;
	
	if(ingeek_push_info(data, data_len) != SUCCESS){
		return ;
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
					Framedatainfo.Operate = 0x0210;
					Framedatainfo.UUID = 0xF2FF;
					Framedatainfo.dataload[0] = 0x00;
					memcpy(&(Framedatainfo.dataload[1]),preply_data,outlen);
					Framedatainfo.Datalength = (uint8_t)outlen+1;
					Framedatainfo.FCS0 = 0xFF;
					Framedatainfo.FCS1 = 0xFF;
				}
			}
		}
		if(ingeek_get_sec_status() != READ_INFO){
			return;
		}
		else{
			Uart3Sent(&Framedatainfo,(Framedatainfo.Datalength)+9);	
		}
	  }
}
void Handle_auth(uint8_t *data, uint32_t data_len)
{
	T_UartSendAuth Framedataauth;
	if(ingeek_get_sec_status() == READ_INFO){
		if(ingeek_push_auth(data, data_len, (unsigned char*)1, (unsigned int*)1) != 0x0000){
		return;
		}	
		else{
		Framedataauth.Header = 0x7E;
		Framedataauth.Address = 0x10;
		Framedataauth.Operate = 0x0210;
		Framedataauth.UUID = 0xF1FF;
		Framedataauth.Datalength = 0x01;
		Framedataauth.dataload[0] = 0x02;
		Framedataauth.FCS0 = 0xFF;	
		Framedataauth.FCS1 = 0xFF;
		Uart3Sent(&Framedataauth,(Framedataauth.Datalength)+9);
		}
	}
}
void Handle_session(uint8_t *data, uint32_t data_len)
{
	T_UartSendSession FramedataSession;
	unsigned int outlen;
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
			 FramedataSession.Operate = 0x0210;
			 FramedataSession.UUID = 0xF4FF;
			 FramedataSession.Datalength = (uint8_t)outlen+1;
			 FramedataSession.dataload[0] = WRITE_SESSION;
			 memcpy(&(FramedataSession.dataload[1]),preply_data,outlen);
			 FramedataSession.FCS0 = 0xFF;
			 FramedataSession.FCS1 = 0xFF;
			 Uart3Sent(&FramedataSession,FramedataSession.Datalength+9);
		 }
	 }
	}
}
	 
void Handle_cmd(uint8_t *data, uint32_t data_len)
{
	T_UartSendCmd FramedataCmd;
	uint8_t *preply_data;
	unsigned int outlen;
	

	DK_Cmd_Meg struct_cmd;
	uint8_t cmd;

	preply_data = gDataload;
	
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
			FramedataCmd.Operate = 0x0210;
			FramedataCmd.UUID = 0xF5FF;
			FramedataCmd.Datalength = outlen;
			FramedataCmd.FCS0 = 0xFF;
			FramedataCmd.FCS1 = 0xFF;
			memcpy(FramedataCmd.dataload,preply_data,outlen);
			Uart3Sent(&FramedataCmd,FramedataCmd.Datalength+9); 
		}
	}			
}
}

void Handle_active()
{
	T_UartSendInfo FramedataActive;
	
	uint8_t *preply_data;
	unsigned int outlen;
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
			FramedataActive.Operate = 0x0210;
			FramedataActive.UUID = 0xF2FF;
			FramedataActive.Datalength = (uint8_t)outlen+1;
			FramedataActive.dataload[0] = 0x00;
			memcpy(&(FramedataActive.dataload[1]),preply_data,outlen);
			FramedataActive.FCS0 = 0xFF;
			FramedataActive.FCS1 = 0xFF;
			Uart3Sent(&FramedataActive,(FramedataActive.Datalength)+9);
		}
	}
}

void Handle_disconnect()
{
	ingeek_se_final();
	ingeek_se_init();
	return;
}
void Handle_broadcast()
{
	uint32_t ret;
	ret = ingeek_get_sec_status();
	if(ret == 0x00FF){
		return;
	}
	if(((ret > 0)&&(ret < 0x00FF))||(ret == 0)){
		Handle_active();
	}
}
