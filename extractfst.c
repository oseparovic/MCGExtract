/*****************************************************************************
	fstextract (main.c)

	This is a simple program to find and extract a file from a .FST file.
	It assumes that you already know the name of the file you wish to
	extract.

	If the file is compressed, this function will decompress it.

	Written by cmunsta
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <string.h>


extern long LZDecomp(unsigned char *OutputBuffer, unsigned char *CompressedBuffer, unsigned long CompBufferLength);


FILE *infile, *outfile;


typedef struct {
	unsigned long offset;		// In this case, unsigned long is 32 bits.
	unsigned long compsize;
	unsigned long uncompsize;
	char filename[250];
} TOC_Entry;


int TOCSize;
TOC_Entry *TableOfContents;

unsigned char *FileBuffer;
unsigned char *CompressedFile;
unsigned char *UncompressedFile;
long DecompressedSize;


char seekname[_MAX_PATH], curname[_MAX_PATH];
char path[_MAX_PATH], drv[_MAX_DRIVE], dir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_EXT];


#define CHECKREADERR(e,n,em)	{ \
	if(e != n) \
	{\
		printf("Error %s\n",em); \
		exit(1); \
	}\
}\



void main(int argc, char *argv[])
{
	int x;	
	int idx;
	int bytesread;
	int byteswritten;
	int err;
	int ReadSize;


	printf("-- fstextract -- read and extract a file from an FST or DPK file --\n\n");

	// If user didn't type in the correct number of parameters, spew the usage info
	if(argc != 4)
	{
		printf("Usage: fstextract <FST file> <file to extract> <file to create>\n\n");
		printf("    The name of the file to extract should *not* contain path info.\n");
		printf("    fstextract will ignore any directory path given it.\n");
		exit(0);
	}

	// Try to open the input and output files
	if((infile = fopen(argv[1],"rb")) == NULL)
	{
		printf("Can't open input FST file '%s'\n",argv[1]);
		exit(1);
	}

	if((outfile = fopen(argv[3], "wb")) == NULL)
	{
		printf("Can't open output file '%s'\n",argv[3]);
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

	// Check the search name, stripping any path info on it.
	// (These functions are DOS functions, and so will likely not work on say Linux,
	// but then, the game itself doesn't have a Linux version that I know of. might
	// still work under wine though)
	_splitpath(argv[2],NULL,NULL,fname,ext);
	_makepath(seekname,NULL,NULL,fname,ext);

	// Search for the given file in the FST's TOC
	idx = -1;
	for(x = 0; x < TOCSize && idx == -1; x++)
	{
		// Get the filename from the table of contents (strip any extraneous path info)
		_splitpath(TableOfContents[x].filename,drv,dir,fname,ext);
		_makepath(curname,NULL,NULL,fname,ext);

		if(stricmp(curname, seekname) == 0)
			idx = x;
	}


	FileBuffer = UncompressedFile = CompressedFile = NULL;

	if(idx != -1)
	{
		printf("Found %s  [CSize:%d, USize:%d]\n\n",TableOfContents[idx].filename, TableOfContents[idx].compsize, TableOfContents[idx].uncompsize);

		// Allocate memory to read in the FST file contents
		FileBuffer = (unsigned char *)malloc(TableOfContents[idx].compsize);
		if(FileBuffer == NULL)
		{
			printf("Error -- Can't allocate memory for the FST file %s.\n",TableOfContents[idx].filename);
			free(TableOfContents);
			exit(1);
		}


		// Read in the FST's file
		fseek(infile, TableOfContents[idx].offset, SEEK_SET);


		ReadSize = TableOfContents[idx].compsize;
		bytesread = fread(FileBuffer, 1, ReadSize, infile);
		CHECKREADERR(bytesread, ReadSize, "reading file from FST");

		// Check if file is already in uncompressed form
		if(TableOfContents[idx].compsize == TableOfContents[idx].uncompsize)
			UncompressedFile = FileBuffer;
		else
		{
			// Allocate a buffer for uncompressing the file to. we alloc a little extra just in case.
			// (under test, the LZDecomp didn't appear to overflow, but it wasn't an extensive test)
			UncompressedFile = (unsigned char *)malloc(TableOfContents[idx].uncompsize + 16);
			if(UncompressedFile == NULL)
			{
				printf("Error -- Can't allocate memory to decompress FST file\n");
				exit(1);
			}

			CompressedFile = FileBuffer;
			FileBuffer = UncompressedFile;
		}

		// Check if we should decompress this file
		if(CompressedFile)
		{
			// Yup, decompress it then
			DecompressedSize = LZDecomp(UncompressedFile, CompressedFile, TableOfContents[idx].compsize);
			if(DecompressedSize != (long)TableOfContents[idx].uncompsize)
			{
				printf("Warning -- Decompressed size does not match the FST's stored value\n");
			}
		}
		else
			DecompressedSize = (long)TableOfContents[idx].uncompsize;

		// Save the uncompressed data to the outputfile
		byteswritten = fwrite(FileBuffer, 1, DecompressedSize, outfile);
		if(byteswritten != DecompressedSize)
		{
			printf("Error writing to output file\n");
			exit(1);
		}

		// Trash the file buffers as we no longer need them. (FileBuffer merely ends up pointing to the
		// same buffer as UncompressedFile, so we actually should not free it as it already has been)
		if(UncompressedFile)
			free(UncompressedFile);

		if(CompressedFile)
			free(CompressedFile);

		FileBuffer = UncompressedFile = CompressedFile = NULL;
	}
	else
		printf("Couldn't find an entry by that name");

	fclose(infile);
	fclose(outfile);
}
