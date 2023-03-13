#include <bcm2835.h>
#include <stdio.h>
#include <stdlib.h>
#include <mysql/mysql.h>
#include "../include/scanll.h"

#define START_END_SCAN RPI_GPIO_P1_15
#define TAKE_READING RPI_GPIO_P1_16


static char* host = "localhost";
static char* user = "root";
static char* pass = "";
static char* dbname = "soil-scans";

unsigned int port = 0;
static char* unix_socket = NULL;
unsigned int flag = 0;

unsigned int ret_code = 0;

// Linked List For Samples, to form full scan.


float i2c_get_sample (,int *code) 
{

}

void sql_commit (MYSQL *conn, SCAN* inscan, int *code)
{

}

SCAN* complete_scan (int *code)
{
    SCAN* head = scan_create();
    while (!bcm2835_gpio_eds(START_END_SCAN))
    {
        if (bcm2835_gpio_eds (TAKE_READING))
        {
            bcm2835_gpio_set_eds (TAKE_READING);
            float ret = i2c_get_sample ( , code);
            if (code)
            {
                // Get X and Y Pos and add to node
                scan_add (head, ret);
                fprintf (stderr, "Error: Read From i2c failed.\n");
            }

        }
    }
    bcm2835_gpio_set_eds(START_END_SCAN);
    *code = 0;
    return head;
}


int main (int argc, char** argv)
{
    MYSQL *conn;
    
    conn = mysql_init (NULL);

    if (!mysql_real_connect (conn, host, user, pass, dbname, port, unix_socket, flag))
    {
        fprintf (stderr, "\nError: %s [%d]\n", mysql_error(conn), mysql_errno(conn));
        return 1;
    }

    if (!bcm2835_init())
    {
        fprintf (stderr, "\nError: BCM2835 INIT Failure\n(Are You Running As Root?)\n");
        return 1;
    }
    // Set RPI pin P1-15 to be an input
    bcm2835_gpio_fsel(START_END_SCAN, BCM2835_GPIO_FSEL_INPT);
    //  with a pullup
    bcm2835_gpio_set_pud(START_END_SCAN, BCM2835_GPIO_PUD_UP);
    // And a low detect enable
    bcm2835_gpio_len(START_END_SCAN);

    // Set RPI pin P1-15 to be an input
    bcm2835_gpio_fsel(TAKE_READING, BCM2835_GPIO_FSEL_INPT);
    //  with a pullup
    bcm2835_gpio_set_pud(TAKE_READING, BCM2835_GPIO_PUD_UP);
    // And a low detect enable
    bcm2835_gpio_len(TAKE_READING);

    while (1)
    {
        if (bcm2835_gpio_eds(START_END_SCAN))
        {
            // Now clear the eds flag by setting it to 1
            bcm2835_gpio_set_eds(START_END_SCAN);
            printf("low event detect for pin 15\n");
            SCAN* ret = complete_scan (&ret_code);
            if (!ret_code)
            {
                sql_commit (conn, ret, &ret_code);
                if (ret_code)
                {
                    fprintf (stderr, "Error: Write to MySQL Failed\n");
                }
            }
            else {
                fprintf (stderr, "Error: Scan Failed, Could Be Error with I2C\n");
            }
        }
 
        // wait a bit
        delay(500);
    }

    bcm2835_close();
    return 0;
}