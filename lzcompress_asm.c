/*****************************************************************************
  lzcompress_asm.c

	This is another Hacktastic implementation of (this time) the LZCompress
	routine from MCX.EXE. MCX.EXE is of course the MechCommander Gold 
	executable that is installed on my machine.


  How to use:
	The single function declared in this file is all you need. It is self
	contained and complete.

	Simply add the line below to your code and call it with the required 
	parameters (all compression is done in memory).

	extern long LZCompress(unsigned char *CompDataBuff, unsigned char *UncompDataBuff, unsigned long UncompDataSize);
	
	Make sure that the compression buffer is large enough to accept all
	data. In fact, in testing, I did notice that it can add an extra byte
	in the buffer at the end. So not only should you have enough space, but
	a little extra too.

	LZCompress will return the number of bytes put in the compression buffer.


  Notes:
	Once again, this is a Lempel-Ziv variant and is likely patented. If
	you intend to use this code in a commercial product, you are going to
	be owing someone money. For non-profit, non-commercial use, you are
	likely fine as the companies who hold the patents don't prosecute
	for such things. This is why Unix can have the Compress utility even
	though Compress uses LZW.


  Author(s):
	Original code: unknown
	Hack and slash version: cmunsta
*****************************************************************************/

unsigned long InBufferUpperLimit;
unsigned long InBufferPos;
unsigned char *InBuffer;
unsigned long OutBufferPos;
unsigned char *OutBuffer;
unsigned long PrefixCode;
unsigned long FreeCode;
unsigned long MaxCode;
unsigned long NBits;
unsigned long BitOffset;
unsigned char K;

unsigned char LZCRealHashBuff[0xc400];		// This was allocated in the code, changed to a static array
unsigned char *LZCHashBuf = LZCRealHashBuff;


unsigned long LZCOutCount = 0;	// I added this as there was a problem getting the count out of the assembly, so HACK!!!


