
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include"cJSON.h"
//#include"initovs.h"

#define MAX_LENTH 100
/*
    char *path_ip = "/home/zzq/openvswitch-1.10.0/operateOVS/vnidINfo";
    char *path_port = "/home/zzq/openvswitch-1.10.0/operateOVS/portINfo";
    char *path_mac = "/home/zzq/openvswitch-1.10.0/operateOVS/macINfo";
    char *dest_mac = "/home/zzq/openvswitch-1.10.0/operateOVS/vnid-mac";
    char *file_brvnid = "/home/zzq/openvswitch-1.10.0/operateOVS/brvnid";
*/
extern char *dest_mac;
extern char *path_brvnid;
extern char *path_port;
    char buffer[MAX_LENTH],temp[50][MAX_LENTH],port1[50][5],vnid[10];
    char comd[50];
  //  char mac[100][20];//store the mac address
    int *port;
    char *vnic;
    int portnum=1;

void get_vnid_mac(char *br,char *p_brvnid,char *p_port,char *d_mac,char *p_mac);
char *get_brname(char *vnid,char *p_brvnid);
void store_vnid_mac(char *filename,char *file_mac);
void create_mac_file(char *filename);
char **get_mac_file(char *filename);
void display_vnid_mac(char *filename);
//void get_mac(char *p_port,char *p_mac,char *d_mac);

int get_mac_form_vnmac(char *filename){

	char *brname;
	int i;
	brname=(char *)malloc(sizeof(char));

	FILE *fa=fopen(filename,"r");
	if(!fa)
		return;
	else
	{
		printf("sucessful open the file %s\n",filename);
	}

	fseek(fa,0,SEEK_END);long length=ftell(fa);fseek(fa,0,SEEK_SET);
	char *data=(char*)malloc(length+1);fread(data,1,length,fa);
	fclose(fa);
	char *out;
	cJSON *json=cJSON_Parse(data);
	cJSON *j=cJSON_GetObjectItem(json,"vnmac");
	cJSON *j_child=j->child;


	while(j_child!=NULL){

		cJSON *item;
		printf("VN ID is %s\n",j_child->string);

		strcpy(brname,j_child->string);
		printf("VN ID is %s\n",brname);
		cJSON *child_array=cJSON_GetArrayItem(j_child,vnid);
		int size=cJSON_GetArraySize(j_child);
		printf("array size is %d\n",size);
		for(i=0;i<size;i++)
		{
			item = cJSON_GetArrayItem(j_child,i);
			printf("vn id %s: \t mac1 is %s\t \n",j_child->string,item->valuestring);
		}
		sprintf(comd,"ovs-vsctl add-br br%s",brname);
		system(comd);
		sprintf(comd,"ovs-vsctl add-port br%s %s",brname,brname);
		system(comd);
		sprintf(comd,"ovs-vsctl set interface %s type=vxlan options:key=%s",brname,brname);
		system(comd);
		sprintf(comd,"ovs-vsctl show");
		system(comd);
		j_child = j_child->next;

	}

}
void get_mac(){

//	check_mac(path_brvnid,dest_mac);


	char *brname,*id;
//	char mac[50][20];
	char path[60],t_path[20];
	int len;
	brname=(char *)malloc(sizeof(char));
	id=(char *)malloc(sizeof(char));

//	get_mac_form_vnmac("json");
//	printf("请输入VN网络: \n");
//	scanf("%s",id);	
//	printf("id is %s\n",id);
//	brname=get_brname(id,file_brvnid);
//	if(!strcmp(brname,"error")) {printf("VN %s 不存在！\n",id);return;}
//	printf("brname is %s\n",brname);
	system("ovs-dpctl show > portINfo");
	
	create_mac_file(dest_mac);
	FILE *fa=fopen(path_brvnid,"r");
	if(!fa) return;
	fseek(fa,0,SEEK_END);long length=ftell(fa);fseek(fa,0,SEEK_SET);
	char *data=(char*)malloc(length+1);fread(data,1,length,fa);
	fclose(fa);
	char *out;
	cJSON *json=cJSON_Parse(data);
	cJSON *j=cJSON_GetObjectItem(json,"brvnid");
	cJSON *j_child=j->child;
	while(j_child!=NULL){

//		printf("%s\n",j_child->valuestring);
		strcpy(brname,j_child->string);
		sprintf(comd,"ovs-appctl fdb/show %s > %smacINfo",brname,brname);
		system(comd);
//add 2013.12.31
		memset(path,0,sizeof(path));
		memset(t_path,0,sizeof(t_path));
		getcwd(path,sizeof(path));
		sprintf(t_path,"/%smacINfo",brname);
		strcat(path,t_path);
//		printf("path is %s\n",path);
//add
//		sprintf(path,"/home/zzq/openvswitch-1.10.0/operateOVS/%smacINfo",brname);
		get_vnid_mac(brname,path_brvnid,path_port,dest_mac,path);
		j_child=j_child->next;
	}
	check_mac(path_brvnid,dest_mac);
//	printf("VN网络			MAC\n");
//	display_vnid_mac(dest_mac);
	free(id);	
	free(brname);


}

