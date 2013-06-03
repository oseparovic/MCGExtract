/*****************************************************************************
  lzdecomp_asm.c

	This is a hacked-up version of the exact decompression routine that
	MechCommander Gold uses. In fact, that is where I got the disassembly
	from, the MCX.EXE from MechCommander Gold installed on my computer.
	The original disassemly is in comment at the end of the file, slightly 
	annotated by me. I merely changed a few label names etc. I used the free 
	version of IDA (The Interactive Disassembler) to get the disassembly.

	Interestingly, the program has debug symbols already (or should I say still)
	in the program, so either someone forgot to strip the EXE, or it was
	done on purpose.


  How to use:
	The single function declared in this file is all you need. It is self
	contained and complete.

	Simply add the line below to your code and call it with the required 
	parameters (all decompression is done in memory).

	extern long LZDecomp(unsigned char *OutputBuffer, unsigned char *CompressedBuffer, unsigned long CompBufferLength);
	
	Make sure that the decompression buffer is large enough to accept 
	the decompressed data.

	LZDecomp returns the number of bytes placed in the output buffer.
		

  Notes:
	Note to those wanting to use this function. This is an implementation of
	a Lempel-Ziv algorithm (specifically the decompressor). I'm not quite
	sure which of the many variants this is, though it looks like a LZW 
	implementation. But the real thing to know is many of the Lempel-Ziv
	variants are patented. In the case of LZW, by Unisys and (I think) IBM.
	Though typically they don't mind if it is used for non-commercial purposes,
	if you intend to use this in a commercial product, you are going to owe 
	them money. (I thought you'd like to know).


  Author(s):
	Original LZDecomp function: unknown.
	Hacktastic code by: cmunsta.
*****************************************************************************/

typedef struct _HashStruct {	// I don't really know what HashStruct is (as in what the structure really looks like)
	unsigned long v1;	// But put this in for hacktastic reasons. Don't think it's actually used anymore other
	unsigned long v2;	// than for two of the pointer declarations below.
	unsigned long v3;
} HashStruct;


typedef unsigned char uchar;
typedef unsigned long ulong;

HashStruct *LZOldChain;		// Vars used by the code
HashStruct *LZChain;
uchar * LZSrcBufEnd;
ulong LZMaxIndex;
ulong LZCodeMask;
ulong LZFreeIndex;
char LZOldSuffix;

char LZHashBuffer[65536];	// This likely can be reduced in size, but I don't know what a good size would be.
							// so it stays as is.

unsigned char LocalPtr[16384];	// This was actually in the func, and was played with on the stack by the routine. 
								// I had to pull it out and make the code use it in a hack way (part of the "ESP thing"
								// comments in the code below) but hey, it is functional so my work is done.


unsigned long save_esp;			// These are used in the hacks I addded to get the function to work.
unsigned long save_ebx;
unsigned long fake_esp = 8192;



