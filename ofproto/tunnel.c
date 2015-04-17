/* Copyright (c) 2013 Nicira, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. */

#include <config.h>
#include "tunnel.h"

#include <errno.h>

#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <float.h>
#include <limits.h>
#include <ctype.h>

#include "ofproto/ofproto-provider.h"
#include "byte-order.h"
#include "dynamic-string.h"
#include "hash.h"
#include "hmap.h"
#include "netdev-vport.h"
#include "odp-util.h"
#include "packets.h"
#include "smap.h"
#include "socket-util.h"
#include "tunnel.h"
#include "vlog.h"
//#include "cJSON.h"

char *file_macip_part = "/openvswitch-1.10.0/operateOVS/mac_ip";
char *file_mac_write_part = "/openvswitch-1.10.0/operateOVS/mac_write";
char *file_vnid_mac_part = "/openvswitch-1.10.0/operateOVS/vnid-mac";
char *file_vnid_br_part = "/openvswitch-1.10.0/operateOVS/brvnid";
char *file_odptype_part = "/openvswitch-1.10.0/operateOVS/odptype";

char file_macip[80];
char file_mac_write[80];
char file_vnid_mac[80];
char file_vnid_br[80];
char file_odptype[80];
/* XXX:
 *
 * Ability to generate metadata for packet-outs
 * Disallow netdevs with names like "gre64_system" to prevent collisions. */

VLOG_DEFINE_THIS_MODULE(tunnel);
/*
struct tnl_match {
    ovs_be64 in_key;
    ovs_be32 ip_src;
    ovs_be32 ip_dst;
    uint32_t odp_port;
    uint32_t skb_mark;
    bool in_key_flow;
};
*/

#define Tranverse64(X)                 ((((uint64_t)(X) & 0xff00000000000000) >> 56) | \
                                         (((uint64_t)(X) & 0x00ff000000000000) >> 40) | \
                                         (((uint64_t)(X) & 0x0000ff0000000000) >> 24) | \
                                         (((uint64_t)(X) & 0x000000ff00000000) >> 8)  | \
					 (((uint64_t)(X) & 0x00000000ff000000) << 8)  | \
					 (((uint64_t)(X) & 0x0000000000ff0000) <<24)  | \
					 (((uint64_t)(X) & 0x000000000000ff00) <<40)  | \
					 (((uint64_t)(X) & 0x00000000000000ff) <<56))


struct tnl_port {
    struct hmap_node match_node;

    const struct ofport *ofport;
    unsigned int netdev_seq;
    struct tnl_match match;
};


char ip[20];
char vnid[10];
char br[10];
int type;
static struct hmap tnl_match_map = HMAP_INITIALIZER(&tnl_match_map);

/* Returned to callers when their ofport will never be used to receive or send
 * tunnel traffic. Alternatively, we could ask the caller to delete their
 * ofport, but this would be unclean in the reconfguration case.  For the first
 * time, an ofproto provider would have to call ofproto_port_del() on itself.*/
static struct tnl_port void_tnl_port;

static struct vlog_rate_limit rl = VLOG_RATE_LIMIT_INIT(1, 5);
static struct vlog_rate_limit dbg_rl = VLOG_RATE_LIMIT_INIT(60, 60);

static struct tnl_port *tnl_find(struct tnl_match *);
//static struct tnl_port *tnl_find_exact(struct tnl_match *);
static uint32_t tnl_hash(struct tnl_match *);
static void tnl_match_fmt(const struct tnl_match *, struct ds *);
static char *tnl_port_fmt(const struct tnl_port *);
static void tnl_port_mod_log(const struct tnl_port *, const char *action);
static const char *tnl_port_get_name(const struct tnl_port *);

//static struct tnl_port * tnl_find2(struct tnl_match *);


/*
  Copyright (c) 2009 Dave Gamble

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

/* cJSON */
/* JSON parser in C. */


//#include "cJSON.h"

static const char *ep;

const char *cJSON_GetErrorPtr(void) {return ep;}

static int cJSON_strcasecmp(const char *s1,const char *s2)
{
	if (!s1) return (s1==s2)?0:1;if (!s2) return 1;
	for(; tolower(*s1) == tolower(*s2); ++s1, ++s2)	if(*s1 == 0)	return 0;
	return tolower(*(const unsigned char *)s1) - tolower(*(const unsigned char *)s2);
}

static void *(*cJSON_malloc)(size_t sz) = malloc;
static void (*cJSON_free)(void *ptr) = free;

static char* cJSON_strdup(const char* str)
{
      size_t len;
      char* copy;

      len = strlen(str) + 1;
      if (!(copy = (char*)cJSON_malloc(len))) return 0;
      memcpy(copy,str,len);
      return copy;
}

void cJSON_InitHooks(cJSON_Hooks* hooks)
{
    if (!hooks) { /* Reset hooks */
        cJSON_malloc = malloc;
        cJSON_free = free;
        return;
    }

	cJSON_malloc = (hooks->malloc_fn)?hooks->malloc_fn:malloc;
	cJSON_free	 = (hooks->free_fn)?hooks->free_fn:free;
}

/* Internal constructor. */
static cJSON *cJSON_New_Item(void)
{
	cJSON* node = (cJSON*)cJSON_malloc(sizeof(cJSON));
	if (node) memset(node,0,sizeof(cJSON));
	return node;
}

/* Delete a cJSON structure. */
void cJSON_Delete(cJSON *c)
{
	cJSON *next;
	while (c)
	{
		next=c->next;
		if (!(c->type&cJSON_IsReference) && c->child) cJSON_Delete(c->child);
		if (!(c->type&cJSON_IsReference) && c->valuestring) cJSON_free(c->valuestring);
		if (c->string) cJSON_free(c->string);
		cJSON_free(c);
		c=next;
	}
}

/* Parse the input text to generate a number, and populate the result into item. */
static const char *parse_number(cJSON *item,const char *num)
{
	double n=0,sign=1,scale=0;int subscale=0,signsubscale=1;

	if (*num=='-') sign=-1,num++;	/* Has sign? */
	if (*num=='0') num++;			/* is zero */
	if (*num>='1' && *num<='9')	do	n=(n*10.0)+(*num++ -'0');	while (*num>='0' && *num<='9');	/* Number? */
	if (*num=='.' && num[1]>='0' && num[1]<='9') {num++;		do	n=(n*10.0)+(*num++ -'0'),scale--; while (*num>='0' && *num<='9');}	/* Fractional part? */
	if (*num=='e' || *num=='E')		/* Exponent? */
	{	num++;if (*num=='+') num++;	else if (*num=='-') signsubscale=-1,num++;		/* With sign? */
		while (*num>='0' && *num<='9') subscale=(subscale*10)+(*num++ - '0');	/* Number? */
	}

	n=sign*n*pow(10.0,(scale+subscale*signsubscale));	/* number = +/- number.fraction * 10^+/- exponent */
	
	item->valuedouble=n;
	item->valueint=(int)n;
	item->type=cJSON_Number;
	return num;
}

/* Render the number nicely from the given item into a string. */
static char *print_number(cJSON *item)
{
	char *str;
	double d=item->valuedouble;
	if (fabs(((double)item->valueint)-d)<=DBL_EPSILON && d<=INT_MAX && d>=INT_MIN)
	{
		str=(char*)cJSON_malloc(21);	/* 2^64+1 can be represented in 21 chars. */
		if (str) sprintf(str,"%d",item->valueint);
	}
	else
	{
		str=(char*)cJSON_malloc(64);	/* This is a nice tradeoff. */
		if (str)
		{
			if (fabs(floor(d)-d)<=DBL_EPSILON && fabs(d)<1.0e60)sprintf(str,"%.0f",d);
			else if (fabs(d)<1.0e-6 || fabs(d)>1.0e9)			sprintf(str,"%e",d);
			else												sprintf(str,"%f",d);
		}
	}
	return str;
}

