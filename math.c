#include<stdio.h>
#include<string.h>
#include<stdlib.h>


void math(const char *data_string)
{
	if(!data_string)
	{
		return ;
	}
	char* data1 = NULL;
	char* data2 = NULL;
	char* sysbol= NULL;
	 char*start = data_string;
	while(*start!='\0')
	{
		if(*start == '='&& data1 == NULL)
		{
			data1 = start+1;
			start++;
			continue;
		}
		if(*start = '&')
		{
			*start = '\0';
		}
		if(*start == '='&&data1 !=NULL && sysbol ==NULL)
		{
			sysbol = start+1;
			start++;
			continue;
		}
		if(*start == '='&& sysbol !=NULL )
		{
			data2 = start+1;
			break;
		}
		start++;
	}
	int int_data1 = atoi(data1);
	int int_data2 = atoi(data2);
	int res_data = 0;
/*	switch(sysbol[0])
	{
		case 1:
		res_data = int_data1+ int_data2;
			break;
		case 2:
		res_data = int_data1- int_data2;
			break;
		case 3:
		res_data = int_data1* int_data2;
			break;
		case 4:
		res_data = int_data1/ int_data2;
			break;
		case 5:
		res_data = int_data1% int_data2;
			break;
	}
	printf("<centle><p><h1>math result is:%d</h1></p></centle>\n",res_data);
}*/



int main()
{
	int content_length = 1;
	char method[1024];
	char query_string[1024];
	char post_data[4096];
	
	memset(method,'\0',sizeof(method));
	memset(query_string,'\0',sizeof(query_string));
	memset(post_data,'\0',sizeof(post_data));
	
	printf("<html>\n<head>\n<title>result</title>\n</head>\n");
	printf("<body>\n");
	strcpy(method,getenv("REQUEST_METHOD"));
	if(strcasecmp("GET",method)==0)
	{	printf("<p>hello</p>");
		strcpy(query_string,getenv("QUERY_STRING"));
	//	math(query_string);
	}else if(strcasecmp("POST",method)==0)
	{
		content_length = atoi(getenv("CONTENT_LENGTH"));
		int i = 0;
		for(;i<content_length;i++)
		{
			read(0,&post_data[i],1);
		}
		post_data[i] = '\0';
	//	math(post_data);
	}else{

		}
	printf("method is :%s<br>",method);
	printf("query_strinf is :%s",query_string);
	printf("</body\n>\n</html>");
	
	return 0;






}