long LZCompress(unsigned char *CompDataBuff, unsigned char *UncompDataBuff, unsigned long UncompDataSize)
{
	long CTempInt;
	long CLocalInt;

    OutBufferPos = (unsigned long)CompDataBuff;
    OutBuffer = CompDataBuff;
    InBufferPos = (unsigned long)UncompDataBuff;
	InBuffer = UncompDataBuff;
    InBufferUpperLimit = InBufferPos + UncompDataSize;

	_asm {
                push    ebp
                mov     ebp, esp
                sub     esp, 8
                push    ebx
                push    esi
                push    edi
                mov     CLocalInt, 0

                ;cmp     uchar * LZCHashBuf, 0			; was checking if LZCHashBuf was already allocated. well
                ;jnz     short CLable1					; it is now :)

                ;push    0C400h							; This merely calls malloc to allocate a 0xc400 byte buffer.
                ;mov     ecx, UserHeap * systemHeap		; This has been converted into a static array so no need
                ;call    UserHeap::malloc(ulong)		; for the malloc call anymore.
                ;mov     uchar * LZCHashBuf, eax
                ;cmp     uchar * LZCHashBuf, 0
                ;jnz     short CLable1
                ;mov     eax, 0CBCB0001h
                ;jmp     LZCompress_EndFunc
; ---------------------------------------------------------------------------

//CLable1:
                mov     CTempInt, 0C00h
                
				;mov     eax, CompDataBuff				; I moved this out of the ASM stuff. Was having a 
                ;mov     OutBufferPos, eax				; stupid problem, so of course, I solved it with a
                ;mov     OutBuffer, eax					; large axe.
                ;mov     eax, [UncompDataBuff]
                ;mov     InBufferPos, eax
                ;mov     InBuffer, eax
                ;add     eax, [UncompDataSize]
                ;mov     InBufferUpperLimit, eax

                mov     BitOffset, 0
                mov     NBits, 9
                mov     MaxCode, 200h
                mov     eax, 0FFFFFFFFh
                mov     ecx, CTempInt
                mov     edi, LZCHashBuf
                rep stosb
                mov     FreeCode, 102h
                mov     eax, 100h
                xor     edx, edx
                mov     edi, OutBufferPos
                mov     ecx, BitOffset
                jecxz   CLabel3

CLabel2:
                shl     ax, 1
                rcl     edx, 1
                loop    CLabel2
                or      al, [edi]

CLabel3:
                stosw
                mov     al, dl
                stosb
                mov     ecx, NBits
                mov     eax, BitOffset
                add     eax, ecx
                mov     cl, al
                shr     al, 3
                add     OutBufferPos, eax
                and     cl, 7
                movzx   ecx, cl
                mov     BitOffset, ecx
                mov     esi, InBufferPos
                cmp     esi, InBufferUpperLimit
                cmc
                jb      CLabel4
                lodsb
                mov     InBufferPos, esi
                clc

CLabel4:
                movzx   eax, al

CLabel5:
                mov     PrefixCode, eax
                mov     esi, InBufferPos
                cmp     esi, InBufferUpperLimit
                cmc
                jb      CLabel6
                lodsb
                mov     InBufferPos, esi
                clc

CLabel6:
                jb      CLabel19
                mov     K, al
                mov     ebx, PrefixCode
                lea     esi, ds:0[ebx*8]
                add     esi, ebx
                add     esi, LZCHashBuf
                xor     edi, edi
                cmp     dword ptr [esi], 0FFFFFFFFh
                jz      CLabel9
                inc     edi
                mov     ebx, [esi]

CLabel7:
                lea     esi, ds:0[ebx*8]
                add     esi, ebx
                add     esi, LZCHashBuf
                cmp     [esi+8], al
                jnz     CLabel8
                clc
                mov     eax, ebx
                jmp     CLabel10
; ---------------------------------------------------------------------------

CLabel8:
                cmp     dword ptr [esi+4], 0FFFFFFFFh
                jz      CLabel9
                mov     ebx, [esi+4]
                jmp     CLabel7
; ---------------------------------------------------------------------------

CLabel9:
                stc

CLabel10:
                jnb     CLabel5
                mov     ebx, FreeCode
                push    ebx
                or      edi, edi
                jz      CLabel11
                mov     [esi+4], ebx
                jmp     CLabel12
; ---------------------------------------------------------------------------

CLabel11:
                mov     [esi], ebx

CLabel12:
                cmp     ebx, 1000h
                jz      CLabel13
                lea     esi, ds:0[ebx*8]
                add     esi, ebx
                add     esi, LZCHashBuf
                mov     dword ptr [esi], 0FFFFFFFFh
                mov     dword ptr [esi+4], 0FFFFFFFFh
                mov     [esi+8], al
                inc     ebx

CLabel13:
                mov     FreeCode, ebx
                pop     ebx
                push    ebx
                mov     eax, PrefixCode
                xor     edx, edx
                mov     edi, OutBufferPos
                mov     ecx, BitOffset
                jecxz   CLabel15

CLabel14:
                shl     ax, 1
                rcl     edx, 1
                loop    CLabel14
                or      al, [edi]

CLabel15:
                stosw
                mov     al, dl
                stosb
                mov     ecx, NBits
                mov     eax, BitOffset
                add     eax, ecx
                mov     cl, al
                shr     al, 3
                add     OutBufferPos, eax
                and     cl, 7
                movzx   ecx, cl
                mov     BitOffset, ecx
                pop     ebx
                mov     al, K
                cmp     ebx, MaxCode
                jb      CLabel4
                cmp     NBits, 0Ch
                jb      CLabel18
                mov     eax, 100h
                xor     edx, edx
                mov     edi, OutBufferPos
                mov     ecx, BitOffset
                jecxz   CLabel17

CLabel16:
                shl     ax, 1
                rcl     edx, 1
                loop    CLabel16
                or      al, [edi]

CLabel17:
                stosw
                mov     al, dl
                stosb
                mov     ecx, NBits
                mov     eax, BitOffset
                add     eax, ecx
                mov     cl, al
                shr     al, 3
                add     OutBufferPos, eax
                and     cl, 7
                movzx   ecx, cl
                mov     BitOffset, ecx
                mov     NBits, 9
                mov     MaxCode, 200h
                mov     eax, 0FFFFFFFFh
                mov     ecx, CTempInt
                mov     edi, LZCHashBuf
                rep stosb
                mov     FreeCode, 102h
                mov     al, K
                jmp     CLabel4
; ---------------------------------------------------------------------------

CLabel18:
                inc     NBits				; Looks to me like this is where it increases the size of the
                shl     MaxCode, 1			; dictionary index value
                jmp     CLabel4
; ---------------------------------------------------------------------------

CLabel19:
                mov     eax, PrefixCode
                xor     edx, edx
                mov     edi, OutBufferPos
                mov     ecx, BitOffset
                jecxz   CLabel21

CLabel20:
                shl     ax, 1
                rcl     edx, 1
                loop    CLabel20
                or      al, [edi]

CLabel21:
                stosw
                mov     al, dl
                stosb
                mov     ecx, NBits
                mov     eax, BitOffset
                add     eax, ecx
                mov     cl, al
                shr     al, 3
                add     OutBufferPos, eax
                and     cl, 7
                movzx   ecx, cl
                mov     BitOffset, ecx
                mov     eax, 101h
                xor     edx, edx
                mov     edi, OutBufferPos
                mov     ecx, BitOffset
                jecxz   CLabel23

CLabel22:
                shl     ax, 1
                rcl     edx, 1
                loop    CLabel22
                or      al, [edi]

CLabel23:
                stosw
                mov     al, dl
                stosb
                mov     ecx, NBits
                mov     eax, BitOffset
                add     eax, ecx
                mov     cl, al
                shr     al, 3
                add     OutBufferPos, eax
                and     cl, 7
                movzx   ecx, cl
                mov     BitOffset, ecx
                mov     eax, BitOffset
                or      eax, eax
                jz      CLabel24
                inc     OutBufferPos

CLabel24:
                mov     eax, OutBufferPos
                sub     eax, OutBuffer

                ;mov     CLocalInt, eax
                ;mov     eax, CLocalInt

				mov		LZCOutCount, eax	; Hack

//LZCompress_EndFunc:
                pop     edi
                pop     esi
                pop     ebx
                mov     esp, ebp
                pop     ebp

	};

	return LZCOutCount; // basically, this returns the number of bytes of data out in CompDataBuff
}


