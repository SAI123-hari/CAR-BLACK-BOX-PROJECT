/* 
 * File:   project.h
 * Author: SAI HARI HARA CHANDRA TALAGADADEEVI
 *
 * Created on 27 December, 2023, 9:42 PM
 */

#ifndef PROJECT_H
#define	PROJECT_H


//global variable declarations
    unsigned char key, key2, address_data=0, address_data2=0, address_password=150, time_address=0;
    char password_arr1[5]="1010", password_arr2[5], new_pass1[5], new_pass2[5], attempts=3;
    unsigned char logs_time[9], logs_gear[3], logs_speed[3];
    char gears[8][3]={"ON","GN","GR","G1","G2","G3","G4","G5"};
    char *menu[5]={"1.View log","2.Clear log","3.Set time","4.Download log","5.Change Paswrd"};
    unsigned short int speed=0, gear=0, wait=0;
    int flag=0,seconds=0, i=0,j=0, star=1, mptr=0, change=0,delay=0, delay1=0, log_value=1, clear_log_flag=0;
    int field=1, events=0;
    char download_log_string[20], real_time[9];
    unsigned char clock_reg[3], time[9];
    
//special symbol macro declarations
#define STAR        0x2A
#define CUR         0x5F
#define ARROW       0x7E 
#define IND         0xA5
    
    
//clcd macro definitions

#define CLCD_PORT			PORTD
#define CLCD_EN				RC2
#define CLCD_RS				RC1
#define CLCD_RW				RC0
#define CLCD_BUSY			RD7
#define PORT_DIR			TRISD7


#define HI												1
#define LO												0

#define INPUT											0xFF
#define OUTPUT											0x00

#define DATA_COMMAND									1
#define INSTRUCTION_COMMAND								0
#define _XTAL_FREQ                  20000000
#define LINE1(x)									(0x80 + (x))
#define LINE2(x)										(0xC0 + (x))

#define TWO_LINE_5x8_MATRIX_8_BIT					clcd_write(0x38, INSTRUCTION_COMMAND)
#define CLEAR_DISP_SCREEN				                clcd_write(0x01, INSTRUCTION_COMMAND)
#define CURSOR_HOME							clcd_write(0x02, INSTRUCTION_COMMAND)
#define DISP_ON_AND_CURSOR_OFF						clcd_write(0x0C, INSTRUCTION_COMMAND)
#define EIGHT_BIT_MODE   0x33
void init_clcd(void);
void clcd_print(const unsigned char *data, unsigned char addr);
void clcd_putch(const unsigned char data, unsigned char addr);
void clcd_write(unsigned char bit_values, unsigned char control_bit);


//matrix keypad macro defintions
#define MAX_ROW				4
#define MAX_COL				3

#define STATE_CHANGE				1
#define LEVEL_CHANGE				0
#define MATRIX_KEYPAD_PORT			PORTB
#define ROW3					PORTBbits.RB7
#define ROW2					PORTBbits.RB6
#define ROW1					PORTBbits.RB5
#define COL4					PORTBbits.RB4
#define COL3					PORTBbits.RB3
#define COL2					PORTBbits.RB2
#define COL1					PORTBbits.RB1


#define MK_SW1					1
#define MK_SW2					2
#define MK_SW3					3
#define MK_SW4					4
#define MK_SW5					5
#define MK_SW6					6
#define MK_SW7					7
#define MK_SW8					8
#define MK_SW9					9
#define MK_SW10				10
#define MK_SW11				11
#define MK_SW12				12

#define ALL_RELEASED	0xFF

#define HI				1
#define LO				0

void init_matrix_keypad(void);
unsigned char scan_key(void);
unsigned char read_switches(unsigned char detection_type);


//macro defintions for ADC 
#define CHANNEL0		0x00
#define CHANNEL1		0x01
#define CHANNEL2		0x02
#define CHANNEL3		0x03
#define CHANNEL4		0x04
#define CHANNEL5		0x05
#define CHANNEL6		0x06
#define CHANNEL7		0x07
#define CHANNEL8		0x08
#define CHANNEL9		0x09
#define CHANNEL10		0x0A

void init_adc(void);
unsigned short read_adc(unsigned char channel);


//macro and function declarations for I2C
void init_i2c(void);
void i2c_start(void);
void i2c_rep_start(void);
void i2c_stop(void);
void i2c_write(unsigned char data);
unsigned char i2c_read(void);



//macro definition and function defintions for RTC

#define RTC_SLAVE_READ		0xD1
#define RTC_SLAVE_WRITE		0xD0


#define SEC_ADDR		0x00
#define MIN_ADDR		0x01
#define HOUR_ADDR		0x02
#define DAY_ADDR		0x03
#define DATE_ADDR		0x04
#define MONTH_ADDR		0x05
#define YEAR_ADDR		0x06
#define CNTL_ADDR		0x07


void write_ds1307(unsigned char address1,  unsigned char data);
unsigned char read_ds1307(unsigned char address1);
void init_ds1307(void);




//macro and function definitions for external EEPROM
#define SLAVE_READ		0xA1
#define SLAVE_WRITE		0xA0


void write_external_eeprom(unsigned char address1,  unsigned char data);
unsigned char read_external_eeprom(unsigned char address1);



//macro and function declarations for UART protocal
#define RX_PIN					TRISC7
#define TX_PIN					TRISC6

void init_uart(void);
void putch(unsigned char byte);
int puts(const char *s);
unsigned char getch(void);
unsigned char getch_with_timeout(unsigned short max_time);
unsigned char getche(void);

//function that will initialize every peripheral
void init_all(void);


//function definitions for timer peripheral
void init_timer1(void);


//actual project functions
void display_dashboard(void);
void display_password(void);
int comapre(char *, char *);
void display_menu(void);
void view_log(void);
void clear_log(void);
void set_time(void);
void download_log(void);
void change_password(void);


//own functions
void store_eeprom(void); //storing events into EEPROM
void read_eeprom(void);  //reading events from EEPROM
void store_password(char *); //storing password into EEPROM
void read_password(void);    //reading password from EEPROM
void get_time(void);        //for reading time from RTC
void time_copy(void);       //function definition for copying real time into time array
#endif	/* PROJECT_H */

