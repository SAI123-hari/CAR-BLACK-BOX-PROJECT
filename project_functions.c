
#include<xc.h>
#include"project.h"

//function definition that will initialize every peripheral
void init_all(void)
{
    init_clcd();
    init_matrix_keypad();
    init_adc();
    init_timer1();
    init_i2c();
    init_ds1307();

}


//clcd function definitions
void clcd_write(unsigned char byte, unsigned char control_bit)
{
	CLCD_RS = control_bit;
	CLCD_PORT = byte;

	/* Should be atleast 200ns */
	CLCD_EN = HI;
	CLCD_EN = LO;

	PORT_DIR = INPUT;
	CLCD_RW = HI;
	CLCD_RS = INSTRUCTION_COMMAND;

	do
	{
		CLCD_EN = HI;
		CLCD_EN = LO;
	} while (CLCD_BUSY);

	CLCD_RW = LO;
	PORT_DIR = OUTPUT;
}

void init_clcd()
{
	/* Set PortD as output port for CLCD data */
	TRISD = 0x00;
	/* Set PortC as output port for CLCD control */
	TRISC = TRISC & 0xF8;

	CLCD_RW = LO;

	
     /* Startup Time for the CLCD controller */
    __delay_ms(30);
    
    /* The CLCD Startup Sequence */
    clcd_write(EIGHT_BIT_MODE, INSTRUCTION_COMMAND	);
    __delay_us(4100);
    clcd_write(EIGHT_BIT_MODE, INSTRUCTION_COMMAND	);
    __delay_us(100);
    clcd_write(EIGHT_BIT_MODE, INSTRUCTION_COMMAND	);
    __delay_us(1); 
    
    CURSOR_HOME;
    __delay_us(100);
    TWO_LINE_5x8_MATRIX_8_BIT;
    __delay_us(100);
    CLEAR_DISP_SCREEN;
    __delay_us(500);
    DISP_ON_AND_CURSOR_OFF;
    __delay_us(100);
}

void clcd_print(const unsigned char *data, unsigned char addr)
{
	clcd_write(addr, INSTRUCTION_COMMAND);
	while (*data != '\0')
	{
		clcd_write(*data++, DATA_COMMAND);
	}
}

void clcd_putch(const unsigned char data, unsigned char addr)
{
	clcd_write(addr, INSTRUCTION_COMMAND);
	clcd_write(data, DATA_COMMAND);
}


//matrix keypad function definitions
void init_matrix_keypad(void)
{
	/* Config PORTB as digital */
	ADCON1 = 0x0F;

	/* Set Rows (RB7 - RB5) as Outputs and Columns (RB4 - RB1) as Inputs */
	TRISB = 0x1E;

	/* Set PORTB input as pull up for columns */
	RBPU = 0;

	MATRIX_KEYPAD_PORT = MATRIX_KEYPAD_PORT | 0xE0;
}

unsigned char scan_key(void)
{
	ROW1 = LO;
	ROW2 = HI;
	ROW3 = HI;

	if (COL1 == LO)
	{
		return 1;
	}
	else if (COL2 == LO)
	{
		return 4;
	}
	else if (COL3 == LO)
	{
		return 7;
	}
	else if (COL4 == LO)
	{
		return 10;
	}

	ROW1 = HI;
	ROW2 = LO;
	ROW3 = HI;

	if (COL1 == LO)
	{
		return 2;
	}
	else if (COL2 == LO)
	{
		return 5;
	}
	else if (COL3 == LO)
	{
		return 8;
	}
	else if (COL4 == LO)
	{
		return 11;
	}

	ROW1 = HI;
	ROW2 = HI;
	ROW3 = LO;
	/* TODO: Why more than 2 times? */
	ROW3 = LO;

	if (COL1 == LO)
	{
		return 3;
	}
	else if (COL2 == LO)
	{
		return 6;
	}
	else if (COL3 == LO)
	{
		return 9;
	}
	else if (COL4 == LO)
	{
		return 12;
	}

	return 0xFF;
}

unsigned char read_switches(unsigned char detection_type)
{
	static unsigned char once = 1, key;

	if (detection_type == STATE_CHANGE)
	{
		key = scan_key();
		if(key != 0xFF && once  )
		{
			once = 0;
			return key;
		}
		else if(key == 0xFF)
		{
			once = 1;
		}
	}
	else if (detection_type == LEVEL_CHANGE)
	{
		return scan_key();
	}

	return 0xFF;
}


