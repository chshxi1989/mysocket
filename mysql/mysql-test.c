#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <mysql.h>
int getMysqlData(char* name, char *buffer)
{
    // connect to mysql server
    int ret = -1;
    MYSQL* mysql_main = NULL;
    MYSQL_RES *res_ptr = NULL;
    MYSQL_ROW sqlrow;
    int i = -1;
    unsigned int num_fields = 0;
    unsigned long* pLength = NULL;
    mysql_main = mysql_init(NULL);
    if(mysql_main == NULL)
    {
        printf("mysql init error\n");
        return -1;
    }

    // connect to 'test' database
    mysql_main = mysql_real_connect(mysql_main, "localhost", "root", "Chshxi611012", "test", 0, NULL, 0);
	if(mysql_main == NULL)
	{
		printf("connect to localhost failed: %s\n", mysql_error(mysql_main));
		return -1;
	}

    // run mysql command to get information from database
    #define MYSQL_COMMAND ("SELECT name,sex,age FROM myfamily WHERE age>5")
    printf("Connect to mysql test database success\n");
    ret = mysql_query(mysql_main, MYSQL_COMMAND);
    if(ret != 0)
    {
        printf("mysql command: %s execute failed: %s\n", MYSQL_COMMAND, mysql_error(mysql_main));
        return -1;
    }

    // get mysql resource
    res_ptr = mysql_use_result(mysql_main);
	if(res_ptr == NULL)
	{
		printf("mysql_use_result failed\n");
		return -1;
	}

    // get result data from mysql resource
    while((sqlrow = mysql_fetch_row(res_ptr)))
    {
        num_fields = mysql_num_fields(res_ptr);
        printf("num_fields: %d\n", num_fields);
        pLength = mysql_fetch_lengths(res_ptr);
        printf("length: %lu\n", *pLength);
        for(i = 0; i < num_fields; i++)
        {
            printf("rom[%i]: %s\n", i, sqlrow[i]);
        }
        printf("\n");
    }
    mysql_free_result(res_ptr);
    mysql_close(mysql_main);
    return 0;
}

int main(int argc, char** argv)
{
    int ret = -1;
	printf("[%s][%d][%s]\n", __FUNCTION__, __LINE__, __TIME__);
    getMysqlData(NULL, NULL);
    return 0;
}
