/*************************************************************************
	> File Name: Insert.c
	> Author: limeng
	> Mail: 1223402150@qq.com 
	> Created Time: 2018年01月07日 星期日 16时10分12秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include"/usr/include/mysql/mysql.h"

int main()
{
	MYSQL *con_ptr;
	int res;

	con_ptr = mysql_init(NULL);
	if(!con_ptr)
	{
		printf("mysql_init failed! \n");
		return EXIT_FAILURE;
	}

	con_ptr = mysql_real_connect(con_ptr,"localhost","root","930806","Market",0,NULL,0);
	//con_ptr = mysql_real_connect(con_ptr,"localhost","root","941014","Market",0,NULL,0);
	if(con_ptr)
	{
		res = mysql_query(con_ptr,"insert into user values(null,'jeffsui',5)");
		if(!res)
		{
			printf("Instert %lu rows\n",(unsigned long)mysql_affected_rows(con_ptr));
		}
		else
		{
			fprintf(stderr,"Insert error %d:%s\n",mysql_errno(con_ptr),mysql_error(con_ptr));
		}
	}
	else
	{
		printf("Connection failed!");
	}

	mysql_close(con_ptr);
	return EXIT_SUCCESS;
}