char *get_brname(char *vnid,char *p_brvnid){

	FILE *fa=fopen(p_brvnid,"r");
	fseek(fa,0,SEEK_END);long length=ftell(fa);fseek(fa,0,SEEK_SET);
	char *data=(char*)malloc(length+1);fread(data,1,length,fa);
	char *out;
	cJSON *json=cJSON_Parse(data);
	cJSON *j=cJSON_GetObjectItem(json,"brvnid");
	cJSON *j_child=j->child;
	while(j_child!=NULL){
//		printf("%s\n",j_child->valuestring);
		if(!strcmp(j_child->valuestring,vnid)) {fclose(fa);free(data);return j_child->string;}
		j_child=j_child->next;
	}
	fclose(fa);
	free(data);
	return "error";
}


void get_vnid_mac(char *br,char *p_brvnid,char *p_port,char *d_mac,char *p_mac){
	char mac[100][20];
	char path_br[60],t_path[10];
	int fflag=0;
	int i;
	char *vnid,*brname;
//	path_br=(char *)malloc(sizeof(char));
	memset(path_br,0,sizeof(path_br));
	memset(t_path,0,sizeof(t_path));
//	memset(path,0,sizeof(path));
	getcwd(path_br,sizeof(path_br));

	printf("*******br name is :%s\n",br);

	sprintf(t_path,"/%s",br);
	strcat(path_br,t_path);

//	sprintf(path_br,"/home/zzq/openvswitch-1.10.0/operateOVS/%s",br);
//	printf("path_br is %s\n",path_br);

//以下是根据传递进来的网桥信息提取出对应的vnid信息
	vnid=(char *)malloc(sizeof(char));
	brname=(char *)malloc(sizeof(char));
	FILE *fa=fopen(p_brvnid,"r");
	if(!fa) return;
	fseek(fa,0,SEEK_END);long length=ftell(fa);fseek(fa,0,SEEK_SET);
	char *data=(char*)malloc(length+1);fread(data,1,length,fa);
	fclose(fa);
	char *out;
	cJSON *json=cJSON_Parse(data);
	cJSON *j=cJSON_GetObjectItem(json,"brvnid");
	cJSON *j_child=j->child;
	while(j_child!=NULL){
//		printf("%s\n",j_child->valuestring);
		if(!strcmp(j_child->string,br)) {strcpy(vnid,j_child->valuestring);break;}
		j_child=j_child->next;
	}
	if(!vnid) {printf("vnid is not existed!\n");return;}
	free(data);
//	fclose(fa);
	
	port=(int *)malloc(sizeof(int));
	memset(buffer,0,sizeof(buffer));
	FILE *fc=fopen(path_br,"r+");
	if(!fc) printf("file open error!");
	int n=0;

	/***********get the mac from json file added by chenyanming ********/

	fa=fopen("json","r");
	if(!fa)
		return;
	else
	{
		printf("sucessful open the file \n");
	}

	fseek(fa,0,SEEK_END); length=ftell(fa);fseek(fa,0,SEEK_SET);
	data = (char*)malloc(length+1);fread(data,1,length,fa);
	fclose(fa);

	json=cJSON_Parse(data);
	j=cJSON_GetObjectItem(json,"vnmac");
	j_child=j->child;


	while(j_child!=NULL){

		cJSON *item;
		if(!strcmp(j_child->string,vnid))
		{
			cJSON *child_array=cJSON_GetArrayItem(j_child,vnid);
			int size=cJSON_GetArraySize(j_child);
			n = size;
			for(i=0;i<n;i++)
			{
				item = cJSON_GetArrayItem(j_child,i);
				printf("vn id %s: \t mac1 is %s\t \n",j_child->string,item->valuestring);
				strcpy(mac[i],item->valuestring);
				printf(" \t mac1 is %s\t \n",mac[i]);
			}
			printf("array size is %d\n",size);
		}
		printf("VN ID is %s\n",j_child->string);
		j_child = j_child->next;
	}
	free(data);

//		strcpy(brname,j_child->string);
//		printf("VN ID is %s\n",brname);




	/***********************end*****************************************/

/*
 * judge there is any vm-machine,if there is any  vnet#*(0,1,5000)
 */
/*
	while(fgets(buffer,MAX_LENTH,fc)!=NULL){
		int i=0,r=0;
//		printf("buffer is %s\n",buffer);
		vnic=NULL;
		vnic=strstr(buffer,"        Port \"vnet");
		if(vnic)  {
			int j=0;
//			printf("--buffer is %s\n",buffer);
 			while(buffer[i]!='\0'){
				
   				if(isdigit(buffer[i])) {
					for(r=4;r>=0;r--){
						//r=i;
						temp[n][j++]=buffer[i-r];
//						printf("temp is n : %d %c\n",n,temp[n][j-1]);
						}
				}
//				else temp[n][i]=' ';
			i++;
			}       
 			temp[n][j]='\0';
			n++;
		}

	}
	fclose(fc);
//	if(!vnic){printf("VN %s没有虚拟机！\n",vnid);return;}


	FILE *pf1 = fopen(p_port,"r");
	if(!pf1) return;
	vnic=(char *)malloc(sizeof(char));
	memset(buffer,0,sizeof(buffer));

//according to the vnet# get the port number from the portInfo file
	while(fgets(buffer,MAX_LENTH,pf1)!=NULL){
		int l;
		int i=0;
		if(!n) break;
		for(l=0;l<n;l++){
		vnic=NULL;
//		printf("temp[%d] is %s\n",l,temp[l]);
//		printf("----%d\n",temp[l][4]-48);
		vnic=strstr(buffer,temp[l]);
		if(vnic)  {
			while(buffer[i]!='\0'){
			if(isdigit(buffer[i])) {port[l]=(int)(buffer[i]-'0');break;}
			i++;}
//		printf("port is %d",port[l]);
		}
		
		}
	
	}//find the port of vm's vnic(虚拟网卡 vnet0...)

	fclose(pf1);


     int b=0;//flag to identify if there is port number == vnet#
     int q;
     for(q=0;q<n;q++){
 //        sprintf(port1[q],"%c",port[q]);
//     int fd3=open("tt",O_RDWR | O_CREAT);
//     write(fd3,port1[q]); 
 //    printf("port1[%d] is %s\n",q,port1[q]);
     memset(buffer,0,sizeof(buffer));
     FILE *pf2 = fopen(p_mac,"r");
     if(!pf2) return;
     while(fgets(buffer,MAX_LENTH,pf2)!=NULL){
         
         if(q>n) {printf("port number error!\n");continue;}

//          p=NULL;
//          p=strstr(buffer,port1[q]);
 //         printf("buffer[4] is %d\n",buffer[4]-'0');
          int k=0,a=0;
//	  printf("port[%d] is %d\n",q,port[q]);
          if((buffer[4]-'0')==port[q]) { 
	  b=1;
          int j;
          for( j=13;j<31;j++) {
          mac[q][k]=buffer[j];
 //         printf("%c",mac[q][k]);
           k++;}
  //        
          mac[q][k-1]='\0';
 //         printf("%d---",mac[q][k-1]);
//          printf("mac[%d] is %s\n",q,mac[q]);
//          printf("MAC is %s",mac[q]);
          break;
        
}    

          
          
}//while
        fclose(pf2);
}//for

if(!b) return;

*/

	int h;

	
	FILE *fe=fopen(d_mac,"r+");
	if(!fe) return;
	fseek(fe,0,SEEK_END);long leng=ftell(fe);fseek(fe,0,SEEK_SET);
	char *jdata=(char*)malloc(leng+1);fread(jdata,1,leng,fe);
	cJSON *cjson=cJSON_Parse(jdata);
	
	cJSON *jj=cJSON_GetObjectItem(cjson,"vnidmac");	


	int flag=0;
//printf("n is %d\n",n);
	cJSON *jj_child=jj->child;
	while(jj_child!=NULL){
		int v;
		if(!strcmp(jj_child->string,vnid)){
			flag=1;
//			printf("jj_child is %s\n",jj_child->valuestring);
			

			cJSON *t_array=cJSON_GetObjectItem(jj,vnid);
			int m=cJSON_GetArraySize(t_array);
				
//			printf("jj_child is %s\n",cJSON_Print(jj_child));
			for(v=0;v<m;v++) cJSON_DeleteItemFromArray(t_array,v);
//			cJSON_DeleteItemFromArray(t_array,0);
//			printf("m is %d\n",cJSON_GetArraySize(t_array));	
			if(!n) {
				cJSON *temp=jj_child;
				cJSON_DeleteItemFromObject(jj,vnid);
				
				jj_child=temp->next;	
//				cJSON_DeleteItemFromObject(jj_child,vnid);
			}
//			printf("t_array is %s\n",cJSON_Print(t_array));
		
//			printf("-----");
			else {
			 for( h=0;h<n;h++) {

//				printf("the length mac is %s\n",mac[h]);
				if(!strlen(mac[h])) {
				//	printf("VN %s 中没有虚拟机！\n",vnid);
				//	cJSON_DeleteItemFromObject(jj,vnid);				
					continue;}
				

				cJSON *add_mac=cJSON_CreateString(mac[h]);
				cJSON_AddItemToArray(t_array,add_mac);

//				printf("%s		%s",vnid,mac[h]);
    			}
			}
		}
		jj_child=jj_child->next;
	}

if(!flag){
	cJSON *totle_mac=cJSON_CreateArray();
	cJSON_AddItemToObject(jj,vnid,totle_mac);
	if(!n)cJSON_DeleteItemFromObject(jj,vnid);
 for( h=0;h<n;h++) {
     int w=0;

	if(!strlen(mac[h])) {/*printf("VN %s 中没有虚拟机！\n",vnid);*/continue;}
	cJSON *taskArray=cJSON_GetObjectItem(jj,vnid);

	cJSON *add_mac=cJSON_CreateString(mac[h]);
	cJSON_AddItemToArray(taskArray,add_mac);

    }
}
//	printf("new json is %s\n",cJSON_Print(cjson));
	out=cJSON_Print(cjson);
	fseek(fe,0,SEEK_SET);
	if(!fflag){
	fwrite(out,strlen(out),1,fe);
	fclose(fe);
//	check_mac(path_brvnid,dest_mac);
	fflag=1;}

	free(jdata);
//	fclose(fe);



//	fclose(df);
//fputs(mac,df);
//}
 //  free(vnic);
  free(port);
   free(brname);
    free(vnid);

//	check_mac(path_brvnid,dest_mac);
	

}

