	int content_length = 1;
	char post_data[4096];
	char query_string[4096];
	char method[1024];
	memset(post_data,'\0',sizeof(post_data));
	memset(method,'\0',sizeof(method));
	memset(query_string,'\0',sizeof(query_string));
	 
	strcpy(method,getenv("REQUEST_METHOD"));
	if(strcasecmp("POST",method)==0)	
	{
		content_length = atoi(getenv("CONTENT_LENGTH"));
		int i = 0;
		for(;i<content_length;i++)
		{
			read(0,&post_data[i],1);
		}
		post_data[i] = '\0';
		sprintf(query_string,"QUERY_STRING=%s",post_data);
		putenv(query_string);
	}
	
	printf("failure\n");
		
