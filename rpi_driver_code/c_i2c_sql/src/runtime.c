#include <bcm2835.h>
#include <stdio.h>
#include <stdlib.h>
#include <mysql/mysql.h>
#include "../include/scanll.h"

#define START_END_SCAN RPI_GPIO_P1_15
#define TAKE_READING RPI_GPIO_P1_16

#define MAX_LEN 32

char wbuf[MAX_LEN];
char rbuf[MAX_LEN];
 
uint16_t clk_div = BCM2835_I2C_CLOCK_DIVIDER_148;
uint8_t slave_address = 0x00;
uint32_t len = 0;

static char* host = "localhost";
static char* user = "root";
static char* pass = "";
static char* dbname = "soil-scans";

unsigned int port = 0;
static char* unix_socket = NULL;
unsigned int flag = 0;

unsigned int ret_code = 0;

// Linked List For Samples, to form full scan.


unsigned float i2c_get_sample (int *code) 
{
    // Return Codes For Func.
    // 0 = Everything Worked Correctly
    // 1 = Write Fail
    // 2 = TIMEOUT
    // 3 = Read Fail
    // 4 = Incorrect Parity
    // 5 = Incorrect ERRCHK Values.

    // GET = HEXCODE FOR GET COMMAND
    // ERRCHK1 = 8BIT RNDM NUM to be Returned
    // ERRCHK2 = BITNOT ERRCHK1 also to be returned.
    // PARITY = 1 if overall sum odd, 0 if overall sum even.
    // {0x00 0x00 0x00 0x00}
    *code = 0;
    len = 4;
    uint8_t get = ___;
    uint8_t errchk1 = (rand() % 256);
    uint8_t errchk2 = ~errchk1;
    uint8_t parity = 0;
    if ((get + errchk1 + errchk2) % 2 == 1 )
    {
        parity = 1;
    }

    wbuf[0] = (char) get;
    wbuf[1] = (char) errchk1;
    wbuf[2] = (char) errchk2;
    wbuf[3] = (char) parity;

    *code = bcm2835_i2c_write (wbuf, len);
    if (*code == 1)
    {
        fprintf (stderr, "\nError: Write to I2C Device Failed. (CODE: %d)\n", *code);
        return 0.0;
    }
    
    // Wait For Pulled High
    unsigned int timeout = 0;
    while (!PULLED_HIGH)
    {
        if (timeout >= 100)
        {
            *code == 2;
            fprintf (stderr, "\nError: Request Timeout (CODE: %d)\n", *code);
            return 0.0;
        }
        delay (100);
        timeout++;
    }
    for (int i=0; i<MAX_LEN; i++)
    {
        rbuf[i] = 'n';
    }

    // Return should be
    // RETURN = HEXCODE FOR RTN COMMAND
    // ERRCHK1 = 8BIT RNDM NUM to be Returned
    // ERRCHK2 = BITNOT ERRCHK1 also to be returned.
    // PARITY = 1 if overall sum odd, 0 if overall sum even.
    *code = bcm2835_i2c_read (rbuf, 4);
    if (*code == 1)
    {
        *code = 3;
        fprintf (stderr, "\nError: Read from I2C Device Failed. (CODE: %d)\n", *code);
        return 0.0;
    }
    if ((get + errchk1 + errchk2) % 2 == 1)
    {
        if (rbuf[3] == (char) 0x00)
        {
            *code = 4;
            fprintf (stderr, "\nError: Parity Bit Set Incorrectly (SET EVEN WHEN SHOULD BE ODD) (CODE: %d)\n", *code);
        }
    } else {
        if (rbuf[3] == (char) 0x01)
        {
            *code = 4;
            fprintf (stderr, "\nError: Parity Bit Set Incorrectly (SET ODD WHEN SHOULD BE EVEN) (CODE: %d)\n", *code);
        }
    } 
    if (rbuf[2] != (char) errchk1 || rbuf[1] != (char) errchk2)
    {
        *code = 5;
        fprintf (stderr, "\nError: ERRCHK Values are Set Incorrectly. (CODE: %d)\n", *code);
    }
    if (*code != 0)
    {
        return 0.0;
    }

    unsigned float ret = ((unsigned int)rbuf[0])/10.0;
    return ret;
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
            unsigned float ret = i2c_get_sample (code);
            if (code)
            {
                fprintf (stderr, "Error: Read From i2c failed.\n");
            } else {
                // Get X and Y Pos and add to node
                scan_add (head, X_VALUE, Y_VALUE, ret);
            }

        }
    }
    bcm2835_gpio_set_eds(START_END_SCAN);
    *code = 0;
    return head;
}


int main (int argc, char** argv)
{
    time_t t;
    srand((unsigned) time(&t));
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

    if (!bcm2835_i2c_begin())
    {
        fprintf (stderr, "\nError: BCM2835 I2C BEGIN Failure\n(Are You Running As Root?)\n");
        return 1;
    }


    bcm2835_gpio_fsel(START_END_SCAN, BCM2835_GPIO_FSEL_INPT);
    //  with a pullup
    bcm2835_gpio_set_pud(START_END_SCAN, BCM2835_GPIO_PUD_UP);
    // And a low detect enable
    bcm2835_gpio_len(START_END_SCAN);

    bcm2835_gpio_fsel(TAKE_READING, BCM2835_GPIO_FSEL_INPT);
    //  with a pullup
    bcm2835_gpio_set_pud(TAKE_READING, BCM2835_GPIO_PUD_UP);
    // And a low detect enable
    bcm2835_gpio_len(TAKE_READING);

    bcm2835_i2c_setSlaveAddress(slave_address);
    bcm2835_i2c_setClockDivider(clk_div);

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