static unsigned parse_hex4(const char *str)
{
	unsigned h=0;
	if (*str>='0' && *str<='9') h+=(*str)-'0'; else if (*str>='A' && *str<='F') h+=10+(*str)-'A'; else if (*str>='a' && *str<='f') h+=10+(*str)-'a'; else return 0;
	h=h<<4;str++;
	if (*str>='0' && *str<='9') h+=(*str)-'0'; else if (*str>='A' && *str<='F') h+=10+(*str)-'A'; else if (*str>='a' && *str<='f') h+=10+(*str)-'a'; else return 0;
	h=h<<4;str++;
	if (*str>='0' && *str<='9') h+=(*str)-'0'; else if (*str>='A' && *str<='F') h+=10+(*str)-'A'; else if (*str>='a' && *str<='f') h+=10+(*str)-'a'; else return 0;
	h=h<<4;str++;
	if (*str>='0' && *str<='9') h+=(*str)-'0'; else if (*str>='A' && *str<='F') h+=10+(*str)-'A'; else if (*str>='a' && *str<='f') h+=10+(*str)-'a'; else return 0;
	return h;
}

/* Parse the input text into an unescaped cstring, and populate item. */
static const unsigned char firstByteMark[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };
static const char *parse_string(cJSON *item,const char *str)
{
	const char *ptr=str+1;char *ptr2;char *out;int len=0;unsigned uc,uc2;
	if (*str!='\"') {ep=str;return 0;}	/* not a string! */
	
	while (*ptr!='\"' && *ptr && ++len) if (*ptr++ == '\\') ptr++;	/* Skip escaped quotes. */
	
	out=(char*)cJSON_malloc(len+1);	/* This is how long we need for the string, roughly. */
	if (!out) return 0;
	
	ptr=str+1;ptr2=out;
	while (*ptr!='\"' && *ptr)
	{
		if (*ptr!='\\') *ptr2++=*ptr++;
		else
		{
			ptr++;
			switch (*ptr)
			{
				case 'b': *ptr2++='\b';	break;
				case 'f': *ptr2++='\f';	break;
				case 'n': *ptr2++='\n';	break;
				case 'r': *ptr2++='\r';	break;
				case 't': *ptr2++='\t';	break;
				case 'u':	 /* transcode utf16 to utf8. */
					uc=parse_hex4(ptr+1);ptr+=4;	/* get the unicode char. */

					if ((uc>=0xDC00 && uc<=0xDFFF) || uc==0)	break;	/* check for invalid.	*/

					if (uc>=0xD800 && uc<=0xDBFF)	/* UTF16 surrogate pairs.	*/
					{
						if (ptr[1]!='\\' || ptr[2]!='u')	break;	/* missing second-half of surrogate.	*/
						uc2=parse_hex4(ptr+3);ptr+=6;
						if (uc2<0xDC00 || uc2>0xDFFF)		break;	/* invalid second-half of surrogate.	*/
						uc=0x10000 + (((uc&0x3FF)<<10) | (uc2&0x3FF));
					}

					len=4;if (uc<0x80) len=1;else if (uc<0x800) len=2;else if (uc<0x10000) len=3; ptr2+=len;
					
					switch (len) {
						case 4: *--ptr2 =((uc | 0x80) & 0xBF); uc >>= 6;
						case 3: *--ptr2 =((uc | 0x80) & 0xBF); uc >>= 6;
						case 2: *--ptr2 =((uc | 0x80) & 0xBF); uc >>= 6;
						case 1: *--ptr2 =(uc | firstByteMark[len]);
					}
					ptr2+=len;
					break;
				default:  *ptr2++=*ptr; break;
			}
			ptr++;
		}
	}
	*ptr2=0;
	if (*ptr=='\"') ptr++;
	item->valuestring=out;
	item->type=cJSON_String;
	return ptr;
}

/* Render the cstring provided to an escaped version that can be printed. */
static char *print_string_ptr(const char *str)
{
	const char *ptr;char *ptr2,*out;int len=0;unsigned char token;
	
	if (!str) return cJSON_strdup("");
	ptr=str;while ((token=*ptr) && ++len) {if (strchr("\"\\\b\f\n\r\t",token)) len++; else if (token<32) len+=5;ptr++;}
	
	out=(char*)cJSON_malloc(len+3);
	if (!out) return 0;

	ptr2=out;ptr=str;
	*ptr2++='\"';
	while (*ptr)
	{
		if ((unsigned char)*ptr>31 && *ptr!='\"' && *ptr!='\\') *ptr2++=*ptr++;
		else
		{
			*ptr2++='\\';
			switch (token=*ptr++)
			{
				case '\\':	*ptr2++='\\';	break;
				case '\"':	*ptr2++='\"';	break;
				case '\b':	*ptr2++='b';	break;
				case '\f':	*ptr2++='f';	break;
				case '\n':	*ptr2++='n';	break;
				case '\r':	*ptr2++='r';	break;
				case '\t':	*ptr2++='t';	break;
				default: sprintf(ptr2,"u%04x",token);ptr2+=5;	break;	/* escape and print */
			}
		}
	}
	*ptr2++='\"';*ptr2++=0;
	return out;
}
/* Invote print_string_ptr (which is useful) on an item. */
static char *print_string(cJSON *item)	{return print_string_ptr(item->valuestring);}

/* Predeclare these prototypes. */
static const char *parse_value(cJSON *item,const char *value);
static char *print_value(cJSON *item,int depth,int fmt);
static const char *parse_array(cJSON *item,const char *value);
static char *print_array(cJSON *item,int depth,int fmt);
static const char *parse_object(cJSON *item,const char *value);
static char *print_object(cJSON *item,int depth,int fmt);

/* Utility to jump whitespace and cr/lf */
static const char *skip(const char *in) {while (in && *in && (unsigned char)*in<=32) in++; return in;}

/* Parse an object - create a new root, and populate. */
cJSON *cJSON_ParseWithOpts(const char *value,const char **return_parse_end,int require_null_terminated)
{
	const char *end=0;
	cJSON *c=cJSON_New_Item();
	ep=0;
	if (!c) return 0;       /* memory fail */

	end=parse_value(c,skip(value));
	if (!end)	{cJSON_Delete(c);return 0;}	/* parse failure. ep is set. */

	/* if we require null-terminated JSON without appended garbage, skip and then check for a null terminator */
	if (require_null_terminated) {end=skip(end);if (*end) {cJSON_Delete(c);ep=end;return 0;}}
	if (return_parse_end) *return_parse_end=end;
	return c;
}
/* Default options for cJSON_Parse */
cJSON *cJSON_Parse(const char *value) {return cJSON_ParseWithOpts(value,0,0);}

/* Render a cJSON item/entity/structure to text. */
char *cJSON_Print(cJSON *item)				{return print_value(item,0,1);}
char *cJSON_PrintUnformatted(cJSON *item)	{return print_value(item,0,0);}

/* Parser core - when encountering text, process appropriately. */
static const char *parse_value(cJSON *item,const char *value)
{
	if (!value)						return 0;	/* Fail on null. */
	if (!strncmp(value,"null",4))	{ item->type=cJSON_NULL;  return value+4; }
	if (!strncmp(value,"false",5))	{ item->type=cJSON_False; return value+5; }
	if (!strncmp(value,"true",4))	{ item->type=cJSON_True; item->valueint=1;	return value+4; }
	if (*value=='\"')				{ return parse_string(item,value); }
	if (*value=='-' || (*value>='0' && *value<='9'))	{ return parse_number(item,value); }
	if (*value=='[')				{ return parse_array(item,value); }
	if (*value=='{')				{ return parse_object(item,value); }

	ep=value;return 0;	/* failure. */
}

/* Render a value to text. */
static char *print_value(cJSON *item,int depth,int fmt)
{
	char *out=0;
	if (!item) return 0;
	switch ((item->type)&255)
	{
		case cJSON_NULL:	out=cJSON_strdup("null");	break;
		case cJSON_False:	out=cJSON_strdup("false");break;
		case cJSON_True:	out=cJSON_strdup("true"); break;
		case cJSON_Number:	out=print_number(item);break;
		case cJSON_String:	out=print_string(item);break;
		case cJSON_Array:	out=print_array(item,depth,fmt);break;
		case cJSON_Object:	out=print_object(item,depth,fmt);break;
	}
	return out;
}

