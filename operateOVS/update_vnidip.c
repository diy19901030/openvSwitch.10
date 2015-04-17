#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cJSON.h"

char *path_tbrvnid;
char *path_brvnid;
char *dest_ip;

void main(){			

	get_br_vnid();
	write_br_vnid(path_tbrvnid,path_brvnid);
	write_vnidip(dest_ip,path_brvnid);


}
