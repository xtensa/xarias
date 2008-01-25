#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>

#define TONUM2(a) ((long int)a[0]+(long int)a[1]*256) 
#define TONUM4(a) ((long int)a[0]+(long int)a[1]*256+(long int)a[2]*65536+(long int)a[3]*16777216) 

//extern struct dirent;

int main(int argv, char** argc)
{
		char *dirname,buf[3000]={0},  ch;
		DIR *dir;
		struct dirent	*entry;
		FILE	*f;

		int8_t  byte;
		int16_t word;

		if(argv<2) { dirname=buf; buf[0]='.'; }
		else dirname=argc[1];

		printf("Reading directory %s ...\n",dirname);
		if(!(dir=opendir(dirname)))
		{
				printf("Error opening directory. Exiting.\n");
				return 1;
		}
		else
		{
				while(entry=readdir(dir))
				{
						if(strcmp(".bmp",((char*)(entry->d_name))+strlen((char*)(entry->d_name))-4)) continue;
						printf("Found file %s \n",(char*)(entry->d_name));
						
						if(!(f=fopen((char*)strcat(strcat(buf,"/"),(char*)(entry->d_name)),"rb")))
						{
								printf("Error opening file\n");
						}
						else
						{
								printf("Opened sucessfully\n");
								
								printf("Reading file header\n");
								fread(buf,1,2,f);
								if(buf[0]!='B' || buf[1]!='M')
								{
										printf("Not correct BMP/DIB file\n");
										continue;
								}
								buf[2]=0;
								printf("         bfType: %s\n",buf);
							
								fread(buf,1,4,f);
								buf[4]=0;
								printf("           bfSize: %ld\n",TONUM4(buf));
								
								fread(buf,1,2,f);
								buf[2]=0;
								printf("      bfReserved1: %s\n",buf);
								
								fread(buf,1,2,f);
								buf[2]=0;
								printf("      bfReserved1: %s\n",buf);
								
								fread(buf,1,4,f);
								buf[4]=0;
								printf("        bfOffBits: %ld\n",TONUM4(buf));
								
							
								printf("Reading info header\n");
							
								fread(buf,1,4,f);
								buf[4]=0;
								printf("           biSize: %ld\n",TONUM4(buf));
								
								fread(buf,1,4,f);
								buf[4]=0;
								printf("          biWidth: %ld\n",TONUM4(buf));
								
								fread(buf,1,4,f);
								buf[4]=0;
								printf("         biHeight: %ld\n",TONUM4(buf));
								
								fread(buf,1,2,f);
								buf[2]=0;
								printf("         biPlanes: %ld\n",TONUM2(buf));
								
								fread(buf,1,2,f);
								buf[2]=0;
								printf("       biBitCount: %ld\n",TONUM2(buf));
								
								fread(buf,1,4,f);
								buf[4]=0;
								printf("    biCompression: %ld\n",TONUM4(buf));
								
								fread(buf,1,4,f);
								buf[4]=0;
								printf("      biSizeImage: %ld\n",TONUM4(buf));
								
								fread(buf,1,4,f);
								buf[4]=0;
								printf("  biXPelsPerMeter: %ld\n",TONUM4(buf));
								
								fread(buf,1,4,f);
								buf[4]=0;
								printf("  biYPelsPerMeter: %ld\n",TONUM4(buf));
								
								fread(buf,1,4,f);
								buf[4]=0;
								printf("        biClrUsed: %ld\n",TONUM4(buf));
								
								fread(buf,1,4,f);
								buf[4]=0;
								printf("   biClrImportant: %ld\n",TONUM4(buf));
								
								
									
								fclose(f);
						}
				}
		}
		return 0;
}