/* Build an array from input text. */
static const char *parse_array(cJSON *item,const char *value)
{
	cJSON *child;
	if (*value!='[')	{ep=value;return 0;}	/* not an array! */

	item->type=cJSON_Array;
	value=skip(value+1);
	if (*value==']') return value+1;	/* empty array. */

	item->child=child=cJSON_New_Item();
	if (!item->child) return 0;		 /* memory fail */
	value=skip(parse_value(child,skip(value)));	/* skip any spacing, get the value. */
	if (!value) return 0;

	while (*value==',')
	{
		cJSON *new_item;
		if (!(new_item=cJSON_New_Item())) return 0; 	/* memory fail */
		child->next=new_item;new_item->prev=child;child=new_item;
		value=skip(parse_value(child,skip(value+1)));
		if (!value) return 0;	/* memory fail */
	}

	if (*value==']') return value+1;	/* end of array */
	ep=value;return 0;	/* malformed. */
}

/* Render an array to text */
static char *print_array(cJSON *item,int depth,int fmt)
{
	char **entries;
	char *out=0,*ptr,*ret;int len=5;
	cJSON *child=item->child;
	int numentries=0,i=0,fail=0;
	
	/* How many entries in the array? */
	while (child) numentries++,child=child->next;
	/* Explicitly handle numentries==0 */
	if (!numentries)
	{
		out=(char*)cJSON_malloc(3);
		if (out) strcpy(out,"[]");
		return out;
	}
	/* Allocate an array to hold the values for each */
	entries=(char**)cJSON_malloc(numentries*sizeof(char*));
	if (!entries) return 0;
	memset(entries,0,numentries*sizeof(char*));
	/* Retrieve all the results: */
	child=item->child;
	while (child && !fail)
	{
		ret=print_value(child,depth+1,fmt);
		entries[i++]=ret;
		if (ret) len+=strlen(ret)+2+(fmt?1:0); else fail=1;
		child=child->next;
	}
	
	/* If we didn't fail, try to malloc the output string */
	if (!fail) out=(char*)cJSON_malloc(len);
	/* If that fails, we fail. */
	if (!out) fail=1;

	/* Handle failure. */
	if (fail)
	{
		for (i=0;i<numentries;i++) if (entries[i]) cJSON_free(entries[i]);
		cJSON_free(entries);
		return 0;
	}
	
	/* Compose the output array. */
	*out='[';
	ptr=out+1;*ptr=0;
	for (i=0;i<numentries;i++)
	{
		strcpy(ptr,entries[i]);ptr+=strlen(entries[i]);
		if (i!=numentries-1) {*ptr++=',';if(fmt)*ptr++=' ';*ptr=0;}
		cJSON_free(entries[i]);
	}
	cJSON_free(entries);
	*ptr++=']';*ptr++=0;
	return out;	
}

/* Build an object from the text. */
static const char *parse_object(cJSON *item,const char *value)
{
	cJSON *child;
	if (*value!='{')	{ep=value;return 0;}	/* not an object! */
	
	item->type=cJSON_Object;
	value=skip(value+1);
	if (*value=='}') return value+1;	/* empty array. */
	
	item->child=child=cJSON_New_Item();
	if (!item->child) return 0;
	value=skip(parse_string(child,skip(value)));
	if (!value) return 0;
	child->string=child->valuestring;child->valuestring=0;
	if (*value!=':') {ep=value;return 0;}	/* fail! */
	value=skip(parse_value(child,skip(value+1)));	/* skip any spacing, get the value. */
	if (!value) return 0;
	
	while (*value==',')
	{
		cJSON *new_item;
		if (!(new_item=cJSON_New_Item()))	return 0; /* memory fail */
		child->next=new_item;new_item->prev=child;child=new_item;
		value=skip(parse_string(child,skip(value+1)));
		if (!value) return 0;
		child->string=child->valuestring;child->valuestring=0;
		if (*value!=':') {ep=value;return 0;}	/* fail! */
		value=skip(parse_value(child,skip(value+1)));	/* skip any spacing, get the value. */
		if (!value) return 0;
	}
	
	if (*value=='}') return value+1;	/* end of array */
	ep=value;return 0;	/* malformed. */
}

/* Render an object to text. */
static char *print_object(cJSON *item,int depth,int fmt)
{
	char **entries=0,**names=0;
	char *out=0,*ptr,*ret,*str;int len=7,i=0,j;
	cJSON *child=item->child;
	int numentries=0,fail=0;
	/* Count the number of entries. */
	while (child) numentries++,child=child->next;
	/* Explicitly handle empty object case */
	if (!numentries)
	{
		out=(char*)cJSON_malloc(fmt?depth+4:3);
		if (!out)	return 0;
		ptr=out;*ptr++='{';
		if (fmt) {*ptr++='\n';for (i=0;i<depth-1;i++) *ptr++='\t';}
		*ptr++='}';*ptr++=0;
		return out;
	}
	/* Allocate space for the names and the objects */
	entries=(char**)cJSON_malloc(numentries*sizeof(char*));
	if (!entries) return 0;
	names=(char**)cJSON_malloc(numentries*sizeof(char*));
	if (!names) {cJSON_free(entries);return 0;}
	memset(entries,0,sizeof(char*)*numentries);
	memset(names,0,sizeof(char*)*numentries);

	/* Collect all the results into our arrays: */
	child=item->child;depth++;if (fmt) len+=depth;
	while (child)
	{
		names[i]=str=print_string_ptr(child->string);
		entries[i++]=ret=print_value(child,depth,fmt);
		if (str && ret) len+=strlen(ret)+strlen(str)+2+(fmt?2+depth:0); else fail=1;
		child=child->next;
	}
	
	/* Try to allocate the output string */
	if (!fail) out=(char*)cJSON_malloc(len);
	if (!out) fail=1;

	/* Handle failure */
	if (fail)
	{
		for (i=0;i<numentries;i++) {if (names[i]) cJSON_free(names[i]);if (entries[i]) cJSON_free(entries[i]);}
		cJSON_free(names);cJSON_free(entries);
		return 0;
	}
	
	/* Compose the output: */
	*out='{';ptr=out+1;if (fmt)*ptr++='\n';*ptr=0;
	for (i=0;i<numentries;i++)
	{
		if (fmt) for (j=0;j<depth;j++) *ptr++='\t';
		strcpy(ptr,names[i]);ptr+=strlen(names[i]);
		*ptr++=':';if (fmt) *ptr++='\t';
		strcpy(ptr,entries[i]);ptr+=strlen(entries[i]);
		if (i!=numentries-1) *ptr++=',';
		if (fmt) *ptr++='\n';*ptr=0;
		cJSON_free(names[i]);cJSON_free(entries[i]);
	}
	
	cJSON_free(names);cJSON_free(entries);
	if (fmt) for (i=0;i<depth-1;i++) *ptr++='\t';
	*ptr++='}';*ptr++=0;
	return out;	
}

/* Get Array size/item / object item. */
int    cJSON_GetArraySize(cJSON *array)							{cJSON *c=array->child;int i=0;while(c)i++,c=c->next;return i;}
cJSON *cJSON_GetArrayItem(cJSON *array,int item)				{cJSON *c=array->child;  while (c && item>0) item--,c=c->next; return c;}
cJSON *cJSON_GetObjectItem(cJSON *object,const char *string)	{cJSON *c=object->child; while (c && cJSON_strcasecmp(c->string,string)) c=c->next; return c;}

