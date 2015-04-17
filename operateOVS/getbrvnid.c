#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cJSON.h"
//#include"updateovs.h"
#include"initovs.h"
/*
char *path_br = "/home/zzq/openvswitch-1.10.0/operateOVS/brINfo";
char *path_brname = "/home/zzq/openvswitch-1.10.0/operateOVS/brName";
char *path_ttbrvnid = "/home/zzq/openvswitch-1.10.0/operateOVS/t_brvnid";
char *path_tbrvnid = "/home/zzq/openvswitch-1.10.0/operateOVS/brvnid";
*/
extern char *path_brname;
extern char *path_tbrvnid;
extern char *path_br;

cJSON *change_vnid(cJSON *json,const char *vnid);
void display_br_vnid(char *filename);
void write_br_vnid(char *src,char *dest);
void get_br_vnid(){
/*	if(access(path_brvnid,0)==-1){
		char *out;
		FILE *t=fopen(path_brvnid,"w+");	
		cJSON *json=cJSON_CreateObject();
		cJSON *add=cJSON_CreateObject();
		cJSON_AddItemToObject(json,"brvnid",add);
		out=cJSON_Print(json);
		printf("json is %s\n",out);
		fwrite(out,strlen(out),sizeof(char),t);
		fclose(t);
}*/
	char brname[20],buffer[128],t[128];
	system(" ovs-vsctl list-br > brName");
	system("ovs-vsctl show > brINfo");
	FILE *f=fopen(path_brname,"r+");
	FILE *fd=fopen(path_tbrvnid,"w+");
	memset(brname,0,sizeof(brname));
	while(fgets(brname,512,f)!=NULL){
		
		int len,flag=0;
		char *vnid=(char *)malloc(sizeof(char));
//		printf("brname is %s",brname);
		len=strlen(brname);
		brname[len-1]='\0';
//		printf("brname is %s\n",brname);
		FILE *fa=fopen(brname,"w");
		fclose(fa);
		sprintf(t,"    Bridge \"%s\"",brname);
//		printf("t is %s\n",t);
		FILE *fb=fopen(path_br,"r+");
		FILE *fc=fopen(brname,"r+");
			while(fgets(buffer,512,fb)!=NULL){
				if(flag) fwrite(buffer,strlen(buffer),1,fc);				
				if(strstr(buffer,t)) {
//					printf("11\n"); 
					flag=1;
//					printf("buffer is %s\n",buffer);
					fwrite(buffer,strlen(buffer),1,fc);}
				
				else if(strstr(buffer,"    Bridge ")) flag=0;
				
			}
		memset(t,0,sizeof(t));
		memset(buffer,0,sizeof(buffer));
		fseek(fc,0,SEEK_SET);
		flag=0;
		while(fgets(buffer,512,fc)!=NULL){
			if(flag) {  break;}
			if(strstr(buffer,"type: vxlan")) flag=1;		
		}

		int i=31,j=0,k=0;
		do{
			vnid[k++]=buffer[i];
			i++;

                }while(buffer[i]>='0'&&buffer[i]<='9');           
 

    		vnid[k]='\0';
//		printf("vnid is %s\n",vnid);
		if(!strlen(vnid)) {/*printf("this bridge does not contain vn network!\n");*/continue;}
		
		brname[strlen(brname)]='-';
		strcat(brname,vnid);
		brname[strlen(brname)]='\n';
//		printf("new brname is %s\n",brname);
		
		fwrite(brname,strlen(brname),1,fd);
//		fflush(fd);
//		
//		fseek(fd,0,SEEK_CUR);
//		free(data);
		free(vnid);
		fclose(fb);
		fclose(fc);
//		fclose(fd);
//		printf("---\n");
}
	fclose(f);
	fclose(fd);
//	write_br_vnid(path_ttbrvnid,path_tbrvnid);
//	display_br_vnid(path_tbrvnid);
	

}

int check_br(cJSON *json,const char *br){
	char *t_br=(char *)malloc(sizeof(char));
//	printf("vnid is %s\n",vnid);
	cJSON *j=cJSON_GetObjectItem(json,"brvnid");
	cJSON *j_child=j->child;
	if(j_child==NULL) {printf("j's child is null\n");return 0;}

//	printf("t_vnid is %s\n",t_vnid);
	while(j_child!=NULL){

		strcpy(t_br,j_child->string);
//		printf("\n%d\n",strcmp(t_vnid,vnid));
//		printf("vnid is %s\n",vnid);
		if(!strcmp(t_br,br)) {return 1;}
//		printf("j_child's next type is %d\n",j_child->type);		
		j_child=j_child->next;
		
		
	}
	return 0;
//	cJSON_Delete(j_child);

}

