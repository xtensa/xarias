#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>

#define TONUM2(a) ((unsigned long int)a[0]%0xff00+(unsigned long int)a[1]%0xff00*256) 
#define TONUM4(a) ((unsigned long int)a[0]%0xff00+(unsigned long int)a[1]%0xff00*256 \
		  +(unsigned long int)a[2]%0xff00*65536+(unsigned long int)a[3]%0xff00*16777216) 

#define EEPROM_SIZE 1

int eepromAddr=0;

//extern struct dirent;


void printLine(FILE *fbin,FILE *fh, char *buf, int biWidth, int biHeight, int whereToWrite,char *name,int line)
{
	int i;
	char *str;

	str=malloc(strlen(name));
	strncpy(str,name,strlen(name)-3);
	str[strlen(name)-4]=0;

	//fh=stdout;
	
	/*
	 * writing only if whole line is prepared
	 */
	if((!((line+1)%8) && line) || line==biHeight-1)
	{
		if( (line==7 && 8<biHeight) || (biHeight<=8 && line==biHeight-1) )
		{
			fprintf(fh,"#define PIC_%s_WIDTH \t%d\n",str,biWidth);
			fprintf(fh,"#define PIC_%s_HEIGHT \t%d\n",str,biHeight);
			fprintf(fh,"#define PIC_%s_SIZE \t%d\n",str,biWidth*(biHeight/8+(biHeight%8?1:0)));
		}

		/*
		 * if writing to EEPROM - .bin
		 */
		if(whereToWrite)
		{
			for(i=0;i<biWidth;i++)
			{
				putc(buf[i], fbin);
			}
	
			/*
			 * if it is the first line of bytes, then write 
			 * header to .h file
			 */
			if( (line==7 && 8<biHeight) || (biHeight<=8 && line==biHeight-1) )
			{
				fprintf(fh,"#define PIC_%s \tread_eeprom(%d,%d,%d);\n\n",str,eepromAddr,biWidth,biHeight);
			}

			eepromAddr+=biWidth;
		}
		else
		/* 
		 * writing to .h
		 */
		{
			/*
			 * if first line is written than write definition
			 */
			if( (line==7 && 8<biHeight) || (biHeight<=8 && line==biHeight-1) )
			{
				fprintf(fh, "const uint8_t PIC_%s[%d] PROGMEM =\n\t\t{\n", str, (biHeight+7)/8*biWidth );
			}

			fprintf(fh,"\t\t\t");
			for(i=0;i<biWidth;i++)
			{
				fprintf(fh,"0x%02x",(unsigned short int)buf[i]%0xff00);
				//fprintf(fh,"0x%d",buf[i]);
				if(i<biWidth-1) fprintf(fh,", ");
			}
			if(line<biHeight-1)
				fprintf(fh,",\n");
			else
				fprintf(fh,"\n");
			
			/*
			 * if last line ...
			 */
			if(line==biHeight-1)
			{
				fprintf(fh,"\t\t};\n\n");
			}
		}
	}

	free(str);
}