/*
==============================================================================
==============================================================================
	This is the disassembly of the LZCompress routine in MCX.EXE. Again,
	I've done very little to it apart from naming a few labels etc.
	Disassembly obtained from IDA (The Interactive Debugger).


.text:0064C920 long __cdecl LZCompress(unsigned char *, unsigned char *, unsigned long) proc near
.text:0064C920                                         ; CODE XREF: PacketFile::writePacket(long,uchar *,long,uchar)+84.p
.text:0064C920
.text:0064C920 CTempInt        = dword ptr -8
.text:0064C920 CLocalInt       = dword ptr -4
.text:0064C920 CompDataBuff    = dword ptr  8
.text:0064C920 UncompDataBuff  = dword ptr  0Ch
.text:0064C920 UncompDataSize  = dword ptr  10h
.text:0064C920
.text:0064C920                 push    ebp
.text:0064C921                 mov     ebp, esp
.text:0064C923                 sub     esp, 8
.text:0064C926                 push    ebx
.text:0064C927                 push    esi
.text:0064C928                 push    edi
.text:0064C929                 mov     [ebp+CLocalInt], 0
.text:0064C930                 cmp     uchar * LZCHashBuf, 0
.text:0064C937                 jnz     short CLable1
.text:0064C939                 push    0C400h
.text:0064C93E                 mov     ecx, UserHeap * systemHeap
.text:0064C944                 call    UserHeap::malloc(ulong)
.text:0064C949                 mov     uchar * LZCHashBuf, eax
.text:0064C94E                 cmp     uchar * LZCHashBuf, 0
.text:0064C955                 jnz     short CLable1
.text:0064C957                 mov     eax, 0CBCB0001h
.text:0064C95C                 jmp     LZCompress_EndFunc
.text:0064C961 ; ---------------------------------------------------------------------------
.text:0064C961
.text:0064C961 CLable1:                                ; CODE XREF: LZCompress(uchar *,uchar *,ulong)+17.j
.text:0064C961                                         ; LZCompress(uchar *,uchar *,ulong)+35.j
.text:0064C961                 mov     [ebp+CTempInt], 0C00h
.text:0064C968                 mov     eax, [ebp+CompDataBuff]
.text:0064C96B                 mov     ulong OutBufferPos, eax
.text:0064C970                 mov     uchar * OutBuffer, eax
.text:0064C975                 mov     eax, [ebp+UncompDataBuff]
.text:0064C978                 mov     ulong InBufferPos, eax
.text:0064C97D                 mov     uchar * InBuffer, eax
.text:0064C982                 add     eax, [ebp+UncompDataSize]
.text:0064C985                 mov     ulong InBufferUpperLimit, eax
.text:0064C98A                 mov     ulong BitOffset, 0
.text:0064C994                 mov     ulong NBits, 9
.text:0064C99E                 mov     ulong MaxCode, 200h
.text:0064C9A8                 mov     eax, 0FFFFFFFFh
.text:0064C9AD                 mov     ecx, [ebp+CTempInt]
.text:0064C9B0                 mov     edi, uchar * LZCHashBuf
.text:0064C9B6                 rep stosb
.text:0064C9B8                 mov     ulong FreeCode, 102h
.text:0064C9C2                 mov     eax, 100h
.text:0064C9C7                 xor     edx, edx
.text:0064C9C9                 mov     edi, ulong OutBufferPos
.text:0064C9CF                 mov     ecx, ulong BitOffset
.text:0064C9D5                 jecxz   short CLabel3
.text:0064C9D7
.text:0064C9D7 CLabel2:                                ; CODE XREF: LZCompress(uchar *,uchar *,ulong)+BC.j
.text:0064C9D7                 shl     ax, 1
.text:0064C9DA                 rcl     edx, 1
.text:0064C9DC                 loop    CLabel2
.text:0064C9DE                 or      al, [edi]
.text:0064C9E0
.text:0064C9E0 CLabel3:                                ; CODE XREF: LZCompress(uchar *,uchar *,ulong)+B5.j
.text:0064C9E0                 stosw
.text:0064C9E2                 mov     al, dl
.text:0064C9E4                 stosb
.text:0064C9E5                 mov     ecx, ulong NBits
.text:0064C9EB                 mov     eax, ulong BitOffset
.text:0064C9F0                 add     eax, ecx
.text:0064C9F2                 mov     cl, al
.text:0064C9F4                 shr     al, 3
.text:0064C9F7                 add     ulong OutBufferPos, eax
.text:0064C9FD                 and     cl, 7
.text:0064CA00                 movzx   ecx, cl
.text:0064CA03                 mov     ulong BitOffset, ecx
.text:0064CA09                 mov     esi, ulong InBufferPos
.text:0064CA0F                 cmp     esi, ulong InBufferUpperLimit
.text:0064CA15                 cmc
.text:0064CA16                 jb      short CLabel4
.text:0064CA18                 lodsb
.text:0064CA19                 mov     ulong InBufferPos, esi
.text:0064CA1F                 clc
.text:0064CA20
.text:0064CA20 CLabel4:                                ; CODE XREF: LZCompress(uchar *,uchar *,ulong)+F6.j
.text:0064CA20                                         ; LZCompress(uchar *,uchar *,ulong)+205.j ...
.text:0064CA20                 movzx   eax, al
.text:0064CA23
.text:0064CA23 CLabel5:                                ; CODE XREF: LZCompress(uchar *,uchar *,ulong):CLabel10.j
.text:0064CA23                 mov     ulong PrefixCode, eax
.text:0064CA28                 mov     esi, ulong InBufferPos
.text:0064CA2E                 cmp     esi, ulong InBufferUpperLimit
.text:0064CA34                 cmc
.text:0064CA35                 jb      short CLabel6
.text:0064CA37                 lodsb
.text:0064CA38                 mov     ulong InBufferPos, esi
.text:0064CA3E                 clc
.text:0064CA3F
.text:0064CA3F CLabel6:                                ; CODE XREF: LZCompress(uchar *,uchar *,ulong)+115.j
.text:0064CA3F                 jb      CLabel19
.text:0064CA45                 mov     uchar K, al
.text:0064CA4A                 mov     ebx, ulong PrefixCode
.text:0064CA50                 lea     esi, ds:0[ebx*8]
.text:0064CA57                 add     esi, ebx
.text:0064CA59                 add     esi, uchar * LZCHashBuf
.text:0064CA5F                 xor     edi, edi
.text:0064CA61                 cmp     dword ptr [esi], 0FFFFFFFFh
.text:0064CA64                 jz      short CLabel9
.text:0064CA66                 inc     edi
.text:0064CA67                 mov     ebx, [esi]
.text:0064CA69
.text:0064CA69 CLabel7:                                ; CODE XREF: LZCompress(uchar *,uchar *,ulong)+16B.j
.text:0064CA69                 lea     esi, ds:0[ebx*8]
.text:0064CA70                 add     esi, ebx
.text:0064CA72                 add     esi, uchar * LZCHashBuf
.text:0064CA78                 cmp     [esi+8], al
.text:0064CA7B                 jnz     short CLabel8
.text:0064CA7D                 clc
.text:0064CA7E                 mov     eax, ebx
.text:0064CA80                 jmp     short CLabel10
.text:0064CA82 ; ---------------------------------------------------------------------------
.text:0064CA82
.text:0064CA82 CLabel8:                                ; CODE XREF: LZCompress(uchar *,uchar *,ulong)+15B.j
.text:0064CA82                 cmp     dword ptr [esi+4], 0FFFFFFFFh
.text:0064CA86                 jz      short CLabel9
.text:0064CA88                 mov     ebx, [esi+4]
.text:0064CA8B                 jmp     short CLabel7
.text:0064CA8D ; ---------------------------------------------------------------------------
.text:0064CA8D
.text:0064CA8D CLabel9:                                ; CODE XREF: LZCompress(uchar *,uchar *,ulong)+144.j
.text:0064CA8D                                         ; LZCompress(uchar *,uchar *,ulong)+166.j
.text:0064CA8D                 stc
.text:0064CA8E
.text:0064CA8E CLabel10:                               ; CODE XREF: LZCompress(uchar *,uchar *,ulong)+160.j
.text:0064CA8E                 jnb     short CLabel5
.text:0064CA90                 mov     ebx, ulong FreeCode
.text:0064CA96                 push    ebx
.text:0064CA97                 or      edi, edi
.text:0064CA99                 jz      short CLabel11
.text:0064CA9B                 mov     [esi+4], ebx
.text:0064CA9E                 jmp     short CLabel12
.text:0064CAA0 ; ---------------------------------------------------------------------------
.text:0064CAA0
.text:0064CAA0 CLabel11:                               ; CODE XREF: LZCompress(uchar *,uchar *,ulong)+179.j
.text:0064CAA0                 mov     [esi], ebx
.text:0064CAA2
.text:0064CAA2 CLabel12:                               ; CODE XREF: LZCompress(uchar *,uchar *,ulong)+17E.j
.text:0064CAA2                 cmp     ebx, 1000h
.text:0064CAA8                 jz      short CLabel13
.text:0064CAAA                 lea     esi, ds:0[ebx*8]
.text:0064CAB1                 add     esi, ebx
.text:0064CAB3                 add     esi, uchar * LZCHashBuf
.text:0064CAB9                 mov     dword ptr [esi], 0FFFFFFFFh
.text:0064CABF                 mov     dword ptr [esi+4], 0FFFFFFFFh
.text:0064CAC6                 mov     [esi+8], al
.text:0064CAC9                 inc     ebx
.text:0064CACA
.text:0064CACA CLabel13:                               ; CODE XREF: LZCompress(uchar *,uchar *,ulong)+188.j
.text:0064CACA                 mov     ulong FreeCode, ebx
.text:0064CAD0                 pop     ebx
.text:0064CAD1                 push    ebx
.text:0064CAD2                 mov     eax, ulong PrefixCode
.text:0064CAD7                 xor     edx, edx
.text:0064CAD9                 mov     edi, ulong OutBufferPos
.text:0064CADF                 mov     ecx, ulong BitOffset
.text:0064CAE5                 jecxz   short CLabel15
.text:0064CAE7
.text:0064CAE7 CLabel14:                               ; CODE XREF: LZCompress(uchar *,uchar *,ulong)+1CC.j
.text:0064CAE7                 shl     ax, 1
.text:0064CAEA                 rcl     edx, 1
.text:0064CAEC                 loop    CLabel14
.text:0064CAEE                 or      al, [edi]
.text:0064CAF0
.text:0064CAF0 CLabel15:                               ; CODE XREF: LZCompress(uchar *,uchar *,ulong)+1C5.j
.text:0064CAF0                 stosw
.text:0064CAF2                 mov     al, dl
.text:0064CAF4                 stosb
.text:0064CAF5                 mov     ecx, ulong NBits
.text:0064CAFB                 mov     eax, ulong BitOffset
.text:0064CB00                 add     eax, ecx
.text:0064CB02                 mov     cl, al
.text:0064CB04                 shr     al, 3
.text:0064CB07                 add     ulong OutBufferPos, eax
.text:0064CB0D                 and     cl, 7
.text:0064CB10                 movzx   ecx, cl
.text:0064CB13                 mov     ulong BitOffset, ecx
.text:0064CB19                 pop     ebx
.text:0064CB1A                 mov     al, uchar K
.text:0064CB1F                 cmp     ebx, ulong MaxCode
.text:0064CB25                 jb      CLabel4
.text:0064CB2B                 cmp     ulong NBits, 0Ch
.text:0064CB32                 jb      short CLabel18
.text:0064CB34                 mov     eax, 100h
.text:0064CB39                 xor     edx, edx
.text:0064CB3B                 mov     edi, ulong OutBufferPos
.text:0064CB41                 mov     ecx, ulong BitOffset
.text:0064CB47                 jecxz   short CLabel17
.text:0064CB49
.text:0064CB49 CLabel16:                               ; CODE XREF: LZCompress(uchar *,uchar *,ulong)+22E.j
.text:0064CB49                 shl     ax, 1
.text:0064CB4C                 rcl     edx, 1
.text:0064CB4E                 loop    CLabel16
.text:0064CB50                 or      al, [edi]
.text:0064CB52
.text:0064CB52 CLabel17:                               ; CODE XREF: LZCompress(uchar *,uchar *,ulong)+227.j
.text:0064CB52                 stosw
.text:0064CB54                 mov     al, dl
.text:0064CB56                 stosb
.text:0064CB57                 mov     ecx, ulong NBits
.text:0064CB5D                 mov     eax, ulong BitOffset
.text:0064CB62                 add     eax, ecx
.text:0064CB64                 mov     cl, al
.text:0064CB66                 shr     al, 3
.text:0064CB69                 add     ulong OutBufferPos, eax
.text:0064CB6F                 and     cl, 7
.text:0064CB72                 movzx   ecx, cl
.text:0064CB75                 mov     ulong BitOffset, ecx
.text:0064CB7B                 mov     ulong NBits, 9
.text:0064CB85                 mov     ulong MaxCode, 200h
.text:0064CB8F                 mov     eax, 0FFFFFFFFh
.text:0064CB94                 mov     ecx, [ebp+CTempInt]
.text:0064CB97                 mov     edi, uchar * LZCHashBuf
.text:0064CB9D                 rep stosb
.text:0064CB9F                 mov     ulong FreeCode, 102h
.text:0064CBA9                 mov     al, uchar K
.text:0064CBAE                 jmp     CLabel4
.text:0064CBB3 ; ---------------------------------------------------------------------------
.text:0064CBB3
.text:0064CBB3 CLabel18:                               ; CODE XREF: LZCompress(uchar *,uchar *,ulong)+212.j
.text:0064CBB3                 inc     ulong NBits
.text:0064CBB9                 shl     ulong MaxCode, 1
.text:0064CBBF                 jmp     CLabel4
.text:0064CBC4 ; ---------------------------------------------------------------------------
.text:0064CBC4
.text:0064CBC4 CLabel19:                               ; CODE XREF: LZCompress(uchar *,uchar *,ulong):CLabel6.j
.text:0064CBC4                 mov     eax, ulong PrefixCode
.text:0064CBC9                 xor     edx, edx
.text:0064CBCB                 mov     edi, ulong OutBufferPos
.text:0064CBD1                 mov     ecx, ulong BitOffset
.text:0064CBD7                 jecxz   short CLabel21
.text:0064CBD9
.text:0064CBD9 CLabel20:                               ; CODE XREF: LZCompress(uchar *,uchar *,ulong)+2BE.j
.text:0064CBD9                 shl     ax, 1
.text:0064CBDC                 rcl     edx, 1
.text:0064CBDE                 loop    CLabel20
.text:0064CBE0                 or      al, [edi]
.text:0064CBE2
.text:0064CBE2 CLabel21:                               ; CODE XREF: LZCompress(uchar *,uchar *,ulong)+2B7.j
.text:0064CBE2                 stosw
.text:0064CBE4                 mov     al, dl
.text:0064CBE6                 stosb
.text:0064CBE7                 mov     ecx, ulong NBits
.text:0064CBED                 mov     eax, ulong BitOffset
.text:0064CBF2                 add     eax, ecx
.text:0064CBF4                 mov     cl, al
.text:0064CBF6                 shr     al, 3
.text:0064CBF9                 add     ulong OutBufferPos, eax
.text:0064CBFF                 and     cl, 7
.text:0064CC02                 movzx   ecx, cl
.text:0064CC05                 mov     ulong BitOffset, ecx
.text:0064CC0B                 mov     eax, 101h
.text:0064CC10                 xor     edx, edx
.text:0064CC12                 mov     edi, ulong OutBufferPos
.text:0064CC18                 mov     ecx, ulong BitOffset
.text:0064CC1E                 jecxz   short CLabel23
.text:0064CC20
.text:0064CC20 CLabel22:                               ; CODE XREF: LZCompress(uchar *,uchar *,ulong)+305.j
.text:0064CC20                 shl     ax, 1
.text:0064CC23                 rcl     edx, 1
.text:0064CC25                 loop    CLabel22
.text:0064CC27                 or      al, [edi]
.text:0064CC29
.text:0064CC29 CLabel23:                               ; CODE XREF: LZCompress(uchar *,uchar *,ulong)+2FE.j
.text:0064CC29                 stosw
.text:0064CC2B                 mov     al, dl
.text:0064CC2D                 stosb
.text:0064CC2E                 mov     ecx, ulong NBits
.text:0064CC34                 mov     eax, ulong BitOffset
.text:0064CC39                 add     eax, ecx
.text:0064CC3B                 mov     cl, al
.text:0064CC3D                 shr     al, 3
.text:0064CC40                 add     ulong OutBufferPos, eax
.text:0064CC46                 and     cl, 7
.text:0064CC49                 movzx   ecx, cl
.text:0064CC4C                 mov     ulong BitOffset, ecx
.text:0064CC52                 mov     eax, ulong BitOffset
.text:0064CC57                 or      eax, eax
.text:0064CC59                 jz      short CLabel24
.text:0064CC5B                 inc     ulong OutBufferPos
.text:0064CC61
.text:0064CC61 CLabel24:                               ; CODE XREF: LZCompress(uchar *,uchar *,ulong)+339.j
.text:0064CC61                 mov     eax, ulong OutBufferPos
.text:0064CC66                 sub     eax, uchar * OutBuffer
.text:0064CC6C                 mov     [ebp+CLocalInt], eax
.text:0064CC6F                 mov     eax, [ebp+CLocalInt]
.text:0064CC72
.text:0064CC72 LZCompress_EndFunc:                     ; CODE XREF: LZCompress(uchar *,uchar *,ulong)+3C.j
.text:0064CC72                 pop     edi
.text:0064CC73                 pop     esi
.text:0064CC74                 pop     ebx
.text:0064CC75                 mov     esp, ebp
.text:0064CC77                 pop     ebp
.text:0064CC78                 retn
.text:0064CC78 long __cdecl LZCompress(unsigned char *, unsigned char *, unsigned long) endp


   and now the data...

.data:007D8FC0 unsigned char * LZCHashBuf dd ?         ; DATA XREF: LZCompress(uchar *,uchar *,ulong)+10.r
.data:007D8FC0                                         ; LZCompress(uchar *,uchar *,ulong)+29.w ...
.data:007D8FC4 unsigned long InBufferUpperLimit dd ?   ; DATA XREF: LZCompress(uchar *,uchar *,ulong)+65.w
.data:007D8FC4                                         ; LZCompress(uchar *,uchar *,ulong)+EF.r ...
.data:007D8FC8 unsigned long InBufferPos dd ?          ; DATA XREF: LZCompress(uchar *,uchar *,ulong)+58.w
.data:007D8FC8                                         ; LZCompress(uchar *,uchar *,ulong)+E9.r ...
.data:007D8FCC unsigned char * InBuffer dd ?           ; DATA XREF: LZCompress(uchar *,uchar *,ulong)+5D.w
.data:007D8FD0 unsigned long OutBufferPos dd ?         ; DATA XREF: LZCompress(uchar *,uchar *,ulong)+4B.w
.data:007D8FD0                                         ; LZCompress(uchar *,uchar *,ulong)+A9.r ...
.data:007D8FD4 unsigned char * OutBuffer dd ?          ; DATA XREF: LZCompress(uchar *,uchar *,ulong)+50.w
.data:007D8FD4                                         ; LZCompress(uchar *,uchar *,ulong)+346.r
.data:007D8FD8 unsigned long PrefixCode dd ?           ; DATA XREF: LZCompress(uchar *,uchar *,ulong):CLabel5.w
.data:007D8FD8                                         ; LZCompress(uchar *,uchar *,ulong)+12A.r ...
.data:007D8FDC unsigned long FreeCode dd ?             ; DATA XREF: LZCompress(uchar *,uchar *,ulong)+98.w
.data:007D8FDC                                         ; LZCompress(uchar *,uchar *,ulong)+170.r ...
.data:007D8FE0 unsigned long MaxCode dd ?              ; DATA XREF: LZCompress(uchar *,uchar *,ulong)+7E.w
.data:007D8FE0                                         ; LZCompress(uchar *,uchar *,ulong)+1FF.r ...
.data:007D8FE4 unsigned long NBits dd ?                ; DATA XREF: LZCompress(uchar *,uchar *,ulong)+74.w
.data:007D8FE4                                         ; LZCompress(uchar *,uchar *,ulong)+C5.r ...
.data:007D8FE8 unsigned long BitOffset dd ?            ; DATA XREF: LZCompress(uchar *,uchar *,ulong)+6A.w
.data:007D8FE8                                         ; LZCompress(uchar *,uchar *,ulong)+AF.r ...
.data:007D8FEC unsigned char K db ?                    ; DATA XREF: LZCompress(uchar *,uchar *,ulong)+125.w

*/