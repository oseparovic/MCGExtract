#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <string.h>


#define READBUFFSIZE	65536

FILE *infile, *outfile, *copyfile;


typedef struct {
	unsigned int offset;
	unsigned int compsize;
	unsigned int uncompsize;
	char filename[250];
	int compress;
} TOC_Entry;

TOC_Entry *TableOfContents; 
int TOCSize;

char curname[_MAX_PATH];

int EndPos;

int ReadSize;
unsigned char *ReadBuff;
unsigned char *CompressedBuff;



extern long LZCompress(unsigned char *CompDataBuff, unsigned char *UncompDataBuff, unsigned long UncompDataSize);

void MakeFST(void);



void main(int argc, char *argv[])
{
	printf("-- makefst -- creates an FST or DPK file from a file list --\n\n");

	if(argc != 3)
	{
		printf("Usage: makefst <file list> <FST/DPK file to create>\n");
		exit(0);
	}


	if((infile = fopen(argv[1],"r")) == NULL)
	{
		printf("Can't open input file '%s'\n",argv[1]);
		exit(0);
	}

	if((outfile = fopen(argv[2], "wb+")) == NULL)
	{
		printf("Can't open output file '%s'\n",argv[2]);
		exit(0);
	}

	MakeFST();

	fclose(infile);
	fclose(outfile);
}

 

void MakeFST(void)
{
	int i;
	char cmd;

	// Run through the files we need to add and count up so we know the size of the TOC.
	TOCSize = 0;
	while(!feof(infile))
	{
		fscanf(infile, "%c %s\n", &cmd, curname);

		if((copyfile = fopen(curname, "rb")) != NULL)
		{
			TOCSize++;
			fclose(copyfile);
		}
	}

	TableOfContents = (TOC_Entry *)malloc(TOCSize * sizeof(TOC_Entry));
	if(TableOfContents == NULL)
	{
		printf("Error -- Can't allocate memory for table of contents\n");
		exit(1);
	}


	// Now we go through and fill in the table of contents
	rewind(infile);
	i=0;
	while(!feof(infile))
	{
		fscanf(infile, "%c %s\n", &cmd, curname);

		if((copyfile = fopen(curname, "rb")) != NULL)
		{
			// Zero fill the name field (might be OK if it wasn't, but the original game FSTs are zero filled)
			memset(TableOfContents[i].filename, 0, 250);

			strcpy(TableOfContents[i].filename, curname);

			ReadSize = filelength(fileno(copyfile));

			TableOfContents[i].compsize = TableOfContents[i].uncompsize = ReadSize;
			TableOfContents[i].offset = 0;

			// Mark if we need to compress this file
			if(cmd == 'c')
				TableOfContents[i].compress = 1;
			else
				TableOfContents[i].compress = 0;
			
			fclose(copyfile);
			i++;
		}
	}

	// Write out the number of TOC entries.
	fwrite(&TOCSize,1,4,outfile);	

	// Write out the prelimimnary table of contents (we'll fix it up later when we get the remaining info)
	for(i=0; i<TOCSize; i++)
	{
		fwrite(&TableOfContents[i].offset,1,4,outfile);
		fwrite(&TableOfContents[i].compsize,1,4,outfile);
		fwrite(&TableOfContents[i].uncompsize,1,4,outfile);
		fwrite(&TableOfContents[i].filename,1,250,outfile);
	}

	// Now we copy the file data
	for(i=0; i<TOCSize; i++)
	{
		copyfile = fopen(TableOfContents[i].filename, "rb");

		// Save the file position offset for the TOC fixup
		TableOfContents[i].offset = ftell(outfile);

		ReadBuff = (unsigned char *)malloc(TableOfContents[i].uncompsize);
		if(ReadBuff == NULL)
		{
			printf("Error -- Can't allocate memory to copy file\n");
			exit(0);
		}

		fread(ReadBuff,1,TableOfContents[i].uncompsize,copyfile);

		// Check if we should simply copy the data in, or compress the data first.
		if(TableOfContents[i].compress)
		{
			CompressedBuff = (unsigned char *)malloc(TableOfContents[i].uncompsize);
			if(CompressedBuff == NULL)
			{
				printf("Error -- Can't allocate compression buffer\n");
				exit(0);
			}

			// Compress the data (and save the compressed size for the TOC)
			TableOfContents[i].compsize = LZCompress(CompressedBuff, ReadBuff, TableOfContents[i].uncompsize);

			fwrite(CompressedBuff, 1, TableOfContents[i].compsize, outfile);

			free(CompressedBuff);
		}
		else
			fwrite(ReadBuff, 1, TableOfContents[i].uncompsize, outfile);

		free(ReadBuff);

		fclose(copyfile);
	}

	EndPos = ftell(outfile);

	// We need to fix up the offsets and compressed sizes in the file's TOC.
	fseek(outfile,4,SEEK_SET);
	for(i=0; i<TOCSize; i++)
	{
		// Update the offset and compressed sizes.
		fwrite(&TableOfContents[i].offset,1,4,outfile);
		fwrite(&TableOfContents[i].compsize,1,4,outfile);

		// Skip over the remainder of the TOC entry as it is accurate.
		fseek(outfile,254,SEEK_CUR);
	}

	fseek(outfile,EndPos,SEEK_SET);

	free(TableOfContents);
}