//function definition for ADC 
void init_adc(void)
{
	/* Selecting right justified ADRES Registers order */
	ADFM = 1;

	/* 
	 * Acqusition time selection bits 
	 * Set for 4 Tad
	 */
	ACQT2 = 0;
	ACQT1 = 1;
	ACQT0 = 0;

	/*
	 * Selecting the conversion clock of Fosc / 32 -> 1.6usecs -> 1Tad
	 * Our device frequency is 20 MHz
	 */
	ADCS0 = 0;
	ADCS1 = 1;
	ADCS2 = 0;

	/* Stop the conversion to start with */
	GODONE = 0;

	

	/* Voltage reference bit as VSS */
	VCFG1 = 0;
	/* Voltage reference bit as VDD */
	VCFG0 = 0;

	/* Just clearing the ADRESH & ADRESL registers, for time pass */
	ADRESH = 0;
	ADRESL = 0;

	/* Turn ON the ADC module */
	ADON = 1;
}

unsigned short read_adc(unsigned char channel)
{
	unsigned short reg_val;

	/*select the channel*/
	ADCON0 = (ADCON0 & 0xC3) | (channel << 2);

	/* Start the conversion */
	GO = 1;
	while (GO);
	reg_val = (ADRESH << 8) | ADRESL; 

	return reg_val;
}


//function definitions for timer1
void init_timer1(void)
{
    GIE=1;
    
    PEIE=1;
    
    TMR1ON=0;
    
    TMR1IF=0;
    
    TMR1IE=1;
    
   // T1CONbits.RD16=0;
}

void __interrupt() isr(void)
{
    static int count=0;
    
    if(TMR1IF)
    {
        TMR1 = TMR1+3038;
        if(count++==80)
        {            
            count=0; 
            seconds++;
        }
        TMR1IF =0;
    }
}


//function definitions for I2C
void init_i2c(void)
{
	/* Set SCL and SDA pins as inputs */
	TRISC3 = 1;
	TRISC4 = 1;
	/* Set I2C master mode */
	SSPCON1 = 0x28;

	SSPADD = 0x30;
	/* Use I2C levels, worked also with '0' */
	CKE = 0;
	/* Disable slew rate control  worked also with '0' */
	SMP = 1;
	/* Clear SSPIF interrupt flag */
	SSPIF = 0;
	/* Clear bus collision flag */
	BCLIF = 0;
}

void i2c_idle(void)
{
	while (!SSPIF);
	SSPIF = 0;
}

void i2c_ack(void)
{
	if (ACKSTAT)
	{
		/* Do debug print here if required */
	}
}

void i2c_start(void)
{
	SEN = 1;
	i2c_idle();
}

void i2c_stop(void)
{
	PEN = 1;
	i2c_idle();
}

void i2c_rep_start(void)
{
	RSEN = 1;
	i2c_idle();
}

void i2c_write(unsigned char data)
{
	SSPBUF = data;
	i2c_idle();
}

void i2c_rx_mode(void)
{
	RCEN = 1;
	i2c_idle();
}

void i2c_no_ack(void)
{
	ACKDT = 1;
	ACKEN = 1;
}

unsigned char i2c_read(void)
{
	i2c_rx_mode();
	//i2c_no_ack();

	return SSPBUF;
}


//function definitions for external EEPROM
void write_external_eeprom(unsigned char address, unsigned char data)
{
	i2c_start();
	i2c_write(SLAVE_WRITE);
	i2c_write(address);
	i2c_write(data);
	i2c_stop();
	for(unsigned int i = 3000;i--;);
}

unsigned char read_external_eeprom(unsigned char address)
{
	unsigned char data;

	i2c_start();
	i2c_write(SLAVE_WRITE);
	i2c_write(address);
	i2c_rep_start();
	i2c_write(SLAVE_READ);
	data = i2c_read();
	i2c_stop();

	return data;
}


//function definitions for UART
void init_uart(void)
{
	/* Serial initialization */
	RX_PIN = 1;
	TX_PIN = 0;

	/* TXSTA:- Transmitor Status and control Register */
	/* 9bit TX enable or disable bit */ 
	TX9 = 0;
	/* UART Tarsmition enable bit */
	TXEN = 1;
	/* Synchronous or Asynchronous mode selection */
	/* Asynchronous */
	SYNC = 0;
	/* Send the Break character bit */
	SENDB = 0;
	/* Low or High baud rate selection bit */
	/* High Baud Rate */
	BRGH = 1;

	/* RCSTA :- Recepition Status and control Register */
	/* TX/RC7 and RX/RC6 act as serial port */ 
	SPEN = 1;
	/* 9bit RX enable or disable bit */
	RX9 = 0;
	/* Continous reception enable or disable */ 
	CREN = 1;

	/* BAUDCTL:- Baud rate control register */
	/* Auto baud detection overflow bit */
	ABDOVF = 0;
	/* 16bit baud generate bit */ 
	BRG16 = 0;
	/* Wakeup enable bit */
	WUE = 0;
	/* Auto baud detect enable bit */
	ABDEN = 0;

	/* Baud Rate Setting Register */
	/* Set to 10 for 115200, 64 for 19200 and 129 for 9600 */
	SPBRG = 129;

    
	/* TX interrupt enable bit */
	TXIE = 1;
	/* TX interrupt flag bit */
	TXIF = 0;
	/* RX interrupt enable bit */
	RCIE = 1;
	/* RX interrupt enable bit */
	RCIF = 0;
}

