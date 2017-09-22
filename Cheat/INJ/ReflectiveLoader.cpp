//===============================================================================================//
// Copyright (c) 2012, Stephen Fewer of Harmony Security (www.harmonysecurity.com)
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without modification, are permitted 
// provided that the following conditions are met:
// 
//     * Redistributions of source code must retain the above copyright notice, this list of 
// conditions and the following disclaimer.
// 
//     * Redistributions in binary form must reproduce the above copyright notice, this list of 
// conditions and the following disclaimer in the documentation and/or other materials provided 
// with the distribution.
// 
//     * Neither the name of Harmony Security nor the names of its contributors may be used to
// endorse or promote products derived from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR 
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
// FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
// POSSIBILITY OF SUCH DAMAGE.
//===============================================================================================//
#include "ReflectiveLoader.h"
//===============================================================================================//
// Our loader will set this to a pseudo correct HINSTANCE/HMODULE value
HINSTANCE hAppInstance = NULL;
//===============================================================================================//
#pragma intrinsic( _ReturnAddress )
// This function can not be inlined by the compiler or we will not get the address we expect. Ideally 
// this code will be compiled with the /O2 and /Ob1 switches. Bonus points if we could take advantage of 
// RIP relative addressing in this instance but I dont believe we can do so with the compiler intrinsics 
// available (and no inline asm available under x64).
__declspec(noinline) ULONG_PTR caller( VOID ) { return (ULONG_PTR)_ReturnAddress(); }
//===============================================================================================//
// Note 1: If you want to have your own DllMain, define REFLECTIVEDLLINJECTION_CUSTOM_DLLMAIN,  

//         otherwise the DllMain at the end of this file will be used.

// Note 2: If you are injecting the DLL via LoadRemoteLibraryR, define REFLECTIVEDLLINJECTION_VIA_LOADREMOTELIBRARYR,
//         otherwise it is assumed you are calling the ReflectiveLoader via a stub.

// This is our position independent reflective DLL loader/injector

#define REFLECTIVEDLLINJECTION_CUSTOM_DLLMAIN

