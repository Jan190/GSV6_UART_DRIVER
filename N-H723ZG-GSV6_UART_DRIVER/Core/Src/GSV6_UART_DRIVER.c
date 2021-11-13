/*
 * GSV6_UART_DRIVER.c
 *
 *  Created on: Nov 1, 2021
 *      Author: Jan
 */

#include "GSV6_UART_DRIVER.h"


uint8_t GSV6_Initialise(GSV6* device, UART_HandleTypeDef* UartHandle, uint8_t DataRate){

	/* Definiramo struct parametre*/
	device->UARTHandle = UartHandle;

	device->force_N[0]=0.0f;
	device->force_N[1]=0.0f;
	device->force_N[2]=0.0f;
	device->force_N[3]=0.0f;
	device->force_N[4]=0.0f;
	device->force_N[5]=0.0f;

	/* Varijable za spremanje broja errora (vracaju se nakon sto se izvrti cijela funkcija)*/
	uint8_t errNum = 0;
	HAL_StatusTypeDef status;

	/*Zaustavljanje slanja podataka s uređaja*/



	/*Postavljanje DataRate-a*/



	/*
	 * Postavljanje vrijednosti tara na vrijednost ulaza,
	 * kako bi izlazna vrijednost senzora bila nula
	 */


	return errNum;
}

/*
 * AKVIZICIJA PODATAKA I SLANJE NAREDBI
 */

HAL_StatusTypeDef GSV6_CmdWriteZero(GSV6* device){

	/*
	 * Definiramo poruku koju cemo poslati
	 * 1.Byte 	 = Prefix za zapocinanje poruke
	 * 2.Byte 	 = prva cetri bita definiraju koliko bytova podataka saljemo u ovom slucaju 5 (5=0101)
	 * 			   druga cetri bita definiraju da je u pitanju request putem serijske komunikacije (9 = 1001)
	 * 3.Byte 	 = Definiramo komandu koju cemo korisiti u poruci u ovom slucaju WriteZero
	 * 4.Byte	 = Definiramo na koji kanal zelimo posalti poruku. Kada je zadan Hex 0x00 znaci da saljemo na sve kanale
	 * 5-8.Byte  = Definiramo koju vrijednost Tara cemo staviti na definirani kanal/kanale koja je velicine int32_t.
	 * 			   U ovom slucaju ta vrijednost ce biti nula
	 * 9.Byte    = Suffix za zavrsavanje poruke
	 */
	uint8_t CmdMsg [9] = {GSV6_MSG_PREFIX, 0x95, GSV6_CMD_WRITEZERO, 0x00, 0x00, 0x00, 0x00, 0x00, GSV_MSG_SUFFIX};

	HAL_StatusTypeDef status = GSV6_CmdSend(device, CmdMsg, 9);

	return status;
}

HAL_StatusTypeDef GSV6_CmdSetZero(GSV6* device){

	/*
	 * Definiramo poruku koju cemo poslati
	 * 1.Byte 	 = Prefix za zapocinanje poruke
	 * 2.Byte 	 = prva cetri bita definiraju koliko bytova podataka saljemo u ovom slucaju 1 (1=0001)
	 * 			   druga cetri bita definiraju da je u pitanju request putem serijske komunikacije (9 = 1001)
	 * 3.Byte 	 = Definiramo komandu koju cemo korisiti u poruci u ovom slucaju SetZero
	 * 4.Byte    = Definiramo na koji kanal zelimo posalti poruku. Kada je zadan Hex 0x00 znaci da saljemo na sve kanale
	 * 5.Byte    = Suffix za zavrsavanje poruke
	 */
	uint8_t CmdMsg [5] = {GSV6_MSG_PREFIX, 0x91, GSV6_CMD_SETZERO, 0x00, GSV_MSG_SUFFIX};

	HAL_StatusTypeDef status = GSV6_CmdSend(device, CmdMsg, 5);

	return status;
}

HAL_StatusTypeDef GSV6_CmdStopTransmission(GSV6* device){

	/*
	 * Definiramo poruku koju cemo poslati
	 * 1.Byte 	 = Prefix za zapocinanje poruke
	 * 2.Byte 	 = prva cetri bita definiraju koliko bytova podataka saljemo u ovom slucaju 0 (0=0000)
	 * 			   druga cetri bita definiraju da je u pitanju request putem serijske komunikacije (9 = 1001)
	 * 3.Byte 	 = Definiramo komandu koju cemo korisiti u poruci u ovom slucaju StopTransmission
	 * 4.Byte    = Suffix za zavrsavanje poruke
	 */
	uint8_t CmdMsg [4] = {GSV6_MSG_PREFIX, 0x90, GSV6_CMD_STOPTRANSMISSION, GSV_MSG_SUFFIX};

	HAL_StatusTypeDef status = GSV6_CmdSend(device, CmdMsg, 4);

	return status;
}