/* Utility for array list handling. */
static void suffix_object(cJSON *prev,cJSON *item) {prev->next=item;item->prev=prev;}
/* Utility for handling references. */
static cJSON *create_reference(cJSON *item) {cJSON *ref=cJSON_New_Item();if (!ref) return 0;memcpy(ref,item,sizeof(cJSON));ref->string=0;ref->type|=cJSON_IsReference;ref->next=ref->prev=0;return ref;}

/* Add item to array/object. *///zzq
void   cJSON_AddItemToArray(cJSON *array, cJSON *item)						{cJSON *c=array->child;if (!item) return;/*if(!strcmp(array->valuestring,item->valuestring)) return ; */if (!c) {array->child=item;} else {while (c && c->next) c=c->next; suffix_object(c,item);}}
void   cJSON_AddItemToObject(cJSON *object,const char *string,cJSON *item)	{if (!item) return; if (item->string) cJSON_free(item->string);item->string=cJSON_strdup(string);cJSON_AddItemToArray(object,item);}
void	cJSON_AddItemReferenceToArray(cJSON *array, cJSON *item)						{cJSON_AddItemToArray(array,create_reference(item));}
void	cJSON_AddItemReferenceToObject(cJSON *object,const char *string,cJSON *item)	{cJSON_AddItemToObject(object,string,create_reference(item));}

cJSON *cJSON_DetachItemFromArray(cJSON *array,int which)			{cJSON *c=array->child;while (c && which>0) c=c->next,which--;if (!c) return 0;
	if (c->prev) c->prev->next=c->next;if (c->next) c->next->prev=c->prev;if (c==array->child) array->child=c->next;c->prev=c->next=0;return c;}
void   cJSON_DeleteItemFromArray(cJSON *array,int which)			{cJSON_Delete(cJSON_DetachItemFromArray(array,which));}
cJSON *cJSON_DetachItemFromObject(cJSON *object,const char *string) {int i=0;cJSON *c=object->child;while (c && cJSON_strcasecmp(c->string,string)) i++,c=c->next;if (c) return cJSON_DetachItemFromArray(object,i);return 0;}
void   cJSON_DeleteItemFromObject(cJSON *object,const char *string) {cJSON_Delete(cJSON_DetachItemFromObject(object,string));}

/* Replace array/object items with new ones. */
void   cJSON_ReplaceItemInArray(cJSON *array,int which,cJSON *newitem)		{cJSON *c=array->child;while (c && which>0) c=c->next,which--;if (!c) return;
	newitem->next=c->next;newitem->prev=c->prev;if (newitem->next) newitem->next->prev=newitem;
	if (c==array->child) array->child=newitem; else newitem->prev->next=newitem;c->next=c->prev=0;cJSON_Delete(c);}
void   cJSON_ReplaceItemInObject(cJSON *object,const char *string,cJSON *newitem){int i=0;cJSON *c=object->child;while(c && cJSON_strcasecmp(c->string,string))i++,c=c->next;if(c){newitem->string=cJSON_strdup(string);cJSON_ReplaceItemInArray(object,i,newitem);}}

/* Create basic types: */
cJSON *cJSON_CreateNull(void)					{cJSON *item=cJSON_New_Item();if(item)item->type=cJSON_NULL;return item;}
cJSON *cJSON_CreateTrue(void)					{cJSON *item=cJSON_New_Item();if(item)item->type=cJSON_True;return item;}
cJSON *cJSON_CreateFalse(void)					{cJSON *item=cJSON_New_Item();if(item)item->type=cJSON_False;return item;}
cJSON *cJSON_CreateBool(int b)					{cJSON *item=cJSON_New_Item();if(item)item->type=b?cJSON_True:cJSON_False;return item;}
cJSON *cJSON_CreateNumber(double num)			{cJSON *item=cJSON_New_Item();if(item){item->type=cJSON_Number;item->valuedouble=num;item->valueint=(int)num;}return item;}
cJSON *cJSON_CreateString(const char *string)	{cJSON *item=cJSON_New_Item();if(item){item->type=cJSON_String;item->valuestring=cJSON_strdup(string);}return item;}
cJSON *cJSON_CreateArray(void)					{cJSON *item=cJSON_New_Item();if(item)item->type=cJSON_Array;return item;}
cJSON *cJSON_CreateObject(void)					{cJSON *item=cJSON_New_Item();if(item)item->type=cJSON_Object;return item;}

/* Create Arrays: */
cJSON *cJSON_CreateIntArray(const int *numbers,int count)		{int i;cJSON *n=0,*p=0,*a=cJSON_CreateArray();for(i=0;a && i<count;i++){n=cJSON_CreateNumber(numbers[i]);if(!i)a->child=n;else suffix_object(p,n);p=n;}return a;}
cJSON *cJSON_CreateFloatArray(const float *numbers,int count)	{int i;cJSON *n=0,*p=0,*a=cJSON_CreateArray();for(i=0;a && i<count;i++){n=cJSON_CreateNumber(numbers[i]);if(!i)a->child=n;else suffix_object(p,n);p=n;}return a;}
cJSON *cJSON_CreateDoubleArray(const double *numbers,int count)	{int i;cJSON *n=0,*p=0,*a=cJSON_CreateArray();for(i=0;a && i<count;i++){n=cJSON_CreateNumber(numbers[i]);if(!i)a->child=n;else suffix_object(p,n);p=n;}return a;}
cJSON *cJSON_CreateStringArray(const char **strings,int count)	{int i;cJSON *n=0,*p=0,*a=cJSON_CreateArray();for(i=0;a && i<count;i++){n=cJSON_CreateString(strings[i]);if(!i)a->child=n;else suffix_object(p,n);p=n;}return a;}

/* Duplication */
cJSON *cJSON_Duplicate(cJSON *item,int recurse)
{
	cJSON *newitem,*cptr,*nptr=0,*newchild;
	/* Bail on bad ptr */
	if (!item) return 0;
	/* Create new item */
	newitem=cJSON_New_Item();
	if (!newitem) return 0;
	/* Copy over all vars */
	newitem->type=item->type&(~cJSON_IsReference),newitem->valueint=item->valueint,newitem->valuedouble=item->valuedouble;
	if (item->valuestring)	{newitem->valuestring=cJSON_strdup(item->valuestring);	if (!newitem->valuestring)	{cJSON_Delete(newitem);return 0;}}
	if (item->string)		{newitem->string=cJSON_strdup(item->string);			if (!newitem->string)		{cJSON_Delete(newitem);return 0;}}
	/* If non-recursive, then we're done! */
	if (!recurse) return newitem;
	/* Walk the ->next chain for the child. */
	cptr=item->child;
	while (cptr)
	{
		newchild=cJSON_Duplicate(cptr,1);		/* Duplicate (with recurse) each item in the ->next chain */
		if (!newchild) {cJSON_Delete(newitem);return 0;}
		if (nptr)	{nptr->next=newchild,newchild->prev=nptr;nptr=newchild;}	/* If newitem->child already set, then crosswire ->prev and ->next and move on */
		else		{newitem->child=newchild;nptr=newchild;}					/* Set newitem->child and move to it */
		cptr=cptr->next;
	}
	return newitem;
}

void cJSON_Minify(char *json)
{
	char *into=json;
	while (*json)
	{
		if (*json==' ') json++;
		else if (*json=='\t') json++;	// Whitespace characters.
		else if (*json=='\r') json++;
		else if (*json=='\n') json++;
		else if (*json=='/' && json[1]=='/')  while (*json && *json!='\n') json++;	// double-slash comments, to end of line.
		else if (*json=='/' && json[1]=='*') {while (*json && !(*json=='*' && json[1]=='/')) json++;json+=2;}	// multiline comments.
		else if (*json=='\"'){*into++=*json++;while (*json && *json!='\"'){if (*json=='\\') *into++=*json++;*into++=*json++;}*into++=*json++;} // string literals, which are \" sensitive.
		else *into++=*json++;			// All other characters.
	}
	*into=0;	// and null-terminate.
}


