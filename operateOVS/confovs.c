#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cJSON.h"
//#include"ovs.h"
/*
char *path_tmp_ip = "/home/zzq/openvswitch-1.10.0/operateOVS/ip_write";

char *path_brvnid1 = "/home/zzq/openvswitch-1.10.0/operateOVS/brvnid";
*/
extern char *path_tmp_ip;
extern char *path_brvnid;
char* get_nve_ip(char *filename,char *vnid,int k);
int check_port(char *port);
char *create_port();
void update_vnid_ip(char *file_br,char *file_ip);
//void doit(char *text);

void conf_ovs(){
	char *ip,*port;
	ip=(char *)malloc(sizeof(char));

	
	char comd[30],comd1[50],comd2[50],comd3[50],id[8];
	char buffer[100];
	int num,f1,f2;
	
	FILE *fa=fopen(path_tmp_ip,"rb+");fseek(fa,0,SEEK_END);long len=ftell(fa);fseek(fa,0,SEEK_SET);
	FILE *fb=fopen(path_brvnid,"rb+");fseek(fb,0,SEEK_END);long clen=ftell(fb);fseek(fb,0,SEEK_SET);
	int flag,fl;
	char *data=(char*)malloc(len+1);fread(data,1,len,fa);
	char *cdata=(char *)malloc(clen+1);fread(cdata,1,clen,fb);
	cJSON *json=cJSON_Parse(data);
	cJSON *cjson=cJSON_Parse(cdata);
	cJSON *j=cJSON_GetObjectItem(json,"vnidip");
	cJSON *cj=cJSON_GetObjectItem(cjson,"brvnid");
	cJSON *j_child=j->child;
	cJSON *cj_child=cj->child;
	
	while(j_child!=NULL){
		
		memset(id,0,sizeof(id));
		char br[6],t_br[6],t_id[6];
//		printf("j_child is %s\n",cJSON_Print(j_child));
		cj_child=cj->child;
		while(cj_child!=NULL){
//		printf("%s\n",cJSON_Print(cj_child));
		if(!strcmp(j_child->string,cj_child->valuestring)){

//			br=(char *)malloc(sizeof(char));
			strcpy(id,j_child->string);
			strcpy(t_id,id);
//			printf("id is %s\n",id);
//			cJSON *iparray=cJSON_GetObjectItem(j_child,id);
			int size,i;
			char *t_ip;
			t_ip=(char *)malloc(sizeof(char));
			size=cJSON_GetArraySize(j_child);
//			printf("size is %d\n",size);
			strcpy(br,cj_child->string);
			strcpy(t_br,br);
//			printf("在VN %s中 ： \n",t_id);
//			sprintf(comd,"/home/zzq/openvswitch-1.10.0/operateOVS/%s",t_br);
//			printf("comd is %s\n",comd);
			
			FILE *f=fopen(t_br,"r");
			if(!f) printf("file open error!\n");
			memset(buffer,0,sizeof(buffer));
			for(i=0;i<size;i++){
				port=(char *)malloc(sizeof(char));
				cJSON *ip1=cJSON_GetArrayItem(j_child,i);
				t_ip=cJSON_Print(ip1);
//				printf("---t_ip is %s\n",t_ip);
			flag=0;	
//			printf("the length of t_ip is %d\n",strlen(t_ip));
			fseek(f,0,SEEK_SET);
			memset(br,0,sizeof(br));
			while(fgets(buffer,100,f)!=NULL){
				if(strstr(buffer,t_ip)) {flag=1;/*printf("remote_ip=%s的端口已存在！\n",t_ip);*/break;}
	
			}//while
			
		if(flag) continue;
loop:		port=create_port();
		fl=0;
		fl=check_port(port);
		if(fl) goto loop;
//		printf("port is %s\n",port);
		sprintf(comd1,"ovs-vsctl add-port %s %s",t_br,port);
		system(comd1);
//		printf("t_id is %s\n",t_id);
		sprintf(comd2,"ovs-vsctl -- set interface %s type=vxlan options:key=%s",port,t_id);
		system(comd2);
		
		sprintf(comd3,"ovs-vsctl -- set interface %s options:remote_ip=%s",port,t_ip);
		system(comd3);
//		printf("1111\n");
		printf("VN %s 新增加成员，并已配置端口: %s		remote_ip=%s\n",t_id,port,t_ip);
//		printf("t_ip is %s\n",t_ip);
		
		free(port);
		
		}//for
		free(t_ip);
		}//if
		
		cj_child=cj_child->next;
		}				

		j_child=j_child->next;
}

	free(data);
	free(cdata);
	free(ip);
//	free(t_ip);
	fclose(fa);
	fclose(fb);
	update_vnid_ip(path_brvnid,path_tmp_ip);
	return;
	
}



char *create_port(){

	srand(((unsigned)time(0)));
	char *s;
	s=(char *)malloc(sizeof(char));
	while(1){
		sprintf(s,"%d",rand()%1000);
		if(strlen(s)==3)
		break;
	}
	return s;
}


