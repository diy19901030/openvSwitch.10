#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cJSON.h"
//#include"ovs.h"

/*
char *path_vnidip = "/home/zzq/openvswitch-1.10.0/operateOVS/vnidip";
*/
cJSON *create_vnidip();

cJSON *create_vnidip(){
	cJSON *json,*add;
	json=cJSON_CreateObject();
	add=cJSON_CreateObject();
//	add=cJSON_CreateString("vnidip");
	cJSON_AddItemToObject(json,"vnidip",add);
	return json;
	
}





void write_vnidip(char *filename,char *path_brvnid)
{

	FILE *fb=fopen(filename,"w+");//fseek(fb,0,SEEK_END);long len=ftell(fb);fseek(fb,0,SEEK_SET);
//	char *data=(char*)malloc(len+1);fread(data,1,len,fb);
	FILE *fc=fopen(path_brvnid,"rb+");fseek(fc,0,SEEK_END);long length=ftell(fc);fseek(fc,0,SEEK_SET);
	char *cdata=(char*)malloc(length+1);fread(cdata,1,length,fc);
//	cJSON *j_get;
	char *vnid,*ip,*t_ip;
	char *out;
		
		ip=(char *)malloc(sizeof(char));
		if(!ip) printf("error!\n");
		t_ip=(char *)malloc(sizeof(char));
		vnid=(char *)malloc(sizeof(char));
		if(!vnid) printf("error!\n");
		cJSON *json=(cJSON *)malloc(sizeof(cJSON));
		json=create_vnidip();		
//		json=cJSON_Parse(data);
		cJSON *cjson=(cJSON *)malloc(sizeof(cJSON));
		if(!json) {printf("json error!\n");return;}
		cjson=cJSON_Parse(cdata);
		if(!cjson) {printf("json error!\n");return;}
		cJSON *cj=cJSON_GetObjectItem(cjson,"brvnid");
		cJSON *j=cJSON_GetObjectItem(json,"vnidip");
		
		ip=get_ip();
		strcpy(t_ip,ip);
		
		cJSON *addip=cJSON_CreateString(t_ip);
		cJSON *cj_child;
		cj_child=cj->child;
		
//		printf("%s\n",cJSON_Print(cj_child));
//		printf("1111111111\n");
		while(cj_child!=NULL){
			
//			memset(vnid,0,sizeof(vnid));
			strcpy(vnid,cj_child->valuestring);
//			printf("11\n");
//			printf("vnid is %s\n",vnid);
			if(check_vnid_ip(j,vnid)) {printf("same!\n");cj_child=cj_child->next;continue;}
			cJSON *j_ip=cJSON_CreateArray();
			cJSON_AddItemToObject(j,vnid,j_ip);
			cJSON *taskArry=cJSON_GetObjectItem(j,vnid);
			cJSON_AddItemToArray(taskArry, addip);
			cj_child=cj_child->next;

		}
//		free(vnid);
//		printf("json is %s\n",cJSON_Print(json));
		out=cJSON_Print(json);
		fseek(fb,0,SEEK_SET);
		fwrite(out,strlen(out),1,fb);
		free(out);
//		printf("1111\n");
		fclose(fc);
		fclose(fb);
//		free(data);
		
//		cJSON_Delete(json);
//		cJSON_Delete(cjson);
		free(cjson);
		free(json);
		free(cdata);
//		
//		cJSON_Delete(j);
//		cJSON_Delete(addip);
//		cJSON_Delete(cj_child);
		cj=NULL;
		cJSON_Delete(cj);
		
		
		cdata=NULL;
		vnid=NULL;
		
		free(vnid);
		ip=NULL;
		free(ip);
		t_ip=NULL;
		free(t_ip);
//		free(vnid);
		
//		free(vnid);
//		cJSON_Delete(json);
//		cJSON_Delete(cjson);
/*		
		if(vnid==NULL) printf("error\n");
		ip=(char *)malloc(sizeof(char));
		t_ip=(char *)malloc(sizeof(char));
//		t_vnid=(char *)malloc(sizeof(char));
		vnid=get_vnid_ip(path_brvnid,br);

		strcpy(t_vnid,vnid);
		free(vnid);vnid=NULL;
//		free(ip);ip=NULL;
		int i,arrySize=0;

		
		
//		printf("the length of ip is %d\n",strlen(t_ip));
//		printf("\nt_vnid is %d\n",strlen(t_vnid));
//		printf("check_vnid is %d\n",check_vnid(json,t_vnid));
		if(check_vnid(json,t_vnid)) {printf("the vnid %s has stored!\n",t_vnid);return ;}
		j=cJSON_GetObjectItem(json,"vnidip");
//		printf("old j is %s\n",cJSON_Print(j));
//		j_ip=(cJSON *)malloc(sizeof(cJSON));
		
//		cJSON *t=j->child->next;
//		printf("t is %s\n",t->string);


		arrySize=cJSON_GetArraySize(taskArry);
		int flag=0;
		for(i=0;i<arrySize;i++){
			j_get=cJSON_GetArrayItem(taskArry,i);

			if(!strcmp(j_get->valuestring,addip->valuestring)) flag=1;
		}
		if(flag) return json;
		
		

//		printf("new taskArry is %s\n",cJSON_Print(taskArry));
		out=cJSON_Print(json);
		printf("new json is %s\n",out);
//		cJSON_Delete(json);
//		free(t_ip);
//		free(ip);
		fseek(fb,0,SEEK_SET);
		fwrite(out,strlen(out),sizeof(char),fb);
		
		fclose(fb);
//		free(data);
*/	

}