long LZDecomp(unsigned char *OutputBuffer, unsigned char *CompressedBuffer, unsigned long CompBufferLength) {
	long LocalInt;
	_asm {
				; Yes yes, this is really just a function pre-amble, I left it because the code was doing
				; some funky things with the stack that I needed to hack around, so the pre-amble (and post 
				; or course) stayed in.
                push    ebp
;                mov     ebp, esp	; This is replaced by the following two lines line
                mov	save_esp, esp
                mov	fake_esp, 8192

                push    ecx
                push    ebx
                push    esi
                push    edi
                mov     LocalInt, 0
                mov     esi, CompressedBuffer
                mov     ebx, CompBufferLength
                mov     edi, OutputBuffer
                xor     eax, eax
                xor     ecx, ecx
                lea     ebx, [ebx+esi-3]
                mov     LZOldChain, eax
                mov     LZSrcBufEnd, ebx
                mov     LZChain, eax

                mov     LZMaxIndex, eax			; Seems like these three are redundant, but I left them in
                mov     LZCodeMask, eax
                mov     LZFreeIndex, eax
                
		mov     LZCodeMask, 1FFh
                mov     LZMaxIndex, 200h
                mov     LZFreeIndex, 102h
                
				mov     LZOldSuffix, al
                mov     ch, 9
                jmp     Label2
; ---------------------------------------------------------------------------

Label1: 
                mov     ch, 9					; This looks like where it resets the dictionary table
                mov     LZCodeMask, 1FFh
                mov     LZMaxIndex, 200h
                mov     LZFreeIndex, 102h
                cmp     esi, LZSrcBufEnd
                ja      LZDecomp_EndFunc
                mov     eax, [esi]
                xor     ebx, ebx
                shr     eax, cl
                add     cl, ch
                mov     edx, LZCodeMask
                mov     bl, cl
                and     cl, 7
                shr     ebx, 3
                and     eax, edx
                add     esi, ebx
                nop
                mov     LZOldChain, eax
                mov     LZOldSuffix, al
                mov     [edi], al
                inc     edi

Label2: 
                cmp     esi, LZSrcBufEnd
                ja      LZDecomp_EndFunc
                mov     eax, [esi]
                xor     ebx, ebx
                shr     eax, cl
                add     cl, ch
                mov     edx, LZCodeMask
                mov     bl, cl
                and     cl, 7
                shr     ebx, 3
                and     eax, edx
                add     esi, ebx
                nop
                cmp     eax, 101h
                jz      LZDecomp_EndFunc
                cmp     eax, 100h
                jz      Label1
                
		mov     edx, fake_esp ;esp	; ESP thing
                dec     fake_esp ;esp

                mov     LZChain, eax
                lea     ebx, LZHashBuffer[eax+eax*2]
                cmp     eax, LZFreeIndex
                jl      Label3
                mov     al, LZOldSuffix
                
		mov 	save_ebx, ebx
		mov		ebx, fake_esp                
		mov     [ebx+10h+LocalPtr], al	; ESP thing
                dec     fake_esp  ;esp
                mov     ebx, save_ebx

				mov     [ebx+2], al
                mov     eax, LZOldChain
                mov     [ebx], ax
                lea     ebx, LZHashBuffer[eax+eax*2]

Label3:
                test    ah, ah
                jz      Label4
                mov     al, [ebx+2]

				mov 	save_ebx, ebx
				mov		ebx, fake_esp                
				mov     [ebx+10h+LocalPtr], al		; ESP thing
                dec     fake_esp   ;esp
				mov		ebx, save_ebx
                
				movzx   eax, word ptr [ebx]
                lea     ebx, LZHashBuffer[eax+eax*2]
                jmp     Label3
; ---------------------------------------------------------------------------

Label4:
                mov     LZOldSuffix, al

				mov 	save_ebx, ebx
				mov		ebx, fake_esp
                mov     [ebx+10h+LocalPtr], al		; ESP thing
                sub     edx, fake_esp ; esp
                mov     ebx, save_ebx

				mov     ebx, LZFreeIndex

Label5:
				mov 	save_ebx, ebx
				mov		ebx, fake_esp
                mov     al, [ebx+10h+LocalPtr]	; ESP thing
                inc     fake_esp ;esp
                mov     ebx, save_ebx

                mov     [edi], al
                inc     edi
                dec     edx
                jnz     Label5
                mov     al, LZOldSuffix
                mov     edx, LZOldChain
                mov     LZHashBuffer+2[ebx+ebx*2], al		
                mov     word ptr LZHashBuffer[ebx+ebx*2], dx
                inc     ebx
                mov     eax, LZChain
                mov     LZFreeIndex, ebx
                mov     edx, LZMaxIndex
                mov     LZOldChain, eax
                cmp     ebx, edx
                jl      Label2
                cmp     ch, 0Ch
                jz      Label2
                inc     ch
                mov     ebx, LZCodeMask
                mov     eax, LZMaxIndex
                add     ebx, ebx
                add     eax, eax
                or      ebx, 1
                mov     LZMaxIndex, eax
                mov     LZCodeMask, ebx
                jmp     Label2
; ---------------------------------------------------------------------------

LZDecomp_EndFunc:
                sub     edi, [OutputBuffer]
                mov     [LocalInt], edi
                mov     eax, [LocalInt]
                pop     edi
                pop     esi
                pop     ebx

;                mov     esp, ebp
				mov     esp, save_esp
                
				pop     ebp

	};

	return LocalInt;	// This will contain the number of bytes decompressed.
}


