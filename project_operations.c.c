#include <xc.h>
#include"project.h"


//function to display dashboard

void display_dashboard(void) {

    if (speed >= 1000) {
        speed = 999;
    }
    key = read_switches(STATE_CHANGE);

    if (key == MK_SW2) {
        clear_log_flag = 0;
        if (gear++ >= 7)
            gear = 7;
        store_eeprom();
        events++;
    } else if (key == MK_SW3) {
        clear_log_flag = 0;
        if (gear-- <= 0)
            gear = 0;
        store_eeprom();
        events++;
    } else if (key == MK_SW11) {
        flag = 1;
        CLEAR_DISP_SCREEN;
        attempts = 3;
        return;
    }
    
    if(events>=10)
    {
        events=10;
    }

    //finding gears
    //default dashboard      

    clcd_print("  TIME    Ev  Sp ", LINE1(0));

    clcd_print(real_time, LINE2(0));
    clcd_print(gears[gear], LINE2(10));
    clcd_putch((speed / 10) / 10 + 48, LINE2(14));
    clcd_putch((speed / 10) % 10 + 48, LINE2(15));
}


//function definition for displaying password

void display_password(void) {
    TMR1ON = 1;
    if (attempts > 0) {
        clcd_print("enter password : ", LINE1(0));
        if (i < 4) {
            if (seconds == 3) {
                i = 0;
                TMR1ON = 0;
                seconds = 0;
                flag = 0;
                return;
            }
            key = read_switches(STATE_CHANGE);
            if (key == MK_SW11) {
                seconds = 0;
                clcd_putch('*', LINE2(i));
                password_arr2[i++] = '0';
            } else if (key == MK_SW12) {
                seconds = 0;
                clcd_putch('*', LINE2(i));
                password_arr2[i++] = '1';
            }
            __delay_ms(100);
            if (i == 4) {
                if (compare(password_arr1, password_arr2)) {
                    i = 0;
                    CLEAR_DISP_SCREEN;
                    clcd_print("Successful...", LINE1(0));
                    clcd_print("Ready to Rock !", LINE2(0));
                    TMR1ON = 0;
                    flag = 2;
                    seconds = 0;
                    CLEAR_DISP_SCREEN;
                    return;
                } else {
                    i = 0;
                    attempts--;
                    CLEAR_DISP_SCREEN;
                    clcd_print("wrong password!", LINE1(0));
                    clcd_print("attempts left", LINE2(0));
                    clcd_putch(attempts + 48, LINE2(14));
                    seconds = 0;
                    __delay_ms(1000);
                    CLEAR_DISP_SCREEN;
                }
            }
        }
    } else {
        TMR1ON = 1;
        i = 0;
        //CLEAR_DISP_SCREEN;
        clcd_print("you are blocked!", LINE1(0));
        clcd_print("seconds left", LINE2(0));

        clcd_putch((20 - seconds) / 10 + 48, LINE2(13));
        clcd_putch((20 - seconds) % 10 + 48, LINE2(14));
        if (seconds == 20) {
            CLEAR_DISP_SCREEN;
            seconds = 0;
            TMR1ON = 0;
            flag = 0;
            return;
        }
    }
}

//function definition for comparing the password strings

int compare(char *str1, char *str2) {
    for (int i = 0; (str1[i] != '\0' && str2[i] != '\0'); i++) {
        if (str1[i] != str2[i])
            return 0;
    }
    return 1;
} 


//function definition for displaying menu

