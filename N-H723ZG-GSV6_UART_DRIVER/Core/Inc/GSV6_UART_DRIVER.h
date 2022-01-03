/*
 * GSV6_UART_DRIVER.h
 *
 *  Created on: Nov 1, 2021
 *      Author: Jan
 */

#ifndef INC_GSV6_UART_DRIVER_H_
#define INC_GSV6_UART_DRIVER_H_

#include "stm32h7xx_hal.h" /* Potrebno header za UART za zadanu familiju STM32 mikorokontrolera*/
#include <string.h>
/*
 * DEFINICIJA PROTOKOLA (DATASHEET STRANICA 9,10)
 */

#define GSV6_MSG_PREFIX    0xAA
#define GSV_MSG_SUFFIX     0x85

/*
 * ERROR KODOVI (DATASHEET STRANICA 16,17)
 */

#define GSV6_ERR_OK                			 0x00 /*Command executed without errors*/
#define GSV6_ERR_OK_CHANGED           		 0x01 /*No error, but further device parameters changed*/

#define GSV6_ERR_CMD_NOTKNOWN   			 0x40 /*Command number unknown*/
#define GSV6_ERR_CMD_NOTIMPL   				 0x41 /*Command not implemented. This may be a command that is not supported by the device type connected.*/
#define GSV6_ERR_FRAME_ERROR  				 0x42 /*Frame error: Wrong suffix*/

#define GSV6_ERR_PAR   						 0x50 /*Parameter wrong*/
#define GSV6_ERR_PAR_ADR   					 0x51 /*Wrong index or address parameter*/
#define GSV6_ERR_PAR_DAT  					 0x52 /*Wrong data parameter*/
#define GSV6_ERR_PAR_BITS   				 0x53 /*Wrong bits inside parameter*/
#define GSV6_ERR_PAR_ABSBIG   				 0x54 /*Parameter absolutely too big*/
#define GSV6_ERR_PAR_ABSMALL   				 0x55 /*Parameter absolutely too small*/
#define GSV6_ERR_PAR_COMBI  				 0x56 /*Wrong parameter / setting combination*/
#define GSV6_ERR_PAR_RELBIG   				 0x57 /*Parameter too big in relation to other parameters / settings*/
#define GSV6_ERR_PAR_RELSMALL    			 0x58 /*Parameter too small in relation to other parameters /settings*/
#define GSV6_ERR_PAR_NOTIMPL   				 0x59 /*Function invoked by parameter is not implemented*/
#define GSV6_ERR_PAR_TIMEOUT   				 0x5A /*Command parameters have not been send within timeout (normally 200ms)*/
#define GSV6_ERR_WRONG_PAR_NUM  		     0x5B /*Wrong number of parameters in frame or parameter number descriptor in frame wrong.*/
#define GSV6_ERR_PAR_NOFIT_SETTINGS		     0x5C /*Parameter improper with respect to device's settings*/
#define GSV6_ERR_MEMORY_WRONG_COND   		 0x5E /*Function leads to hardware (connection) collision, e.g. short-circuit*/
#define GSV6_ERR_MEMORY_ACCESS_DENIED   	 0x5F /*Data requested not available (e.g. not initialized)*/

#define GSV6_ERR_ACC_DEN   					 0x70 /*Access denied*/
#define GSV6_ERR_ACC_BLK  					 0x71 /*Access denied, because write functions are blocked*/
#define GSV6_ERR_ACC_PWD   					 0x72 /*Access denied: Missing password/PIN*/
#define GSV6_ERR_ACC_MAXWR   				 0x74 /*Access denied: Maximum executions reached*/
#define GSV6_ERR_ACC_PORT   				 0x75 /*Access from this port denied (other port seems to have write access)*/

#define GSV6_ERR_INTERNA   					 0x80 /*Internal exception in device.  Please contact manufacturer*/
#define GSV6_ERR_ARITH   					 0x81 /*Internal arithmetic exception in device.  Please contact manufacturer*/
#define GSV6_ERR_INTER_ADC   				 0x82 /*Erratic behaviour of AD converter. Please contact manufacturer*/
#define GSV6_ERR_MWERT_ERR   				 0x83 /*Actual measuring value inappropriate to fulfil request*/
#define GSV6_ERR_EEPROM   					 0x84 /*Erratic behaviour of EEPROM memory. Please contact manufacturer*/

