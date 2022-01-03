/*
 * GSV6_UART_DRIVER.c
 *
 *  Created on: Nov 1, 2021
 *      Author: Jan
 */

#include "GSV6_UART_DRIVER.h"



/**
  *
  * @param  device je definirani structure u koji ce se upisati svi potrebni parametri za porektanja
  * 		svih funkcija unutar funkcije
  *
  * @param  UartHandle je pointer na UART_HandleTypeDef structure u kojem su sadrzane informacije
  * 		o konfiguraciji UART modula
  *
  * @param  DRate je varijabla kojom definiramo koliki cemo SPS (sample per second) imati u svojem ADC-u
  * 		Moguci izbor SPS. Lijevo se nalazi idealna vrijednost, a desno stvarna vrijednost ADC-a:
  *
  * 										 ------------------
  * 										|50Hz	| 49.9002Hz|
  * 										 ------------------
  * 										|100Hz	| 100.402Hz|
  * 										 ------------------
  * 										|150Hz	| 148.81Hz |
  * 										 ------------------
  * 										|300Hz	| 297.619Hz|
  * 										 ------------------
  * 										|500Hz	| 490.196Hz|
  * 										 ------------------
  * 										|600Hz	| 595.238Hz|
  * 										 ------------------
  * 										|700Hz	| 694.444Hz|
  * 										 ------------------
  * 										|750Hz	| 757.576Hz|
  * 										 ------------------
  *
  * Zbog samog CLK mikrokontrolera koji radi obradu podataka nije moguce izabrati cijeli broj nego decimalne kako je prikazno.
  * U trazeni parametar upisuje se cijeli broj (npr. 50Hz)
  *
  * @retval Broj errora (errNum)
  *
  */


uint8_t GSV6_Initialise(GSV6* device, UART_HandleTypeDef* UartHandle, uint16_t DRate){

	/* Definiramo struct parametre*/
	device->UARTHandle = UartHandle;

	device->force_N[0]=0.0f;
	device->force_N[1]=0.0f;
	device->force_N[2]=0.0f;
	device->force_N[3]=0.0f;
	device->force_N[4]=0.0f;
	device->force_N[5]=0.0f;

	/*Definiranu vrijednost DataRate potrebno je pospremiti kao najblizu mogucu vrijednost SPS Rate sto je dano u tablici iznad*/

	switch (DRate){
	case 50:
		device->DataRate[0] = 0x42;
		device->DataRate[1] = 0x47;
		device->DataRate[2] = 0x99;
		device->DataRate[3] = 0xCE;
		break ;
	case 100:
		device->DataRate[0] = 0x42;
		device->DataRate[1] = 0xC8;
		device->DataRate[2] = 0xCD;
		device->DataRate[3] = 0xD3;
		break ;
	case 150:
		device->DataRate[0] = 0x43;
		device->DataRate[1] = 0x14;
		device->DataRate[2] = 0xCF;
		device->DataRate[3] = 0x5C;
		break ;
	case 300:
		device->DataRate[0] = 0x43;
		device->DataRate[1] = 0x94;
		device->DataRate[2] = 0xCF;
		device->DataRate[3] = 0x3B;
		break ;
	case 500:
		device->DataRate[0] = 0x43;
		device->DataRate[1] = 0xF5;
		device->DataRate[2] = 0x19;
		device->DataRate[3] = 0x17;
		break ;
	case 600:
		device->DataRate[0] = 0x44;
		device->DataRate[1] = 0x14;
		device->DataRate[2] = 0xCF;
		device->DataRate[3] = 0x3B;
		break ;
	case 700:
		device->DataRate[0] = 0x44;
		device->DataRate[1] = 0x2D;
		device->DataRate[2] = 0x9C;
		device->DataRate[3] = 0x6A;
		break ;
	case 750:
		device->DataRate[0] = 0x44;
		device->DataRate[1] = 0x3D;
		device->DataRate[2] = 0x64;
		device->DataRate[3] = 0xDD;
		break ;

	/*U slucaju zadavanja krive vrijednosti DataRate postavlja se vrijednost od 750Hz*/
	default:
		device->DataRate[0] = 0x44;
		device->DataRate[1] = 0x3D;
		device->DataRate[2] = 0x64;
		device->DataRate[3] = 0xDD;
		break ;
	}

	/* Varijable za spremanje broja errora (vracaju se nakon sto se izvrsi cijela funkcija)*/
	uint8_t errNum = 0;
	HAL_StatusTypeDef status;

	/********		ZAUSTAVLJANJE SLANJA PORUKA		********/
	status = GSV6_CmdStopTransmission(device);
	errNum += (status != HAL_OK);
	status = 0;

	/********		POSTAVLJANJE DATARATE		*******/
	status = GSV6_CmdWriteDataRate(device);
	errNum += (status != HAL_OK);
	status = 0;

	/********		TARIRANJE(NULIRANJE) SVIH KANALA		********/
	status = GSV6_CmdSetZero(device);
	errNum += (status != HAL_OK);
	status = 0;

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
	HAL_StatusTypeDef status;


	status = GSV6_CmdSend(device, CmdMsg, 4);
	errNum += (status != HAL_OK);
	status = 0;

	status = GSV6_CmdRead(device, MsgRec, 28);
	errNum += (status != HAL_OK);
	status = 0;

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
		 *  s adrese ForceDataRaw u adresu od device na mjesto force_N[0]..
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

HAL_StatusTypeDef GSV6_CmdWriteDataRate(GSV6* device){

	/*
		 * Definiramo poruku koju cemo poslati
		 * 1.Byte 	 = Prefix za zapocinanje poruke
		 * 2.Byte 	 = prva cetri bita definiraju koliko bytova podataka saljemo u ovom slucaju 4 (5=0100)
		 * 			   druga cetri bita definiraju da je u pitanju request putem serijske komunikacije (9 = 1001)
		 * 3.Byte 	 = Definiramo komandu koju cemo korisiti u poruci u ovom slucaju WriteDataRate
		 * 4-7.Byte  = Definiramo koju vrijednost DataRate cemo staviti na definirani ADC convertor
		 * 8.Byte    = Suffix za zavrsavanje poruke
		 */
	uint8_t CmdMsg [8] = {GSV6_MSG_PREFIX, 0x94, GSV6_CMD_WRITEDATARATE, device->DataRate[0], device->DataRate[1], device->DataRate[2], device->DataRate[3], GSV_MSG_SUFFIX};

	HAL_StatusTypeDef status = GSV6_CmdSend(device, CmdMsg, 8);

	return status;

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


