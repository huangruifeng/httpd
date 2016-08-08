#include"httpd.h"



void print_debug(const char* msg)
{
	#ifdef _DEBUG_
	printf("%s\n",msg);
	#endif
}

void print_log(const char *fun ,int line,int err_no,const char *err_str)
{
	printf("[%s:%d][%d][%s]",fun,line,err_no,err_str);
}

void clear_header(int client)
{
	char buf[1024];
	memset(buf,'\0',sizeof(buf));
	int ret = 0;
	do
	{
		ret = get_line(client,buf,sizeof(buf));
	}while(ret > 0 && strcmp(buf,"\n")!=0);
}

int get_line(int sock,char *buf,int max_len)
{
	if(!buf || max_len < 0)
	{

		return -1;
	}
	int i = 0;
	int n = 0;
	char c = '\0';
	while(i<max_len-1 && c !='\n')
	{
	
		n = recv(sock,&c,1,0);
		if(n > 0)
		{
			if(c == '\r')
			{
				n = recv(sock,&c,1,MSG_PEEK);
				if(n > 0 && c =='\n')	
				{
					recv(sock,&c,1,0);	
				}else
				{
					c = '\n';
				}
			}
			buf[i++] = c;
		}else
		{
			c = '\n';
		}
	}
		buf[i] = '\0';
		return i;
}
void echo_html(int client,const char*path,unsigned int file_size)
{
	if(!path)return;
	int in_fd = open(path,O_RDONLY);
	if(in_fd < 0)
	{
		print_debug("open index.html error");
		return ;
	}
	print_debug("send echo head success");
	if(sendfile(client,in_fd,NULL,file_size)< 0)
	{
		print_debug("sendfile error");
		close(in_fd);
		return ;
	}
	print_debug("sendfile success");
	close(in_fd);
}

void exe_cgi(int sock_client,const char*path,const char*method,const char * query_string)
{
	print_debug("enter cgi\n");
	char buf[_COMM_SIZE_];
	int numchars = 0;
	int content_length = 1;

	int cgi_input[2] = {0};
	int cgi_output[2] = {0};
	pid_t id ;
	print_debug(method);
	
	if(strcasecmp(method,"GET")==0)
	{
		clear_header(sock_client);
	}else
	{
		do{
			memset(buf,'\0',sizeof(buf));
			numchars = get_line(sock_client,buf,sizeof(buf));
			if(strncasecmp(buf,"Content-Length:",strlen("Content-Length")) == 0)
			{
				content_length = atoi(&buf[16]);
			}

		}while(numchars > 0 && strcmp(buf,"\n"));
		if(content_length == -1)
		{
			return ;
		}
	}
	memset(buf,'\0',sizeof(buf));
	strcpy(buf,HTTP_VERSION);
	strcat(buf,"200 OK\r\n\r\n");
	send(sock_client,buf,strlen(buf),0);
	
	if(pipe(cgi_input)== -1)
	{
		return;
	}
	if(pipe(cgi_output)== -1)
	{	
		close(cgi_input[0]);
		close(cgi_input[1]);
		return;
	}
	printf("fork begin\n");
	id = fork();
	if(id < 0 )
	{
		close(cgi_input[0]);
		close(cgi_output[0]);
		close(cgi_input[1]);
		close(cgi_output[1]);
		return ;
	}
	else if(id == 0)
	{
	printf("this is children\n");
		char query_env[_COMM_SIZE_/10];
		char method_env[_COMM_SIZE_];
		char content_len_env[_COMM_SIZE_];
		memset(query_env,'\0',sizeof(query_env));
		memset(method_env,'\0',sizeof(method_env));
		memset(content_len_env,'\0',sizeof(content_len_env));
		
		close(cgi_input[1]);
		close(cgi_output[0]);

		dup2(cgi_input[0],0);
		dup2(cgi_output[1],1);
		
		sprintf(method_env,"REQUEST_METHOD=%s",method);
		putenv(method_env);
		if(strcasecmp("GET",method)==0)//get
		{
			sprintf(query_env,"QUERY_STRING=%s",query_string);
			putenv(query_env);
		}else
		{
		char post_data[4096];
		memset(post_data,'\0',sizeof(post_data));
		 
			int i = 0;
			for(;i<content_length;i++)
			{
				read(0,&post_data[i],1);
			}
			post_data[i] = '\0';
			sprintf(query_env,"QUERY_STRING=%s",post_data);
			putenv(query_env);
	
		
		}
		execl(path,NULL);
		exit(1);
	}else
	{
		printf("this is father\n");
		close(cgi_input[0]);
		close(cgi_output[1]);
		
		int i = 0;
		char c = '\0';
		if(strcasecmp("POST",method)==0)
		{
			for(;i<content_length;i++)
			{
				recv(sock_client,&c,1,0);
				write(cgi_input[1],&c,1);
			}
		}
		while(read(cgi_output[0],&c,1)>0)
		{
			send(sock_client,&c,1,0);
		}
		close(cgi_input[1]);
		close(cgi_output[0]);
		waitpid(id,NULL,0);
	}

}

