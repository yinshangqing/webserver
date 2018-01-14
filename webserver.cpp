/*************************************************************************
	> File Name: webserver.cpp
	> Author: yinshangqing
	> Mail: 841668821@qq.com 
	> Created Time: 2018年1月14日 星期日 20点06分09秒
************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <arpa/inet.h>
#include <iostream>
#include <fcntl.h>
using namespace std;

#define PORT 8081
#define BACKLOG 5
#define MAXDATASIZE 1000
#define MAXBUF 1024

void process_cli(int connectfd, sockaddr_in client);

int sendobj(int connectfd,char* serverfilepath);

int IsDIR(char* fpath);

int fileordirExist(char* fpath);

char* getextname(char*);

int writehead(FILE* cfp, char* extname);

void* start_routine(void* arg);

	void msg404(int connectfd);


struct ARG
{

       int connfd;

       sockaddr_in client;
};


int main()
{
      int listenfd, connectfd;

      //id of thread
      pthread_t thread;

      //pass this var to the thread
      ARG *arg;

      //server's address info
      struct sockaddr_in server;

      //client's address info
      struct sockaddr_in client;

      int sin_size;

      //create tcp socket
      if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
      {
          cout<<"creating socket failed!"<<endl;
          exit(1);
      }

      int opt = SO_REUSEADDR;

      setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

      bzero(&server,sizeof(server));

      server.sin_family = AF_INET;

      server.sin_port = htons(PORT);

      server.sin_addr.s_addr = INADDR_ANY;

      //bind...
      if(bind(listenfd,(struct sockaddr *)&server,sizeof(struct sockaddr)) == -1)
      {
          cout<<"bind error!"<<endl;
          exit(1);
      }

      cout<<"The Web Server is ruuning..."<<endl;

      if(listen(listenfd,BACKLOG) == -1)
      {
          cout<<"listen() error"<<endl;
          exit(1);
      }

      sin_size = sizeof(struct sockaddr_in);

      while(1)
      {
          if((connectfd = accept(listenfd,(struct sockaddr *)&client,(socklen_t*)&sin_size)) == -1)
          {
              cout<<"accept error!"<<endl;

          } else
          {
            cout<<"Accepting Request..."<<endl;
          }

          arg = new ARG;

          arg->connfd = connectfd;

          memcpy((void *)&arg->client, &client, sizeof(client));

          cout<<"Craeting thread..."<<endl;

          if(pthread_create(&thread, NULL, start_routine, (void*)arg))
          {
              perror("pthread_create() error");
              exit(1);
          }

      }

      close(listenfd);

      return 0;
}

//handle the request of the client
void process_cli(int connectfd, sockaddr_in client)
{
    int num;

    char requestline[MAXDATASIZE], filepath[MAXDATASIZE], cmd[MAXDATASIZE],extname[MAXDATASIZE];

    int c;

    FILE *fp;

    FILE *cfp;

    fp = fdopen(connectfd,"r");

    cout<<"The host is "<<inet_ntoa(client.sin_addr) <<endl;

    fgets(requestline,MAXDATASIZE,fp);

    cout<<"The request is "<<requestline<<endl;

    strcpy(filepath,"./");

    sscanf(requestline,"%s%s",cmd,filepath+2);

    strcpy(extname, getextname(filepath));

    cout<<"string comparing :::::::::::::start::::::::::::::: "<<endl;

    if(strcmp(cmd,"GET") == 0)
    {
        //is this a file or dir or notexist?
        if(fileordirExist(filepath))
        {
            //is this a dir
            if(IsDIR(filepath))
            {
                cout << "directory" << endl;
                if( fileordirExist( strcat(filepath,"index.htm") ))
                {
                    sendobj(connectfd,"index.htm");

                }
                else if(fileordirExist(strcat(filepath,"index.html")))
                {
                    sendobj(connectfd,"index.html");

                }
                else
                {
                    msg404(connectfd);
                }

            }
            else
            {
                //is a file
                cout << "file" << endl;
                sendobj(connectfd,filepath);
            }
        }
        else
        {
            cout<<"404 "<<endl;
            msg404(connectfd);
        }
    }

    cout<<":::::::::::::end::::::::::::::: "<<endl;

    close(connectfd);

}

//send the 404 error message to the client
void msg404(int connectfd)
{
    char* msg;

    msg  = "HTTP/1.0 404 Not Found Content-Type: text/plain 404 not found by Manio";

    send(connectfd,msg,strlen(msg),0);
}

//is the filepath a file  or directory
int fileordirExist(char* fpath)
{
    struct stat filestat;

    return (  stat(fpath,&filestat) != -1);
}

// is the filepath a directory
int IsDIR(char* fpath)
{
    struct stat filestat;

    return ( stat(fpath,&filestat) != -1 && S_ISDIR(filestat.st_mode));

}

//send the data of the file which the client want
int sendobj(int connectfd,char* serverfilepath)
{
    cout << "send object..." << endl;
    FILE *sfp, *cfp, *fp;

    char c;
    char buf_res[MAXBUF];
    int type, fd, n;

    sfp = fopen(serverfilepath,"rb");
    cfp = fdopen(connectfd,"wb");

    //write the header of respond message
    type = writehead(cfp,getextname(serverfilepath));
    fflush(cfp);

	/* type of text/html*/
	if(type==0 || type==3){
		if((fp=fopen(serverfilepath,"r"))<0){
			perror("open error");
			return 1;
		}

		bzero(buf_res, MAXBUF);
		while(fgets(buf_res,MAXBUF,fp)!=NULL){
			write(connectfd, buf_res, strlen(buf_res));
		}
	}

	/* type of image/jpg */
	else if(type==1){

		if((fd=open(serverfilepath,O_RDONLY))<0){
			perror("open error");
			return 1;
		}

		bzero(buf_res, MAXBUF);
		while((n=read(fd,buf_res,MAXBUF))>0){
			write(connectfd, buf_res, n);
			bzero(buf_res, n);
		}
	}

    return 0;
}

//write the packet header to the client
int writehead(FILE* cfp, char* extname)
{
    char* content = "text/plain";
    int type = 3;

    if( strcmp(extname,"html") == 0 || strcmp(extname,"htm") == 0)
    {
        content = "text/html";
        type  = 0;
    }
    else if ( strcmp(extname,"gif") == 0 )
    {
        content = "image/gif";
        type = 1;
    }
    else if ( strcmp(extname,"jpeg") == 0 || strcmp(extname,"jpg") == 0)
    {
        content = "image/jpeg";
        type = 1;
    }
    else if ( strcmp(extname,"png") == 0)
    {
        content = "image/png";
        type = 1;
    }
    else if ( strcmp(extname,"css") == 0 )
    {
        content = "text/css";
        type = 2;
    }


    cout<<"HTTP/1.0 200 OK "<<endl;

    cout<<"Content-Type: "<<content<<" "<<endl;

    fprintf(cfp,"HTTP/1.1 200 OK\r\n");

    fprintf(cfp,"Content-Type: %s\r\n\r\n",content);

    return type;

}

//get the extent name of the file
char* getextname(char* filepath)
{
    char* p;

    if(( p  =  strrchr(filepath,'.')) != NULL)
        return p+1;

    return NULL;
}



//invoked by pthread_create
void* start_routine(void* arg)
{

    ARG *info;

    info = (ARG *)arg;

    //handle client's requirement
    process_cli(info->connfd, info->client);

    delete arg;

    pthread_exit(NULL);
}