void update_vnid_ip(char *file_br,char *file_ip){



	FILE *fa=fopen(file_br,"rb+");fseek(fa,0,SEEK_END);long len=ftell(fa);fseek(fa,0,SEEK_SET);
	FILE *fb=fopen(file_ip,"rb+");fseek(fb,0,SEEK_END);long len1=ftell(fb);fseek(fb,0,SEEK_SET);
	char *data=(char*)malloc(len+1);fread(data,1,len,fa);
	char *cdata=(char*)malloc(len1+1);fread(cdata,1,len1,fb);
	char *t_br,*t_id;
	char buffer[150];
	t_br=(char *)malloc(sizeof(char));
	if(!t_br) printf("t_br fail!\n");
	t_id=(char *)malloc(sizeof(char));
	if(!t_id) printf("t_id fail!\n");
	cJSON *json=cJSON_Parse(data);
	cJSON *cjson=cJSON_Parse(cdata);
	cJSON *j=cJSON_GetObjectItem(json,"brvnid");
	cJSON *cj=cJSON_GetObjectItem(cjson,"vnidip");
	cJSON *cj_child=cj->child;
	cJSON *j_child=j->child;
	while(j_child!=NULL){
		char t_buffer[100];
		memset(buffer,0,sizeof(buffer));
		strcpy(t_br,j_child->string);
		strcpy(t_id,j_child->valuestring);
//		printf("t_id is %s\n",t_id);
		int j,k;
		char t[40],t_port[10];
		char tt_ip[20];
		memset(buffer,0,sizeof(buffer));
//		printf("t_br is %s\n",t_br);
		sprintf(t,"ovs-vsctl list-ifaces %s > %sinf",t_br,t_br);
		system(t);
		memset(t,0,sizeof(t));
		sprintf(t,"%sinf",t_br);
		memset(t_br,0,sizeof(t_br));
		FILE *fc=fopen(t,"r");
		while(fgets(buffer,100,fc)!=NULL){
			j=0;k=0;
			while(buffer[j]!='\n') t_port[k++]=buffer[j++];
			t_port[k]='\0';
			memset(t,0,sizeof(t));
			sprintf(t,"ovs-vsctl get interface %s options",t_port);
			FILE *fd=popen(t,"r");
			memset(t_buffer,0,sizeof(t_buffer));
			fgets(t_buffer,sizeof(t_buffer),fd);
			pclose(fd);
//			printf("t_buffer is %s\n",t_buffer);
			memset(tt_ip,0,sizeof(tt_ip));
			if(strlen(t_buffer)!=3){
			j=0;k=0;
			k=strlen(t_id)+20;
			while(t_buffer[k]!='\0') tt_ip[j++]=t_buffer[k++];
			tt_ip[j-3]='\0';
//			printf("tt_ip length is %d\n",strlen(tt_ip));
			int flag=0;
			if(strlen(tt_ip)){
//				printf("---%s %s\n",t_port,tt_ip);
				cJSON *j_ip=cJSON_GetObjectItem(cj,t_id);
				int size,i;
//				char *ip;
//				ip=(char *)malloc(sizeof(char));
				size=cJSON_GetArraySize(j_ip);
				for(i=0;i<size;i++){
					char ip[20];
					memset(ip,0,sizeof(ip));
					
//					if(!ip) printf("ip fail!\n");
					cJSON *jt_ip=cJSON_GetArrayItem(j_ip,i);
//					printf("jt_ip is %s\n",jt_ip->valuestring);
					strcpy(ip,jt_ip->valuestring);
//					printf("ip is %s\n",ip);
//					printf("ip length is %d",strlen(ip));
					if(!strcmp(ip,tt_ip)) {flag=1;break;}
//					printf("1111\n");			
//					free(ip);
//					printf("2222\n");		
				}
//				free(ip);
				if(!flag) {
					printf("VN %s中 remote_ip %s的主机已退出网络\n",t_id,tt_ip);
					flag=0;
					memset(t,0,sizeof(t));
					sprintf(t,"ovs-vsctl del-port %s",t_port);
					system(t);					
					printf("port %s del!\n",t_port);
					
				}
				
			}
			
			
			}//if
		}
	j_child=j_child->next;
	}
	free(t_br);
	free(t_id);
	free(data);
	free(cdata);
//	free(ip);
	fclose(fa);
	fclose(fb);
	return;
}



int check_port(char *port){
	char comd[70],buffer[100],*p=NULL;
	sprintf(comd,"ovs-vsctl show > checkport");
	system(comd);
	FILE *f=fopen("checkport","r");
	if(!f) printf("file checkport open error\n");
	memset(buffer,0,sizeof(buffer));
//	printf("port is %s\n",port);
	while(fgets(buffer,100,f)!=NULL){
//	    printf("buffer is %s\n",buffer);
            p=strstr(buffer,port);
	    if(p) return 1;
     
}//while
	return 0;
}








