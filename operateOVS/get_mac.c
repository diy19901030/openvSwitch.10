#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cJSON.h"
//#include"updateovs.h"
#include"initovs.h"

extern char *dest_mac;
extern char *path_brvnid;
extern char *path_port;

void main(){

	get_mac_form_vnmac("json");
	get_mac();

}
