#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <string.h>

/*
-----------------------------------------------------------------
	PAK file format (Nearly identical to the SOL/PKK/MPK/SAV file formats)

000000 DWORD    PAK File Identifier (value is always 0xFEEDFACE)
000004 DWORD    Offset where file data actually starts
000008 DWORD*n  Table of offsets to the data in the file. (n is the table size)
                  - If an offset has a 0x40 in its high byte, it is compressed, and
				    the true offset is (Offset & ~0x40000000) and the first DWORD
				    at that offset is the uncompressed size.
			      - If the offset has 0xE0 in its high byte, this entry is effectively
				    empty.

	Number of entries in table = ("DataStartOffset" - 8) / 4

-----------------------------------------------------------------

The following is actually just a quick modification of makesol.c, so if the comments
seem a bit odd at times, or the code a little redundant, you know why.

*/


typedef struct _PAK_Entry {
	unsigned long Offset;
	int compressed;
	int empty;
	struct _PAK_Entry *Next;
} PAK_Entry;


PAK_Entry *EntryList;
PAK_Entry *LastEntry;

long NumEntries;
long DataStartOffset;
unsigned long Checksum;

long tmpsize;


FILE *infile, *outfile, *tfile, *copyfile;

char cmd;
char filename[_MAX_PATH];
unsigned char *CopyBuffer;
unsigned char *CompressedBuffer;


extern long LZCompress(unsigned char *CompDataBuff, unsigned char *UncompDataBuff, unsigned long UncompDataSize);


void OutputAllData(void);
void AdjustDataForOutput(void);
void CreateTempDatafile(void);
void MakeNode(void);
void AddUncompressedFile(void);
void AddCompressedFile(void);
void AddEmptyEntry(void);



void main(int argc, char *argv[])
{
	printf("-- makepak -- creates a PAK file from a file list --\n\n");

	if(argc != 3)
	{
		printf("Usage: makepak <file list> <PAK file to create>\n");
		exit(0);
	}

	if((infile = fopen(argv[1],"r")) == NULL)
	{
		printf("Can't open input file list '%s'\n",argv[1]);
		exit(0);
	}

	if((outfile = fopen(argv[2], "wb+")) == NULL)
	{
		printf("Can't open output file '%s'\n",argv[2]);
		exit(0);
	}

	if((tfile = fopen("$_temppak_$.dat","wb")) == NULL)
	{
		printf("Can't open temporary data file.\n");
		exit(0);
	}

	CreateTempDatafile();

	fclose(infile);

	// Close and re-open the temporary file for reading
	fclose(tfile);
	if((tfile = fopen("$_temppak_$.dat","rb")) == NULL)
	{
		printf("Error -- Can't re-open temporary data file for reading\n");
		exit(0);
	}

	AdjustDataForOutput();

	fclose(tfile);

	OutputAllData();

	fclose(outfile);	
}


void OutputAllData(void)
{
	int x;
	PAK_Entry *TOCEntry;

	Checksum = 0xfeedface;		// Don't have a checksum anymore, just an identifier of value "0xfeedface".
	fwrite(&Checksum,4,1,outfile);
	fwrite(&DataStartOffset,4,1,outfile);

	TOCEntry = EntryList;
	for(x=0;x<NumEntries;x++)
	{
		fwrite(&TOCEntry->Offset,4,1,outfile);

		TOCEntry = TOCEntry->Next;
	}

	fwrite(CopyBuffer,1,tmpsize,outfile);
}


void AdjustDataForOutput(void)
{
	int x;
	long sizeread;
	PAK_Entry *TOCEntry;

	tmpsize = filelength(fileno(infile));

	CopyBuffer = (unsigned char *)malloc(tmpsize);
	if(CopyBuffer == NULL)
	{
		printf("Error -- Couldn't allocate buffer to copy the temp data\n");
		exit(0);
	}

	sizeread = fread(CopyBuffer, 1,tmpsize, tfile);
	if(sizeread != tmpsize)
	{
		printf("Error reading temporary data file into memory\n");
		exit(0);
	}

	// Calc where the real data will start in the output file
	DataStartOffset = (NumEntries * 4) + 8;

	// Adjust the table of contents offsets, flag them as compressed or empty as needed.
	TOCEntry = EntryList;
	for(x=0; x<NumEntries; x++)
	{
		TOCEntry->Offset += DataStartOffset;

		if(TOCEntry->compressed)
			TOCEntry->Offset |= 0x40000000;
		
		if(TOCEntry->empty)
			TOCEntry->Offset |= 0xe0000000;

		TOCEntry = TOCEntry->Next;
	}
}