void display_menu(void) {
    key = read_switches(LEVEL_CHANGE);

    if (key == MK_SW11) {
        delay++;
    } else if (delay < 500 && delay > 0) {
        if (change-- <= 0) {
            change = 0;
        }

        if (star-- <= 1) {
            star = 1;
            if (mptr-- <= 0) {
                mptr = 0;
            }
        }
        CLEAR_DISP_SCREEN;
        delay = 0;
    } else if (delay > 500) {
        delay = 0;
        switch (change) {
            case 0:
                flag = 11;
                address_data2 = 0;
                if(events)
                {
                    read_eeprom();
                }
                else
                {
                    CLEAR_DISP_SCREEN;
                    clcd_print("No Logs ",LINE1(0));
                    clcd_print("Till Now.",LINE2(8));
                    __delay_ms(2000);
                    flag=0;
                    CLEAR_DISP_SCREEN;
                    return;
                }
                CLEAR_DISP_SCREEN;
                return;

            case 1:
                flag = 22;
                clear_log_flag = 1;
                events=0;
                TMR1ON = 1;
                CLEAR_DISP_SCREEN;
                return;

            case 2:
                flag = 33;
                time_copy();
                CLEAR_DISP_SCREEN;
                return;

            case 3:
                flag = 44;
                CLEAR_DISP_SCREEN;
                return;

            case 4:
                flag = 55;
                i = 0;
                delay = delay1 = 0;
                CLEAR_DISP_SCREEN;
                return;
        }
    }
    if (key == MK_SW12) {
        delay1++;
    } else if (delay1 < 500 && delay1 > 0) {
        if (change++ >= 4) {
            change = 4;
        }

        delay1 = 0;
        if (star++ >= 2) {
            star = 2;
            if (mptr++ >= 3) {
                mptr = 3;
            }
        }
        CLEAR_DISP_SCREEN;
    } else if (delay1 > 500) {
        delay1 = 0;
        flag = 0;
        mptr = 0, star = 1, change = 0;
        CLEAR_DISP_SCREEN;
        return;
    }


    if (star == 1) {
        clcd_putch(ARROW, LINE1(0));
        clcd_putch(' ', LINE2(0));
    } else {
        clcd_putch(' ', LINE1(0));
        clcd_putch(ARROW, LINE2(0));
    }

    clcd_print(menu[mptr], LINE1(1));
    clcd_print(menu[mptr + 1], LINE2(1));
}


//function definition for view log operation

void view_log(void) {
    if (clear_log_flag) {
        clcd_print("No logs to show.", LINE1(0));
        clcd_print("Start New...", LINE2(0));
        __delay_ms(3000);
        flag = 2;
        CLEAR_DISP_SCREEN;
        return;
    }

    key = read_switches(STATE_CHANGE);
    if (key == MK_SW12) {
        CLEAR_DISP_SCREEN;

        if (log_value < 10 && log_value<events) {
            logs_speed[0] = '\0';
            read_eeprom();
            log_value++;
        }
    } else if (key == MK_SW11) {

        if (address_data2 <= 0) {
            address_data2 = 0;
        }

        CLEAR_DISP_SCREEN;
        if (log_value > 1) {
            logs_speed[0] = '\0';
            address_data2 -= 24;
            read_eeprom();
            log_value--;
        }
    } else if (key == MK_SW10) {
        flag = 0;
        log_value = 1;
        address_data2 = 0;
        CLEAR_DISP_SCREEN;
        return;
    }


    clcd_print("Log No ->", LINE1(0));
    clcd_putch((log_value / 10) + 48, LINE1(12));
    clcd_putch((log_value % 10) + 48, LINE1(13));
    clcd_putch('.', LINE1(14));
    clcd_print(logs_time, LINE2(1));
    clcd_print(logs_gear, LINE2(10));
    clcd_print(logs_speed, LINE2(13));

}


//function definition for clear log operation

void clear_log(void) {
    clcd_print("Logs are cleared .", LINE1(0));
    clcd_print("Ready for New...", LINE2(0));
    if (seconds == 4) {
        TMR1ON = 0;
        seconds = 0;
        flag = 2;
        address_data = address_data2 = 0;
        CLEAR_DISP_SCREEN;
    }
}



//function definition for set time operation

