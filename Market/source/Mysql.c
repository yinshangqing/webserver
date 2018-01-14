/*************************************************************************
	> File Name: Mysql.c
	> Author: limeng
	> Mail: 1223402150@qq.com 
	> Created Time: 2018年01月07日 星期日 15时38分35秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "/usr/include/mysql/mysql.h"

int main(int argc,char *argv[])
{
	MYSQL my_connection,*con_ptr;
	MYSQL_RES *res_ptr;
	MYSQL_ROW sqlrow;
	int iRet;
	int iTableRow,iTableCol,i,j;
	char *server = "localhost";
	char *user = "root";
	char *password = "930806";
	//char *password = "941014";
	char *database = "Market";
	unsigned int iTimeOut = 6;

	con_ptr = mysql_init(&my_connection);
	if(!con_ptr)
	{
		fprintf(stderr,"mysql_init failed! \n");
		return EXIT_FAILURE;
	}

	iRet = mysql_options(&my_connection,MYSQL_OPT_CONNECT_TIMEOUT,(const char *)&iTimeOut);
	if(iRet)
	{
		fprintf(stderr,"Connect is Timeout! \n");
		return EXIT_FAILURE;
	}

	con_ptr = mysql_real_connect(&my_connection,server,user,password,database,0,NULL,0);
	if(con_ptr)
	{
		printf("Connection success! \n");
       
		iRet = mysql_query(&my_connection,"select * from user");
		if(iRet)
		{
			fprintf(stderr,"select error %d:%s ! \n",mysql_errno(&my_connection),mysql_error(&my_connection));
			return EXIT_FAILURE;
		}

		res_ptr = mysql_store_result(&my_connection);
		if(res_ptr)
		{
			iTableRow = mysql_num_rows(res_ptr);
			iTableCol = mysql_num_fields(res_ptr);

			for(i=0;i<iTableRow;i++)
			{
				sqlrow = mysql_fetch_row(res_ptr);
				for(j=0;j<iTableCol;j++)
				{
					printf("%-8s",sqlrow[j]);
				}
				printf("\n");
			}
			mysql_free_result(res_ptr);
		}
		mysql_close(&my_connection);
	}
	else
	{
		fprintf(stderr,"Connection failed! \n");
		if(mysql_errno(&my_connection))
		{
			fprintf(stderr,"Connection error %d:%s ! \n",mysql_errno(&my_connection),mysql_error(&my_connection));
		}
	}


	return EXIT_SUCCESS;
}