/*
==============================================================================
==============================================================================
	The following is the disassembly produced by IDA. A few of the labels 
	and variables were named by me, but it is pretty much straight out of
	IDA.

	You'll notice that the function above is an almost identical copy. I 
	pretty much just did a cut and paste job and then hacked the code 
	until it was beaten into submission. The hack tool of choice was a
	mallet :)


.text:0064CC80 long __cdecl LZDecomp(unsigned char *, unsigned char *, unsigned long) proc near
.text:0064CC80                                         ; CODE XREF: FastFile::readFast(long,void *,long)+13A.p
.text:0064CC80                                         ; PacketFile::readPacket(long,uchar *)+104.p ...
.text:0064CC80
.text:0064CC80 LocalPtr        = byte ptr -10h
.text:0064CC80 LocalInt        = dword ptr -4
.text:0064CC80 OutputBuffer    = dword ptr  8
.text:0064CC80 CompressedBuffer= dword ptr  0Ch
.text:0064CC80 CompBufferLength= dword ptr  10h
.text:0064CC80
.text:0064CC80                 push    ebp
.text:0064CC81                 mov     ebp, esp
.text:0064CC83                 push    ecx
.text:0064CC84                 push    ebx
.text:0064CC85                 push    esi
.text:0064CC86                 push    edi
.text:0064CC87                 mov     [ebp+LocalInt], 0
.text:0064CC8E                 mov     esi, [ebp+CompressedBuffer]
.text:0064CC91                 mov     ebx, [ebp+CompBufferLength]
.text:0064CC94                 mov     edi, [ebp+OutputBuffer]
.text:0064CC97                 xor     eax, eax
.text:0064CC99                 xor     ecx, ecx
.text:0064CC9B                 lea     ebx, [ebx+esi-3]
.text:0064CC9F                 mov     HashStruct * LZOldChain, eax
.text:0064CCA4                 mov     uchar * LZSrcBufEnd, ebx
.text:0064CCAA                 mov     HashStruct * LZChain, eax
.text:0064CCAF                 mov     ulong LZMaxIndex, eax
.text:0064CCB4                 mov     ulong LZCodeMask, eax
.text:0064CCB9                 mov     ulong LZFreeIndex, eax
.text:0064CCBE                 mov     ulong LZCodeMask, 1FFh
.text:0064CCC8                 mov     ulong LZMaxIndex, 200h
.text:0064CCD2                 mov     ulong LZFreeIndex, 102h
.text:0064CCDC                 mov     char LZOldSuffix, al
.text:0064CCE1                 mov     ch, 9
.text:0064CCE3                 jmp     short Label2
.text:0064CCE5 ; ---------------------------------------------------------------------------
.text:0064CCE5
.text:0064CCE5 Label1:                                 ; CODE XREF: LZDecomp(uchar *,uchar *,ulong)+F0.j
.text:0064CCE5                 mov     ch, 9
.text:0064CCE7                 mov     ulong LZCodeMask, 1FFh
.text:0064CCF1                 mov     ulong LZMaxIndex, 200h
.text:0064CCFB                 mov     ulong LZFreeIndex, 102h
.text:0064CD05                 cmp     esi, uchar * LZSrcBufEnd
.text:0064CD0B                 ja      LZDecomp_EndFunc
.text:0064CD11                 mov     eax, [esi]
.text:0064CD13                 xor     ebx, ebx
.text:0064CD15                 shr     eax, cl
.text:0064CD17                 add     cl, ch
.text:0064CD19                 mov     edx, ulong LZCodeMask
.text:0064CD1F                 mov     bl, cl
.text:0064CD21                 and     cl, 7
.text:0064CD24                 shr     ebx, 3
.text:0064CD27                 and     eax, edx
.text:0064CD29                 add     esi, ebx
.text:0064CD2B                 nop
.text:0064CD2C                 mov     HashStruct * LZOldChain, eax
.text:0064CD31                 mov     char LZOldSuffix, al
.text:0064CD36                 mov     [edi], al
.text:0064CD38                 inc     edi
.text:0064CD39
.text:0064CD39 Label2:                                 ; CODE XREF: LZDecomp(uchar *,uchar *,ulong)+63.j
.text:0064CD39                                         ; LZDecomp(uchar *,uchar *,ulong)+18C.j ...
.text:0064CD39                 cmp     esi, uchar * LZSrcBufEnd
.text:0064CD3F                 ja      LZDecomp_EndFunc
.text:0064CD45                 mov     eax, [esi]
.text:0064CD47                 xor     ebx, ebx
.text:0064CD49                 shr     eax, cl
.text:0064CD4B                 add     cl, ch
.text:0064CD4D                 mov     edx, ulong LZCodeMask
.text:0064CD53                 mov     bl, cl
.text:0064CD55                 and     cl, 7
.text:0064CD58                 shr     ebx, 3
.text:0064CD5B                 and     eax, edx
.text:0064CD5D                 add     esi, ebx
.text:0064CD5F                 nop
.text:0064CD60                 cmp     eax, 101h
.text:0064CD65                 jz      LZDecomp_EndFunc
.text:0064CD6B                 cmp     eax, 100h
.text:0064CD70                 jz      Label1
.text:0064CD76                 mov     edx, esp
.text:0064CD78                 dec     esp
.text:0064CD79                 mov     HashStruct * LZChain, eax
.text:0064CD7E                 lea     ebx, char * LZHashBuffer[eax+eax*2]
.text:0064CD85                 cmp     eax, ulong LZFreeIndex
.text:0064CD8B                 jl      short Label3
.text:0064CD8D                 mov     al, char LZOldSuffix
.text:0064CD92                 mov     [esp+10h+LocalPtr], al
.text:0064CD95                 dec     esp
.text:0064CD96                 mov     [ebx+2], al
.text:0064CD99                 mov     eax, HashStruct * LZOldChain
.text:0064CD9E                 mov     [ebx], ax
.text:0064CDA1                 lea     ebx, char * LZHashBuffer[eax+eax*2]
.text:0064CDA8
.text:0064CDA8 Label3:                                 ; CODE XREF: LZDecomp(uchar *,uchar *,ulong)+10B.j
.text:0064CDA8                                         ; LZDecomp(uchar *,uchar *,ulong)+13D.j
.text:0064CDA8                 test    ah, ah
.text:0064CDAA                 jz      short Label4
.text:0064CDAC                 mov     al, [ebx+2]
.text:0064CDAF                 mov     [esp+10h+LocalPtr], al
.text:0064CDB2                 dec     esp
.text:0064CDB3                 movzx   eax, word ptr [ebx]
.text:0064CDB6                 lea     ebx, char * LZHashBuffer[eax+eax*2]
.text:0064CDBD                 jmp     short Label3
.text:0064CDBF ; ---------------------------------------------------------------------------
.text:0064CDBF
.text:0064CDBF Label4:                                 ; CODE XREF: LZDecomp(uchar *,uchar *,ulong)+12A.j
.text:0064CDBF                 mov     char LZOldSuffix, al
.text:0064CDC4                 mov     [esp+10h+LocalPtr], al
.text:0064CDC7                 sub     edx, esp
.text:0064CDC9                 mov     ebx, ulong LZFreeIndex
.text:0064CDCF
.text:0064CDCF Label5:                                 ; CODE XREF: LZDecomp(uchar *,uchar *,ulong)+157.j
.text:0064CDCF                 mov     al, [esp+10h+LocalPtr]
.text:0064CDD2                 inc     esp
.text:0064CDD3                 mov     [edi], al
.text:0064CDD5                 inc     edi
.text:0064CDD6                 dec     edx
.text:0064CDD7                 jnz     short Label5
.text:0064CDD9                 mov     al, char LZOldSuffix
.text:0064CDDE                 mov     edx, HashStruct * LZOldChain
.text:0064CDE4                 mov     [ebx+ebx*2+7D8FF6h], al
.text:0064CDEB                 mov     word ptr char * LZHashBuffer[ebx+ebx*2], dx
.text:0064CDF3                 inc     ebx
.text:0064CDF4                 mov     eax, HashStruct * LZChain
.text:0064CDF9                 mov     ulong LZFreeIndex, ebx
.text:0064CDFF                 mov     edx, ulong LZMaxIndex
.text:0064CE05                 mov     HashStruct * LZOldChain, eax
.text:0064CE0A                 cmp     ebx, edx
.text:0064CE0C                 jl      Label2
.text:0064CE12                 cmp     ch, 0Ch
.text:0064CE15                 jz      Label2
.text:0064CE1B                 inc     ch
.text:0064CE1D                 mov     ebx, ulong LZCodeMask
.text:0064CE23                 mov     eax, ulong LZMaxIndex
.text:0064CE28                 add     ebx, ebx
.text:0064CE2A                 add     eax, eax
.text:0064CE2C                 or      ebx, 1
.text:0064CE2F                 mov     ulong LZMaxIndex, eax
.text:0064CE34                 mov     ulong LZCodeMask, ebx
.text:0064CE3A                 jmp     Label2
.text:0064CE3F ; ---------------------------------------------------------------------------
.text:0064CE3F
.text:0064CE3F LZDecomp_EndFunc:                       ; CODE XREF: LZDecomp(uchar *,uchar *,ulong)+8B.j
.text:0064CE3F                                         ; LZDecomp(uchar *,uchar *,ulong)+BF.j ...
.text:0064CE3F                 sub     edi, [ebp+OutputBuffer]
.text:0064CE42                 mov     [ebp+LocalInt], edi
.text:0064CE45                 mov     eax, [ebp+LocalInt]
.text:0064CE48                 pop     edi
.text:0064CE49                 pop     esi
.text:0064CE4A                 pop     ebx
.text:0064CE4B                 mov     esp, ebp
.text:0064CE4D                 pop     ebp
.text:0064CE4E                 retn
.text:0064CE4E long __cdecl LZDecomp(unsigned char *, unsigned char *, unsigned long) endp


    The data...
	LZHashBuffer is a large buffer (16K) so I'm only showing the start and end of it. 
	All you need really.

.data:007D8FF4 char * LZHashBuffer dd ?                ; DATA XREF: LZDecomp(uchar *,uchar *,ulong)+FE.r
.data:007D8FF4                                         ; LZDecomp(uchar *,uchar *,ulong)+121.r ...
.data:007D8FF8                 db    ? ;
.data:007D8FF9                 db    ? ;
.data:007D8FFA                 db    ? ;
	... and continues on up to
.data:007DCFF2                 db    ? ;
.data:007DCFF3                 db    ? ;

.data:007DCFF4 struct HashStruct * LZOldChain dd ?     ; DATA XREF: LZDecomp(uchar *,uchar *,ulong)+1F.w
.data:007DCFF4                                         ; LZDecomp(uchar *,uchar *,ulong)+AC.w ...
.data:007DCFF8 struct HashStruct * LZChain dd ?        ; DATA XREF: LZDecomp(uchar *,uchar *,ulong)+2A.w
.data:007DCFF8                                         ; LZDecomp(uchar *,uchar *,ulong)+F9.w ...
.data:007DCFFC unsigned long LZMaxIndex dd ?           ; DATA XREF: LZDecomp(uchar *,uchar *,ulong)+2F.w
.data:007DCFFC                                         ; LZDecomp(uchar *,uchar *,ulong)+48.w ...
.data:007DD000 unsigned long LZCodeMask dd ?           ; DATA XREF: LZDecomp(uchar *,uchar *,ulong)+34.w
.data:007DD000                                         ; LZDecomp(uchar *,uchar *,ulong)+3E.w ...
.data:007DD004 unsigned long LZFreeIndex dd ?          ; DATA XREF: LZDecomp(uchar *,uchar *,ulong)+39.w
.data:007DD004                                         ; LZDecomp(uchar *,uchar *,ulong)+52.w ...
.data:007DD008 unsigned char * LZSrcBufEnd dd ?        ; DATA XREF: LZDecomp(uchar *,uchar *,ulong)+24.w
.data:007DD008                                         ; LZDecomp(uchar *,uchar *,ulong)+85.r ...

   This one doesn't seem to get used. So who knows what it is for. Probably old leftover stuff.
   (for old DOS goo it would seem from the name)
.data:007DD00C unsigned char * LZOrigDOSBuf db    ? ;
.data:007DD00D                 db    ? ;
.data:007DD00E                 db    ? ;
.data:007DD00F                 db    ? ;

.data:007DD010 char LZOldSuffix db ?                   ; DATA XREF: LZDecomp(uchar *,uchar *,ulong)+5C.w
.data:007DD010                                         ; LZDecomp(uchar *,uchar *,ulong)+B1.w ...
.data:007DD011                 align 8


*/