void create_mac_file(char *filename){

	FILE *fa=fopen(filename,"w+");
	fseek(fa,0,SEEK_END);long length=ftell(fa);fseek(fa,0,SEEK_SET);
	char *data=(char*)malloc(length+1);fread(data,1,length,fa);
	char *out;
	cJSON *json=cJSON_CreateObject();
	cJSON *add=cJSON_CreateObject();
//	add=cJSON_CreateString("vnidip");
	cJSON_AddItemToObject(json,"vnidmac",add);
	out=cJSON_Print(json);
	fwrite(out,strlen(out),1,fa);
	free(data);
	fclose(fa);

}

/*
 * file1--brvnid file2--vnid-mac
 * function:
 * 	检查brvnid的文件中的vn和vnid-mac的vn是否一致，如果不一致则在vnis-mac中增加相关的字段
 */

void check_mac(char *file1,char *file2){

	char vnid[10];
	int flag;
	FILE *fa=fopen(file1,"r+");
	fseek(fa,0,SEEK_END);long length=ftell(fa);fseek(fa,0,SEEK_SET);
	char *data=(char*)malloc(length+1);fread(data,1,length,fa);
	fclose(fa);
	FILE *fb=fopen(file2,"r+");
	fseek(fb,0,SEEK_END);long length1=ftell(fb);fseek(fb,0,SEEK_SET);
	char *data1=(char*)malloc(length1+1);fread(data1,1,length1,fb);
	fclose(fb);
	cJSON *json1,*j1,*j_child1;
	json1=cJSON_Parse(data1);
	cJSON *json=cJSON_Parse(data);
//	add=cJSON_CreateString("vnidip");
	cJSON *j=cJSON_GetObjectItem(json,"brvnid");
//	printf("111\n");
	cJSON *j_child=j->child;	
	while(j_child!=NULL){
		flag=0;
		memset(vnid,0,sizeof(vnid));
		strcpy(vnid,j_child->valuestring);
//		printf("vnid is %s\n",vnid);
		
		j1=cJSON_GetObjectItem(json1,"vnidmac");
		j_child1=j1->child;
		while(j_child1!=NULL){
			if(!strcmp(vnid,j_child1->string)) {
//				printf("vnid is %s\n",vnid);
				flag=1;
				break;
			}
			j_child1=j_child1->next;	
		}
		if(!flag){
		cJSON *add=cJSON_CreateObject();
		cJSON *add_arr=cJSON_CreateArray();
//				add=cJSON_CreateString("vnidip");
//				cJSON_AddItemToObject(json,"vnidmac",add);
		cJSON_AddItemToObject(j1,vnid,add_arr);

		}
		j_child=j_child->next;
		
	}
		FILE *fr=fopen(file2,"w+");
		fseek(fr,0,SEEK_SET);
		char *out=cJSON_Print(json1);
//		printf("out is %s\n",out);
		fwrite(out,strlen(out),1,fr);
		fclose(fr);


}