static struct tnl_port *
tnl_port_add__(const struct ofport *ofport, uint32_t odp_port,
               bool warn)
{
    const struct netdev_tunnel_config *cfg;
    struct tnl_port *existing_port;
    struct tnl_port *tnl_port;

    cfg = netdev_get_tunnel_config(ofport->netdev);
    ovs_assert(cfg);

    tnl_port = xzalloc(sizeof *tnl_port);
    tnl_port->ofport = ofport;
    tnl_port->netdev_seq = netdev_change_seq(tnl_port->ofport->netdev);

   

    tnl_port->match.in_key = cfg->in_key; /*have modified -- tnl_port->match.in_key = cfg->in_key*/
    tnl_port->match.ip_src = cfg->ip_src;
    tnl_port->match.ip_dst = cfg->ip_dst;
    tnl_port->match.skb_mark = cfg->ipsec ? IPSEC_MARK : 0;
    tnl_port->match.in_key_flow = 0;/*have modified -- tnl_port->match.in_key = cfg->in_key_flow*/
    tnl_port->match.odp_port = odp_port;



/*
    VLOG_INFO("tnl_port_add__ ofport->pp.name is %s\n",ofport->pp.name);
    VLOG_INFO("tnl_port_add__ ofport->pp.port_no is %u\n",ofport->pp.port_no);
    VLOG_INFO("tnl_port_add__ cfg->in_key is %llx\n",cfg->in_key);
    if(ofport->pp.hw_addr) VLOG_INFO("tnl_port_add__ ofport->pp.hw_addr is %u\n",ofport->pp.hw_addr);
    VLOG_INFO("tnl_port_add__ hash  is %lu\n",tnl_hash(&tnl_port->match));
    VLOG_INFO("tnl_port_add__ ofport name is %s\n",ofport->ofproto->name);
    VLOG_INFO("tnl_port_add__ ofport type is %s\n",ofport->ofproto->type);
    VLOG_INFO("tnl_port_add__ ofport fallback id is %llu\n",ofport->ofproto->fallback_dpid);
    VLOG_INFO("tnl_port_add__ ofport datapath id is %llu\n",ofport->ofproto->datapath_id);
    VLOG_INFO("tnl_port_add__ tnl_port->match.ip_dst is %08x\n",ntohl(tnl_port->match.ip_dst));
    VLOG_INFO("tnl_port_add__ tnl_port->match.odp_port is %d\n",tnl_port->match.odp_port);
    VLOG_INFO("tnl_port_add__ tnl_port->match.in_key is %llu\n",tnl_port->match.in_key);
*/
    existing_port = tnl_find_exact(&tnl_port->match);
    if (existing_port) {
        if (warn) {
            struct ds ds = DS_EMPTY_INITIALIZER;
            tnl_match_fmt(&tnl_port->match, &ds);
            VLOG_WARN("%s: attempting to add tunnel port with same config as "
                      "port '%s' (%s)", tnl_port_get_name(tnl_port),
                      tnl_port_get_name(existing_port), ds_cstr(&ds));
            ds_destroy(&ds);
            free(tnl_port);
        }
        return &void_tnl_port;
    }

    hmap_insert(&tnl_match_map, &tnl_port->match_node,
                tnl_hash(&tnl_port->match));
    tnl_port_mod_log(tnl_port, "adding");

    struct tnl_port *tmp_tnl_port;
    struct tnl_port *tnl_port1;
    uint64_t inkey;
    struct netdev_tunnel_config *cfg1;
    cfg1 = xzalloc(sizeof * cfg1);
    cfg1->in_key = 0;
    inkey = 0;
    tnl_port1 = xzalloc(sizeof *tnl_port1);
    tnl_port1->match.in_key = 0;
    tnl_port1->match.ip_src = 0;
    tnl_port1->match.ip_dst = inet_addr("192.168.2.10");
    tnl_port1->match.odp_port = 4;
    tnl_port1->match.skb_mark = 0;
    tnl_port1->match.in_key_flow = 0;
/*
    VLOG_INFO("tnl_port_add__ inkey length is %d\n",sizeof(tnl_port1->match.in_key));
    VLOG_INFO("tnl_port_add__ cfg->in_key length is %d\n",sizeof(cfg->in_key));
    VLOG_INFO("tnl_port_add__ in_key  is %llx\n",tnl_port1->match.in_key);
    VLOG_INFO("tnl_port_add__ src ip  is %08x\n",tnl_port1->match.ip_src);
    VLOG_INFO("tnl_port_add__ dst ip  is %08x\n",tnl_port1->match.ip_dst);
    VLOG_INFO("tnl_port_add__ odp_port  is %d\n",tnl_port1->match.odp_port);
    VLOG_INFO("tnl_port_add__ skb_mark  is %d\n",tnl_port1->match.skb_mark);
    VLOG_INFO("tnl_port_add__ in_key_flow  is %d\n",tnl_port1->match.in_key_flow);

    tmp_tnl_port = tnl_find_exact(&tnl_port1->match);
    if(tmp_tnl_port) {
	VLOG_INFO("tnl_port_add__ tmp tnl_port ip_dst is %08x\n", ntohl(tmp_tnl_port->match.ip_dst));
	VLOG_INFO("tnl_port_add__ tmp tnl_port ofport ofp_port is %d\n", tmp_tnl_port->ofport->ofp_port);
    }
*/
    return tnl_port;
}

/* Adds 'ofport' to the module with datapath port number 'odp_port'. 'ofport's
 * must be added before they can be used by the module. 'ofport' must be a
 * tunnel. */
struct tnl_port *
tnl_port_add(const struct ofport *ofport, uint32_t odp_port)
{
    return tnl_port_add__(ofport, odp_port, true);
}

/* Checks if the tnl_port pointed to by 'tnl_portp' needs reconfiguration due
 * to changes in its netdev_tunnel_config.  If it does, updates 'tnl_portp' to
 * point to a new tnl_port and returns true.  Otherwise, returns false.
 * 'ofport' and 'odp_port' should be the same as would be passed to
 * tnl_port_add(). */
bool
tnl_port_reconfigure(const struct ofport *ofport, uint32_t odp_port,
                     struct tnl_port **tnl_portp)
{
    struct tnl_port *tnl_port = *tnl_portp;


    if (tnl_port == &void_tnl_port) {
//	VLOG_INFO("tnl_port_reconfigure\n");
        *tnl_portp = tnl_port_add__(ofport, odp_port, false);
        return *tnl_portp != &void_tnl_port;
    } else if (tnl_port->ofport != ofport
               || tnl_port->match.odp_port != odp_port
               || tnl_port->netdev_seq != netdev_change_seq(ofport->netdev)) {
        VLOG_DBG("reconfiguring %s", tnl_port_get_name(tnl_port));
        tnl_port_del(tnl_port);
        *tnl_portp = tnl_port_add(ofport, odp_port);
        return true;
    }
    return false;
}

/* Removes 'tnl_port' from the module. */
void
tnl_port_del(struct tnl_port *tnl_port)
{
    if (tnl_port && tnl_port != &void_tnl_port) {
        tnl_port_mod_log(tnl_port, "removing");
        hmap_remove(&tnl_match_map, &tnl_port->match_node);
        free(tnl_port);
    }
}

/* Transforms 'flow' so that it appears to have been received by a tunnel
 * OpenFlow port controlled by this module instead of the datapath port it
 * actually came in on.  Sets 'flow''s in_port to the appropriate OpenFlow port
 * number.  Returns the 'ofport' corresponding to the new in_port.
 *
 * Callers should verify that 'flow' needs to be received by calling
 * tnl_port_should_receive() before this function.
 *
 * Leaves 'flow' untouched and returns null if unsuccessful. */