void display_vnid_ip(char *filename){
	
	FILE *fb=fopen(filename,"rb+");fseek(fb,0,SEEK_END);long len=ftell(fb);fseek(fb,0,SEEK_SET);
	char *data=(char*)malloc(len+1);fread(data,1,len,fb);
	printf("VN		      IP\n");
	cJSON *json=cJSON_Parse(data);
	cJSON *j=cJSON_GetObjectItem(json,"vnidip");
	cJSON *j_child;
	j_child=j->child;
	while(j_child!=NULL){
		printf("%s",j_child->string);
		cJSON *j_ip=cJSON_GetArrayItem(j_child,0);
		printf("		%s\n",j_ip->valuestring);
		j_child=j_child->next;
	}
	fclose(fb);
	
	cJSON_Delete(json);
//	cJSON_Delete(j);
	free(data);
}


//filename1 is dest_ip(vnidip),filename2 is path_tmp_ip(ip_write)
void display_update_vnid_ip(char *filename1,char *filename2){
	
	char *vnid;
	char vn[30][10];
	int j,vn_count;

	FILE *fa=fopen(filename1,"r+");
	fseek(fa,0,SEEK_END);long length1=ftell(fa);fseek(fa,0,SEEK_SET);
	char *data1=(char*)malloc(length1+1);fread(data1,1,length1,fa);
	cJSON *json1=cJSON_Parse(data1);
	FILE *fb=fopen(filename2,"r+");
	fseek(fb,0,SEEK_END);long length2=ftell(fb);fseek(fb,0,SEEK_SET);
	char *data2=(char*)malloc(length2+1);fread(data2,1,length2,fb);
	cJSON *json2=cJSON_Parse(data2);
	cJSON *j2=cJSON_GetObjectItem(json2,"vnidip");
	cJSON *j_child2=j2->child;


//	add=cJSON_CreateString("vnidip");
	cJSON *j1=cJSON_GetObjectItem(json1,"vnidip");
	cJSON *j_child1=j1->child;
//	printf("type is %d\n",j_child->type);
	vn_count=0;
	while(j_child1!=NULL){
		
		vnid=j_child1->string;

		printf("%s		%s\n",vnid,cJSON_GetArrayItem(j_child1,0)->valuestring);
		cJSON *temp;
		j2=cJSON_GetObjectItem(json2,"vnidip");
		j_child2=j2->child;
		while(j_child2!=NULL){
			if(!strcmp(vnid,j_child2->string)) {
				int x=0;
				while(vnid[x]!='\0'){vn[vn_count][x]=vnid[x];x++;}
				vn[vn_count][x]='\0';
				vn_count++;
				int size=cJSON_GetArraySize(j_child2);
				int i;
				for(i=0;i<size;i++) {
					printf("%s",j_child2->string);
					printf("		%s\n",cJSON_GetArrayItem(j_child2,i)->valuestring);
				}
/*			
			cJSON *temp2=j_child2->next;
			cJSON_DeleteItemFromObject(j2,vnid);
			j_child2=temp2;
*/
			}
			j_child2=j_child2->next;
		}
/*		temp=j_child1->next;
		cJSON_DeleteItemFromObject(j1,vnid);
		j_child1=temp;
*/
		j_child1=j_child1->next;
	}
	int h,flag;
//	if(count<num){
	   j2=cJSON_GetObjectItem(json2,"vnidip");
	   j_child2=j2->child;
	   while(j_child2!=NULL){
		flag=0;
		vnid=j_child2->string;
		for(h=0;h<vn_count;h++) {/*printf("vn[%d] is %s\n",h,vn[h]);*/if(!strcmp(vn[h],vnid)) flag=1;}
	     if(flag==0){
		int size=cJSON_GetArraySize(j_child2);
		int i;
		for(i=0;i<size;i++) {
			printf("%s",j_child2->string);
			printf("		%s\n",cJSON_GetArrayItem(j_child2,i)->valuestring);
		}				
	      }
		j_child2=j_child2->next;
	   }
	  
//	}

	free(data1);
	free(data2);
	fclose(fa);
	fclose(fb);

}




