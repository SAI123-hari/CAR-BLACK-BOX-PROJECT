/*
 * File:   project_main.c
 * Author: SAI HARI HARA CHANDRA TALAGADADEEVI
 *
 * Created on 27 December, 2023, 9:42 PM
 */


#include <xc.h>
#include"project.h"


void main(void) 
{
    //configuring every peripheral
    init_all();
    //read_password();
    
    while(1)
    {
        //read time from RTC using I2C
        get_time();
        if(key==MK_SW1)
        {
            clear_log_flag=0;
            CLEAR_DISP_SCREEN;
            store_eeprom();
            clcd_print("Collision ...",LINE1(1));
            clcd_print("Call Police",LINE2(3));
            while(1);
        }
        if(wait++==200)
        {
            wait=0;
            speed=read_adc(CHANNEL4);           
        }
        
        if(flag==0)
        {
            display_dashboard();
        }
        else if(flag==1)
        { 
            display_password();
        }
        else if(flag==2)
        {
            display_menu();
        }
        else if(flag==11)
        {
            view_log();
        }
        else if(flag==22)
        {
            clear_log();
        }
        else if(flag==33)
        {
            set_time();
        }
        else if(flag==44)
        {
            download_log();
        }
        else if(flag==55)
        {
            change_password();
        }
        
    }
    return;
}