void CreateTempDatafile(void)
{
	NumEntries = 0;
	EntryList = NULL;
	LastEntry = NULL;

	while(!feof(infile))
	{
		fscanf(infile, "%c %s\n", &cmd, filename);

		switch(cmd)
		{
			case 'c':		// add as compressed file
				AddCompressedFile();
				break;

			case 'u':		// add as uncompressed file
				AddUncompressedFile();
				break;

			case 'e':		// add an empty entry to the TOC
				AddEmptyEntry();
				break;

			default:		// Unknown command, print error but keep going.
				printf("Warning - Unknown command '%c'. Ignoring\n", cmd);
				break;
		}
	}
}


void MakeNode(void)
{
	PAK_Entry *NewNode;

	NewNode = (PAK_Entry *)malloc(sizeof(PAK_Entry));
	if(NewNode == NULL)
	{
		printf("Error allocating memory for TOC list\n");
		exit(1);
	}
	NewNode->Next = NULL;
	NewNode->Offset = 0;
	NewNode->compressed = 0;
	NewNode->empty = 0;

	if(EntryList == NULL)
	{
		EntryList = NewNode;
	}

	if(LastEntry != NULL)
	{
		LastEntry->Next = NewNode;
	}

	LastEntry = NewNode;
}


void AddEmptyEntry(void)
{
	MakeNode();

	LastEntry->empty = 1;
	LastEntry->compressed = 0;
	LastEntry->Offset = ftell(tfile);

	NumEntries++;
}


void AddUncompressedFile(void)
{
	long fsize;
	long sizeread;
	long sizewritten;

	if((copyfile = fopen(filename,"rb")) == NULL)
	{
		printf("Error -- can't open file %s\n", filename);
		exit(1);
	}

	fsize = filelength(fileno(copyfile));

	CopyBuffer = (unsigned char *)malloc(fsize);
	if(CopyBuffer == NULL)
	{
		printf("Error -- Couldn't allocate buffer to copy %s.\n",filename);
		exit(1);
	}

	sizeread = fread(CopyBuffer,1,fsize,copyfile);
	if(sizeread != fsize)
	{
		printf("Error reading file %s\n",filename);
		exit(1);
	}

	fclose(copyfile);

	MakeNode();

	LastEntry->compressed = 0;
	LastEntry->empty = 0;
	LastEntry->Offset = ftell(tfile);

	sizewritten = fwrite(CopyBuffer,1,fsize,tfile);
	if(sizewritten != fsize)
	{
		printf("Error writing %s to tempfile.\n",filename);
		exit(1);
	}

	free(CopyBuffer);

	NumEntries++;
}


void AddCompressedFile(void)
{
	long fsize;
	long sizeread;
	long sizecompressed;
	long sizewritten;

	if((copyfile = fopen(filename,"rb")) == NULL)
	{
		printf("Error -- can't open file %s\n", filename);
		exit(1);
	}

	fsize = filelength(fileno(copyfile));

	CopyBuffer = (unsigned char *)malloc(fsize);
	if(CopyBuffer == NULL)
	{
		printf("Error -- Couldn't allocate buffer to copy %s.\n",filename);
		exit(1);
	}

	sizeread = fread(CopyBuffer,1,fsize,copyfile);
	if(sizeread != fsize)
	{
		printf("Error reading file %s\n",filename);
		exit(1);
	}

	fclose(copyfile);

	// Since pretty much everything is a text file, we shouldn't have to worry about the 
	// data growing instead of shrinking, but it is technically possible.
	CompressedBuffer = (unsigned char *)malloc(fsize);
	if(CompressedBuffer == NULL)
	{
		printf("Error -- Couldn't allocate buffer to compress %s.\n",filename);
		exit(1);
	}

	sizecompressed = LZCompress(CompressedBuffer, CopyBuffer, fsize);
	
	MakeNode();

	LastEntry->compressed = 1;
	LastEntry->empty = 0;
	LastEntry->Offset = ftell(tfile);

	// Compressed files have the uncompressed file size stored in the first DWORD of the file data.
	fwrite(&fsize,4,1,tfile);

	sizewritten = fwrite(CompressedBuffer,1,sizecompressed,tfile);
	if(sizewritten != sizecompressed)
	{
		printf("Error writing %s to tempfile.\n",filename);
		exit(1);
	}

	free(CopyBuffer);
	free(CompressedBuffer);

	NumEntries++;
}
