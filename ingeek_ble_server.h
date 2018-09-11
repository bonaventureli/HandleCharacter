#ifndef INGEEK_BLE_SERVER_H
#define INGEEK_BLE_SERVER_H
/* auth: andy
 * version:0.2
 * change time: 2018/9/11
 * */

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
 #include "digital_key_api.h"

 #include <string.h>
/*********************************************************************
 * CONSTANTS
 */
#define SUCCESS 0

/*********************************************************************
 * TYPEDEFS
 */
 
typedef signed   char      int8_t;
typedef signed   short     int16_t;
typedef signed   long      int32_t;
typedef signed   long long int64_t;

typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned long      uint32_t;
typedef unsigned long long uint64_t;


 typedef struct{
	uint8_t	Header;
	uint8_t	Address;
	uint16_t Operate;
	uint16_t UUID;
	uint8_t	Datalength;
	uint8_t	*dataload;
	uint16_t	FCS;
	
	uint8_t	dataload_len;
}T_UartFrame;

 typedef struct{
	uint8_t	Header;
	uint8_t	Address;
	uint16_t Operate;
	uint16_t UUID;
	uint8_t	Datalength;
	uint8_t	dataload[30];
	uint8_t	FCS0;
	uint8_t	FCS1;
}T_UartSendInfo;

 typedef struct{
	uint8_t	Header;
	uint8_t	Address;
	uint16_t Operate;
	uint16_t UUID;
	uint8_t	Datalength;
	uint8_t	dataload[1];
	uint8_t	FCS0;
	uint8_t	FCS1;
}T_UartSendAuth;

 typedef struct{
	uint8_t	Header;
	uint8_t	Address;
	uint16_t Operate;
	uint16_t UUID;
	uint8_t	Datalength;
	uint8_t	dataload[113];
	uint8_t	FCS0;
	uint8_t	FCS1;
}T_UartSendSession;

 typedef struct{
	uint8_t	Header;
	uint8_t	Address;
	uint16_t Operate;
	uint16_t UUID;
	uint8_t	Datalength;
	uint8_t	dataload[16];
	uint8_t	FCS0;
	uint8_t	FCS1;
}T_UartSendCmd;

typedef enum {
    e_Header,
    e_Address,
    e_Operate,
    e_UUID,
	e_Datelength,
}Frame_status;

 
typedef enum {
    CARINFO_VALID,
    READ_INFO,
    WRITE_AUTH,
    WRITE_SESSION,
    CARINFO_INVALID=0xFF
}MCU_status;

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * API FUNCTIONS
 ********************************************************************/
uint8_t Handle_Character_A( uint8_t *Data, uint32_t frame_len);
void Handle_info(uint8_t *data, uint32_t data_len);
void Handle_auth(uint8_t *data, uint32_t data_len);
void Handle_session(uint8_t *data, uint32_t data_len);
void Handle_cmd(uint8_t *data, uint32_t data_len);
void Handle_active();
void Handle_disconnect();
void Handle_broadcast();
/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif
#endif /* INGEEK_BLE_SERVER_H */