void putch(unsigned char byte) 
{
	/* Output one byte */
	/* Set when register is empty */
	while(!TXIF)
	{
		continue;
	}
	TXREG = byte;
} 

int puts(const char *s)
{
	while(*s)		
	{
		putch(*s++);	
	}
	return 0;
}

unsigned char getch(void)
{
	/* Retrieve one byte */
	/* Set when register is not empty */
	while(!RCIF)
	{
		continue;
	}
	return RCREG;
}

unsigned char getche(void)
{
	unsigned char c;

	putch(c = getch());

	return (c);
}


//function definitions for RTC read and write


void init_ds1307(void)
{
	unsigned char dummy;

	/* Setting the CH bit of the RTC to Stop the Clock */
	dummy = read_ds1307(SEC_ADDR);
	write_ds1307(SEC_ADDR, dummy | 0x80); 

	/* Seting 12 Hr Format */
	dummy = read_ds1307(HOUR_ADDR);
	write_ds1307(HOUR_ADDR, dummy | 0x40); 

	/* 
	 * Control Register of DS1307
	 * Bit 7 - OUT
	 * Bit 6 - 0
	 * Bit 5 - OSF
	 * Bit 4 - SQWE
	 * Bit 3 - 0
	 * Bit 2 - 0
	 * Bit 1 - RS1
	 * Bit 0 - RS0
	 * 
	 * Seting RS0 and RS1 as 11 to achive SQW out at 32.768 KHz
	 */ 
	write_ds1307(CNTL_ADDR, 0x93); 

	/* Clearing the CH bit of the RTC to Start the Clock */
	dummy = read_ds1307(SEC_ADDR);
	write_ds1307(SEC_ADDR, dummy & 0x7F); 

}


void write_ds1307(unsigned char address, unsigned char data)
{
	i2c_start();
	i2c_write(RTC_SLAVE_WRITE);
	i2c_write(address);
	i2c_write(data);
	i2c_stop();
}

unsigned char read_ds1307(unsigned char address)
{
	unsigned char data;

	i2c_start();
	i2c_write(RTC_SLAVE_WRITE);
	i2c_write(address);
	i2c_rep_start();
	i2c_write(RTC_SLAVE_READ);
	data = i2c_read();
	i2c_stop();

	return data;
}

void get_time(void)
{
	clock_reg[0] = read_ds1307(HOUR_ADDR);
	clock_reg[1] = read_ds1307(MIN_ADDR);
	clock_reg[2] = read_ds1307(SEC_ADDR);
    
    //BCD values will returned from the RTC
	if (clock_reg[0] & 0x40)  //checking 12 hours or 24 hours format 
	{
		real_time[0] = '0' + ((clock_reg[0] >> 4) & 0x01); //in msb side only 1 bit is set because of BCD  (because 1 oocupies only i bit if 12 hours format only 1 can be in MSB)
		real_time[1] = '0' + (clock_reg[0] & 0x0F); //LSB bits for the value both for 12 and 24 hour format MSB bits will be vary from 0-9
	}
	else
	{
		real_time[0] = '0' + ((clock_reg[0] >> 4) & 0x03); //if 24 hour format 2 is the highest value it will occupy 2 bits in MSB side
		real_time[1] = '0' + (clock_reg[0] & 0x0F); //in any format the MSB will occupy 4 bits range is from(0-9)
	}
	real_time[2] = ':';
	real_time[3] = '0' + ((clock_reg[1] >> 4) & 0x0F); //highest value is 59 for MIN 5 in MSB bits and 9 in LSB bits the min ranges from (00-59) both can occupy 4 bits each as it is BCD
	real_time[4] = '0' + (clock_reg[1] & 0x0F); //for the LSB values how much the value in(00-59)  the MSB will be range from  0-9 only
	real_time[5] = ':';
	real_time[6] = '0' + ((clock_reg[2] >> 4) & 0x0F); //seconds also range from (00-59) and 1 st values will be occupied by 10's values(0-9) MSB
	real_time[7] = '0' + (clock_reg[2] & 0x0F);  //last values are 1's values also ranges from(0-9)
	real_time[8] = '\0';
}
 