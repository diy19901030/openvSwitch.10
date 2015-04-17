#include <stdio.h> 
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
#include"cJSON.h"
//#include"ovs.h"
//#include"initinfo.h"

#ifdef SOLARIS 
#include <sys/sockio.h> 
#endif 
#define MAXINTERFACES 16 
#define MAX_LENTH 100



char* get_vnid_ip(char *filename,char *br){
	FILE *fb=fopen(filename,"rb+");fseek(fb,0,SEEK_END);long len=ftell(fb);fseek(fb,0,SEEK_SET);
	char *data=(char*)malloc(len+1);fread(data,1,len,fb);
	//cJSON *json,*j,*j_child;
	char *vnid,*ip,*t_ip,t_vnid[10];
	char *out;
	vnid=(char *)malloc(sizeof(char));
	cJSON *json=cJSON_Parse(data);
	cJSON *j=cJSON_GetObjectItem(json,"brvnid");
	cJSON *j_child=j->child;
	memset(t_vnid,0,sizeof(t_vnid));
	while(j_child!=NULL){
		if(!strcmp(j_child->string,br))
		{
			strcpy(vnid,j_child->valuestring);
			printf("vnid is %s",vnid);
			return vnid;
		}
		j_child=j_child->next;
	}
	
	return "error";
}

//get NVE's IP

char* get_ip(){

	register int fd, intrface, retn = 0;
	int fd1;
	struct ifreq buf[MAXINTERFACES];
	char *bufip;
	struct arpreq arp;
	struct ifconf ifc;
	if ((fd = socket (AF_INET, SOCK_DGRAM, 0)) >= 0)
	{
	ifc.ifc_len = sizeof(buf);
	ifc.ifc_buf = (caddr_t) buf;
	if (!ioctl (fd, SIOCGIFCONF, (char *) &ifc))
	{
	//获取接口信息
	intrface = ifc.ifc_len / sizeof (struct ifreq);
	//printf("interface num is intrface=%d\n\n\n",intrface);
	//根据借口信息循环获取设备IP和MAC地址
	//while (intrface-- > 0)
	//{
	//获取设备名称
	intrface=1;
	//printf ("net device %s\n", buf[intrface].ifr_name);

	//判断网卡类型
	if (!(ioctl (fd, SIOCGIFFLAGS, (char *) &buf[intrface])))
	{
	if (buf[intrface].ifr_flags & IFF_PROMISC)
	{
	puts ("the interface is PROMISC");
	retn++;
	}
	}
	else
	{
	char str[256];
	sprintf (str, "cpm: ioctl device %s", buf[intrface].ifr_name);
	perror (str);
	}
	//判断网卡状态
	if (buf[intrface].ifr_flags & IFF_UP)
	{
	//puts("the interface status is UP");
	}
	else
	{
	puts("the interface status is DOWN");
	}


	//fd1=open(dest,O_RDWR|O_CREAT);
	lseek(fd1,0,SEEK_END);
	//获取当前网卡的IP地址
	if (!(ioctl (fd, SIOCGIFADDR, (char *) &buf[intrface])))
	{
	//puts ("IP address is:");
	//puts(inet_ntoa(((struct sockaddr_in*)(&buf[intrface].ifr_addr))->sin_addr));

	bufip=(char *)malloc(sizeof(char));
	strcpy(bufip,inet_ntoa(((struct sockaddr_in*)(&buf[intrface].ifr_addr))->sin_addr));

	//write(fd1,bufip,strlen(bufip));
	//write(fd1,"\n",strlen("\n"));

	//puts("");

	}
	else
	{
	char str[256];
	sprintf (str, "cpm: ioctl device %s", buf[intrface].ifr_name);
	perror (str);
	}

	//close (fd);
	//close (fd1);
	// free(bufip);
	}
	}
	return bufip;
}