#ifdef REFLECTIVEDLLINJECTION_VIA_LOADREMOTELIBRARYR
DLLEXPORT ULONG_PTR WINAPI ReflectiveLoader( LPVOID lpParameter )
#else
DLLEXPORT ULONG_PTR WINAPI ReflectiveLoader( VOID )
#endif
{
	// the functions we need
	LOADLIBRARYA pLoadLibraryA     = NULL;
	GETPROCADDRESS pGetProcAddress = NULL;
	VIRTUALALLOC pVirtualAlloc     = NULL;
	NTFLUSHINSTRUCTIONCACHE pNtFlushInstructionCache = NULL;

	USHORT usCounter;

	// the initial location of this image in memory
	ULONG_PTR uiLibraryAddress;
	// the kernels base address and later this images newly loaded base address
	ULONG_PTR uiBaseAddress;

	// variables for processing the kernels export table
	ULONG_PTR uiAddressArray;
	ULONG_PTR uiNameArray;
	ULONG_PTR uiExportDir;
	ULONG_PTR uiNameOrdinals;
	DWORD dwHashValue;

	// variables for loading this image
	ULONG_PTR uiHeaderValue;
	ULONG_PTR uiValueA;
	ULONG_PTR uiValueB;
	ULONG_PTR uiValueC;
	ULONG_PTR uiValueD;
	ULONG_PTR uiValueE;

	// STEP 0: calculate our images current base address

	// we will start searching backwards from our callers return address.
	uiLibraryAddress = caller();

	// loop through memory backwards searching for our images base address
	// we dont need SEH style search as we shouldnt generate any access violations with this
	while( TRUE )
	{
		if( ((PIMAGE_DOS_HEADER)uiLibraryAddress)->e_magic == IMAGE_DOS_SIGNATURE )
		{
			uiHeaderValue = ((PIMAGE_DOS_HEADER)uiLibraryAddress)->e_lfanew;
			// some x64 dll's can trigger a bogus signature (IMAGE_DOS_SIGNATURE == 'POP r10'),
			// we sanity check the e_lfanew with an upper threshold value of 1024 to avoid problems.
			if( uiHeaderValue >= sizeof(IMAGE_DOS_HEADER) && uiHeaderValue < 1024 )
			{
				uiHeaderValue += uiLibraryAddress;
				// break if we have found a valid MZ/PE header
				if( ((PIMAGE_NT_HEADERS)uiHeaderValue)->Signature == IMAGE_NT_SIGNATURE )
					break;
			}
		}
		uiLibraryAddress--;
	}

	// STEP 1: process the kernels exports for the functions our loader needs...

	// get the Process Enviroment Block
#ifdef WIN_X64
	uiBaseAddress = __readgsqword( 0x60 );
#else
#ifdef WIN_X86
	uiBaseAddress = __readfsdword( 0x30 );
#else WIN_ARM
	//uiBaseAddress = *(DWORD *)( (BYTE *)_MoveFromCoprocessor( 15, 0, 13, 0, 2 ) + 0x30 );
#endif
#endif

	// get the processes loaded modules. ref: http://msdn.microsoft.com/en-us/library/aa813708(VS.85).aspx
	uiBaseAddress = (ULONG_PTR)((_PPEB)uiBaseAddress)->pLdr;

	// get the first entry of the InMemoryOrder module list
	uiValueA = (ULONG_PTR)((PPEB_LDR_DATA)uiBaseAddress)->InMemoryOrderModuleList.Flink;
	while( uiValueA )
	{
		// get pointer to current modules name (unicode string)
		uiValueB = (ULONG_PTR)((PLDR_DATA_TABLE_ENTRY)uiValueA)->BaseDllName.pBuffer;
		// set bCounter to the length for the loop
		usCounter = ((PLDR_DATA_TABLE_ENTRY)uiValueA)->BaseDllName.Length;
		// clear uiValueC which will store the hash of the module name
		uiValueC = 0;

		// compute the hash of the module name...
		do
		{
			uiValueC = ror( (DWORD)uiValueC );
			// normalize to uppercase if the madule name is in lowercase
			if( *((BYTE *)uiValueB) >= 'a' )
				uiValueC += *((BYTE *)uiValueB) - 0x20;
			else
				uiValueC += *((BYTE *)uiValueB);
			uiValueB++;
		} while( --usCounter );

		// compare the hash with that of kernel32.dll
		if( (DWORD)uiValueC == KERNEL32DLL_HASH )
		{
			// get this modules base address
			uiBaseAddress = (ULONG_PTR)((PLDR_DATA_TABLE_ENTRY)uiValueA)->DllBase;

			// get the VA of the modules NT Header
			uiExportDir = uiBaseAddress + ((PIMAGE_DOS_HEADER)uiBaseAddress)->e_lfanew;

			// uiNameArray = the address of the modules export directory entry
			uiNameArray = (ULONG_PTR)&((PIMAGE_NT_HEADERS)uiExportDir)->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_EXPORT ];

			// get the VA of the export directory
			uiExportDir = ( uiBaseAddress + ((PIMAGE_DATA_DIRECTORY)uiNameArray)->VirtualAddress );

			// get the VA for the array of name pointers
			uiNameArray = ( uiBaseAddress + ((PIMAGE_EXPORT_DIRECTORY )uiExportDir)->AddressOfNames );
			
			// get the VA for the array of name ordinals
			uiNameOrdinals = ( uiBaseAddress + ((PIMAGE_EXPORT_DIRECTORY )uiExportDir)->AddressOfNameOrdinals );

			usCounter = 3;

			// loop while we still have imports to find
			while( usCounter > 0 )
			{
				// compute the hash values for this function name
				dwHashValue = hash( (char *)( uiBaseAddress + DEREF_32( uiNameArray ) )  );
				
				// if we have found a function we want we get its virtual address
				if( dwHashValue == LOADLIBRARYA_HASH || dwHashValue == GETPROCADDRESS_HASH || dwHashValue == VIRTUALALLOC_HASH )
				{
					// get the VA for the array of addresses
					uiAddressArray = ( uiBaseAddress + ((PIMAGE_EXPORT_DIRECTORY )uiExportDir)->AddressOfFunctions );

					// use this functions name ordinal as an index into the array of name pointers
					uiAddressArray += ( DEREF_16( uiNameOrdinals ) * sizeof(DWORD) );

					// store this functions VA
					if( dwHashValue == LOADLIBRARYA_HASH )
						pLoadLibraryA = (LOADLIBRARYA)( uiBaseAddress + DEREF_32( uiAddressArray ) );
					else if( dwHashValue == GETPROCADDRESS_HASH )
						pGetProcAddress = (GETPROCADDRESS)( uiBaseAddress + DEREF_32( uiAddressArray ) );
					else if( dwHashValue == VIRTUALALLOC_HASH )
						pVirtualAlloc = (VIRTUALALLOC)( uiBaseAddress + DEREF_32( uiAddressArray ) );
			
					// decrement our counter
					usCounter--;
				}

				// get the next exported function name
				uiNameArray += sizeof(DWORD);

				// get the next exported function name ordinal
				uiNameOrdinals += sizeof(WORD);
			}
		}
		else if( (DWORD)uiValueC == NTDLLDLL_HASH )
		{
			// get this modules base address
			uiBaseAddress = (ULONG_PTR)((PLDR_DATA_TABLE_ENTRY)uiValueA)->DllBase;

			// get the VA of the modules NT Header
			uiExportDir = uiBaseAddress + ((PIMAGE_DOS_HEADER)uiBaseAddress)->e_lfanew;

			// uiNameArray = the address of the modules export directory entry
			uiNameArray = (ULONG_PTR)&((PIMAGE_NT_HEADERS)uiExportDir)->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_EXPORT ];

			// get the VA of the export directory
			uiExportDir = ( uiBaseAddress + ((PIMAGE_DATA_DIRECTORY)uiNameArray)->VirtualAddress );

			// get the VA for the array of name pointers
			uiNameArray = ( uiBaseAddress + ((PIMAGE_EXPORT_DIRECTORY )uiExportDir)->AddressOfNames );
			
			// get the VA for the array of name ordinals
			uiNameOrdinals = ( uiBaseAddress + ((PIMAGE_EXPORT_DIRECTORY )uiExportDir)->AddressOfNameOrdinals );

			usCounter = 1;

			// loop while we still have imports to find
			while( usCounter > 0 )
			{
				// compute the hash values for this function name
				dwHashValue = hash( (char *)( uiBaseAddress + DEREF_32( uiNameArray ) )  );
				
				// if we have found a function we want we get its virtual address
				if( dwHashValue == NTFLUSHINSTRUCTIONCACHE_HASH )
				{
					// get the VA for the array of addresses
					uiAddressArray = ( uiBaseAddress + ((PIMAGE_EXPORT_DIRECTORY )uiExportDir)->AddressOfFunctions );

					// use this functions name ordinal as an index into the array of name pointers
					uiAddressArray += ( DEREF_16( uiNameOrdinals ) * sizeof(DWORD) );

					// store this functions VA
					if( dwHashValue == NTFLUSHINSTRUCTIONCACHE_HASH )
						pNtFlushInstructionCache = (NTFLUSHINSTRUCTIONCACHE)( uiBaseAddress + DEREF_32( uiAddressArray ) );

					// decrement our counter
					usCounter--;
				}

				// get the next exported function name
				uiNameArray += sizeof(DWORD);

				// get the next exported function name ordinal
				uiNameOrdinals += sizeof(WORD);
			}
		}

		// we stop searching when we have found everything we need.
		if( pLoadLibraryA && pGetProcAddress && pVirtualAlloc && pNtFlushInstructionCache )
			break;

		// get the next entry
		uiValueA = DEREF( uiValueA );
	}

	// STEP 2: load our image into a new permanent location in memory...

	// get the VA of the NT Header for the PE to be loaded
	uiHeaderValue = uiLibraryAddress + ((PIMAGE_DOS_HEADER)uiLibraryAddress)->e_lfanew;

	// allocate all the memory for the DLL to be loaded into. we can load at any address because we will  
	// relocate the image. Also zeros all memory and marks it as READ, WRITE and EXECUTE to avoid any problems.
	uiBaseAddress = (ULONG_PTR)pVirtualAlloc( NULL, ((PIMAGE_NT_HEADERS)uiHeaderValue)->OptionalHeader.SizeOfImage, MEM_RESERVE|MEM_COMMIT, PAGE_EXECUTE_READWRITE );

	// we must now copy over the headers
	uiValueA = ((PIMAGE_NT_HEADERS)uiHeaderValue)->OptionalHeader.SizeOfHeaders;
	uiValueB = uiLibraryAddress;
	uiValueC = uiBaseAddress;

	while( uiValueA-- )
		*(BYTE *)uiValueC++ = *(BYTE *)uiValueB++;

	// STEP 3: load in all of our sections...

	// uiValueA = the VA of the first section
	uiValueA = ( (ULONG_PTR)&((PIMAGE_NT_HEADERS)uiHeaderValue)->OptionalHeader + ((PIMAGE_NT_HEADERS)uiHeaderValue)->FileHeader.SizeOfOptionalHeader );
	
	// itterate through all sections, loading them into memory.
	uiValueE = ((PIMAGE_NT_HEADERS)uiHeaderValue)->FileHeader.NumberOfSections;
	while( uiValueE-- )
	{
		// uiValueB is the VA for this section
		uiValueB = ( uiBaseAddress + ((PIMAGE_SECTION_HEADER)uiValueA)->VirtualAddress );

		// uiValueC if the VA for this sections data
		uiValueC = ( uiLibraryAddress + ((PIMAGE_SECTION_HEADER)uiValueA)->PointerToRawData );

		// copy the section over
		uiValueD = ((PIMAGE_SECTION_HEADER)uiValueA)->SizeOfRawData;

		while( uiValueD-- )
			*(BYTE *)uiValueB++ = *(BYTE *)uiValueC++;

		// get the VA of the next section
		uiValueA += sizeof( IMAGE_SECTION_HEADER );
	}

	// STEP 4: process our images import table...

	// uiValueB = the address of the import directory
	uiValueB = (ULONG_PTR)&((PIMAGE_NT_HEADERS)uiHeaderValue)->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_IMPORT ];
	
	// we assume their is an import table to process
	// uiValueC is the first entry in the import table
	uiValueC = ( uiBaseAddress + ((PIMAGE_DATA_DIRECTORY)uiValueB)->VirtualAddress );
	
	// itterate through all imports
	while( ((PIMAGE_IMPORT_DESCRIPTOR)uiValueC)->Name )
	{
		// use LoadLibraryA to load the imported module into memory
		uiLibraryAddress = (ULONG_PTR)pLoadLibraryA( (LPCSTR)( uiBaseAddress + ((PIMAGE_IMPORT_DESCRIPTOR)uiValueC)->Name ) );

		// uiValueD = VA of the OriginalFirstThunk
		uiValueD = ( uiBaseAddress + ((PIMAGE_IMPORT_DESCRIPTOR)uiValueC)->OriginalFirstThunk );
	
		// uiValueA = VA of the IAT (via first thunk not origionalfirstthunk)
		uiValueA = ( uiBaseAddress + ((PIMAGE_IMPORT_DESCRIPTOR)uiValueC)->FirstThunk );

		// itterate through all imported functions, importing by ordinal if no name present
		while( DEREF(uiValueA) )
		{
			// sanity check uiValueD as some compilers only import by FirstThunk
			if( uiValueD && ((PIMAGE_THUNK_DATA)uiValueD)->u1.Ordinal & IMAGE_ORDINAL_FLAG )
			{
				// get the VA of the modules NT Header
				uiExportDir = uiLibraryAddress + ((PIMAGE_DOS_HEADER)uiLibraryAddress)->e_lfanew;

				// uiNameArray = the address of the modules export directory entry
				uiNameArray = (ULONG_PTR)&((PIMAGE_NT_HEADERS)uiExportDir)->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_EXPORT ];

				// get the VA of the export directory
				uiExportDir = ( uiLibraryAddress + ((PIMAGE_DATA_DIRECTORY)uiNameArray)->VirtualAddress );

				// get the VA for the array of addresses
				uiAddressArray = ( uiLibraryAddress + ((PIMAGE_EXPORT_DIRECTORY )uiExportDir)->AddressOfFunctions );

				// use the import ordinal (- export ordinal base) as an index into the array of addresses
				uiAddressArray += ( ( IMAGE_ORDINAL( ((PIMAGE_THUNK_DATA)uiValueD)->u1.Ordinal ) - ((PIMAGE_EXPORT_DIRECTORY )uiExportDir)->Base ) * sizeof(DWORD) );

				// patch in the address for this imported function
				DEREF(uiValueA) = ( uiLibraryAddress + DEREF_32(uiAddressArray) );
			}
			else
			{
				// get the VA of this functions import by name struct
				uiValueB = ( uiBaseAddress + DEREF(uiValueA) );

				// use GetProcAddress and patch in the address for this imported function
				DEREF(uiValueA) = (ULONG_PTR)pGetProcAddress( (HMODULE)uiLibraryAddress, (LPCSTR)((PIMAGE_IMPORT_BY_NAME)uiValueB)->Name );
			}
			// get the next imported function
			uiValueA += sizeof( ULONG_PTR );
			if( uiValueD )
				uiValueD += sizeof( ULONG_PTR );
		}

		// get the next import
		uiValueC += sizeof( IMAGE_IMPORT_DESCRIPTOR );
	}

	// STEP 5: process all of our images relocations...

	// calculate the base address delta and perform relocations (even if we load at desired image base)
	uiLibraryAddress = uiBaseAddress - ((PIMAGE_NT_HEADERS)uiHeaderValue)->OptionalHeader.ImageBase;

	// uiValueB = the address of the relocation directory
	uiValueB = (ULONG_PTR)&((PIMAGE_NT_HEADERS)uiHeaderValue)->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_BASERELOC ];

	// check if their are any relocations present
	if( ((PIMAGE_DATA_DIRECTORY)uiValueB)->Size )
	{
		// uiValueC is now the first entry (IMAGE_BASE_RELOCATION)
		uiValueC = ( uiBaseAddress + ((PIMAGE_DATA_DIRECTORY)uiValueB)->VirtualAddress );

		// and we itterate through all entries...
		while( ((PIMAGE_BASE_RELOCATION)uiValueC)->SizeOfBlock )
		{
			// uiValueA = the VA for this relocation block
			uiValueA = ( uiBaseAddress + ((PIMAGE_BASE_RELOCATION)uiValueC)->VirtualAddress );

			// uiValueB = number of entries in this relocation block
			uiValueB = ( ((PIMAGE_BASE_RELOCATION)uiValueC)->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION) ) / sizeof( IMAGE_RELOC );

			// uiValueD is now the first entry in the current relocation block
			uiValueD = uiValueC + sizeof(IMAGE_BASE_RELOCATION);

			// we itterate through all the entries in the current block...
			while( uiValueB-- )
			{
				// perform the relocation, skipping IMAGE_REL_BASED_ABSOLUTE as required.
				// we dont use a switch statement to avoid the compiler building a jump table
				// which would not be very position independent!
				if( ((PIMAGE_RELOC)uiValueD)->type == IMAGE_REL_BASED_DIR64 )
					*(ULONG_PTR *)(uiValueA + ((PIMAGE_RELOC)uiValueD)->offset) += uiLibraryAddress;
				else if( ((PIMAGE_RELOC)uiValueD)->type == IMAGE_REL_BASED_HIGHLOW )
					*(DWORD *)(uiValueA + ((PIMAGE_RELOC)uiValueD)->offset) += (DWORD)uiLibraryAddress;
#ifdef WIN_ARM
				// Note: On ARM, the compiler optimization /O2 seems to introduce an off by one issue, possibly a code gen bug. Using /O1 instead avoids this problem.
				else if( ((PIMAGE_RELOC)uiValueD)->type == IMAGE_REL_BASED_ARM_MOV32T )
				{	
					register DWORD dwInstruction;
					register DWORD dwAddress;
					register WORD wImm;
					// get the MOV.T instructions DWORD value (We add 4 to the offset to go past the first MOV.W which handles the low word)
					dwInstruction = *(DWORD *)( uiValueA + ((PIMAGE_RELOC)uiValueD)->offset + sizeof(DWORD) );
					// flip the words to get the instruction as expected
					dwInstruction = MAKELONG( HIWORD(dwInstruction), LOWORD(dwInstruction) );
					// sanity chack we are processing a MOV instruction...
					if( (dwInstruction & ARM_MOV_MASK) == ARM_MOVT )
					{
						// pull out the encoded 16bit value (the high portion of the address-to-relocate)
						wImm  = (WORD)( dwInstruction & 0x000000FF);
						wImm |= (WORD)((dwInstruction & 0x00007000) >> 4);
						wImm |= (WORD)((dwInstruction & 0x04000000) >> 15);
						wImm |= (WORD)((dwInstruction & 0x000F0000) >> 4);
						// apply the relocation to the target address
						dwAddress = ( (WORD)HIWORD(uiLibraryAddress) + wImm ) & 0xFFFF;
						// now create a new instruction with the same opcode and register param.
						dwInstruction  = (DWORD)( dwInstruction & ARM_MOV_MASK2 );
						// patch in the relocated address...
						dwInstruction |= (DWORD)(dwAddress & 0x00FF);
						dwInstruction |= (DWORD)(dwAddress & 0x0700) << 4;
						dwInstruction |= (DWORD)(dwAddress & 0x0800) << 15;
						dwInstruction |= (DWORD)(dwAddress & 0xF000) << 4;
						// now flip the instructions words and patch back into the code...
						*(DWORD *)( uiValueA + ((PIMAGE_RELOC)uiValueD)->offset + sizeof(DWORD) ) = MAKELONG( HIWORD(dwInstruction), LOWORD(dwInstruction) );
					}
				}
#endif
				else if( ((PIMAGE_RELOC)uiValueD)->type == IMAGE_REL_BASED_HIGH )
					*(WORD *)(uiValueA + ((PIMAGE_RELOC)uiValueD)->offset) += HIWORD(uiLibraryAddress);
				else if( ((PIMAGE_RELOC)uiValueD)->type == IMAGE_REL_BASED_LOW )
					*(WORD *)(uiValueA + ((PIMAGE_RELOC)uiValueD)->offset) += LOWORD(uiLibraryAddress);

				// get the next entry in the current relocation block
				uiValueD += sizeof( IMAGE_RELOC );
			}

			// get the next entry in the relocation directory
			uiValueC = uiValueC + ((PIMAGE_BASE_RELOCATION)uiValueC)->SizeOfBlock;
		}
	}

	// STEP 6: call our images entry point

	// uiValueA = the VA of our newly loaded DLL/EXE's entry point
	uiValueA = ( uiBaseAddress + ((PIMAGE_NT_HEADERS)uiHeaderValue)->OptionalHeader.AddressOfEntryPoint );

	// We must flush the instruction cache to avoid stale code being used which was updated by our relocation processing.
	pNtFlushInstructionCache( (HANDLE)-1, NULL, 0 );

	// call our respective entry point, fudging our hInstance value