int main(int argv, char** argc)
{
	char *dirname,buf[300]={0}, ch, *filename;
	DIR *dir;
	struct dirent	*entry;
	FILE *f,*fh,*fbin;

	unsigned int ub, whereToWrite;
	int biBitCount, bytesPerLine,line, bfSize, eepomLeft=EEPROM_SIZE;
	int biHeight, biWidth, biCompression, bfOffBits, biSizeImage;

	if(argv<3)
	{
		printf("Usage: %s <output_file> [<directory>]\n", argc[0]);
		return 0;
	}	

	if(argv<3 || argc[2][0]==0x00) { dirname=buf; buf[0]='.'; }
	else { dirname=argc[2]; }

	filename=calloc(strlen(argc[1])+5,sizeof(char));
	/*
	 * Creating .bin file that will be programmed into EEPROM"
	 */
	strcpy(filename,argc[1]);
	strcat(filename,".bin");
	if(!(fbin=fopen(filename,"wb")))
	{
		printf("Cannot create output .bin file. Exiting...");
		return 1;
	}
	printf("Successfully created %s\n",filename);

	/*
	 * Creating .h file that shoud be included into program
	 */
	strcpy(filename,argc[1]);
	strcat(filename,".h");
	if(!(fh=fopen(filename,"wb")))
	{
		printf("Cannot create output .h file. Exiting...");
		return 1;
	}
	printf("Successfully created %s\n",filename);

	/*
	 * Creating .bin file that should be loaded into eeprom
	 */
	strcpy(filename,argc[1]);
	strcat(filename,".bin");
	if(!(fbin=fopen(filename,"wb")))
	{
		printf("Cannot create output file. Exiting...");
		return 1;
	}
	printf("Successfully created %s\n",filename);
	free(filename);

	printf("Reading directory %s ...\n",dirname);
	if(!(dir=opendir(dirname)))
	{
		printf("Error opening directory. Exiting.\n");
		fclose(fbin);
		return 1;
	}
	else
	{
		while(entry=readdir(dir))
		{
			if(strcmp(".bmp",((char*)(entry->d_name))+strlen((char*)(entry->d_name))-4)) continue;
			strcpy(buf,dirname); 
			printf("--------\nFound file %s\n",(char*)strcat(strcat(buf,"/"),(char*)(entry->d_name)));
			if(!(f=fopen(buf,"rb")))
			{
				fclose(fh);
				fclose(fbin);
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
				printf("           bfType: %s\n",buf);
			
				fread(buf,1,4,f);
				buf[4]=0;
				bfSize=TONUM4(buf);
				printf("           bfSize: %ld\n",bfSize);
							
				fread(buf,1,2,f);
				buf[2]=0;
				printf("      bfReserved1: %s\n",buf);
						
				fread(buf,1,2,f);
				buf[2]=0;
				printf("      bfReserved1: %s\n",buf);
					
				fread(buf,1,4,f);
				buf[4]=0;
				bfOffBits=TONUM4(buf);  
				printf("        bfOffBits: %ld\n",bfOffBits);
							
			
				printf("Reading info header\n");
			
				fread(buf,1,4,f);
				buf[4]=0;
				printf("           biSize: %ld \n",TONUM4(buf));
				
				fread(buf,1,4,f);
				buf[4]=0;
				biWidth=TONUM4(buf);
				printf("          biWidth: %ld\n",biWidth);
				
				fread(buf,1,4,f);
				buf[4]=0;
				biHeight=TONUM4(buf);
				printf("         biHeight: %ld\n",biHeight);
				
				fread(buf,1,2,f);
				buf[2]=0;
				printf("         biPlanes: %ld\n",TONUM2(buf));
					
				fread(buf,1,2,f);
				buf[2]=0;
				biBitCount=TONUM2(buf);
				printf("       biBitCount: %ld\n",biBitCount);
					
				fread(buf,1,4,f);
				buf[4]=0;
				biCompression=TONUM4(buf);
				printf("    biCompression: %ld\n",biCompression);
					
				fread(buf,1,4,f);
				buf[4]=0;
				biSizeImage=TONUM4(buf);
				printf("      biSizeImage: %ld\n",biSizeImage);
						
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
						
					
				if(biCompression) 
				{
					printf("Compressed bitmaps are not supported\n");
					continue;
				}
					
				if(biWidth>256) 
				{
					printf("Width greater than 256 is not supported\n");
					continue;
				}
						
				switch(biBitCount)
				{
					case 8:
					{
						/*
						 * Checking if we should write to .h or to .bin 
						 */
						if(EEPROM_SIZE-eepromAddr>=(biHeight/8+(biHeight%8?1:0))*biWidth)
							whereToWrite=1; // write to .bin
						else
							whereToWrite=0; // write to .h

						bytesPerLine=(biSizeImage)/biHeight;
						for(line=0;line<biHeight;line++) // loop through bitmap lines
						{
							int i;

							if(!(line%8)) // new byte line started
							{
								for(i=0;i<biWidth;i++)
									buf[i]=0;
							}
							
							fseek(f, bfOffBits+(biHeight-line-1)*bytesPerLine,SEEK_SET);
							//fseek(f, bfOffBits,SEEK_SET);
							//while(!feof(f)){ub=fgetc(f); printf("%x ",ub);}
							//if(0)
							for(i=0;i<biWidth;i++)
							{
								ub=fgetc(f);
								if(ub<128)
								{
									printf("1 ");
									buf[i] |= (1 << (line%8));
								}
								else
								{
									printf("0 ");
								}
							}
							printf("\n");

							printLine(fbin,fh,buf,biWidth,biHeight,whereToWrite,(char*)(entry->d_name),line);

						}
						printf("---\n");
						//printLine(fbin,fh,buf,biWidth,biHeight,whereToWrite,(char*)(entry->d_name),line);
						break;
					}
					default:
					{
						printf("%ld-bit mode is not supported\n",biBitCount);
					}
				}
								
				fclose(f);
			}
		}
 	}
	fclose(fbin);
	fclose(fh);

 	return 0;
}
