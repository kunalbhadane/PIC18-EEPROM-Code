#include <xc.h>
#include "EEPROM.h"
#include "../UART/UART.h"

//Write data to an internal EEPROM
void WriteEEPROM(uint16_t address, uint8_t datas) {
	//To save INTCON register value as interrupts should be disabled while
	//writing to EEPROM
    uint8_t INTCON_SAVE;  
    EEADRH = address >> 8;
    EEADR = address & 0xFF; //Address to write
    EEDATA = datas; //Data to write
    EEPGD = 0; //Selecting EEPROM Data Memory
    CFGS = 0;
    WREN = 1; //Enable writing of EEPROM
    INTCON_SAVE = INTCON; //Backup INCON interrupt register
    INTCON = 0x00;
    EECON2 = 0x55; //Required sequence for write to internal EEPROM
    EECON2 = 0xAA; //Required sequence for write to internal EEPROM
    WR = 1; //Initiate write cycle
    INTCON = INTCON_SAVE; //Enables Interrupt
    WREN = 0; //To disable write
    while (EEIF == 0)//Checking for completion of write operation
    {
      __nop();
    }
    EEIF = 0; //Clearing EEIF bit
}

//Read data from EEPROM
uint8_t ReadEEPROM(uint16_t address) {
    EEADRH = address >> 8;
    EEADR = address & 0xFF; //Address to be read
    EEPGD = 0; //Selecting EEPROM Data Memory
    CFGS = 0;
    RD = 1; //Initiate read cycle
    return EEDATA; //Returning data
}

//-----------------------------------------------------------------------------

void Rom_Write(signed long int x1, uint16_t y) {

//Create union to store signed long int (4 bytes)

    union {
        signed long int long_int;
        uint8_t b[4];
    } longbreak;
    uint8_t checksum;
    uint8_t a1 = 0;
    longbreak.long_int = x1; 
	
	checksum = longbreak.b[0] + longbreak.b[1] + longbreak.b[2] + longbreak.b[3];

    for (a1 = 0; a1 < 4; a1++) {
        WriteEEPROM(y + a1, longbreak.b[a1]);
        __delay_ms(10);

    }
    WriteEEPROM(y + 4, checksum);
    __delay_ms(10);
}

signed long int Rom_Read(uint16_t y) {

    union {
        signed long int long_int;
        uint8_t read[4];
    } lj;
    uint8_t checksum, chsum;
    uint8_t aa1 = 0;

    for (aa1 = 0; aa1 < 4; aa1++) {
        lj.read[aa1] = ReadEEPROM(y + aa1);
        __delay_ms(5);

    }
    checksum = ReadEEPROM(y + 4);
    __delay_ms(5);

    chsum = lj.read[0] + lj.read[1] + lj.read[2] + lj.read[3];


    if (chsum != checksum)
        return 0;
    else
        return lj.long_int;
}