const struct ofport *
tnl_port_receive(struct flow *flow)
{
    char *pre_flow_str = NULL;
    struct tnl_port *tnl_port;
    struct tnl_match match;

    memset(&match, 0, sizeof match);
    match.odp_port = flow->in_port;
    match.ip_src = flow->tunnel.ip_dst;
    match.ip_dst = flow->tunnel.ip_src;
    match.in_key = flow->tunnel.tun_id;
    match.skb_mark = flow->skb_mark;

    tnl_port = tnl_find(&match);
    if (!tnl_port) {
        struct ds ds = DS_EMPTY_INITIALIZER;

        tnl_match_fmt(&match, &ds);
        VLOG_WARN_RL(&rl, "receive tunnel port not found (%s)", ds_cstr(&ds));
        ds_destroy(&ds);
        return NULL;
    }
/*   
    VLOG_INFO("tnl_port_receive src ip is %0x\n",ntohl(flow->tunnel.ip_src));
    VLOG_INFO("tnl_port_receive dst ip is %0x\n",ntohl(flow->tunnel.ip_dst));

    VLOG_INFO("tnl_port_receive flow src ip is %0x\n",ntohl(flow->nw_src));
    VLOG_INFO("tnl_port_receive flow dst ip is %0x\n",ntohl(flow->nw_dst));
    VLOG_INFO("tnl_port_receive flow src mac is %02x:%02x:%02x:%02x:%02x:%02x\n", 
										(unsigned char)(flow->dl_src[0]),
										(unsigned char)(flow->dl_src[1]),
										(unsigned char)(flow->dl_src[2]),
										(unsigned char)(flow->dl_src[3]),
										(unsigned char)(flow->dl_src[4]),
										(unsigned char)(flow->dl_src[5]));
    VLOG_INFO("tnl_port_receive flow dst mac is %02x:%02x:%02x:%02x:%02x:%02x\n", 
										(unsigned char)(flow->dl_dst[0]),
										(unsigned char)(flow->dl_dst[1]),
										(unsigned char)(flow->dl_dst[2]),
										(unsigned char)(flow->dl_dst[3]),
										(unsigned char)(flow->dl_dst[4]),
										(unsigned char)(flow->dl_dst[5]));
*/
 //   VLOG_INFO("tnl_port_receive!\n");
//    system("echo @@@@@@@ >> 444");
 //   printf("********\n");
    if (!VLOG_DROP_DBG(&dbg_rl)) {
        pre_flow_str = flow_to_string(flow);
    }
//    VLOG_INFO("tnl_port_receive ofp_port is %d\n",flow->in_port);
    flow->in_port = tnl_port->ofport->ofp_port;
    memset(&flow->tunnel, 0, sizeof flow->tunnel);
    flow->tunnel.tun_id = match.in_key;

    if (pre_flow_str) {
        char *post_flow_str = flow_to_string(flow);
        char *tnl_str = tnl_port_fmt(tnl_port);
        VLOG_DBG("flow received\n"
                 "%s"
                 " pre: %s\n"
                 "post: %s",
                 tnl_str, pre_flow_str, post_flow_str);
        free(tnl_str);
        free(pre_flow_str);
        free(post_flow_str);
    }
    return tnl_port->ofport;
}

/* Given that 'flow' should be output to the ofport corresponding to
 * 'tnl_port', updates 'flow''s tunnel headers and returns the actual datapath
 * port that the output should happen on.  May return OVSP_NONE if the output
 * shouldn't occur. */
uint32_t
tnl_port_send(const struct tnl_port *tnl_port, struct flow *flow)
{

    const struct netdev_tunnel_config *cfg;
    char *pre_flow_str = NULL;

    if (tnl_port == &void_tnl_port) {
        return OVSP_NONE;
    }

    cfg = netdev_get_tunnel_config(tnl_port->ofport->netdev);
    ovs_assert(cfg);

    if (!VLOG_DROP_DBG(&dbg_rl)) {
        pre_flow_str = flow_to_string(flow);
    }

    flow->tunnel.ip_src = tnl_port->match.ip_src;
    flow->tunnel.ip_dst = tnl_port->match.ip_dst;
    flow->skb_mark = tnl_port->match.skb_mark;
/*
    VLOG_INFO("tnl_port_send tun src ip is %0x\n",ntohl(flow->tunnel.ip_src));
    VLOG_INFO("tnl_port_send tun dst ip is %0x\n",ntohl(flow->tunnel.ip_dst));
    VLOG_INFO("tnl_port_send id is %u\n",flow->tunnel.tun_id);
    VLOG_INFO("tnl_port_send flow src ip is %0x\n",ntohl(flow->nw_src));
    VLOG_INFO("tnl_port_send flow dst ip is %0x\n",ntohl(flow->nw_dst));
    VLOG_INFO("tnl_port_send flow src mac is %02x:%02x:%02x:%02x:%02x:%02x\n", 
										(unsigned char)(flow->dl_src[0]),
										(unsigned char)(flow->dl_src[1]),
										(unsigned char)(flow->dl_src[2]),
										(unsigned char)(flow->dl_src[3]),
										(unsigned char)(flow->dl_src[4]),
										(unsigned char)(flow->dl_src[5]));
    VLOG_INFO("tnl_port_send flow dst mac is %02x:%02x:%02x:%02x:%02x:%02x\n", 
										(unsigned char)(flow->dl_dst[0]),
										(unsigned char)(flow->dl_dst[1]),
										(unsigned char)(flow->dl_dst[2]),
										(unsigned char)(flow->dl_dst[3]),
										(unsigned char)(flow->dl_dst[4]),
										(unsigned char)(flow->dl_dst[5]));
*/
//    VLOG_INFO("IS-IS level 1 send!\n");
    if (!cfg->out_key_flow) {
        flow->tunnel.tun_id = cfg->out_key;
    }

    if (cfg->ttl_inherit && is_ip_any(flow)) {
        flow->tunnel.ip_ttl = flow->nw_ttl;
    } else {
        flow->tunnel.ip_ttl = cfg->ttl;
    }

    if (cfg->tos_inherit && is_ip_any(flow)) {
        flow->tunnel.ip_tos = flow->nw_tos & IP_DSCP_MASK;
    } else {
        flow->tunnel.ip_tos = cfg->tos;
    }

    if ((flow->nw_tos & IP_ECN_MASK) == IP_ECN_CE) {
        flow->tunnel.ip_tos |= IP_ECN_ECT_0;
    } else {
        flow->tunnel.ip_tos |= flow->nw_tos & IP_ECN_MASK;
    }

    flow->tunnel.flags = (cfg->dont_fragment ? FLOW_TNL_F_DONT_FRAGMENT : 0)
        | (cfg->csum ? FLOW_TNL_F_CSUM : 0)
        | (cfg->out_key_present ? FLOW_TNL_F_KEY : 0);

    if (pre_flow_str) {
        char *post_flow_str = flow_to_string(flow);
        char *tnl_str = tnl_port_fmt(tnl_port);
        VLOG_DBG("flow sent\n"
                 "%s"
                 " pre: %s\n"
                 "post: %s",
                 tnl_str, pre_flow_str, post_flow_str);
        free(tnl_str);
        free(pre_flow_str);
        free(post_flow_str);
    }

    return tnl_port->match.odp_port;

}

static uint32_t
tnl_hash(struct tnl_match *match)
{
    BUILD_ASSERT_DECL(sizeof *match % sizeof(uint32_t) == 0);
    return hash_words((uint32_t *) match, sizeof *match / sizeof(uint32_t), 0);
}

struct tnl_port *
tnl_find_exact(struct tnl_match *match)
{
    struct tnl_port *tnl_port;

//    VLOG_INFO("tnl_find_exact begin match src ip is %08x\n", ntohl(match->ip_src));
//    VLOG_INFO("tnl_find_exact  hash is %d\n", tnl_hash(match));

    HMAP_FOR_EACH_WITH_HASH (tnl_port, match_node, tnl_hash(match),
                             &tnl_match_map) {
//	    VLOG_INFO("tnl_find_exact match odp_port is %d\n", match->odp_port);
//	    VLOG_INFO("tnl_find_exact match dst ip is %08x\n", ntohl(tnl_port->match.ip_dst));
        if (!memcmp(match, &tnl_port->match, sizeof *match)) {

            return tnl_port;
        }
    }
    return NULL;
}

