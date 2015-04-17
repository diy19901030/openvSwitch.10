#include <stdio.h> 
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/param.h> 
#include <sys/ioctl.h> 
#include <sys/socket.h> 
#include <net/if.h> 
#include <netinet/in.h> 
#include <net/if_arp.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<fcntl.h>
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h> 
#include<unistd.h>
#include"cJSON.h"
//#include"initovs.h"

#ifdef SOLARIS 
#include <sys/sockio.h> 
#endif 


#define MAXINTERFACES 16 
#define MAX_LENTH 100

/*
char *path_id = "/home/zzq/openvswitch-1.10.0/operateOVS/vnid-ip";
char *path_tmp_ip = "/home/zzq/openvswitch-1.10.0/operateOVS/ip_write";
char *path_tmp_mac = "/home/zzq/openvswitch-1.10.0/operateOVS/mac_write";
char *path_tbrvnid3 = "/home/zzq/openvswitch-1.10.0/operateOVS/t_brvnid";
char *path_brvnid3 = "/home/zzq/openvswitch-1.10.0/operateOVS/brvnid";
*/
extern char *path_tmp_ip;//write the vnid-ip file
//extern char *path_brvnid;
//extern char *path_tbrvnid;
extern char *path_tbrvnid;
extern char *path_brvnid;
extern char *dest_ip;
extern char *dest_mac_bak;
extern char *dest_mac;
/*
 * set the process name
 */
char *pname="updateovs";
char *pname1="ovs-vswitchd";
char *pname2="ovsdb-server";










//char *dest_upd = "/home/zzq1/openvswitch-1.10.0/operateOVS/vnid-ip.update";


void update(char *p_ip,char *p_mac);
unsigned int BKDRHash(char *str);
int detect_process(const char * process_name);
int f1,f2;

