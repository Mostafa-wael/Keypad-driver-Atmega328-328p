#ifndef KeyPad_4X4
#define KeyPad_4X4

#ifndef F_CPU
	#define F_CPU 16000000UL
#endif

#include "DIO_def.h"
#include <util/delay.h>

#ifndef port
#define port A
#endif
#if port == A
#define KEY_DDR  DDRA_reg
#define KEY_PORT PORTA_reg
#define KEY_PIN  PINA_reg
#elif port == B
#define KEY_DDR  DDRB_reg
#define KEY_PORT PORTB_reg
#define KEY_PIN  PINB_reg
#elif port == C
#define KEY_DDR  DDRC_reg
#define KEY_PORT PORTC_reg
#define KEY_PIN  PINC_reg
#elif port == D
#define KEY_DDR  DDRD_reg
#define KEY_PORT PORTD_reg
#define KEY_PIN  PIND_reg
#endif
// for KeyPads smaller than 4X4, connect the extra col/row to VCC
// pins 0,1,2,3 for columns 
// pins 4,5,6,7 for rows
const u8 KeyPad [4][4] =   {{'0','1','2','3'},
							{'4','5','6','7'},
							{'8','9','A','B'},
							{'C','D','E','F'}};
u8 col,row;

void kpdInit()
{
	KEY_DDR = 0xf0;
	KEY_PORT = 0xff; // can be omitted -to make sure that a clear low value will be send-
	
	do 
	{
		KEY_PORT &= 0x0f; // grounding all rows, the columns are left as they are
		col = KEY_PIN & 0x0f; // read all the columns
	} while (col != 0x0f); // stack here until all keys are released -all are ones-
}
u8 kpdWaitUntilPressed ()
{
	do 
	{
		do 
		{
			_delay_ms(20); // wait before reading a new value to avoid debouncing
			col = KEY_PIN & 0x0f; // read all columns
		} while (col == 0x0f); // if there was no key pressed, repeat the operation again
		_delay_ms(20); // if a key was pressed wait to avoid debouncing
		col = KEY_PIN & 0x0f; // read columns again
	} while (col == 0x0f); // if a key was pressed exit the loop !
	return 1; // returns one when a key is pressed
}
u8 kpdGetKey()
{
	u8 i, numRows = 4;
	for (i=0; i<numRows; i++)
	{
		KEY_PORT = 0xff; // all pins are ones except the n-row
		KEY_PORT &= ~1<<(i+4); // ground row 0 -pin 4- 
		col = KEY_PIN & 0x0f; // read the columns
		if (col != 0x0f)
		{
			row = i;
			break;
		}
	}
	u8 i;
	for(i=col; i & 1 == 0; i>>1);
	col=i;
	return KeyPad[row][col];
	
	/* or use :
		switch (col)
		{
			case 0x0e : return KeyPad[row][0]; break; // 1110
			case 0x0d : return KeyPad[row][1]; break; // 1101
			case 0x0b : return KeyPad[row][2]; break; // 1011
			case 0x07 : return KeyPad[row][3]; break; // 0111
		}
	*/
	return 0xff; // dummy value to avoid warnings
}

u8 kpdGetPressedKey () // use this function instead of kpdWaitUntilPressed ()+kpdGet()
{
	u8 iRow,iCol,rpw,col;
	while (1)
	{
		KEY_DDR = 0xf0; // all row are ones
		for (iRow = 4; iRow<8; iRow++)
		{
			KEY_PORT &= ~1<<(iRow); // ground row 0 -pin 4-
			for (iCol=0; iCol<4; iCol++)
			{
				if((KEY_PORT | 1<<iCol) ==0)
				{
					col = iCol - 4;
					row = iRow;
					while ((KEY_PORT | 1<<iCol) ==0) 
					{
						_delay_ms(20);
					}
					KEY_PORT |= 1<<(iRow);
					return KeyPad [row][col];
				}
			}
			KEY_PORT |= 1<<(iRow); // return row 0 -pin 4- to high -VCC-
		}
	}
}
#endif