static struct tnl_port *
tnl_find(struct tnl_match *match_)
{
    struct tnl_match match = *match_;
    struct tnl_port *tnl_port;

//    VLOG_INFO("tnl_find in_key is %d\n", match.in_key);
 //   VLOG_INFO("tnl_find begin match dst ip is %08x\n", match.ip_dst);

    /* remote_ip, local_ip, in_key */
    tnl_port = tnl_find_exact(&match);
    if (tnl_port) {
        return tnl_port;
    }

    /* remote_ip, in_key */
    match.ip_src = 0;
//    match.in_key = 0;
    tnl_port = tnl_find_exact(&match);
    if (tnl_port) {
        return tnl_port;
    }
    match.ip_src = match_->ip_src;

    /* remote_ip, local_ip */
    match.in_key = 0;
    match.in_key_flow = true;
    tnl_port = tnl_find_exact(&match);
    if (tnl_port) {
        return tnl_port;
    }

    /* remote_ip */
    match.ip_src = 0;
    tnl_port = tnl_find_exact(&match);
    if (tnl_port) {
        return tnl_port;
    }

//    VLOG_INFO("tnl_find end match src ip is %08x\n", match.ip_src);
//    VLOG_INFO("tnl_find end match dst ip is %08x\n", match.ip_dst);

    return NULL;
}

static void
tnl_match_fmt(const struct tnl_match *match, struct ds *ds)
{
    ds_put_format(ds, IP_FMT"->"IP_FMT, IP_ARGS(match->ip_src),
                  IP_ARGS(match->ip_dst));

    if (match->in_key_flow) {
        ds_put_cstr(ds, ", key=flow");
    } else {
        ds_put_format(ds, ", key=%#"PRIx64, ntohll(match->in_key));
    }

    ds_put_format(ds, ", dp port=%"PRIu32, match->odp_port);
    ds_put_format(ds, ", skb mark=%"PRIu32, match->skb_mark);
}

static void
tnl_port_mod_log(const struct tnl_port *tnl_port, const char *action)
{
    if (VLOG_IS_DBG_ENABLED()) {
        struct ds ds = DS_EMPTY_INITIALIZER;

        tnl_match_fmt(&tnl_port->match, &ds);
        VLOG_INFO("%s tunnel port %s (%s)", action,
                  tnl_port_get_name(tnl_port), ds_cstr(&ds));
        ds_destroy(&ds);
    }
}

static char *
tnl_port_fmt(const struct tnl_port *tnl_port)
{
    const struct netdev_tunnel_config *cfg =
        netdev_get_tunnel_config(tnl_port->ofport->netdev);
    struct ds ds = DS_EMPTY_INITIALIZER;

    ds_put_format(&ds, "port %"PRIu32": %s (%s: ", tnl_port->match.odp_port,
                  tnl_port_get_name(tnl_port),
                  netdev_get_type(tnl_port->ofport->netdev));
    tnl_match_fmt(&tnl_port->match, &ds);

    if (cfg->out_key != cfg->in_key ||
        cfg->out_key_present != cfg->in_key_present ||
        cfg->out_key_flow != cfg->in_key_flow) {
        ds_put_cstr(&ds, ", out_key=");
        if (!cfg->out_key_present) {
            ds_put_cstr(&ds, "none");
        } else if (cfg->out_key_flow) {
            ds_put_cstr(&ds, "flow");
        } else {
            ds_put_format(&ds, "%#"PRIx64, ntohll(cfg->out_key));
        }
    }

    if (cfg->ttl_inherit) {
        ds_put_cstr(&ds, ", ttl=inherit");
    } else {
        ds_put_format(&ds, ", ttl=%"PRIu8, cfg->ttl);
    }

    if (cfg->tos_inherit) {
        ds_put_cstr(&ds, ", tos=inherit");
    } else if (cfg->tos) {
        ds_put_format(&ds, ", tos=%#"PRIx8, cfg->tos);
    }

    if (!cfg->dont_fragment) {
        ds_put_cstr(&ds, ", df=false");
    }

    if (cfg->csum) {
        ds_put_cstr(&ds, ", csum=true");
    }

    ds_put_cstr(&ds, ")\n");

    return ds_steal_cstr(&ds);
}

static const char *
tnl_port_get_name(const struct tnl_port *tnl_port)
{
    return netdev_get_name(tnl_port->ofport->netdev);
}

//add by zzq--find the exact ofp_port

uint16_t
tnl_find2(const uint32_t odp_port, const char *ip_dst, int key){

 //   VLOG_INFO("tnl_find2\n");


	struct tnl_port *tmp_tnl_port;
	struct tnl_port *tnl_port1;
	uint64_t in_key = Tranverse64(key);
	size_t hash;
	tnl_port1 = xzalloc(sizeof (struct tnl_port));
	tnl_port1->match.in_key = in_key;
	tnl_port1->match.ip_src = 0;
	tnl_port1->match.ip_dst = inet_addr(ip_dst);
	tnl_port1->match.odp_port = odp_port;
	tnl_port1->match.skb_mark = 0;
	tnl_port1->match.in_key_flow = 0;

	hash = tnl_hash(&tnl_port1->match);
//	VLOG_INFO("tnl_find2 key is %lu\n", tnl_port1->match.in_key);
//	VLOG_INFO("tnl_find2 tnl_hash is %lu\n", hash);
	tmp_tnl_port = tnl_find_exact(&tnl_port1->match);
 		if(tmp_tnl_port) {
//			VLOG_INFO("tnl_find2 tmp_tnl_port ip_dst is %08x\n", ntohl(tmp_tnl_port->match.ip_dst));
//			VLOG_INFO("tnl_find2 tmp_tnl_port ofp_port is %d\n", tmp_tnl_port->ofport->ofp_port);
			free(tnl_port1);
			return tmp_tnl_port->ofport->ofp_port;
                }
       free(tnl_port1);

	return 0;


}
/*
struct tnl_port *
tnl_find_exact1(struct tnl_match *match,uint16_t ofp_port)
{
    struct tnl_port *tnl_port;


    HMAP_FOR_EACH_WITH_HASH (tnl_port, match_node, tnl_hash(match),
                             &tnl_match_map) {
	VLOG_INFO("tnl_find_exact1 ----- ofp_port is %d\n", ofp_port);
        if (!memcmp(match, &tnl_port->match, sizeof *match)) {
	    if(tnl_port->ofport->ofp_port==ofp_port){ 
		VLOG_INFO("tnl_find_exact1 ofp_port is %d\n", tnl_port->ofport->ofp_port);		
		continue;
	    }
            else return tnl_port;
        }
    }
    return NULL;
}
*/
char * trans_int_to_char( uint8_t *array )
{
	int i;
	int temp1;
	int temp2;
	char *s;
	s = (char *)malloc( 13 * sizeof(char) ); // Áœžö×Ö·û¶ÔÓŠMACÖÐµÄÒ»Î»ËùÒÔÐèÒª12žö×Ö·û¿ÕŒä
	if( s == NULL )
	{
		printf( "Sorry , Can Not Malloc For String\n" );
		return NULL;
	}
	memset( s, 0, 12 );
	
	if( array == NULL )
	{
		printf( "Sorry, A Bad Arg\n" );
		return NULL;
	}

	int count = 0;
	for( i = 0; i < 6; i++ )
	{
		temp1 = 0;
		temp2 = 0;
		temp1 = array[i] / 16; // ±ÈÈçfe,ÄÇÃŽ±íÊŸÈ¡µÃf
		temp2 = array[i] % 16; // ±ÈÈçfe,ÄÇÃŽ±íÊŸÈ¡µÃe

		if( temp1 > 9 ) // ×ª»»³É16œøÖÆµÄ×Ö·û
		s[count] = 'a'+temp1 -10;
		else
		s[count] = '0' + temp1;

		count++;
		if( temp2 > 9 ) // ×ª»»³É16œøÖÆµÄ×Ö·û
			s[count] = 'a' + temp2 - 10;
		else
			s[count] = '0' + temp2;

		count++;
		s[count++] = ':';
	}
	s[17]='\0';
	return s;
}