void main(){
	
	int f3=0;
	f3=detect_process_update(pname);
	if(f3) {
		printf("update已运行！\n");
		return;
	}

	
	init_daemon();

	f1=detect_process(pname1);
	f2=detect_process(pname2);
	if(f1==0||f2==0) {
		
		FILE *sourcefile;
		FILE *desfile;
		char c;
		sourcefile = fopen("vnid-mac","r");
		if(!sourcefile) printf("sourcefile open error!\n");
		desfile = fopen("vnid-mac-bak","w");
		if(!desfile) printf("desfile open error!\n");
		while((c=fgetc(sourcefile))!= EOF)
		{
        		fputc(c,desfile);
		}

		fclose(sourcefile);
		fclose(desfile);

		system("./restartovs");
//		exit(0);
	}

//	system("ovs-vsctl show > tmp_brinfo");
	system("ovs-vsctl list-br > brName");

/*
 * a:file ip-writer(vnidip)
 * 	"vnidip":	{
		"5000":	["192.168.1.10"]

	c:file tem_brinfo
	ovs-vsctl show ...

	e:file brname
		br0
		br1
		br5000


 */
	unsigned int a,c,d,e,f;

    while(1){
		sleep(2);
		FILE *fa=fopen(path_tmp_ip,"rb+");
		if(!fa) {
			/*printf("ip_write open error!\n");printf("000\n");*/
			continue;
		}
		else {
		//		printf("111\n");
				fseek(fa,0,SEEK_END);long len=ftell(fa);fseek(fa,0,SEEK_SET);
				char *data=(char*)malloc(len+1);fread(data,1,len,fa);
				a=BKDRHash(data);
				fclose(fa);
				free(data);data=NULL;
				break;
			}
    }

	FILE *fd=fopen("tmp_brinfo","rb");fseek(fd,0,SEEK_END);long len2=ftell(fd);fseek(fd,0,SEEK_SET);
	char *data2=(char*)malloc(len2+1);fread(data2,1,len2,fd);
	FILE *fe=fopen("brName","rb");
	fseek(fe,0,SEEK_END);long len4=ftell(fe);fseek(fe,0,SEEK_SET);
	char *data4=(char*)malloc(len4+1);fread(data4,1,len4,fe);
//	a=BKDRHash(data);
	c=BKDRHash(data2);
	e=BKDRHash(data4);
	
	fclose(fd);
	fclose(fe);
	free(data4);data4=NULL;
	free(data2);data2=NULL;
	

//	system("ovs-vsctl list-br > tmp_brinfo");
	while(1){
		sleep(13);
		
		f1=detect_process(pname1);
		f2=detect_process(pname2);
		if(f1==0||f2==0) {
			FILE *sourcefile;
			FILE *desfile;
			char c;
			sourcefile = fopen("vnid-mac","r");
			if(!sourcefile) printf("sourcefile open error!\n");
			desfile = fopen("vnid-mac-bak","w");
			if(!desfile) printf("desfile open error!\n");
			while((c=fgetc(sourcefile))!= EOF)
			{
        			fputc(c,desfile);
			}

			fclose(sourcefile);
			fclose(desfile);

			system("./restartovs");
			continue;
//			exit(0);
		}
		
		FILE *fb=fopen(path_tmp_ip,"rb");
		if(!fb) continue;		
		fseek(fb,0,SEEK_END);long len1=ftell(fb);fseek(fb,0,SEEK_SET);
		char *data1=(char*)malloc(len1+1);fread(data1,1,len1,fb);
		fclose(fb);
		unsigned int b=BKDRHash(data1);

		if(a!=b) {
//			printf("aaa\n");
//			system("./update_vn");
//			printf("222\n");
			system("./autoconf");
			a=b;
		}
//		sleep(3);
		system("ovs-vsctl show > tmp_brinfo");
		sleep(1);
		FILE *fc=fopen("tmp_brinfo","rb");
		if(!fc) continue;
		fseek(fc,0,SEEK_END);long len3=ftell(fc);fseek(fc,0,SEEK_SET);
		char *data3=(char*)malloc(len3+1);fread(data3,1,len3,fc);
		fclose(fc);
		d=BKDRHash(data3);
//		printf("c is %d\n",c);
		
		if(c!=d) {
//			printf("VN网络有变化！\n");
//			printf("bbb\n");
//			printf("d is %d\n",d);
//			get_br_vnid(path_tbrvnid3,path_brvnid3);
//			printf("333-1\n");
			system("./update_vn");	
//			printf("333-2\n");		
			system("./autoconf");
			c=d;
		}
		
		sleep(1);
		system("ovs-vsctl list-br > brName");
		FILE *ff=fopen("brName","rb");
		if(!ff) continue;
		fseek(ff,0,SEEK_END);long len5=ftell(ff);fseek(ff,0,SEEK_SET);
		char *data5=(char*)malloc(len5+1);fread(data5,1,len5,ff);
		fclose(ff);
		f=BKDRHash(data5);
		if(e!=f) {
//			printf("ccc！\n");
//			printf("@@@@@@@@\n");
/*
			get_br_vnid();
			write_br_vnid(path_tbrvnid,path_brvnid);
			write_vnidip(dest_ip,path_brvnid);
//			get_mac();*/
//			printf("444\n");
			system("./update_vnidip");

			e=f;
		}	
//		printf("$$$$$$$\n");	
//		get_mac();
//		printf("555\n");
		system("./get_mac");
		
		free(data3);data3=NULL;
		free(data1);data1=NULL;
		free(data5);data5=NULL;
	}
//	update(path_tmp_ip,path_tmp_mac);

//	return;


}


unsigned int BKDRHash(char *str)
{
    unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
    unsigned int hash = 0;
 
    while (*str)
    {
        hash = hash * seed + (*str++);
    }
 
    return (hash & 0x7FFFFFFF);
}


int detect_process(const char * process_name)
{
//	printf("process_name is %s\n",process_name);
        FILE *ptr;
        char buff[512];
        char ps[128];
        sprintf(ps,"ps -e | grep -c '%s'",process_name);
        strcpy(buff,"ABNORMAL");
        if((ptr=popen(ps, "r")) != NULL)
        {
                while (fgets(buff, 512, ptr) != NULL)
                {
                        if(atoi(buff)>=1)
                        {
                                pclose(ptr);
                                return 1;
                        }
                }
        }
        if(strcmp(buff,"ABNORMAL")==0)  
         return 0;        
        pclose(ptr);
        return 0;
}

int detect_process_update(const char * process_name)
{
//	printf("process_name is %s\n",process_name);
        FILE *ptr;
        char buff[512];
        char ps[128];
        sprintf(ps,"ps -e | grep -c '%s'",process_name);
        strcpy(buff,"ABNORMAL");
        if((ptr=popen(ps, "r")) != NULL)
        {
                while (fgets(buff, 512, ptr) != NULL)
                {
//			printf("atoi(buff) is %d\n",atoi(buff));
                        if(atoi(buff)>=2)
                        {
                                pclose(ptr);
                                return 1;
                        }
                }
        }
        if(strcmp(buff,"ABNORMAL")==0)  
         return 0;        
        pclose(ptr);
        return 0;
}


