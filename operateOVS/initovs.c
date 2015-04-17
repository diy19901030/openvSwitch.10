#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cJSON.h"
//#include"initovs.h"

    char *brname,*portname;
/*
    char *path_id = "/home/zzq/openvswitch-1.10.0/operateOVS/vnidINfo";
    char *dest_ip = "/home/zzq/openvswitch-1.10.0/operateOVS/vnidip";
    char *path_tmp = "/home/zzq/openvswitch-1.10.0/operateOVS/vnid-mac";
    char *path_tmp_mac = "/home/zzq/openvswitch-1.10.0/operateOVS/mac_write";
    char *path_brvnid = "/home/zzq/openvswitch-1.10.0/operateOVS/brvnid";
char *path_tttbrvnid = "/home/zzq/openvswitch-1.10.0/operateOVS/t_brvnid";
char *path_tmp_ip1 = "/home/zzq/openvswitch-1.10.0/operateOVS/ip_write";
char *path_ip_mac="/home/zzq/openvswitch-1.10.0/operateOVS/ipmac";
*/
extern char *dest_ip;
extern char *path_tbrvnid;
extern char *path_brvnid;
extern char *path_tmp_ip;
extern char *path_tmp_mac;
extern char *dest_mac;
extern char *path_ip_mac;
extern char *path_encap;
extern char *path_encap_write;
extern char *dest_mac_bak;