char *find_ip(unsigned char *mac){
	char * ch = trans_int_to_char(mac);
//	char ip=(char *)malloc(20*sizeof(char));
	
	memset(ip,0,17);
	FILE *fb=fopen(file_macip,"r");
//	VLOG_INFO("file_macip is %s\n", file_macip);
	if(!fb) return NULL;	
	fseek(fb,0,SEEK_END);long len=ftell(fb);fseek(fb,0,SEEK_SET);
	char *data=(char*)malloc(len+1);fread(data,1,len,fb);
	fclose(fb);
	cJSON *json=cJSON_Parse(data);
	free(data);
	if(!json) return NULL;
	cJSON *j=cJSON_GetObjectItem(json,"macip");
//	VLOG_INFO("111\n");
	cJSON *j_child;
	j_child=j->child;
	while(j_child!=NULL){
		int size=cJSON_GetArraySize(j_child);
		int i;
		for(i=0;i<size;i++) {
			if(!strcmp(cJSON_GetArrayItem(j_child,i)->valuestring,ch)){
				
				strcpy(ip,j_child->string);
//				VLOG_INFO("find ip is %s\n",ip);
				free(ch);
				cJSON_Delete(json);
				return ip;
			}
		}
		j_child=j_child->next;
	}
	cJSON_Delete(json);
	free(ch);
	return NULL;


}

int find_key(unsigned char *mac){

	char *ch  = trans_int_to_char(mac);
	FILE *fb=fopen(file_mac_write,"r");
	
//	VLOG_INFO("file_mac_write is %s\n", file_mac_write);
	int key;
	if(!fb) return NULL;	
	fseek(fb,0,SEEK_END);long len=ftell(fb);fseek(fb,0,SEEK_SET);
	char *data=(char*)malloc(len+1);fread(data,1,len,fb);
	fclose(fb);
	cJSON *json=cJSON_Parse(data);
	free(data);
	if(!json) return NULL;
	cJSON *j=cJSON_GetObjectItem(json,"vnidmac");
	
	cJSON *j_child;
	j_child=j->child;
	while(j_child!=NULL){
		int size=cJSON_GetArraySize(j_child);
		int i;
		for(i=0;i<size;i++) {
			if(!strcmp(cJSON_GetArrayItem(j_child,i)->valuestring,ch)){
				key = atoi(j_child->string);
//				VLOG_INFO("find key is %d\n",key);
				free(ch);
				cJSON_Delete(json);
				return key;
			}
		}
		j_child=j_child->next;
	}
	
	free(ch);
	cJSON_Delete(json);
	return NULL;

}

char *find_vnid(unsigned char *mac){

	char *ch  = trans_int_to_char(mac);
	FILE *fb=fopen(file_vnid_mac,"r");
	memset(vnid,0,sizeof(vnid));
//	VLOG_INFO("file_vnid_mac is %s\n", file_vnid_mac);
	if(!fb) return NULL;	
	fseek(fb,0,SEEK_END);long len=ftell(fb);fseek(fb,0,SEEK_SET);
	char *data=(char*)malloc(len+1);fread(data,1,len,fb);
	fclose(fb);
	cJSON *json=cJSON_Parse(data);
	free(data);
	if(!json) return NULL;
	cJSON *j=cJSON_GetObjectItem(json,"vnidmac");
	
	cJSON *j_child;
	j_child=j->child;
	while(j_child!=NULL){
		int size=cJSON_GetArraySize(j_child);
		int i;
		for(i=0;i<size;i++) {
			if(!strcmp(cJSON_GetArrayItem(j_child,i)->valuestring,ch)){
//				VLOG_INFO("find vnid is %s\n",j_child->string);
				strcpy(vnid,j_child->string);
				free(ch);
				cJSON_Delete(json);
				return vnid;
			}
		}
		j_child=j_child->next;
	}

	free(ch);
	cJSON_Delete(json);
	return NULL;

}

char *find_br(char *key){
	
	FILE *fb=fopen(file_vnid_br,"r");
//	VLOG_INFO("file_vnid_br is %s\n", file_vnid_br);
	memset(br,0,sizeof(br));
	if(!fb) return NULL;	
	fseek(fb,0,SEEK_END);long len=ftell(fb);fseek(fb,0,SEEK_SET);
	char *data=(char*)malloc(len+1);fread(data,1,len,fb);
	fclose(fb);
	cJSON *json=cJSON_Parse(data);
	free(data);
	if(!json) return NULL;
	cJSON *j=cJSON_GetObjectItem(json,"brvnid");
	
	cJSON *j_child;
	j_child=j->child;
	while(j_child!=NULL){


		if(!strcmp(j_child->valuestring,key)){
//			VLOG_INFO("find br is %s\n",j_child->string);
			strcpy(br,j_child->string);
			cJSON_Delete(json);
			return br;
			}
		j_child=j_child->next;
	}


	cJSON_Delete(json);
	return NULL;
}

int *br_to_vnid(char *br){
	FILE *fb=fopen(file_vnid_br,"r");
	if(!fb) return NULL;	
	fseek(fb,0,SEEK_END);long len=ftell(fb);fseek(fb,0,SEEK_SET);
	char *data=(char*)malloc(len+1);fread(data,1,len,fb);
	fclose(fb);
	int key;
	cJSON *json=cJSON_Parse(data);
	free(data);
	if(!json) return NULL;
	cJSON *j=cJSON_GetObjectItem(json,"brvnid");
	
	cJSON *j_child;
	j_child=j->child;
	while(j_child!=NULL){

		if(!strcmp(j_child->string,br)){
			key=atoi(j_child->valuestring);
			cJSON_Delete(json);
			return key;
			}
		j_child=j_child->next;
	}
	cJSON_Delete(json);		
	return NULL;
}
void get_dir(){

	FILE*stream;
	char buf[1024];
	memset(buf,'/0',sizeof(buf));
	stream=popen("who|awk '{print $1}'","r");
	fread(buf,sizeof(char),sizeof(buf),stream);
	char name[20];
	memcpy(name,"/home/",6);
	int x;
	for(x=0;buf[x]!='\n';x++)
	  name[x+6]=buf[x];
	name[x+6]='\0';
	int pos=strlen(name);
	memcpy(file_macip,name,pos);
	for(x=0;file_macip_part[x]!='\0';x++)
		file_macip[x+pos]=file_macip_part[x];
	file_macip[x+pos]='\0';
//	zlog_debug("brvnid is %s",brvnid);
	memcpy(file_mac_write,name,pos);
	for(x=0;file_mac_write_part[x]!='\0';x++)
		file_mac_write[x+pos]=file_mac_write_part[x];
	file_mac_write[x+pos]='\0';
	memcpy(file_vnid_mac,name,pos);
	for(x=0;file_vnid_mac_part[x]!='\0';x++)
		file_vnid_mac[x+pos]=file_vnid_mac_part[x];
	file_vnid_mac[x+pos]='\0';
	memcpy(file_vnid_br,name,pos);
	for(x=0;file_vnid_br_part[x]!='\0';x++)
		file_vnid_br[x+pos]=file_vnid_br_part[x];
	file_vnid_br[x+pos]='\0';
	memcpy(file_odptype,name,pos);
	for(x=0;file_odptype_part[x]!='\0';x++)
		file_odptype[x+pos]=file_odptype_part[x];
	file_odptype[x+pos]='\0';
//	VLOG_INFO("get_dir\n");
//	printf("brvnid is %s\n", brvnid);
//	printf("mac_on_off is %s\n", mac_on_off);
	return;

}

int
get_odp_type()
{

	
	FILE *fb=fopen(file_odptype,"r");
	
	if(!fb) return NULL;	
	fseek(fb,0,SEEK_SET);
	type = fgetc(fb);
	fclose(fb);	

	return type;
}


