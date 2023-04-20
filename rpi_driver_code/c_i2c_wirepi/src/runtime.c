#include <wiringPi.h>
#include <mysql/mysql.h>
#include <wiringPiI2C.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/scanll.h"

#define START_SCAN 3
#define TAKE_READING 4


uint8_t dev_id = 0x08;

static char* host = "localhost";
static char* user = "root";
static char* pass = "";
static char* dbname = "soil-scans";

unsigned int port = 0;
static char* unix_socket = NULL;
unsigned int flag = 0;

// ISRs
uint8_t isr = 0; 
void start_isr (void) {isr |= 1};
void read_isr (void) {isr |= 2};


// MySQL Functions
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
void sql_commit (MYSQL *conn, SCAN* inscan)
{
    mysql_query (conn, "SELECT * FROM `soil-scans` WHERE `phdata`=\"NULL\"");
    MYSQL_RES* result mysql_store_result (conn);

    if (result == NULL)
    {
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

// Scan Functions
float i2c_sample(int fd)
{
    while (wiringPiI2CWrite(fd, 8) < 0)
    {
        printf ("I2C Write Error\n");
    }

    delay (15000);

    int ret = wiringPiI2CRead(fd);
    while (ret < 0)
    {
        printf ("I2C Read Error\n");
        ret = wiringPiI2CRead(fd);
    }

    float fret = (ret)/10.0;
    return fret;
}
SCAN* complete_scan (int fd)
{
    SCAN* head = scan_create();
    while(!(isr & 1))
    {

        if(isr & 2)
        {
            float ret = i2c_sample (fd);
            scan_add (head, 0, 0, ret);
            isr &= (255 - 2);
        }
    }
    isr &= (255 - 1);
    return head;
}

int main (int argc, char** argv)
{

    conn = mysql_init (NULL);
    
    if(!mysql_real_connect (conn, host, user, pass, dbname, port, unix_socket, flag))
    {
        fprintf (stderr, "\nError: %s [%d]\n", mysql_error(conn), mysql_errno(conn));
        return 1;
    }
    
    if (!wiringPiSetup())
    {
        fprintf (stderr, "\nError: WiringPi INIT Failure\n(Are You Running As Root?)\n");
        return 1;
    }

    int fd = wiringPiI2CSetup(dev_id);
    if (fd == -1)
    {
        fprintf (stderr, "\nError: WiringPi I2C BEGIN Failure\n(Are You Running As Root?)\n");
        return 1;
    }

    pinMode (START_SCAN, INPUT);
    pullUpDnControl (START_SCAN, PUD_UP);
    wiringPiISR (START_SCAN, INT_EDGE_FALLING, &start_isr);

    pinMode(TAKE_READING, INPUT);
    pullUpDnControl (TAKE_READING, PUD_UP);
    wiringPiISR (START_SCAN, INT_EDGE_FALLING, &read_isr);

    while (1)
    {
        if (isr & 1)
        {
            isr &= (255-1);
            if (verify_frame(conn))
            {
                fprintf (stderr, "Error: No SQL Frame Detected, Ignoring Scan Button. Please Create a Frame.\n");
            } else {
                SCAN* ret = complete_scan (fd);
                sql_commit (conn, ret); 
            }
        }
        delay (500);
    }
    return 0;
}