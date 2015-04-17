#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cJSON.h"

/*
char *path_brvnid3 = "/home/zzq/openvswitch-1.10.0/operateOVS/brvnid";
char *path_tbrvnid2 = "/home/zzq/openvswitch-1.10.0/operateOVS/t_brvnid";
char *dest_ip2 = "/home/zzq/openvswitch-1.10.0/operateOVS/vnidip";
char *path_tmp_ip = "/home/zzq/openvswitch-1.10.0/operateOVS/ip_write";
*/

extern char *path_tbrvnid;
extern char *path_brvnid;
extern char *dest_ip;
void main(){

	get_br_vnid();
	
	write_br_vnid(path_tbrvnid,path_brvnid);

	write_vnidip(dest_ip,path_brvnid);
	return;

}