/*
void display_vnid(char *filename){

	FILE *fb=fopen(filename,"rb+");fseek(fb,0,SEEK_END);long len=ftell(fb);fseek(fb,0,SEEK_SET);
	char *data=(char*)malloc(len+1);fread(data,1,len,fb);
	cJSON *json=cJSON_Parse(data);
	cJSON *j=cJSON_GetObjectItem(json,"vnidip");
	cJSON *j_child;
	j_child=j->child;
	while(j_child!=NULL){
		printf("ip is %d ",j_child->type);
		
		j_child=j_child->next;
	}
	fclose(fb);
}
*/
int check_vnid_ip(cJSON *json,const char *vnid){
	char *t_vnid=(char *)malloc(sizeof(char));
//	printf("vnid is %s\n",vnid);
//	cJSON *j=cJSON_GetObjectItem(json,"vnidip");
	cJSON *j_child=json->child;
//	if(j_child==NULL) {printf("j's child is null\n");return 0;}

//	printf("t_vnid is %s\n",t_vnid);
	while(j_child!=NULL){
//		if(j_child->type!=5){printf("------\n");break;}
//		printf("string is %s\n",j_child->string);
		
		strcpy(t_vnid,j_child->string);
//		printf("\n%d\n",strcmp(t_vnid,vnid));
//		printf("vnid is %s\n",vnid);
		if(!strcmp(t_vnid,vnid)) {return 1;}
//		printf("j_child's next type is %d\n",j_child->type);		
		j_child=j_child->next;
		
		
	}

//	cJSON_Delete(j_child);
	 return 0;
}
//file0--vnid-mac file1--mac_write file2--mac_ip
void display_vnid_mac_ip(char *file0,char *file1,char *file2,char *file3){
	printf("VN	         MAC			    IP\n");
	char mac[20];
	char *vnid;
	char t_vnid[10];
	char vn[30][10];
	int num,count,vn_count;
	int file_flag=0;
	FILE *fa0=fopen(file0,"r+");
	fseek(fa0,0,SEEK_END);long length0=ftell(fa0);fseek(fa0,0,SEEK_SET);
	char *data0=(char*)malloc(length0+1);fread(data0,1,length0,fa0);
	fclose(fa0);	
	FILE *fa=fopen(file1,"r+");
	if(!fa){
		cJSON *json4=cJSON_Parse(data0);
		if(!json4) {printf("json form error!\n");return;}
		cJSON *j4=cJSON_GetObjectItem(json4,"vnidmac");

		cJSON *j_child4=j4->child;
		while(j_child4!=NULL){
			int size=cJSON_GetArraySize(j_child4);
			int i;
//	    	printf("size is %d\n",size);
	 
			for(i=0;i<size;i++) {
				printf("%s",j_child4->string);
				printf("%25s",cJSON_GetArrayItem(j_child4,i)->valuestring);
				printf("%20s\n",get_ip());
			
		}
			j_child4=j_child4->next;

		}		
		return;		
	}
	fseek(fa,0,SEEK_END);long length=ftell(fa);fseek(fa,0,SEEK_SET);
	char *data=(char*)malloc(length+1);fread(data,1,length,fa);
	fclose(fa);
	FILE *fb=fopen(file2,"r+");
	fseek(fb,0,SEEK_END);long length1=ftell(fb);fseek(fb,0,SEEK_SET);
	char *data1=(char*)malloc(length1+1);fread(data1,1,length1,fb);
	fclose(fb);
	cJSON *json0=cJSON_Parse(data0);
	if(!json0) {printf("json form error!\n");return;}
	cJSON *j0=cJSON_GetObjectItem(json0,"vnidmac");

	cJSON *j_child0=j0->child;
	if(!j_child0) {/*printf("vnid-mac empty!\n");*/file_flag=1;}
	if(file_flag){
		FILE *fc=fopen(file3,"r+");
		fseek(fc,0,SEEK_END);long length3=ftell(fc);fseek(fc,0,SEEK_SET);
		char *data3=(char*)malloc(length3+1);fread(data3,1,length3,fc);
		fclose(fc);
		json0=cJSON_Parse(data3);	
		j0=cJSON_GetObjectItem(json0,"vnidmac");
		j_child0=j0->child;
	}
	cJSON *json=cJSON_Parse(data);
	if(!json) {printf("json form error!\n");return;}
	cJSON *j,*j_child;
	j=cJSON_GetObjectItem(json,"vnidmac");
	j_child=j->child;
	cJSON *json1=cJSON_Parse(data1);
	if(!json1) {printf("json form error!\n");return;}
	cJSON *j1=cJSON_GetObjectItem(json1,"macip");
	cJSON *j_child1;
	num=count=0;
	while(j_child!=NULL){
		num++;
		j_child=j_child->next;
	}
	while(j_child0!=NULL){
		vn_count=0;
		vnid=j_child0->string;
		int size=cJSON_GetArraySize(j_child0);
		int i;
//	    printf("size is %d\n",size);
	 
		for(i=0;i<size;i++) {
			printf("%s",vnid);
			printf("%25s",cJSON_GetArrayItem(j_child0,i)->valuestring);
			printf("%20s\n",get_ip());
			
		}
	   
		memset(t_vnid,0,sizeof(t_vnid));
		strcpy(t_vnid,vnid);
/*		cJSON *temp0=j_child0->next;
		cJSON_DeleteItemFromObject(j0,vnid);
		j_child0=temp0;*/
//		printf("t_vnid is %s\n",t_vnid);
		j=cJSON_GetObjectItem(json,"vnidmac");
		j_child=j->child;
//		       printf("j_child->string is %s\n",j_child->string);
		while(j_child!=NULL){
			
//			printf("vnid is %s j_child->string is %s\n",vnid,j_child->string);
			if(!strcmp(t_vnid,j_child->string)){
				int x=0;
				while(t_vnid[x]!='\0'){vn[vn_count][x]=t_vnid[x];x++;}
				vn[vn_count][x]='\0';
				vn_count++;
				count++;
				int size2=cJSON_GetArraySize(j_child);
				int k;
				memset(mac,0,sizeof(mac));
				for(k=0;k<size2;k++) {
				printf("%s",t_vnid);
				printf("%25s",cJSON_GetArrayItem(j_child,k)->valuestring);
				strcpy(mac,cJSON_GetArrayItem(j_child,k)->valuestring);
//				cJSON_DeleteItemFromArray(j_child,k);
				j_child1=j1->child;
				  while(j_child1!=NULL){
					cJSON *array1=cJSON_GetObjectItem(j_child1,j_child1->string);
					int size1=cJSON_GetArraySize(j_child1);
					int j;
//					memset(mac,0,sizeof(mac));
					for(j=0;j<size1;j++) {
						if(!strcmp(cJSON_GetArrayItem(j_child1,j)->valuestring,mac)) printf("%20s\n", j_child1->string);
//						cJSON_DeleteItemFromArray(j_child,k);
					}
					j_child1=j_child1->next;
				 }
				}	
			}
			j_child=j_child->next;
		}	
	  
		j_child0=j_child0->next;
	}//while
   int h,flag;
   if(count<num){
	j=cJSON_GetObjectItem(json,"vnidmac");
	j_child=j->child;
	while(j_child!=NULL){
		flag=0;
		vnid=j_child->string;
		for(h=0;h<vn_count;h++) {if(!strcmp(vn[h],vnid)) flag=1;}
//		j0=cJSON_GetObjectItem(json0,"vnidmac");
//		j_child0=j0->child;
//		while(j_child0!=NULL){
//			if(strcmp(vnid,j_child0->string)!=0){
//				printf("vnid is %s\n",vnid);
//				printf("j_child0 is %s\n",j_child0->string);
		if(flag==0){				
				int size=cJSON_GetArraySize(j_child);
				int i;
				for(i=0;i<size;i++) {
					printf("%s",vnid);
					printf("%25s",cJSON_GetArrayItem(j_child,i)->valuestring);
					strcpy(mac,cJSON_GetArrayItem(j_child,i)->valuestring);
					cJSON_GetObjectItem(json1,"macip");
					j_child1=j1->child;
				  	while(j_child1!=NULL){
						cJSON *array1=cJSON_GetObjectItem(j_child1,j_child1->string);
						int size1=cJSON_GetArraySize(j_child1);
						int j;
//					memset(mac,0,sizeof(mac));
						for(j=0;j<size1;j++) {
						   if(!strcmp(cJSON_GetArrayItem(j_child1,j)->valuestring,mac)) printf("%20s\n", j_child1->string);
						}
						j_child1=j_child1->next;
					}
				}
//			}
//			j_child0=j_child0->next;
//		}
		}				
		j_child=j_child->next;			
	}
    }

}
/*
//file1=vnid-mac file2=ip_write file3=mac_ip file4=mac_write
void display_vnid_mac_ip(char *file3,char *file4){

	printf("VN	      MAC			    IP\n");

	FILE *fa=fopen(file1,"r+");
	fseek(fa,0,SEEK_END);long length=ftell(fa);fseek(fa,0,SEEK_SET);
	char *data=(char*)malloc(length+1);fread(data,1,length,fa);
//	char *out;
	char *ip;
	ip=(char *)malloc(sizeof(char));
	cJSON *json=cJSON_Parse(data);
//	add=cJSON_CreateString("vnidip");
	cJSON *j=cJSON_GetObjectItem(json,"vnidmac");
	cJSON *j_child=j->child;
//	printf("type is %d\n",j_child->type);
	ip=get_ip();
	while(j_child!=NULL){
		
		
//		cJSON *array=cJSON_GetObjectItem(j_child,j_child->string);
		int size=cJSON_GetArraySize(j_child);
		int i;
		for(i=0;i<size;i++) {
			printf("%s",j_child->string);
			printf("%20s",ip);
			printf("%35s\n",cJSON_GetArrayItem(j_child,i)->valuestring);
		}
		j_child=j_child->next;
	}
//	out=cJSON_Print(json);
//	fwrite(out,strlen(out),1,fa);
	free(ip);	
	free(data);
	fclose(fa);
	
	FILE *fb=fopen(file2,"r+");
	FILE *fc=fopen(file3,"r+");
	FILE *fd=fopen(file4,"r+");
	fseek(fb,0,SEEK_END);long length1=ftell(fb);fseek(fb,0,SEEK_SET);
	char *data1=(char*)malloc(length1+1);fread(data1,1,length1,fb);
//	cJSON *json1=cJSON_Parse(data);
	fseek(fc,0,SEEK_END);long length2=ftell(fc);fseek(fc,0,SEEK_SET);
	char *data2=(char*)malloc(length2+1);fread(data2,1,length2,fc);
	fseek(fd,0,SEEK_END);long length3=ftell(fd);fseek(fd,0,SEEK_SET);
	char *data3=(char*)malloc(length3+1);fread(data3,1,length3,fd);
	cJSON *json1=cJSON_Parse(data1);
//	add=cJSON_CreateString("vnidip");
	cJSON *j1=cJSON_GetObjectItem(json1,"vnidip");
	cJSON *j_child1=j1->child;
	cJSON *json2=cJSON_Parse(data2);
//	add=cJSON_CreateString("vnidip");
	cJSON *j2=cJSON_GetObjectItem(json2,"macip");
	cJSON *j_child2=j2->child;
	cJSON *json3=cJSON_Parse(data3);
//	add=cJSON_CreateString("vnidip");
	cJSON *j3=cJSON_GetObjectItem(json3,"vnidmac");
	cJSON *j_child3=j3->child;	
	while(j_child1!=NULL){
		int size1=cJSON_GetArraySize(j_child1);
		int i;
		char t_ip[20],t_mac[20],t_vn[10];
		memset(t_vn,0,sizeof(t_vn));
		for(i=0;i<size1;i++) {
//			int flag=0;
//			if(flag) 
			memset(t_ip,0,sizeof(t_ip));
			strcpy(t_vn,j_child1->string);
//			printf("%s",t_vn);
//			printf("	%s",ip);
//			printf("%20s",cJSON_GetArrayItem(j_child1,i)->valuestring);
			strcpy(t_ip,cJSON_GetArrayItem(j_child1,i)->valuestring);
//			printf("t_ip is %s\n",t_ip);
			j_child2=j2->child;
			while(j_child2!=NULL){
				if(!strcmp(t_ip,j_child2->string)){
					int j;
					int size2=cJSON_GetArraySize(j_child2);
					for(j=0;j<size2;j++){
						memset(t_mac,0,sizeof(t_mac));
						strcpy(t_mac,cJSON_GetArrayItem(j_child2,j)->valuestring);
						j_child3=j3->child;	
						while(j_child3!=NULL){
							
							int k;
							int size3=cJSON_GetArraySize(j_child3);
							if(!strcmp(t_vn,j_child3->string)){
								for(k=0;k<size3;k++){
									if(!strcmp(t_mac,cJSON_GetArrayItem(j_child3,k)->valuestring)){
										printf("%s%20s%35s\n",t_vn,cJSON_GetArrayItem(j_child1,i)->valuestring,t_mac);
									
										}
								}
							
							}
							j_child3=j_child3->next;
						}
					}
				}
				j_child2=j_child2->next;
			}
			
		}
			j_child1=j_child1->next;
		
}
	free(data1);
	free(data2);
	free(data3);
	fclose(fb);
	fclose(fc);
	fclose(fd);


}

*/