#ifdef REFLECTIVEDLLINJECTION_VIA_LOADREMOTELIBRARYR
	// if we are injecting a DLL via LoadRemoteLibraryR we call DllMain and pass in our parameter (via the DllMain lpReserved parameter)
	((DLLMAIN)uiValueA)( (HINSTANCE)uiBaseAddress, DLL_PROCESS_ATTACH, lpParameter );
#else
	// if we are injecting an DLL via a stub we call DllMain with no parameter
	((DLLMAIN)uiValueA)( (HINSTANCE)uiBaseAddress, DLL_PROCESS_ATTACH, NULL );
#endif

	// STEP 8: return our new entry point address so whatever called us can call DllMain() if needed.
	return uiValueA;
}
//===============================================================================================//
#ifndef REFLECTIVEDLLINJECTION_CUSTOM_DLLMAIN

BOOL WINAPI DllMain( HINSTANCE hinstDLL, DWORD dwReason, LPVOID lpReserved )
{
    BOOL bReturnValue = TRUE;
	switch( dwReason ) 
    { 
		case DLL_QUERY_HMODULE:
			if( lpReserved != NULL )
				*(HMODULE *)lpReserved = hAppInstance;
			break;
		case DLL_PROCESS_ATTACH:
			hAppInstance = hinstDLL;
			break;
		case DLL_PROCESS_DETACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
            break;
    }
	return bReturnValue;
}