void* accept_request(void *arg)
{
	print_debug("get a new connect...\n");
	pthread_detach(pthread_self());
	int sock_client = (int)arg;
	
	int cgi = 0;
	char* query_string = NULL;
	char method[_COMM_SIZE_/10];
	char url[_COMM_SIZE_];
	char buffer[_COMM_SIZE_];
	char path[_COMM_SIZE_];
	memset(method,'\0',sizeof(method));
	memset(url,'\0',sizeof(url));
	memset(buffer,'\0',sizeof(buffer));
	memset(path,'\0',sizeof(path));
	if(get_line(sock_client,buffer,sizeof(buffer))<0)
	{//it is no message
		print_debug("close connect...");
		return NULL;
	}

	int i = 0;
	int j = 0;
	while(!isspace(buffer[j])&&i<sizeof(method)-1&&j<sizeof(buffer))
	{
		method[i] = buffer[j];
		i++;j++;
	}
	
	if(strcasecmp(method,"GET")&& strcasecmp(method,"POST"))
	{//request method is not know;
		print_debug("close connect...1");
		return NULL;
	}
	
	while(isspace(buffer[j])&&j <sizeof(buffer)){
		j++;
	}
	i = 0;
////
	while(!isspace(buffer[j]) &&\
		 i< sizeof(url)-1 &&\
		 j<sizeof(buffer))
	{	url[i] = buffer[j];
		i++;j++;
	}
	print_debug(method);

	print_debug(url);

	if(strcasecmp(method,"POST")== 0)
	{
		cgi = 1;
	}
	if(strcasecmp(method,"GET")==0)
	{
		query_string = url;

		while(*query_string !='?'&&*query_string != '\0')
		{
			query_string++;
		}	
		if(*query_string =='?')
		{
			*query_string = '\0';
			query_string++;
			cgi = 1;
		}
	}
	sprintf(path,"htdocs%s",url);
	if(path[strlen(path)-1]=='/')
	{
		strcat(path,MAIN_PAGE);
	}
	print_debug(path);
	system("pwd");
	struct stat st;
	if(stat(path,&st)< 0)
	{//not find file
		print_debug("miss cgi");
		clear_header(sock_client);

	}else
	{

		 if(S_ISDIR(st.st_mode))
		{
			strcat(path,"/");
			strcat(path,MAIN_PAGE);
		}else if(st.st_mode & S_IXGRP || st.st_mode & S_IXUSR \
						|| st.st_mode & S_IXOTH)
		{
			cgi = 1;
		}else
		{
		
		}

		if(cgi)
		{
			exe_cgi(sock_client,path,method,query_string);
		}else
		{
			clear_header(sock_client);
			print_debug("begin enter our echo_html");
			echo_html(sock_client,path,st.st_size);
		}
	}
	close(sock_client);
	print_debug("close connect...1");
	return NULL;

}

int start(short port)
{
	int listen_sock = socket(AF_INET,SOCK_STREAM,0);
	if(listen_sock == -1)
	{
		print_log(__FUNCTION__,__LINE__,errno,strerror(errno));
		exit(1);
	}
	
	int flag =1;
	setsockopt(listen_sock,SOL_SOCKET,SO_REUSEPORT,&flag,sizeof(flag));
	

	struct sockaddr_in local;
	local.sin_family = AF_INET;
	local.sin_port = htons(port);
	local.sin_addr.s_addr = htonl(INADDR_ANY);
	socklen_t len = sizeof(local);
	
	if(bind(listen_sock,(struct sockaddr*)&local,len) == -1)
	{
		print_log(__FUNCTION__,__LINE__,errno,strerror(errno));
		exit(2);
	}

	if(listen(listen_sock,_BACK_LOG_) == -1)
	{	
		print_log(__FUNCTION__,__LINE__,errno,strerror(errno));
		exit(3);
	}

	return listen_sock;
}

int main(int argc,char*argv[])
{
	if(argc != 2)
	{
		printf("Usage is %s[port]\n",argv[0]);
		exit(1);
	}

	int port = atoi(argv[1]);
	int sock = start(port);

	struct sockaddr_in client;
	socklen_t len = 0;
	
	while(1)
	{
		int new_sock = accept(sock,(struct sockaddr*)&client,&len);
		if(new_sock < 0)
		{
			print_log(__FUNCTION__,__LINE__,errno,strerror(errno));
			continue;
		}
		
		pthread_t  new_thread;
		pthread_create(&new_thread,NULL,accept_request,(void*)new_sock);
	}
	return 0;
}