/*
void write_all_vnidip(char *file1,char *file2,char *file3){

	FILE *f=fopen(file3,"r+");
	fseek(f,0,SEEK_END);long length=ftell(f);fseek(f,0,SEEK_SET);
	char *data=(char*)malloc(length+1);fread(data,1,length,f);
	char *out;

	FILE *fa=fopen(file1,"r+");
	fseek(fa,0,SEEK_END);long length1=ftell(fa);fseek(fa,0,SEEK_SET);
	char *data1=(char*)malloc(length1+1);fread(data1,1,length1,fa);
//	char *out;
	cJSON *json1=cJSON_Parse(data);
	cJSON *j1=cJSON_GetObjectItem(json,"vnidip");
	cJSON *j_child1=j1->child;
//	printf("type is %d\n",j_child->type);
	while(j_child1!=NULL){
		
		
//		cJSON *array=cJSON_GetObjectItem(j_child,j_child->string);
		int size=cJSON_GetArraySize(j_child1);
		int i;
		for(i=0;i<size;i++) {
			printf("%s",j_child1->string);
			printf("		%s\n",cJSON_GetArrayItem(j_child1,i)->valuestring);
		}
		j_child1=j_child1->next;
	}
//	out=cJSON_Print(json);
//	fwrite(out,strlen(out),1,fa);
	free(data1);
	fclose(fa);


}
*/