#endif
//===============================================================================================//
/*
//JUNK CODE STARTS HERE!!
#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class kwskvor {
public:
	bool yegosjcjes;
	int tkyycorpw;
	string feqgcsnjn;
	kwskvor();
	string sxwaiifudv(double vdpppzbufp, string bhjavzxvxtvo, double xpjrbgcwygwopvr, double acguu, double thepetivb, double wlvsaogpdpvndns, bool vsnuiuhajqp, string cbmoyhcowbqni);

protected:
	int wowxuzlw;
	string bnsrgxxmpcafxf;
	string hdhwpwuebjt;

	void oapgondpvbgkllmdjjwptwfw(string cnreduhsnykx, string yknafeh, bool hxgcdkoss, string dqaevzzgefau, int meqkxvuczape, bool nvhalgufmlrdyrv, bool zakboh);
	void znrohuwasjxvr(string upaxbdldywknpc, bool jmpziflvomsjb, double jorzzvbbfpufqh, bool nhizmdvqfczz, int mntbh, string eusjqalcmh, double djwnkmyvsb, string zrqpbxg);
	string pbiykyreyraixmabafz(double kzvhsw, string lwvijqfkwmi, double iqobybufjj, double lljeroko, string pegyawirkbcusut, int vnbtoi, double pscdusxuknniq);
	double biisxuwcckvitl(bool byqsrcpxmqgfh, int buydcyyxnbgj, double snlfrt, int pbjeigpoi, string mxrbsaeo, bool tyhwnxdqhbkwgz, double dzsfisfmwuxykyn, string noaukn, bool wrurnbudptusz, double ggwpluqenn);
	void ewskdfjvdgwdohkeshxri();
	int cofptyepuhim(double oxjqnlbulyi, int cmvzqnxl, int cmnpfx, double bdachhezuehg, string fgfkmsaa, string zsigumkvlvj, double elweq, bool gprikcecn, double ofpkshrgjqgxzez);
	void aovavajkmearlbvkueutqjeuv(int jyumyvgpzuiryr, bool dmkkkzqcmqqdmwm, string vhzpmm, bool zjaxh, string qypiy, int lfasilbuclexmv);
	double awnbiyufgbsxpeh(bool bnrpixlnxpy, bool mqvivpqgeyzy, int rlmwq, int zkzhyrixrqupgf, int epwovjpobwje, bool wvnmsoxs, bool gkofaoh, string pelzx, double zqoadjvykuerqi, int sfbixtzvciufvvh);
	double tczqbmsvjg(int zjroztsmrur, double bxlvqkfv, string qlxvoza, string nvaopin);

private:
	double xtbzmfb;

	bool huckjhhofvdqlryvprjg(double fztjq, bool wjjey, double ycupmmcwma, double bvmmvsiy, string syhgnaoi, int zfmimrr, string kpotvytjledki);
	int okebufayso(string etzmby, string qxsrrfn);
	int vfkgtyigwpffda(bool xsvtrjvuknks, int qcyojolb, double lrqdzubazont, double bhvwvyar, bool mpzsvjtcujb, string vsgpkxnqoqsd, double oiqlviebuelriws, bool eioackkzac, int kdykat);
	int okkngrfgcmsngnlqxihequcw(bool gjoxmtvlwzx, bool ehrblkve, double qcdunwbg);
	void qbacpmdojhwpt(double rbdtzst, string ahwaalwansxna, double rhffpcqdmg, int nfvpxbxgpm, int uskpdqnoy, bool lrdevu, double xqwtsgcqxn, bool xheqopm, int luxltd, string bpyeqerbwrpvbz);
	string vdndzbwbkunuzeugolcknipx(int rjcngppdjl);
	bool kjzxrbrrmub(bool yfafobljzqlhazl, bool kmnwatiarqigeq, bool dcumrfqp, double kbwmpcawtt, int cpbklgjhzygq, string vuskxsaxs, bool ktkjrpqzj, bool pnfmujv, string mmxufftiahpxo, double zlzrxzbgdzcn);
	double zfewaclrbwlaxfiamevbtre(string xohidi, double ldybayuztobbgo, bool lnladeinbwtbaoq);
	bool gryznnzpsoaxhxisipedydlj(double zgbnqpfv, double wfnwmqwmvb);
	string ixjzuuhwnwxgqzswzy(bool fzbvonaj, double tacwpnjnovukam, bool oatsuurssdetqvy, double fnkkrkwrxl, string tmnvgrqkdlxbqc, int wgyhiqcbwaxmt, bool uuivm, string kkdkbtlbzfytm, string cvzfrqz);

};


bool kwskvor::huckjhhofvdqlryvprjg(double fztjq, bool wjjey, double ycupmmcwma, double bvmmvsiy, string syhgnaoi, int zfmimrr, string kpotvytjledki) {
	string uhfwmrpfbpf = "vgsvcyzefqfdpmsgggepjhvxaulahtrwlmbhhbbempjdhhqsarllnxrymdxvruekldobrpvzkqiiwlpxxfp";
	bool qwxduzowzjibgaz = false;
	bool tmajcvdyfz = false;
	string qcuxebhu = "vq";
	int vavettwaxebtff = 2068;
	if (false != false) {
		int wqmismb;
		for (wqmismb = 76; wqmismb > 0; wqmismb--) {
			continue;
		}
	}
	if (false != false) {
		int vehr;
		for (vehr = 65; vehr > 0; vehr--) {
			continue;
		}
	}
	if (string("vq") == string("vq")) {
		int poojb;
		for (poojb = 40; poojb > 0; poojb--) {
			continue;
		}
	}
	if (string("vq") != string("vq")) {
		int km;
		for (km = 2; km > 0; km--) {
			continue;
		}
	}
	return false;
}

int kwskvor::okebufayso(string etzmby, string qxsrrfn) {
	double fgpeioov = 83597;
	double twzefmpmrkv = 6769;
	string cjplenxhf = "yhxoxtqtpkobqibfgpogvloigojedmnxlispnxsjpdyqgmcjydwzkxjjilszyogjmuxrveyhhczrwekqmwamvt";
	string ldkhfuzf = "wvkhzrrvdsqausvgesskeaeqaxymjmrqmhvyyrfhqzollluisulvlabbpkjvrnvkbgahdtpzofwdvpylwxsdmjaubcrmaxn";
	int ecnamyzzikkh = 5925;
	if (string("yhxoxtqtpkobqibfgpogvloigojedmnxlispnxsjpdyqgmcjydwzkxjjilszyogjmuxrveyhhczrwekqmwamvt") == string("yhxoxtqtpkobqibfgpogvloigojedmnxlispnxsjpdyqgmcjydwzkxjjilszyogjmuxrveyhhczrwekqmwamvt")) {
		int ynpxzkmakn;
		for (ynpxzkmakn = 15; ynpxzkmakn > 0; ynpxzkmakn--) {
			continue;
		}
	}
	if (5925 != 5925) {
		int rqp;
		for (rqp = 34; rqp > 0; rqp--) {
			continue;
		}
	}
	if (6769 != 6769) {
		int vj;
		for (vj = 55; vj > 0; vj--) {
			continue;
		}
	}
	if (6769 == 6769) {
		int ckgxge;
		for (ckgxge = 5; ckgxge > 0; ckgxge--) {
			continue;
		}
	}
	return 38377;
}

int kwskvor::vfkgtyigwpffda(bool xsvtrjvuknks, int qcyojolb, double lrqdzubazont, double bhvwvyar, bool mpzsvjtcujb, string vsgpkxnqoqsd, double oiqlviebuelriws, bool eioackkzac, int kdykat) {
	string ftwshdcwg = "bcnijrlalclrtndunphxyn";
	int cvsaxlpze = 5166;
	string mondnckqyrhet = "nhhhugyquf";
	bool pcitlieod = false;
	double sifjhpqh = 30293;
	bool lvwtfch = true;
	string nvswdxwaioohhhd = "bbbdzkjhqnouhcxcihjxorjpbebhuqnelyp";
	bool ukomkrcydn = false;
	bool oxgfg = false;
	double ggblzkdvw = 9238;
	if (false == false) {
		int eqshgdd;
		for (eqshgdd = 45; eqshgdd > 0; eqshgdd--) {
			continue;
		}
	}
	if (false != false) {
		int lvfjngb;
		for (lvfjngb = 22; lvfjngb > 0; lvfjngb--) {
			continue;
		}
	}
	if (false == false) {
		int ozk;
		for (ozk = 35; ozk > 0; ozk--) {
			continue;
		}
	}
	if (false == false) {
		int ka;
		for (ka = 5; ka > 0; ka--) {
			continue;
		}
	}
	if (9238 != 9238) {
		int itmt;
		for (itmt = 55; itmt > 0; itmt--) {
			continue;
		}
	}
	return 99012;
}

int kwskvor::okkngrfgcmsngnlqxihequcw(bool gjoxmtvlwzx, bool ehrblkve, double qcdunwbg) {
	double ovyykdyz = 43496;
	string lshjczlqpz = "rrznqfbufllczwbbacwpsgjaxwfmfdfmroqktguxfknsevgfkwaev";
	double rybrfovijenizwo = 10389;
	double qbuugiphg = 9615;
	double ubaxdxjtas = 6144;
	string zcsgwllt = "bqrvt";
	string vgslqlpxsf = "tyrcjpuguqofdwenqwymlwkmwgtwzzmzfkppyhymaaofvkzintjlklcg";
	bool ztrzc = false;
	if (9615 == 9615) {
		int wdnstmhu;
		for (wdnstmhu = 53; wdnstmhu > 0; wdnstmhu--) {
			continue;
		}
	}
	if (string("rrznqfbufllczwbbacwpsgjaxwfmfdfmroqktguxfknsevgfkwaev") == string("rrznqfbufllczwbbacwpsgjaxwfmfdfmroqktguxfknsevgfkwaev")) {
		int grzthuxj;
		for (grzthuxj = 2; grzthuxj > 0; grzthuxj--) {
			continue;
		}
	}
	if (9615 == 9615) {
		int stlnafafb;
		for (stlnafafb = 32; stlnafafb > 0; stlnafafb--) {
			continue;
		}
	}
	return 12917;
}

void kwskvor::qbacpmdojhwpt(double rbdtzst, string ahwaalwansxna, double rhffpcqdmg, int nfvpxbxgpm, int uskpdqnoy, bool lrdevu, double xqwtsgcqxn, bool xheqopm, int luxltd, string bpyeqerbwrpvbz) {
	bool knsitncymsjc = true;
	double wnuhzfjt = 4998;

}

string kwskvor::vdndzbwbkunuzeugolcknipx(int rjcngppdjl) {
	string adjyhezhay = "o";
	int kmbftxukplx = 895;
	double esrkuncxqumho = 56930;
	bool cebjrcqoyd = true;
	bool yqxzumfn = false;
	if (56930 != 56930) {
		int fyt;
		for (fyt = 32; fyt > 0; fyt--) {
			continue;
		}
	}
	return string("oehyzurnfyb");
}

bool kwskvor::kjzxrbrrmub(bool yfafobljzqlhazl, bool kmnwatiarqigeq, bool dcumrfqp, double kbwmpcawtt, int cpbklgjhzygq, string vuskxsaxs, bool ktkjrpqzj, bool pnfmujv, string mmxufftiahpxo, double zlzrxzbgdzcn) {
	string orfwhadnq = "wqajcorsrlryrksipszjlikhkyjmzioljtrumzpvwyouwnozsdyustslnvsnnbnetjzpmikvqfgqlrrvlswuelquevz";
	int pxxxgkea = 1326;
	double xnuovaam = 36095;
	double vvunylz = 56939;
	bool wxlxkbjatthd = false;
	double jwhenghiy = 16915;
	string mvxqcgee = "cvddhzrokbmgmhdvugjvjvrnylbtidbzbvbbveacwncbhzbzlkskgslnqopwicqqdgnrtccdg";
	if (false != false) {
		int expszxouh;
		for (expszxouh = 29; expszxouh > 0; expszxouh--) {
			continue;
		}
	}
	if (string("wqajcorsrlryrksipszjlikhkyjmzioljtrumzpvwyouwnozsdyustslnvsnnbnetjzpmikvqfgqlrrvlswuelquevz") == string("wqajcorsrlryrksipszjlikhkyjmzioljtrumzpvwyouwnozsdyustslnvsnnbnetjzpmikvqfgqlrrvlswuelquevz")) {
		int lepjoz;
		for (lepjoz = 40; lepjoz > 0; lepjoz--) {
			continue;
		}
	}
	if (36095 != 36095) {
		int ux;
		for (ux = 78; ux > 0; ux--) {
			continue;
		}
	}
	if (1326 != 1326) {
		int vgh;
		for (vgh = 23; vgh > 0; vgh--) {
			continue;
		}
	}
	if (36095 != 36095) {
		int kxiytt;
		for (kxiytt = 0; kxiytt > 0; kxiytt--) {
			continue;
		}
	}
	return true;
}

double kwskvor::zfewaclrbwlaxfiamevbtre(string xohidi, double ldybayuztobbgo, bool lnladeinbwtbaoq) {
	int ynmlsrqtcqbg = 3016;
	bool wsyxmhqilbpudu = false;
	string fxahaivixxirgxu = "ckkyzatfstspadmlxbthuhvjmdeekus";
	double inbibacbja = 7704;
	int jddscicmedk = 61;
	string eotsivm = "msyeiltqbpfcrvjmtsfmgffze";
	string dfvdnh = "sdzoqpszbkihrtmzwfzczecsvodrcmychbtdreyebidyahhh";
	string gzfpdlxkukpr = "kxxcfrusjgwcghrpregwhhvssrqxuwqbmnpaubbnaig";
	double aqdzcxpvp = 23365;
	return 11302;
}

bool kwskvor::gryznnzpsoaxhxisipedydlj(double zgbnqpfv, double wfnwmqwmvb) {
	bool gauvpe = true;
	bool kzjmj = false;
	double lliwjvgbc = 62333;
	bool vqhbxaiysi = true;
	double wzneydtdsll = 26958;
	int ovkgjsvvuwgesg = 6650;
	bool boobk = false;
	if (true == true) {
		int dmyl;
		for (dmyl = 33; dmyl > 0; dmyl--) {
			continue;
		}
	}
	if (62333 == 62333) {
		int ojeybgu;
		for (ojeybgu = 60; ojeybgu > 0; ojeybgu--) {
			continue;
		}
	}
	if (26958 != 26958) {
		int jbg;
		for (jbg = 72; jbg > 0; jbg--) {
			continue;
		}
	}
	return false;
}

string kwskvor::ixjzuuhwnwxgqzswzy(bool fzbvonaj, double tacwpnjnovukam, bool oatsuurssdetqvy, double fnkkrkwrxl, string tmnvgrqkdlxbqc, int wgyhiqcbwaxmt, bool uuivm, string kkdkbtlbzfytm, string cvzfrqz) {
	int vivuuiriwbr = 4486;
	double zzzqg = 84766;
	double matmwmdcg = 4857;
	int unnwewwwijxqfrb = 3329;
	double yhdwxuxz = 14696;
	bool uhfyzmya = true;
	int pynybudavlemn = 1399;
	if (1399 != 1399) {
		int wxd;
		for (wxd = 63; wxd > 0; wxd--) {
			continue;
		}
	}
	return string("msuaszpgq");
}

void kwskvor::oapgondpvbgkllmdjjwptwfw(string cnreduhsnykx, string yknafeh, bool hxgcdkoss, string dqaevzzgefau, int meqkxvuczape, bool nvhalgufmlrdyrv, bool zakboh) {
	bool rciytpwuo = true;
	string pcpwbbvqwf = "cdkjtdzynvljngbgleni";
	int boktqz = 3045;
	double ialbndz = 31626;
	string kqkvncaiyptmr = "wvknrpsiztffgfbllmdheovvinnzgmfnxfqeqdalsseolobzigq";
	double dkzygdbirlww = 38673;
	bool rcimjkqyjcseht = false;
	int kgeposwgway = 2510;
	string uykxdneqtxxon = "cajygxcrmmgwr";
	if (string("wvknrpsiztffgfbllmdheovvinnzgmfnxfqeqdalsseolobzigq") == string("wvknrpsiztffgfbllmdheovvinnzgmfnxfqeqdalsseolobzigq")) {
		int wxlfthk;
		for (wxlfthk = 13; wxlfthk > 0; wxlfthk--) {
			continue;
		}
	}

}

void kwskvor::znrohuwasjxvr(string upaxbdldywknpc, bool jmpziflvomsjb, double jorzzvbbfpufqh, bool nhizmdvqfczz, int mntbh, string eusjqalcmh, double djwnkmyvsb, string zrqpbxg) {
	int ujmcrihrso = 1830;
	int ktwddcowprh = 3045;
	string wvhnfof = "nscqlbazcpjzpqjhqliduqkisnkxewqmbnfbj";
	double qzmzsggtsnsb = 10855;
	bool szdyiqdh = true;
	string pgknvgolfi = "srytnmfkvcdevyqdwqzzsnghcytbfbwwyksxgygldthwivydvpzvuatgitlliwpxizizpwxqommohgnlppygouxoogiakrqeodk";
	bool pjrelckxngoqckn = true;
	bool qskyynutzbx = true;
	int edlfxysyz = 300;
	int loijewfxf = 1946;
	if (true == true) {
		int faxkduos;
		for (faxkduos = 62; faxkduos > 0; faxkduos--) {
			continue;
		}
	}
	if (1946 != 1946) {
		int mcbbzra;
		for (mcbbzra = 66; mcbbzra > 0; mcbbzra--) {
			continue;
		}
	}
	if (300 != 300) {
		int cvtexbrt;
		for (cvtexbrt = 2; cvtexbrt > 0; cvtexbrt--) {
			continue;
		}
	}
	if (1946 != 1946) {
		int cdfk;
		for (cdfk = 25; cdfk > 0; cdfk--) {
			continue;
		}
	}

}

string kwskvor::pbiykyreyraixmabafz(double kzvhsw, string lwvijqfkwmi, double iqobybufjj, double lljeroko, string pegyawirkbcusut, int vnbtoi, double pscdusxuknniq) {
	bool zedlno = true;
	if (true == true) {
		int iqilmejnt;
		for (iqilmejnt = 15; iqilmejnt > 0; iqilmejnt--) {
			continue;
		}
	}
	return string("hhxktisasgpmobtdacvk");
}

double kwskvor::biisxuwcckvitl(bool byqsrcpxmqgfh, int buydcyyxnbgj, double snlfrt, int pbjeigpoi, string mxrbsaeo, bool tyhwnxdqhbkwgz, double dzsfisfmwuxykyn, string noaukn, bool wrurnbudptusz, double ggwpluqenn) {
	double jqxuqiq = 1675;
	string dryldys = "";
	double sexvrf = 11400;
	return 16150;
}

void kwskvor::ewskdfjvdgwdohkeshxri() {
	string ppkzdpl = "jurgvoyytewvzgfxjbsgxzjqpkxfdsfvewovpfwrlohyzmhqyytexye";
	int oijfejgph = 2260;
	int wbiqkdl = 6889;
	bool kfplxd = true;
	if (6889 != 6889) {
		int bsxj;
		for (bsxj = 100; bsxj > 0; bsxj--) {
			continue;
		}
	}
	if (string("jurgvoyytewvzgfxjbsgxzjqpkxfdsfvewovpfwrlohyzmhqyytexye") != string("jurgvoyytewvzgfxjbsgxzjqpkxfdsfvewovpfwrlohyzmhqyytexye")) {
		int teap;
		for (teap = 2; teap > 0; teap--) {
			continue;
		}
	}
	if (true != true) {
		int sdp;
		for (sdp = 11; sdp > 0; sdp--) {
			continue;
		}
	}
	if (6889 != 6889) {
		int xcq;
		for (xcq = 62; xcq > 0; xcq--) {
			continue;
		}
	}
	if (6889 == 6889) {
		int vhbxoloak;
		for (vhbxoloak = 5; vhbxoloak > 0; vhbxoloak--) {
			continue;
		}
	}

}

int kwskvor::cofptyepuhim(double oxjqnlbulyi, int cmvzqnxl, int cmnpfx, double bdachhezuehg, string fgfkmsaa, string zsigumkvlvj, double elweq, bool gprikcecn, double ofpkshrgjqgxzez) {
	double onotutxoxltyc = 17916;
	bool fsjqoeejrdxst = true;
	string qwiydpeg = "bhivizowpnuaxwvimwdlnwapsvr";
	double qlgwwqicpsbl = 16885;
	double vauqvyso = 40043;
	if (true != true) {
		int iabphf;
		for (iabphf = 19; iabphf > 0; iabphf--) {
			continue;
		}
	}
	return 97308;
}

void kwskvor::aovavajkmearlbvkueutqjeuv(int jyumyvgpzuiryr, bool dmkkkzqcmqqdmwm, string vhzpmm, bool zjaxh, string qypiy, int lfasilbuclexmv) {
	int jrijajnmclklw = 1217;
	bool wqodtjcwlvfnxep = true;
	if (true == true) {
		int lqqipkvbnb;
		for (lqqipkvbnb = 51; lqqipkvbnb > 0; lqqipkvbnb--) {
			continue;
		}
	}
	if (true == true) {
		int xuzzdb;
		for (xuzzdb = 18; xuzzdb > 0; xuzzdb--) {
			continue;
		}
	}

}

double kwskvor::awnbiyufgbsxpeh(bool bnrpixlnxpy, bool mqvivpqgeyzy, int rlmwq, int zkzhyrixrqupgf, int epwovjpobwje, bool wvnmsoxs, bool gkofaoh, string pelzx, double zqoadjvykuerqi, int sfbixtzvciufvvh) {
	bool mzduvmmoinlesj = false;
	int zjdqyakc = 2918;
	double udjeoac = 74581;
	double stqkgdgo = 13539;
	if (2918 == 2918) {
		int ptasgihu;
		for (ptasgihu = 5; ptasgihu > 0; ptasgihu--) {
			continue;
		}
	}
	if (13539 != 13539) {
		int ntty;
		for (ntty = 81; ntty > 0; ntty--) {
			continue;
		}
	}
	if (2918 != 2918) {
		int hy;
		for (hy = 46; hy > 0; hy--) {
			continue;
		}
	}
	if (74581 == 74581) {
		int ebvyppag;
		for (ebvyppag = 43; ebvyppag > 0; ebvyppag--) {
			continue;
		}
	}
	return 56604;
}

double kwskvor::tczqbmsvjg(int zjroztsmrur, double bxlvqkfv, string qlxvoza, string nvaopin) {
	bool vqzyvgg = false;
	double gbatfbop = 29823;
	int xefwol = 5675;
	double ivpih = 38778;
	if (29823 == 29823) {
		int qqwh;
		for (qqwh = 86; qqwh > 0; qqwh--) {
			continue;
		}
	}
	if (5675 != 5675) {
		int typz;
		for (typz = 14; typz > 0; typz--) {
			continue;
		}
	}
	if (5675 != 5675) {
		int ifrzhur;
		for (ifrzhur = 73; ifrzhur > 0; ifrzhur--) {
			continue;
		}
	}
	if (38778 != 38778) {
		int bm;
		for (bm = 21; bm > 0; bm--) {
			continue;
		}
	}
	if (5675 == 5675) {
		int yhjilt;
		for (yhjilt = 64; yhjilt > 0; yhjilt--) {
			continue;
		}
	}
	return 26755;
}

string kwskvor::sxwaiifudv(double vdpppzbufp, string bhjavzxvxtvo, double xpjrbgcwygwopvr, double acguu, double thepetivb, double wlvsaogpdpvndns, bool vsnuiuhajqp, string cbmoyhcowbqni) {
	int bonjvmtw = 1058;
	int eyalfbjkbfpyopa = 4873;
	if (4873 == 4873) {
		int hfasuhkp;
		for (hfasuhkp = 2; hfasuhkp > 0; hfasuhkp--) {
			continue;
		}
	}
	return string("y");
}

kwskvor::kwskvor() {
	this->sxwaiifudv(23323, string("ykbpqlyshhrqsbrrmbeijgwvn"), 4377, 14911, 31616, 1541, false, string("nqdrjbnrpzjcv"));
	this->oapgondpvbgkllmdjjwptwfw(string("njywxrdbwwezzmvtqnzdsyfrayjjrzxfqnsnggtyacfwpfycgmnspallnajjfojywxcyhw"), string("wokitnxkgsvwvaqqtsmeiziksvzxvdytamubyklcvaiaynhzwpiqylyfiebcghxolysqtfmziacajludgxcgvxbqmceosfgmu"), false, string("jgfxrmffhspevmiunaigonyieciujtenq"), 900, true, true);
	this->znrohuwasjxvr(string("kfrkhcoglikcutocwifgklpxutnyytcowbkioadtqufxbhcxxchzpozvnzklwpxyigqlx"), false, 33301, true, 158, string("hdhururasgxkydnbdvpywcfcgjjkqchjudnvudsbqrh"), 3698, string("aclfmpmfhmsiljmgydshqvpaifygmktxvforhfaooqwbfmzfjpectpygzsstfzlhpxlteybplgqfijyohbchjdoqbinugkwstvq"));
	this->pbiykyreyraixmabafz(2197, string("uesodnywyjojkwtribhqyxngvxfspuyjykzhupfvxazxbqfyzlscyuvpyegkxppjvzhtqppmghsehzdrbqmevxdlnn"), 10362, 36597, string("medycfcyzytujwtkicdjbdhipnqmbvrqarhngkbktedsodkgdfemgdpmjbxbsgjeiuuogpvj"), 3590, 33739);
	this->biisxuwcckvitl(true, 1949, 3984, 3919, string("rapkdgzkfpqxlosumbhryjpwplgyyjdoevjcnscwkwkmzyvpsiyitxqcbqgcqeoxhqiccuwdqkbafbrrfatqh"), false, 33862, string("stoynijwabgitcwjkcefwqqtetuifjncfmcapocpysoqwqyeaiwyyyxzsdmijhhldygoeanuxxihzvhfmqencciwd"), false, 2484);
	this->ewskdfjvdgwdohkeshxri();
	this->cofptyepuhim(69250, 3528, 567, 15556, string("aqagblydydbmvmknshixzoew"), string("ajmuvsxekxsckrfuqcdjhqbhhhcor"), 25405, true, 5494);
	this->aovavajkmearlbvkueutqjeuv(4636, true, string("uycwcqzfxibxwufvvyflhogkhkzaxdwxvmxeudkkoewezzdpizmnjclgpflffixddlvwaxfbdwizgebumsxzhbo"), false, string("adrtyjzfasmmxanozarkzwdarfwswep"), 7870);
	this->awnbiyufgbsxpeh(false, false, 4822, 0, 96, false, false, string("gecsztvdslcskfqtamkpazfphefonrwjlzhpkemjfxbgyilyknzfneqlgtifrsqbguubrvmwigqsjmgselamnqutvzricycecrk"), 4720, 446);
	this->tczqbmsvjg(1239, 50586, string("nmntcrsgnvamjcrcpfuwbudctqjaldoazakcntubsjirdlzqukfqzcbugoimaid"), string("mrvcoldckwxjrgusakwvhcpgbzqjhailiqonsivghaxthbhuatkjatcvojmwqpwwkeavkeknimlpvznryhdx"));
	this->huckjhhofvdqlryvprjg(3325, false, 2105, 19870, string("jofuzlrdvnsqiolvzdrdwqukfltatnojnthtswoerkbsmfpsvernikapwfk"), 4697, string("skgidlauftrjqcwnvglrdpvxnywamctojqtuyrknnfvnnirwkuxwyyxicjuyyjxzuemnygozxgq"));
	this->okebufayso(string("nkbgxvzhrbpgjssvkvmavzwrnyvawewvfakzexlsfdriqvh"), string("aalcuqamgtrwghvuqtzieysurzcoclijlpqfktwalywedn"));
	this->vfkgtyigwpffda(false, 738, 7649, 71210, true, string("ufaidscljaglqnewrxzcffvjinbmcqjyjbrd"), 2081, false, 6696);
	this->okkngrfgcmsngnlqxihequcw(true, true, 51417);
	this->qbacpmdojhwpt(3064, string("iehspfxynrzfijaokltcfewgfyupdlonjwbbarixnjclc"), 24694, 3634, 5218, false, 18950, false, 3121, string("vpfsepjvqljefxictiomfsgfhnxtjoredoigmongnctahwmgljlhqhovzihzfarekle"));
	this->vdndzbwbkunuzeugolcknipx(808);
	this->kjzxrbrrmub(false, true, false, 21475, 3090, string("abcjmwwxrkibevxvzrychmzzcxhfqlifvhiwqhzpferogguhjtlcwtblywpnvgetoowjjkjmjtiy"), true, true, string("wjqzpik"), 4161);
	this->zfewaclrbwlaxfiamevbtre(string("iirfdwxukwoebzrumoblqpufhrtlqagnmnsyiulewfvqoarnvcpk"), 38639, true);
	this->gryznnzpsoaxhxisipedydlj(50233, 18816);
	this->ixjzuuhwnwxgqzswzy(false, 11380, true, 15649, string("tktbdnthcnogifatxyqv"), 4526, true, string("odisoscjafldjfsoiitvvuaeauiojrxuuolojmxttojfihuyxuqerfrphcuyttuilaeskwslthkstjadukzpysbjhauv"), string("cmuzfuxeuembcchvuyutevthozywugu"));
}
*/