#define GSV6_ERR_RET_TXBUF   				 0x91 /*Device transmission buffer full*/
#define GSV6_ERR_RET_BUSY   				 0x92 /*Device too busy to execute request*/
#define GSV6_ERR_RET_RXBUF   				 0x99 /*Device receive buffer full*/



  /*
   * DEFINICIJA MOGUCIH FUNKCIJA
   */
#define GSV6_CMD_READZERO			0x02 /*(DATASHEET STRANICA 25)*/
#define GSV6_CMD_WRITEZERO			0X03 /*(DATASHEET STRANICA 25)*/
#define GSV6_CMD_SETZERO			0X0C /*(DATASHEET STRANICA 27)*/
#define GSV6_CMD_READUSERSCALE		0X14 /*(DATASHEET STRANICA 30)*/
#define GSV6_CMD_STOPTRANSMISSION	0X23 /*(DATASHEET STRANICA 35)*/
#define GSV6_CMD_STARTTRANSMISSION	0X24 /*(DATASHEET STRANICA 36)*/
#define GSV6_CMD_GETRAWVALUE		0X3A /*(DATASHEET STRANICA 41)*/
#define GSV6_CMD_GETVALUE			0X3B /*(DATASHEET STRANICA 41)*/
#define GSV6_CMD_RESETDEVICE		0x78 /*(DATASHEET STRANICA 67)*/
#define GSV6_CMD_READDATARATE		0X8A /*(DATASHEET STRANICA 72)*/
#define GSV6_CMD_WRITEDATARATE		0X8B /*(DATASHEET STRANICA 72)*/
#define GSV6_CMD_READUSEROFFSET		0X9A /*(DATASHEET STRANICA 76)*/
#define GSV6_CMD_WRITEUSEROFFSET	0X9B /*(DATASHEET STRANICA 77)*/

  /*
   * SENZOR STRUCT
   */

typedef struct{

	/* UART handle */
	UART_HandleTypeDef* UARTHandle;

	/*Varijabla za definiranje data rate*/
	uint8_t DataRate[4];

	/* Očitane sile s kanala (kanal1, kanal2, kanal3, kanal4, kanal5, kanal5) u N*/
	uint8_t force_N[6];

} GSV6;

  /*
   * INICIJALIZACIJA
   */

uint8_t GSV6_Initialise(GSV6* device, UART_HandleTypeDef* UARTHandle, uint16_t DRate); /*Potreban struct definiran iznad i UART handle*/

  /*
   * HIGH-LEVEL FUNKCIJE : AKVIZICIJA PODATAKA I SLANJE NAREDBI
   */

HAL_StatusTypeDef GSV6_CmdReadZero();
HAL_StatusTypeDef GSV6_CmdWriteZero(GSV6* device);
HAL_StatusTypeDef GSV6_CmdSetZero(GSV6* device);

HAL_StatusTypeDef GSV6_CmdReadUserScale();

HAL_StatusTypeDef GSV6_CmdStopTransmission(GSV6* device);
HAL_StatusTypeDef GSV6_CmdStartTransmission();

HAL_StatusTypeDef GSV6_CmdGetRawValue();
uint8_t GSV6_CmdGetValue(GSV6* device);

HAL_StatusTypeDef GSV6_CmdResetdevice();

HAL_StatusTypeDef GSV6_CmdReadDataRate(GSV6* device);
HAL_StatusTypeDef GSV6_CmdWriteDataRate(GSV6* device);

HAL_StatusTypeDef GSV6_CmdReadUserOffset();
HAL_StatusTypeDef GSV6_CmdWriteUserOffset();

  /*
   * LOW-LEVEL FUNKCIJE
   *
   */

HAL_StatusTypeDef GSV6_CmdSend(GSV6* device, uint8_t* CmdMsg, uint16_t Size);
HAL_StatusTypeDef GSV6_CmdRead(GSV6* device, uint8_t* MsgRec,uint16_t Size );
HAL_StatusTypeDef GSV6_CmdReadConst(GSV6* device, uint8_t* MsgRec,uint16_t Size );


#endif /* INC_GSV6_UART_DRIVER_H_ */