void set_time(void) {

    key = read_switches(LEVEL_CHANGE);
    
    if(key==MK_SW10)
    {
        CLEAR_DISP_SCREEN;
        flag=2;
        return;
    }

    //logic for incrementing time
    if (key == MK_SW11) {
        delay++;
    } else if (delay < 500 && delay > 0) {
        //code for incrementing time on short press
        delay = 0;
        CLEAR_DISP_SCREEN;

        switch (field) {
            case 1:
                
                if (++time[1] > '9') {
                    time[1] = '0';
                    time[0]++;
                }
                if (time[0] == '2' && time[1] == '4') {
                    time[0] = time[1] = '0';
                }
                break;

            case 2:
                if(++time[4] > '9')
                {
                    time[4]='0';
                    ++time[3];
                }
                
                if(time[3] == '5' && time[4] == '9')
                {
                    time[3]=time[4]='0';
                }
                break;

            case 3:
                if(++time[7]>'9')
                {
                    time[7]='0';
                    ++time[6];
                }
                
                if(time[6] == '5' && time[7] == '9')
                {
                    time[6]=time[7]='0';
                }
                break;
        }
    } else if (delay > 500) {
        //logic for long press saving and going back to main menu
        CLEAR_DISP_SCREEN;
        delay = 0;

        //code for saving time into RTC
        /* store time into RTC*/
        clcd_print("New time saved", LINE1(0));
        clcd_print("Returning..", LINE2(0));
        write_ds1307(HOUR_ADDR, ((time[0] << 4) | (time[1])));
        write_ds1307(MIN_ADDR, ((time[3] << 4) | (time[4])));
        write_ds1307(SEC_ADDR, ((time[6] << 4) | (time[7])));
        __delay_ms(2000);
        CLEAR_DISP_SCREEN;
        flag = 2;
    }


    //logic for changing fields
    if (key == MK_SW12) {
        delay1++;
    } else if (delay1 < 500 && delay1 > 0) {
        //logic for short press changing fields
        delay1 = 0;
        CLEAR_DISP_SCREEN;

        if (field++ >= 3) {
            field = 1;
        }
    } else if (delay1 > 500) {
        //logic for long press going back to main menu without changing time
        delay1 = 0;
        CLEAR_DISP_SCREEN;
        clcd_print("Time not saved !", LINE1(0));
        clcd_print("Going to back.", LINE2(0));
        __delay_ms(2000);
        CLEAR_DISP_SCREEN;
        flag = 2;
        return;
    }

    //for indicating fields
    switch (field) {
        case 1:
            clcd_putch(IND, LINE1(0));
            clcd_putch(IND, LINE1(1));
            break;

        case 2:
            clcd_putch(IND, LINE1(3));
            clcd_putch(IND, LINE1(4));
            break;

        case 3:
            clcd_putch(IND, LINE1(6));
            clcd_putch(IND, LINE1(7));
            break;
    }

    clcd_print("Change", LINE1(10));
    clcd_print("Time", LINE2(11));
    clcd_print(time, LINE2(0));
}



//function definition for download log operation

void download_log(void) {
    //
    CLEAR_DISP_SCREEN;
    clcd_print("Downloading", LINE1(0));
    if (clear_log_flag) {
        clcd_print("No logs to show.", LINE1(0));
        clcd_print("Start New...", LINE2(0));
        flag = 2;
        CLEAR_DISP_SCREEN;
        
    }
    __delay_ms(5000);
    GIE = PEIE = TMR1ON = 0;
    init_uart();

    //write logic for UART protocal
    for (i = 0; i < events; i++) {
        //read from EEPROM
        for (j = 0; j < 16; j++) 
        {
            if(j == 2 || j == 5)
            {
                download_log_string[j] = ':';
            }
            else if(j == 8 || j== 9 || j == 12 || j == 13)
            {
                 download_log_string[j] = ' ';
            }
            else
            {
                download_log_string[j] = read_external_eeprom(address_data2++);
            }
        }
        
        download_log_string[j]='\0';
        //integrate UART        
        puts(download_log_string);
        puts("\n\r");
    }
    __delay_ms(2000);
    flag = 1;
     GIE = PEIE = TMR1ON = 1;
    return;
}

//function for changing the password