HAL_StatusTypeDef GSV6_CmdStartTransmission(GSV6* device){

	/*
	 * Definiramo poruku koju cemo poslati
	 * 1.Byte 	 = Prefix za zapocinanje poruke
	 * 2.Byte 	 = prva cetri bita definiraju koliko bytova podataka saljemo u ovom slucaju 0 (0=0000)
	 * 			   druga cetri bita definiraju da je u pitanju request putem serijske komunikacije (9 = 1001)
	 * 3.Byte 	 = Definiramo komandu koju cemo korisiti u poruci u ovom slucaju StartTransmission
	 * 4.Byte    = Suffix za zavrsavanje poruke
	 */
	uint8_t CmdMsg [4] = {GSV6_MSG_PREFIX, 0x90, GSV6_CMD_STARTTRANSMISSION, GSV_MSG_SUFFIX};

	HAL_StatusTypeDef status = GSV6_CmdSend(device, CmdMsg, 4);

	return status;
}

uint8_t GSV6_CmdGetValue(GSV6* device){

	/*
	 * Definiramo poruku koju cemo poslati
	 * 1.Byte 	 = Prefix za zapocinanje poruke
	 * 2.Byte 	 = prva cetri bita definiraju koliko bytova podataka saljemo u ovom slucaju 0 (0=0000)
	 * 			   druga cetri bita definiraju da je u pitanju request putem serijske komunikacije (9 = 1001)
	 * 3.Byte 	 = Definiramo komandu koju cemo korisiti u poruci u ovom slucaju GetValue
	 * 4.Byte    = Suffix za zavrsavanje poruke
	 */
	uint8_t CmdMsg [4] = {GSV6_MSG_PREFIX, 0x90, GSV6_CMD_GETVALUE, GSV_MSG_SUFFIX};

	/*
	 * Definiranje potrebnih varijabli odnosno buffera za zaprimanje poruke, polje za pretvaranje 4 byte u jednu varijablu i varijabla
	 * za količinu nastalih errora
	 */
	uint8_t MsgRec [28];
	uint32_t ForceDataRaw [6];
	uint8_t errNum =0;


	HAL_StatusTypeDef status = GSV6_CmdSend(device, CmdMsg, 4);

	HAL_StatusTypeDef status1 = GSV6_CmdRead(device, MsgRec, 28);

	/* Očitavanje zaprimljene poruke i provjera je li stigla cijela poruka prije njeznog ocitanja*/
	if(MsgRec[0] == 0xAA && MsgRec[27] == 0x85){

		ForceDataRaw [0] = MsgRec[6] | (MsgRec[5]<<8) | (MsgRec[4]<<16) | (MsgRec[3]<<24);
		ForceDataRaw [1] = MsgRec[10] | (MsgRec[9]<<8) | (MsgRec[8]<<16) | (MsgRec[7]<<24);
		ForceDataRaw [2] = MsgRec[14] | (MsgRec[13]<<8) | (MsgRec[12]<<16) | (MsgRec[11]<<24);
		ForceDataRaw [3] = MsgRec[18] | (MsgRec[17]<<8) | (MsgRec[16]<<16) | (MsgRec[15]<<24);
		ForceDataRaw [4] = MsgRec[22] | (MsgRec[21]<<8) | (MsgRec[20]<<16) | (MsgRec[19]<<24);
		ForceDataRaw [5] = MsgRec[26] | (MsgRec[25]<<8) | (MsgRec[24]<<16) | (MsgRec[23]<<24);
	}

	/*Provjera za svaki slucaj da je float velicine 32bita*/
	if(sizeof(float) == sizeof(uint32_t)){

		/*
		 * Pomocu funkcije memcpy kopiramo memoriju velicine float(32bita)
		 *  s adrese ForceDataRaw u adresu od device na mjesto force_N[0]
		 *  */
		memcpy(&device->force_N[0], &ForceDataRaw[0], sizeof(float));
		memcpy(&device->force_N[1], &ForceDataRaw[1], sizeof(float));
		memcpy(&device->force_N[2], &ForceDataRaw[2], sizeof(float));
		memcpy(&device->force_N[3], &ForceDataRaw[3], sizeof(float));
		memcpy(&device->force_N[4], &ForceDataRaw[4], sizeof(float));
		memcpy(&device->force_N[5], &ForceDataRaw[5], sizeof(float));

	}
	return errNum;
}

/*
 * LOW-LEVEL FUNKCIJE
 *
 */
HAL_StatusTypeDef GSV6_CmdSend(GSV6* device, uint8_t* CmdMsg, uint16_t Size){

	return HAL_UART_Transmit(device->UARTHandle, CmdMsg, Size, HAL_MAX_DELAY);

}


HAL_StatusTypeDef GSV6_CmdRead(GSV6* device, uint8_t* MsgRec,uint16_t Size ){

	return HAL_UART_Receive(device->UARTHandle, MsgRec, Size, HAL_MAX_DELAY);

}


HAL_StatusTypeDef GSV6_CmdReadConst(GSV6* device, uint8_t* MsgRec,uint16_t Size ){

	return HAL_UART_Receive_DMA(device->UARTHandle, MsgRec, Size);

}


