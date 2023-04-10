#include <bcm2835.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    // Probably Should Pull Some Line High, In Order to have a BUSY LED, which lets User know when measurement is in progress.


    // Return Codes For Func.
    // 0 = Everything Worked Correctly
    // 3 = Read Fail
    // 6 = Ardiuno Failed Read from pH Sensor, everything else worked fine.


    // Unneccessary
    /*
    // GET = HEXCODE FOR GET COMMAND
    // ERRCHK1 = 8BIT RNDM NUM to be Returned
    // ERRCHK2 = BITNOT ERRCHK1 also to be returned.
    // PARITY = 1 if overall sum odd, 0 if overall sum even.
    // {0x00 0x00 0x00 0x00} = Len 4.
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
    }*/

    for (int i=0; i<MAX_LEN; i++)
    {
        rbuf[i] = 'n';
    }

    // Return should be
    // RETURN = HEXCODE FOR RTN COMMAND
    *code = bcm2835_i2c_read (rbuf, 4);
    if (*code == 1)
    {
        *code = 3;
        fprintf (stderr, "\nError: Read from I2C Device Failed. (CODE: %d)\n", *code);
        return 0.0;
    }
    if (rbuf[0] == (char) 0x00 || rbuf[0] == (char) 0xff)
    {
        *code = 6;
        fprintf (stderr, "\nError: Arduino Didn't Read from Sensor Correctly. (CODE: %d)\n", *code);
        return 0.0;
    }

    // Unsure how sensor responds
    unsigned float ret = ((unsigned int)rbuf[0])/10.0;
    return ret;
}

int verify_frame (MYSQL *conn)
{
    mysql_query (conn, "SELECT * FROM `soil-scans` WHERE `phdata`=\"NULL\"");
    MYSQL_RES* result mysql_store_result (conn);

    if (result != NULL)
    {
        return 0;
    }
    return 1;
}

void sql_commit (MYSQL *conn, SCAN* inscan, int *code)
{
    mysql_query (conn, "SELECT * FROM `soil-scans` WHERE `phdata`=\"NULL\"");
    MYSQL_RES* result mysql_store_result (conn);

    if (result == NULL)
    {
        *code = 10;
        return;
    }

    int num_fields = mysql_num_fields (result);
    MYSQL_ROW row = mysql_fetch_row (result);

    char ph_data[200] = "";
    
    int scan_id = atoi(row[0]);
    char quaddat[100] = row[3];
    int quadcoords[8];

    char* tok;
    int id = 0;
    tok = strtok (quaddat, ' ');
    while (tok != NULL)
    {
        quadcoords[id] = atoi(tok);
        id++;
        tok = strtok (NULL, ' ');
    }

    int max = 0;
    int min = 10000000; 

    for (int i=0; i<4; i++)
    {
        if (quadcoords[2*i] > max)
        {
            max = quadcoords[2*i];
        }
        if (quadcoords[2*i] < min)
        {
            min = quadcoords[2*i];
        }
    }

    int width = max - min;
    int amnt_squares = width / 50;

    // idx of the form (amnt_square * Y) + X
    // Sort by this, then just strcat based upon LL
    // TODO SORT FUNCTION


    SCAN_NODE* curr;
    char buff[50];
    curr = inscan->head;
    for (int i=0; i<inscan->length; i++)
    {
        sprintf (buff, "%f ", curr->ph);
        strcat (ph_data, buff);
        curr = curr->next;
    }

    char qbuff[200];
    sprintf (qbuff, "UPDATE `soil-scans` SET `phdata`=\"%s\" WHERE `id`=%d;", ph_data, scan_id);
    mysql_query (conn , qbuff);
}

SCAN* complete_scan (int *code)
{
    SCAN* head = scan_create();
    while (!bcm2835_gpio_eds(START_END_SCAN))
    {
        bcm2835_gpio_set_eds (START_END_SCAN);
        if (bcm2835_gpio_eds (TAKE_READING))
        {
            
            unsigned float ret = i2c_get_sample (code);
            if (code)
            {
                fprintf (stderr, "Error: Read From i2c failed.\n");
            } else {
                // Get X and Y Pos and add to node
                scan_add (head, X_VALUE, Y_VALUE, ret);
            }
            bcm2835_gpio_set_eds (TAKE_READING);
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
    // And a fall edge detect enable
    bcm2835_gpio_afen(START_END_SCAN);

    bcm2835_gpio_fsel(TAKE_READING, BCM2835_GPIO_FSEL_INPT);
    //  with a pullup
    bcm2835_gpio_set_pud(TAKE_READING, BCM2835_GPIO_PUD_UP);
    // And a falling edge detect enable
    bcm2835_gpio_afen(TAKE_READING);

    bcm2835_i2c_setSlaveAddress(slave_address);
    bcm2835_i2c_setClockDivider(clk_div);

    while (1)
    {
        if (bcm2835_gpio_eds(START_END_SCAN))
        {
            // Now clear the eds flag by setting it to 1
            bcm2835_gpio_set_eds(START_END_SCAN);
            if (verify_frame(conn))
            {
                fprintf (stderr, "Error: No SQL Frame Detected, Ignoring Scan Button. Please Create a Frame for the Scan to Bind to.\n");
            } else {

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
        }
 
        // wait a bit
        delay(500);
    }

    bcm2835_close();
    return 0;
}