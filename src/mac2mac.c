/*
 * mac2mac.c
 *
 * Descripcion: Convert Mac Address into different mac formats
 *
 * Author: Fernando Diaz Sanchez <sirfids@gmail.com>
 * 
 * Fecha: 18 Feb 2014
 *
 * Licence: MIT
 *
 * */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <getopt.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

struct _oui{
        char* hypen;
        char* colon;
        char* dotted;
        char* cdot;
        char* none;
        char* oui;
};

#define handle_error(msg) \
           do { perror(msg); exit(EXIT_FAILURE); } while (0)

void* xcalloc(size_t nr, size_t size);
bool check_format(const char* format, const char* mac);
char* apply_format(const char* format, const char* mac);
void transform_mac(const char* mac, struct _oui* oui);
void str_tolower(char* str);
void str_toupper(char* str);
void print_usage();
unsigned char* shm_alloc(size_t len, int fd);
int shm_dealloc(void* addr, size_t len);
void process_file(const char* strfile, struct _oui* oui);
void get_line(const unsigned char* str,char* dest, size_t ini, size_t fin);
unsigned char* load_oui();
void unload_oui(unsigned char* data, struct stat st_oui);
char* search_oui(const char* format, const char* mac);
int file_exist (char *filename);

bool fmayus = false;
bool ffile  = false;
bool ffmoui  = true;
unsigned char* data_oui;
struct stat st_oui;

int main(int argc, char **argv)
{
	struct _oui* oui;
	char* mac     = xcalloc(17+1, sizeof(char));
	char* strfile = xcalloc(99+1, sizeof(char));
	oui           = xcalloc(1, sizeof(struct _oui) );

	/* Parse cmd line */
	int option = 0;

	while ((option = getopt(argc, argv,"mMhs:f:")) != -1) {
        	switch (option) {
	             case 'm' : fmayus = false;
        	         break;
	             case 'M' : fmayus = true;
        	         break;
	             case 'h' : print_usage();
        	         exit(0);
	             case 's' : strncpy(mac,optarg,17); 
        	         break;
	             case 'f' : strncpy(strfile,optarg,100); 
				ffile = true;
        	         break;
	             default: print_usage(); 
        	         exit(EXIT_FAILURE);
	        }
	}

	//strncpy(mac,argv[1],17);

	if ( argc > 1 )
	{

		// Load data oui
		data_oui = load_oui();

		if ( fmayus == true )
			str_toupper(mac);
		else
			str_tolower(mac);
	
		if ( ffile == false )
			transform_mac(mac, oui);
		else
			process_file(strfile,oui);
	
		free(oui);
		free(mac);
		free(strfile);
		oui = NULL;
		mac = NULL;
		strfile = NULL;

		unload_oui(data_oui,st_oui);
	}else{
		print_usage();
	}
	
	return 0;
}

void print_usage()
{
	printf("How to use: mac2mac [-m|-M] (-s mac-address | -f file) \n\n");
	printf("Options:\n");
	printf("  -m = Convert to lowercase\n");
	printf("  -M = Convert to uppercase\n");
	printf("  -f = Get mac address from a file\n\n");
}

void transform_mac(const char* mac, struct _oui* oui)
{
	bool format_ok = false;

	if ( check_format("XX:XX:XX:XX:XX:XX",mac) == true  ) format_ok = true;
    if ( check_format("XX-XX-XX-XX-XX-XX",mac) == true  ) format_ok = true;
    if ( check_format("XXXX.XXXX.XXXX",mac) == true  )    format_ok = true;
	if ( check_format("XXXX-XXXX-XXXX",mac) == true  )    format_ok = true;
    if ( check_format("XXXXXXXXXXXX",mac) == true  )      format_ok = true;

	if ( format_ok == true ){
	        oui->hypen = apply_format("XX-XX-XX-XX-XX-XX",mac);
        	oui->colon = apply_format("XX:XX:XX:XX:XX:XX",mac);
	        oui->dotted = apply_format("XXXX.XXXX.XXXX",mac);
	        oui->cdot = apply_format("XXXX-XXXX-XXXX",mac);
	        oui->none = apply_format("XXXXXXXXXXXX",mac);
		
		if (ffmoui == true){
			oui->oui  = search_oui("XX-XX-XX",oui->hypen);
			/* Print MAC */
			printf("%s   %s   %s   %s   %s   %s\n",oui->hypen,
					oui->colon,
					oui->dotted,
					oui->cdot,
					oui->none,
					oui->oui);
			free(oui->oui);
			oui->oui = NULL;
		}else{
			printf("%s   %s   %s   %s   %s   <mini.oui.txt file miss>\n",
					oui->hypen,
					oui->colon,
					oui->dotted,
					oui->cdot,
					oui->none);
		}

		/* Free Memory */
		free(oui->hypen);
		free(oui->colon);
		free(oui->dotted);
		free(oui->cdot);
		free(oui->none);
		oui->hypen = NULL;
		oui->colon = NULL;
		oui->dotted = NULL;
		oui->cdot = NULL;
		oui->none = NULL;
		
	}else{
		printf("Mac Address Format (%s) invalid\n",mac);
	}
}

