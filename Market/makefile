#makefile
c_mysql_Mysql:Mysql.c
	@gcc -lm -lpthread -ldl -I/usr/include/mysql Mysql.c -o Mysql -L/usr/lib64/mysql -lmysqlclient
	@gcc -lm -lpthread -ldl -I/usr/include/mysql Insert.c -o Insert -L/usr/lib64/mysql -lmysqlclient
clean:
	@rm -rf Mysql
	@rm -rf Insert
#makefile