char comd1[100],comd2[100];
void main(){
	char p[5];
	char a[3];
	int f1,f2;
//	p=(char *)malloc(sizeof(char));
    brname=(char *)malloc(sizeof(char));
    portname=(char *)malloc(sizeof(char));
    if(access(dest_ip,0)==-1){
        char *t;
	FILE *f = fopen(dest_ip,"w+");
	cJSON *json=create_vnidip();
	t=cJSON_Print(json);
	fwrite(t,strlen(t),sizeof(char),f);
	fclose(f);
	cJSON_Delete(json);
//	free(t);


    }
/*
	f1=detect_process(pname1);
	f2=detect_process(pname2);
	if(f1==0||f2==0) {
		system("insmod /home/zzq/openvswitch-1.10.0/datapath/linux/openvswitch.ko");

		system("ovsdb-server /usr/local/etc/openvswitch/conf.db \
			--remote=punix:/usr/local/var/run/openvswitch/db.sock \
			--remote=db:Open_vSwitch,manager_options \
			--private-key=db:SSL,private_key \
			--certificate=db:SSL,certificate \
			--bootstrap-ca-cert=db:SSL,ca_cert \
			--pidfile --detach --log-file");

		system("ovs-vsctl --no-wait init");

		system("ovs-vswitchd --pidfile --detach");

	}
*/

    system("ovs-vsctl show > brINfo");
while(1){
	printf("/----------------欢迎使用NVo3-----------/\n");
	printf("*显示网桥与VN网络映射关系请按 1 \n");
	printf("*显示本机VN与IP映射关系请按 2 \n");
	printf("*显示全网VN与IP映射关系请按 3 \n");
//	printf("*配置VN网络请按 13 \n");
//	printf("*显示本机VN与MAC映射关系请按 4\n");
//	printf("*显示全网VN与MAC映射关系请按 5\n");
//	printf("*显示IP与MAC映射关系请按 6\n");
	printf("*显示全网VN-MAC-IP映射请按 4 \n");
//	printf("*MAC可达性通告使用VXLAN封装请按 8 \n");
//	printf("*MAC可达性通告不使用VXLAN封装请按 9 \n");
	printf("*选择封装能力类型请按 5 \n");
	printf("*显示全网NVE封装能力请按 6 \n");
//	printf("*test 7 \n");
//	printf("*获取VN-MAC映射请按 12 \n");
	printf("*退出请按 q \n");
	scanf("%s",&p);
//	printf("p is %s",p);


	if(!strcmp(p,"1")) {get_br_vnid();write_br_vnid(path_tbrvnid,path_brvnid);display_br_vnid(path_brvnid);return ;}
	else if(!strcmp(p,"2")) {
//		get_br_vnid();
//		write_vnidip(dest_ip,path_brvnid);
		display_vnid_ip(dest_ip);
		return;
	}
	if(!strcmp(p,"3")) {printf("VN		      IP\n");display_update_vnid_ip(dest_ip,path_tmp_ip);return;}
//	else if(!strcmp(p,"4")) {get_br_vnid();write_br_vnid(path_tbrvnid,path_brvnid);conf_ovs();return;}
//	else if(!strcmp(p,"4")){ get_mac();display_vnid_mac(dest_mac);return;}
//	else if(!strcmp(p,"5")){printf("VN		      MAC\n");display_vnid_mac(path_tmp_mac);display_vnid_mac(dest_mac);return;}
//	else if(!strcmp(p,"6")){printf("IP		      		MAC\n");display_mac(dest_mac);get_ip_mac(path_ip_mac);return;}
	else if(!strcmp(p,"4")){
		//file1=vnid-mac file2=vnidip file3=mac_ip file4=mac_write
		display_vnid_mac_ip(dest_mac,path_tmp_mac,path_ip_mac,dest_mac_bak);
//		display_encap(path_encap_write);
		return;}
//	else if(!strcmp(p,"7")){check_mac(path_brvnid,dest_mac);return;}
//	else if(!strcmp(p,"9")){printf("MAC可达性通告不使用VXLAN封装!\n");system("./stopmac");return;}
	else if(!strcmp(p,"5")){
		printf("请选择封装能力: \nVXLAN--1\nNVGRE--2\nmpls_in_udp--3\n");
		scanf("%s",&a);

		getencap(a);
		return;
	}
	else if(!strcmp(p,"6")){display_encap(path_encap_write);return;}
//	else if(!strcmp(p,"12")){get_mac();display_vnid_mac(dest_mac);return;}
//	else if(!strcmp(p,"13")){get_br_vnid();write_br_vnid(path_tbrvnid,path_brvnid);conf_ovs();return;}
	else if(!strcmp(p,"q")) return;
	
	else printf("输入错误，请重新输入！\n");}
//    printf("VNid-ip映射关系还未建立，请输入以下信息建立映射关系\n");
/*    printf("请输入网桥的名称 :");
    scanf("%s",brname);
//    printf("请输入端口的名称 :");
//    scanf("%s",portname);
    
    system("ovs-dpctl show > portINfo");
    sprintf(comd1,"ovs-appctl fdb/show %s > macINfo",brname);
    system(comd1);
*/
//    sprintf(comd2,"grep -A 3 'Port %s' brINfo | grep 'key' > vnidINfo",portname);
//    system(comd2);

/*	FILE *fb=fopen(dest_ip,"rb+");fseek(fb,0,SEEK_END);long len=ftell(fb);fseek(fb,0,SEEK_SET);
	char *data=(char*)malloc(len+1);fread(data,1,len,fb);*/
//	char *out,j_vnid;
//	cJSON *json; 
//	cJSON *cjson=(cJSON *)malloc(sizeof(cJSON));
//	json=cJSON_Parse(data);
//	cJSON *json=create_vnidip();
//	printf("json is %s",cJSON_Print(json));
	
	
//	display_vnid_ip(dest_ip);

//	display_ip(dest_ip);
//	printf("cjson's type is %d\n",cjson->type);
//	printf("vnid is %s",display_vnid(cjson));
//	printf("  ip is %s\n",display_ip(cjson,display_vnid(cjson)));
//	printf("cjson is %s\n",cJSON_Print(cjson));	
//	out=cJSON_Print(cjson);
	
//	fseek(fb,0,SEEK_SET);
//	fwrite(out,strlen(out),sizeof(char),fb);
	
//	fclose(fb);
//	cJSON_Delete(json);
//	cJSON_Delete(cjson);
//	free(data);
//	cJSON_Delete(cjson);
/*
	FILE *f=fopen(dest_ip,"rb");fseek(f,0,SEEK_END);long len=ftell(f);fseek(f,0,SEEK_SET);
	char *data=(char*)malloc(len+1);fread(data,1,len,f);fclose(f);
	printf("VNid-ip映射关系已经建立\n");
	cJSON *json=cJSON_Parse(data);
	printf("vnid is %s",display_vnid(json));
	printf("  ip is %s\n",display_ip(json,display_vnid(json)));

*/
//	fclose(f);
 

    }