void change_password(void) {
    key = read_switches(LEVEL_CHANGE);
    if (key == MK_SW11) {
        delay++;
    } else if (key == MK_SW12) {
        delay1++;
    } else if (i < 4) {
        clcd_print("Enter password $", LINE1(0));
        if (delay < 10000 && delay > 0) {
            delay = 0;
            clcd_putch('*', LINE2(i));
            new_pass1[i++] = '0';
        } else if (delay1 < 10000 && delay1 > 0) {
            delay1 = 0;
            clcd_putch('*', LINE2(i));
            new_pass1[i++] = '1';
        } else if (delay1 > 10000) {
            delay = 0;
            flag = 2;
            return;
        } else {
            delay = 0;
            delay1 = 0;
        }
    } else if (i >= 4) {
        if (i == 4) {
            clcd_putch('*', LINE2(3));
            __delay_ms(200);
            CLEAR_DISP_SCREEN;
            i++;
        }
        clcd_print("Re-enter ...    ", LINE1(0));
        if (delay < 10000 && delay > 0) {
            delay = 0;
            clcd_putch('*', LINE2(j));
            new_pass2[j++] = '0';
        } else if (delay1 < 10000 && delay1 > 0) {
            delay1 = 0;
            clcd_putch('*', LINE2(j));
            new_pass2[j++] = '1';
        } else if (delay1 > 10000) {
            flag = 2;
            return;
        } else {
            delay = delay1 = 0;
        }
        //__delay_ms(200);
        if (i >= 4 && j >= 4) {
            if (compare(new_pass1, new_pass2)) {
                i = 0;
                CLEAR_DISP_SCREEN;
                clcd_print("Change password", LINE1(0));
                clcd_print("Successful!", LINE2(0));
                i = 0, j = 0;
                //store_password(new_pass2);
                password_arr1[0] = new_pass2[0];
                password_arr1[1] = new_pass2[1];
                password_arr1[2] = new_pass2[2];
                password_arr1[3] = new_pass2[3];
                __delay_ms(1000);
                flag = 2;
                CLEAR_DISP_SCREEN;
                return;
            } else {
                i = 0;
                CLEAR_DISP_SCREEN;
                clcd_print("Wrong password!", LINE1(0));
                clcd_print("Try again..", LINE2(0));
                i = 0, j = 0;
                __delay_ms(1000);
                CLEAR_DISP_SCREEN;
            }
        }
    }

}


//function definition for storing events into EEPROM

void store_eeprom(void) {
    if (address_data >= 120) {
        address_data = 0;
    }

    //for storing time value
    for (int i = 0; i < 8; i++) {
        write_external_eeprom(address_data++, real_time[i]);
    }

    //for storing gear value
    for (int i = 0; i < 2; i++) {
        write_external_eeprom(address_data++, gears[gear][i]);
    }

    //for storing speed value
    write_external_eeprom(address_data++, (speed / 10) / 10 + 48);
    write_external_eeprom(address_data++, (speed / 10) % 10 + 48);
}


//function definition for reading data from EEPROM

void read_eeprom(void) {
    if (address_data2 >= 118) {
        address_data2 = 118;
    }
    //for fetching time from EEPROM
    for (int i = 0; i < 8; i++) {
        logs_time[i] = read_external_eeprom(address_data2++);
    }

    //for reading gear value from EEPROM
    for (int i = 0; i < 2; i++) {
        logs_gear[i] = read_external_eeprom(address_data2++);
    }

    //for reading speed value from EEPROM

    logs_speed[0] = (read_external_eeprom(address_data2++));
    logs_speed[1] = (read_external_eeprom(address_data2++));
    logs_speed[2] = '\0';

}


//store password into EEPROM

void store_password(char str[]) {
    for (int i = 0; i < 4; i++) {
        write_external_eeprom(address_password++, str[i]);
    }
    address_password = 150;
}

//read password from EEPROM

void read_password(void) {
    int i;
    for (i = 0; i < 4; i++) {
        password_arr1[i] = read_external_eeprom(address_password++);
    }
    password_arr1[i] = '\0';
    address_password = 150;
}


//function definition for exchanging real_time and time array data's

void time_copy(void) 
{
        //copy from real time to time array
        for (int i = 0; i < 9; i++)
        {
            time[i] = real_time[i];
        }    
}