void display_br_vnid(char *filename){
	printf("网桥名称		网络号\n");
	FILE *fb=fopen(filename,"rb+");fseek(fb,0,SEEK_END);long len=ftell(fb);fseek(fb,0,SEEK_SET);
	char *data=(char*)malloc(len+1);fread(data,1,len,fb);
	cJSON *json=cJSON_Parse(data);
	cJSON *j=cJSON_GetObjectItem(json,"brvnid");
	cJSON *j_child;
	j_child=j->child;
	while(j_child!=NULL){
		printf("%s",j_child->string);
//		cJSON *j_ip=cJSON_GetArrayItem(j_child,0);
		printf("			%s\n",j_child->valuestring);
		j_child=j_child->next;
	}
	fclose(fb);



}

int check_br_vnid(cJSON *json,const char *vnid){

	char *t_vnid=(char *)malloc(sizeof(char));
//	printf("vnid is %s\n",vnid);
	cJSON *j=cJSON_GetObjectItem(json,"brvnid");
	cJSON *j_child=j->child;
	if(j_child==NULL) {printf("j's child is null\n");return 0;}

		printf("j_child vnid is %s\n",j_child->valuestring);
		strcpy(t_vnid,j_child->valuestring);
//		printf("\n%d\n",strcmp(t_vnid,vnid));
//		printf("vnid is %s\n",vnid);
		if(!strcmp(t_vnid,vnid)) {return 1;}
		else return 0;

}

cJSON *change_vnid(cJSON *json,const char *vnid){
	char *t_vnid=(char *)malloc(sizeof(char));
//	printf("vnid is %s\n",vnid);
	cJSON *j=cJSON_GetObjectItem(json,"brvnid");
	cJSON *j_child=j->child;
	if(j_child==NULL) {printf("j's child is null\n");return json;}

		
//		strcpy(t_vnid,j_child->string);
//		printf("\n%d\n",strcmp(t_vnid,vnid));
//		printf("vnid is %s\n",vnid);
	strcpy(j_child->valuestring,vnid);
	return json;
}

void write_br_vnid(char *src,char *dest){

	FILE *f=fopen(src,"r+");
//	FILE *fb=fopen(dest,"r+");
	FILE *fb=fopen(dest,"w+");	
	cJSON *json=cJSON_CreateObject();
	cJSON *add=cJSON_CreateObject();
	cJSON_AddItemToObject(json,"brvnid",add);
/*	fseek(fb,0,SEEK_END);long len=ftell(fb);fseek(fb,0,SEEK_SET);
	char *data=(char*)malloc(len+1);fread(data,1,len,fb);
	cJSON *json=cJSON_Parse(data);*/
	cJSON *cjson=cJSON_GetObjectItem(json,"brvnid");
	char buffer[128];
	char vnid[10],br[10],*out;
//	vnid=(char *)malloc(sizeof(char));
//	br=(char *)malloc(sizeof(char));
	memset(buffer,0,sizeof(buffer));
	memset(vnid,0,sizeof(vnid));
	memset(br,0,sizeof(br));
	while(fgets(buffer,512,f)!=NULL){

		int i=0,k;
		k=0;
//		printf("buffer is %s\n",buffer);
		while(buffer[i]!='-'&&buffer[i]!='\n'){
//			printf("buffer[%d] is %c\n",i,buffer[i]);
			br[k++]=buffer[i];
			i++;
		}
		//br[k]='\0';
//		printf("br is %s\n",br);
		k=0;
		i++;
		while(buffer[i]!='\0'&&buffer[i]!='\n'){
			vnid[k++]=buffer[i];
			i++;	
		}
		//vnid[k]='\0';
//		printf("vnid is %s\n",vnid);
		cJSON *id=cJSON_CreateString(vnid);
		cJSON_AddItemToObject(cjson,br,id);
	}
	out=cJSON_Print(json);
	fwrite(out,strlen(out),1,fb);
	
	fclose(f);
	fclose(fb);
	free(out);

}