void*
xcalloc(size_t nr, size_t size)
{
	register void* ptr = calloc(nr, size);
	if(!ptr){
		perror("calloc error");
		exit(EXIT_FAILURE);
	}
	return ptr;
}

bool 
check_format(const char* format, const char* mac)
{
	// Verificar longitud
	if( strlen(format) == strlen(mac) ){
		while(*format != '\0'){
			if ( *format == 'X' ){
				if ( ! isxdigit(*mac) ) return false;
			}else{
				if ( *format != *mac ) return false;
			}
			format++;
			mac++;
		}
	}else{
		return false;
	}
	return true;
}

char* 
apply_format(const char* format, const char* mac)
{
	char* ptr = xcalloc(17+1,sizeof(char));
	char* ptr_ini = ptr;
	while(*format != '\0'){
		if ( *format == 'X' ){
			if ( ! isxdigit(*mac) ) mac++;
			*ptr = *mac;
			mac++;
		}
		else{
			*ptr = *format;
		}
		format++;
		ptr++;
	}
	ptr = ptr_ini;
	return ptr;
}

char*
search_oui(const char* format, const char* mac)
{
	// Get the MAC OUI
	char* str_oui = xcalloc(8+1,sizeof(char));
	char* str_ini = str_oui;
	char* res     = xcalloc(100+1,sizeof(char));
	char* res_ini = res;
	int count = 0;

	while(*format != '\0'){
		*str_oui = toupper(*mac);
		mac++;
		str_oui++;
		format++;
	}
	str_oui = str_ini;

	// Seek the OUI
	char* p;
	p = strstr(data_oui,str_oui);

	// get line if found
	if ( p == NULL ){
		strncpy(res,"UNKOWN OUI",15);
	}else{
		while(*p != '\n'){
			count++;
			*res = *p;
			p++;
			res++;
			if ( count > 100 ) break;
		}
	}

	res = res_ini;

	// Free Heap
	free(str_oui);
	str_oui = NULL;

	return res;
}

void str_tolower(char* str)
{
	while(*str != '\0'){
		*str = tolower(*str);
		str++;
	}
}

void str_toupper(char* str)
{
	while(*str != '\0'){
		*str = toupper(*str);
		str++;
	}
}

unsigned char*
shm_alloc(size_t len, int fd)
{
	void* addr;
	addr = mmap(NULL,		/* Where to start */
		    len, 		/* Size */
		    PROT_READ, 		/* Prot */
		    MAP_SHARED,		/* Map */
                    fd, 		/* File descriptor */
		    0);			/* Offset */
	if ( addr == MAP_FAILED ){
		perror("mmap error");
		exit(EXIT_FAILURE);
	}
	return addr;
}

int 
shm_dealloc(void* addr, size_t len)
{
	int res;
	res = munmap(addr, len);
	if ( res == -1 ) handle_error("nunmap error");
	return res;
}

void 
process_file(const char* strfile, struct _oui* oui)
{
	int fd;
	struct stat st;
	unsigned char* data;
	size_t ini = 0;
	size_t fin = 0;
	char* str_aux = NULL;
	char* str_aux_ini;

	fd = open(strfile,O_RDONLY);
	if ( fd == -1 ) handle_error("open error");

	if ( fstat(fd, &st) == -1) handle_error("fstat error");

	data = shm_alloc(st.st_size,fd);


	/* Print MAC */
	printf("HYPHEN              COLON               DOTTED           CDOTTED          NONE           OUI\n");

	for (int len=0;len<st.st_size;len++  ){
		if ( data[len] == '\n' ){
			fin = len;
			str_aux = xcalloc((fin-ini)+1,sizeof(char));
			str_aux_ini = str_aux;
			get_line(data,str_aux,ini,fin);
			str_aux = str_aux_ini;

			if ( fmayus == true )
	                        str_toupper(str_aux);
        	        else
                	        str_tolower(str_aux);

			transform_mac(str_aux,oui);
			ini = len+1;
			free(str_aux);
			str_aux = NULL;
		}
	}

	if ( close(fd) == -1 ) handle_error("close error");

	shm_dealloc(data,st.st_size);
}

unsigned char* 
load_oui()
{
	int fd;
	unsigned char* data;
	char* ptr;

	if ( file_exist("mini.oui.txt") ){

	        fd = open("mini.oui.txt",O_RDONLY);
        	if ( fd == -1 ){
			handle_error("open error");
		}else{

 		       if ( fstat(fd, &st_oui) == -1) handle_error("fstat error");

        		data = shm_alloc(st_oui.st_size,fd);

			if ( close(fd) == -1 ) handle_error("close error");
		}
	}else{
		ffmoui = false;
	}

	return data;
}

void
unload_oui(unsigned char* data, struct stat st_oui)
{
	if (data != NULL) shm_dealloc(data,st_oui.st_size);	
}

void get_line(const unsigned char* str,char* dest, size_t ini, size_t fin)
{
	for (int len=ini;len<fin;len++  ){
		*dest = str[len];
		dest++;
	}

}

int file_exist (char *filename)
{
	struct stat   buffer;   
	return (stat (filename, &buffer) == 0);
}
