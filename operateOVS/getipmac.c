#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include"cJSON.h"

void get_ip_mac(char *filename){

	FILE *fa=fopen(filename,"r");
	fseek(fa,0,SEEK_END);long length=ftell(fa);fseek(fa,0,SEEK_SET);
	char *data=(char*)malloc(length+1);fread(data,1,length,fa);
	char *out;
	cJSON *json=cJSON_Parse(data);
	cJSON *j=cJSON_GetObjectItem(json,"macip");
	cJSON *j_child=j->child;
	while(j_child!=NULL){
		int size=cJSON_GetArraySize(j_child);
		int i;
		for(i=0;i<size;i++) {
			printf("%s",j_child->string);                       
			printf("		%s\n",cJSON_GetArrayItem(j_child,i)->valuestring);
		}
		j_child=j_child->next;
	}
	free(data);
	fclose(fa);
}