void display_vnid_mac(char *filename){
	
	FILE *fa=fopen(filename,"r+");
	fseek(fa,0,SEEK_END);long length=ftell(fa);fseek(fa,0,SEEK_SET);
	char *data=(char*)malloc(length+1);fread(data,1,length,fa);
	char *out;
	cJSON *json=cJSON_Parse(data);
//	add=cJSON_CreateString("vnidip");
	cJSON *j=cJSON_GetObjectItem(json,"vnidmac");
	cJSON *j_child=j->child;
//	printf("type is %d\n",j_child->type);
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
	out=cJSON_Print(json);
//	fwrite(out,strlen(out),1,fa);
	free(data);
	fclose(fa);

}

void display_mac(char *filename){
	
	FILE *fa=fopen(filename,"r+");
	fseek(fa,0,SEEK_END);long length=ftell(fa);fseek(fa,0,SEEK_SET);
	char *data=(char*)malloc(length+1);fread(data,1,length,fa);
	char *out;
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
			printf("%s",ip);
			printf("		%s\n",cJSON_GetArrayItem(j_child,i)->valuestring);
		}
		j_child=j_child->next;
	}
//	out=cJSON_Print(json);
	free(ip);	
//	fwrite(out,strlen(out),1,fa);
	free(data);
	fclose(fa);

}





