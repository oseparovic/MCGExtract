/*****************************************************************************
	fstdir

	Quite simply lists the files in a FST (or DPK file). Nothing more.

	Written by cmunsta
*****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <string.h>

FILE *infile;


typedef struct {
	unsigned long offset;		// In this case, unsigned long is 32 bits.
	unsigned long compsize;
	unsigned long uncompsize;
	char filename[250];
} TOC_Entry;


int TOCSize;
TOC_Entry *TableOfContents;


#define CHECKREADERR(e,n,em)	{ \
	if(e != n) \
	{\
		printf("Error %s\n",em); \
		exit(1); \
	}\
}



void main(int argc, char *argv[])
{
	int x;	
	int err;


	printf("-- fstdir -- Displays the table of contents of an FST or DPK file --\n\n");

	// If user didn't type in the correct number of parameters, spew the usage info
	if(argc != 2)
	{
		printf("Usage: fstdir <FST file>\n\n");
		exit(0);
	}

	// Try to open the input and output files
	if((infile = fopen(argv[1],"rb")) == NULL)
	{
		printf("Can't open input FST file '%s'\n",argv[1]);
		exit(1);
	}

	// Get number of entries in the table of contents of the FST file.
	fread(&TOCSize,1,4,infile);

	// Allocate memory for the table of contents
	TableOfContents = (TOC_Entry *)malloc(TOCSize * sizeof(TOC_Entry));

	if(TableOfContents == NULL)
	{
		printf("Error -- Can't allocate memory for table of contents\n");
		exit(1);
	}

	// Now read the table of contents of the FST file.
	for(x = 0; x < TOCSize; x++)
	{
		err = fread(&TableOfContents[x].offset,4,1,infile);
		CHECKREADERR(err,1,"reading FST table of contents");
		err = fread(&TableOfContents[x].compsize,4,1,infile);
		CHECKREADERR(err,1,"reading FST table of contents");
		err = fread(&TableOfContents[x].uncompsize,4,1,infile);
		CHECKREADERR(err,1,"reading FST table of contents");
		err = fread(&TableOfContents[x].filename,1,250,infile);
		CHECKREADERR(err,250,"reading FST table of contents");
	}

	// We don't need the input file anymore, so close it
	fclose(infile);

	// Now simply display the TOC on screen (I don't display the file offset because that 
	// isn't useful information for most users).
	printf("Table of Contents for <%s>\n\n",argv[1]);
	printf(" Index  CompSize UncmpSize  Filename\n");
	printf("------- -------- ---------  ------------------------------------------\n");
	for(x = 0; x < TOCSize; x++)
	{
		printf("[%5d]  %6d   %6d    %s\n",x, TableOfContents[x].compsize, TableOfContents[x].uncompsize, TableOfContents[x].filename);
	}
}
