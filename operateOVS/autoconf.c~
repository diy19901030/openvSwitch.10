#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cJSON.h"

/*
char *path_brvnid2 = "/home/zzq/openvswitch-1.10.0/operateOVS/brvnid";
char *path_tbrvnid2 = "/home/zzq/openvswitch-1.10.0/operateOVS/t_brvnid";
char *dest_ip1 = "/home/zzq/openvswitch-1.10.0/operateOVS/vnidip";
char *path_tmp_ip = "/home/zzq/openvswitch-1.10.0/operateOVS/ip_write";
*/

char *path_tbrvnid;
char *path_brvnid;
char *path_tmp_ip;


char* get_nve_ip(char *filename,char *vnid,int k);
int check_port(char *port);
char *create_port();
void update_vnid_ip(char *file_br,char *file_ip);
//void doit(char *text);

void main(){

	
	char comd[30],comd1[50],comd2[50],comd3[50],id[8];
	char buffer[100];
	int num,f1,f2;
	
//	dispay_nve(path_tmp_ip);

	get_br_vnid();
	
	write_br_vnid(path_tbrvnid,path_brvnid);
//	sleep(1);
//	write_vnidip(dest_ip1,path_brvnid2);
//	sleep(2);
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
	if(!j_child) return;
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
			char *tip;
			tip=(char *)malloc(sizeof(char));
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
				char *port;
				port=(char *)malloc(sizeof(char));
				cJSON *ip1=cJSON_GetArrayItem(j_child,i);
				tip=cJSON_Print(ip1);
//				printf("---tip is %s\n",tip);
			flag=0;	
//			printf("the length of tip is %d\n",strlen(tip));
			fseek(f,0,SEEK_SET);
			memset(br,0,sizeof(br));
			while(fgets(buffer,100,f)!=NULL){
				if(strstr(buffer,tip)) {flag=1;/*printf("remote_ip=%s的端口已存在！\n",tip);*/break;}
	
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
		
		sprintf(comd3,"ovs-vsctl -- set interface %s options:remote_ip=%s",port,tip);
		system(comd3);
		printf("VN %s 新增加成员，并已配置端口: %s		remote_ip=%s\n",t_id,port,tip);
//		printf("tip is %s\n",tip);
		free(port);
		
		}//for
		free(tip);
		}//if
		
		cj_child=cj_child->next;
		}				

		j_child=j_child->next;
}

	free(data);
	free(cdata);
	fclose(fa);
	fclose(fb);
//	update_br(path_brvnid,path_tmp_ip);
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
	t_id=(char *)malloc(sizeof(char));
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
			memset(t,0,sizeof(t));
			sprintf(t,"ovs-vsctl del-port %s",t_port);
//			system(t);	
//			if(!strlen(tt_ip)) system(t);
			if(strlen(tt_ip)){
//				printf("---%s %s\n",t_port,tt_ip);
				cJSON *j_ip=cJSON_GetObjectItem(cj,t_id);
				if(!j_ip) {continue;}
				int size,i;
				char ip[20];
				size=cJSON_GetArraySize(j_ip);
				for(i=0;i<size;i++){
					
					memset(ip,0,sizeof(ip));
//					char *ip;
//					ip=(char *)malloc(sizeof(char));
					cJSON *jt_ip=cJSON_GetArrayItem(j_ip,i);
//					printf("jt_ip is %s\n",jt_ip->valuestring);
					strcpy(ip,jt_ip->valuestring);
//					printf("ip is %s\n",ip);
					if(!strcmp(ip,tt_ip)) {flag=1;break;}	
//					free(ip);		
				}
				if(!flag) {
					printf("VN %s中 remote_ip %s的主机已退出网络\n",t_id,tt_ip);
					flag=0;
//					memset(t,0,sizeof(t));
//					sprintf(t,"ovs-vsctl del-port %s",t_port);
					system(t);					
					printf("port %s del!\n",t_port);
					
				}
				
			}
			
			
			}//if
		}
	j_child=j_child->next;
	}

	fclose(fa);
	fclose(fb);
	return;
}



void update_br(char *file_br,char *file_ip){

	printf("update_br\n");
	char comd4[50];
	FILE *fa=fopen(file_br,"rb+");fseek(fa,0,SEEK_END);long len=ftell(fa);fseek(fa,0,SEEK_SET);
	FILE *fb=fopen(file_ip,"rb+");fseek(fb,0,SEEK_END);long len1=ftell(fb);fseek(fb,0,SEEK_SET);
	char *data=(char*)malloc(len+1);fread(data,1,len,fa);
	char *cdata=(char*)malloc(len1+1);fread(cdata,1,len1,fb);
	cJSON *json=cJSON_Parse(data);
	cJSON *cjson=cJSON_Parse(cdata);
	cJSON *j=cJSON_GetObjectItem(json,"brvnid");
	
	
	cJSON *j_child=j->child;
	while(j_child!=NULL){
		cJSON *cj=cJSON_GetObjectItem(cjson,"vnidip");
		cJSON *cj_child=cj->child;
		while(cj_child!=NULL){
//			printf("j_child->string is %s\n",j_child->string);
//			printf("cj_child->string is %s\n",cj_child->string);
			if(strcmp(j_child->valuestring,cj_child->string)!=0) 
			{
				char tbr[6];
				memset(tbr,0,sizeof(tbr));
				strcpy(tbr,j_child->string);
				memset(comd4,0,sizeof(comd4));
				sprintf(comd4,"ovs-vsctl del-br %s",tbr);
				printf("tbr is %s\n",tbr);
				printf("comd4 is %s\n",comd4);			
				system(comd4);
								
			}
			cj_child=cj_child->next;		
		}
		j_child=j_child->next;
	}
	free(data);
	free(cdata);
	fclose(fa);
	fclose(fb);
	
}


int check_port(char *port){
	char comd[70],buffer[100],*p=NULL;
	sprintf(comd,"ovs-vsctl show > checkport");
	system(comd);
	FILE *f=fopen("checkport","r");
	memset(buffer,0,sizeof(buffer));
//	printf("port is %s\n",port);
	while(fgets(buffer,100,f)!=NULL){
//	    printf("buffer is %s\n",buffer);
            p=strstr(buffer,port);
	    if(p) return 1;
     
}//while
	return 0;
}






