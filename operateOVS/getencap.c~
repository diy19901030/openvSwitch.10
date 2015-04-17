#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cJSON.h"

char *path_encap;

void getencap(char *ch){

	char *out;
	FILE *t=fopen(path_encap,"w+");	
	cJSON *json=cJSON_CreateObject();
	cJSON *add=cJSON_CreateObject();
	cJSON_AddItemToObject(json,"ipencap",add);
	cJSON *j=cJSON_GetObjectItem(json,"ipencap");
//	printf("ch is %s\n",ch);
	if(!strcmp(ch,"1")){
		cJSON *vxlan=cJSON_CreateArray();
		cJSON_AddItemToObject(j,"vxlan",vxlan);
	}
	else if(!strcmp(ch,"2")){
		cJSON *gre=cJSON_CreateArray();
		cJSON_AddItemToObject(j,"nvgre",gre);
	}
	if(!strcmp(ch,"3")){
		cJSON *mpls=cJSON_CreateArray();
		cJSON_AddItemToObject(j,"mpls_in_udp",mpls);
	}
	out=cJSON_Print(json);
//	printf("json is %s\n",out);
	fwrite(out,strlen(out),sizeof(char),t);
	fclose(t);

}

void display_encap(char *filename)
{
	FILE *fa=fopen(filename,"r+");
	if(!fa) {printf("encap file open error!\n");return;}
	fseek(fa,0,SEEK_END);long length=ftell(fa);fseek(fa,0,SEEK_SET);
	char *data=(char*)malloc(length+1);fread(data,1,length,fa);
	char *ip;
	ip=(char *)malloc(sizeof(char));
	
//	char *out;
	cJSON *json=cJSON_Parse(data);
//	add=cJSON_CreateString("vnidip");
	cJSON *j=cJSON_GetObjectItem(json,"ipencap");
	cJSON *j_child=j->child;
//	printf("type is %d\n",j_child->type);
	printf("IP			封装类型\n");
	ip=get_ip();
	printf("%s		vxlan\n",ip);
	while(j_child!=NULL){
		
		
//		cJSON *array=cJSON_GetObjectItem(j_child,j_child->string);
		int size=cJSON_GetArraySize(j_child);
		int i;
		for(i=0;i<size;i++) {
			printf("%s",j_child->string);
			printf("		%s\n",cJSON_GetArrayItem(j_child,i)->valuestring);
		}
		j_child=j_child->next;
	}
//	out=cJSON_Print(json);
//	fwrite(out,strlen(out),1,fa);
	free(data);
	fclose(fa);

}

