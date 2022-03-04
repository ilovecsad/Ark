#pragma once
#include <ntifs.h>


typedef struct _MM_GRAPHICS_VAD_FLAGS        // 15 elements, 0x4 bytes (sizeof) 
{
	/*0x000*/     ULONG32      Lock : 1;                   // 0 BitPosition                   
	/*0x000*/     ULONG32      LockContended : 1;          // 1 BitPosition                   
	/*0x000*/     ULONG32      DeleteInProgress : 1;       // 2 BitPosition                   
	/*0x000*/     ULONG32      NoChange : 1;               // 3 BitPosition                   
	/*0x000*/     ULONG32      VadType : 3;                // 4 BitPosition                   
	/*0x000*/     ULONG32      Protection : 5;             // 7 BitPosition                   
	/*0x000*/     ULONG32      PreferredNode : 6;          // 12 BitPosition                  
	/*0x000*/     ULONG32      PageSize : 2;               // 18 BitPosition                  
	/*0x000*/     ULONG32      PrivateMemoryAlwaysSet : 1; // 20 BitPosition                  
	/*0x000*/     ULONG32      WriteWatch : 1;             // 21 BitPosition                  
	/*0x000*/     ULONG32      FixedLargePageSize : 1;     // 22 BitPosition                  
	/*0x000*/     ULONG32      ZeroFillPagesOptional : 1;  // 23 BitPosition                  
	/*0x000*/     ULONG32      GraphicsAlwaysSet : 1;      // 24 BitPosition                  
	/*0x000*/     ULONG32      GraphicsUseCoherentBus : 1; // 25 BitPosition                  
	/*0x000*/     ULONG32      GraphicsPageProtection : 3; // 26 BitPosition                  
}MM_GRAPHICS_VAD_FLAGS, *PMM_GRAPHICS_VAD_FLAGS;
typedef struct _MM_PRIVATE_VAD_FLAGS         // 15 elements, 0x4 bytes (sizeof) 
{
	/*0x000*/     ULONG32      Lock : 1;                   // 0 BitPosition                   
	/*0x000*/     ULONG32      LockContended : 1;          // 1 BitPosition                   
	/*0x000*/     ULONG32      DeleteInProgress : 1;       // 2 BitPosition                   
	/*0x000*/     ULONG32      NoChange : 1;               // 3 BitPosition                   
	/*0x000*/     ULONG32      VadType : 3;                // 4 BitPosition                   
	/*0x000*/     ULONG32      Protection : 5;             // 7 BitPosition                   
	/*0x000*/     ULONG32      PreferredNode : 6;          // 12 BitPosition                  
	/*0x000*/     ULONG32      PageSize : 2;               // 18 BitPosition                  
	/*0x000*/     ULONG32      PrivateMemoryAlwaysSet : 1; // 20 BitPosition                  
	/*0x000*/     ULONG32      WriteWatch : 1;             // 21 BitPosition                  
	/*0x000*/     ULONG32      FixedLargePageSize : 1;     // 22 BitPosition                  
	/*0x000*/     ULONG32      ZeroFillPagesOptional : 1;  // 23 BitPosition                  
	/*0x000*/     ULONG32      Graphics : 1;               // 24 BitPosition                  
	/*0x000*/     ULONG32      Enclave : 1;                // 25 BitPosition                  
	/*0x000*/     ULONG32      ShadowStack : 1;            // 26 BitPosition                  
}MM_PRIVATE_VAD_FLAGS, *PMM_PRIVATE_VAD_FLAGS;


typedef struct _MMVAD_FLAGS            // 9 elements, 0x4 bytes (sizeof) 
{
	/*0x000*/     ULONG32      Lock : 1;             // 0 BitPosition                  
	/*0x000*/     ULONG32      LockContended : 1;    // 1 BitPosition                  
	/*0x000*/     ULONG32      DeleteInProgress : 1; // 2 BitPosition                  
	/*0x000*/     ULONG32      NoChange : 1;         // 3 BitPosition                  
	/*0x000*/     ULONG32      VadType : 3;          // 4 BitPosition                  
	/*0x000*/     ULONG32      Protection : 5;       // 7 BitPosition                  
	/*0x000*/     ULONG32      PreferredNode : 6;    // 12 BitPosition                 
	/*0x000*/     ULONG32      PageSize : 2;         // 18 BitPosition                 
	/*0x000*/     ULONG32      PrivateMemory : 1;    // 20 BitPosition                 
}MMVAD_FLAGS, *PMMVAD_FLAGS;

typedef struct _MM_SHARED_VAD_FLAGS            // 11 elements, 0x4 bytes (sizeof) 
{
	/*0x000*/     ULONG32      Lock : 1;                     // 0 BitPosition                   
	/*0x000*/     ULONG32      LockContended : 1;            // 1 BitPosition                   
	/*0x000*/     ULONG32      DeleteInProgress : 1;         // 2 BitPosition                   
	/*0x000*/     ULONG32      NoChange : 1;                 // 3 BitPosition                   
	/*0x000*/     ULONG32      VadType : 3;                  // 4 BitPosition                   
	/*0x000*/     ULONG32      Protection : 5;               // 7 BitPosition                   
	/*0x000*/     ULONG32      PreferredNode : 6;            // 12 BitPosition                  
	/*0x000*/     ULONG32      PageSize : 2;                 // 18 BitPosition                  
	/*0x000*/     ULONG32      PrivateMemoryAlwaysClear : 1; // 20 BitPosition                  
	/*0x000*/     ULONG32      PrivateFixup : 1;             // 21 BitPosition                  
	/*0x000*/     ULONG32      HotPatchAllowed : 1;          // 22 BitPosition                  
}MM_SHARED_VAD_FLAGS, *PMM_SHARED_VAD_FLAGS;


typedef struct _MMVAD_FLAGS2             // 7 elements, 0x4 bytes (sizeof) 
{
	/*0x000*/     ULONG32      FileOffset : 24;        // 0 BitPosition                  
	/*0x000*/     ULONG32      Large : 1;              // 24 BitPosition                 
	/*0x000*/     ULONG32      TrimBehind : 1;         // 25 BitPosition                 
	/*0x000*/     ULONG32      Inherit : 1;            // 26 BitPosition                 
	/*0x000*/     ULONG32      NoValidationNeeded : 1; // 27 BitPosition                 
	/*0x000*/     ULONG32      PrivateDemandZero : 1;  // 28 BitPosition                 
	/*0x000*/     ULONG32      Spare : 3;              // 29 BitPosition                 
}MMVAD_FLAGS2, *PMMVAD_FLAGS2;


typedef struct _MMVAD_SHORT
{
	RTL_BALANCED_NODE VadNode;
 	UINT32 StartingVpn;               /*0x18*/
	UINT32 EndingVpn;                 /*0x01C*/ 
	UCHAR StartingVpnHigh;           
	UCHAR EndingVpnHigh;
	UCHAR CommitChargeHigh;
	UCHAR SpareNT64VadUChar;
	INT32 ReferenceCount;
	EX_PUSH_LOCK PushLock;            /*0x028*/ 
	struct 
	{
		union
		{
			ULONG_PTR flag;
			MM_PRIVATE_VAD_FLAGS PrivateVadFlags;                        /*0x030*/
			MMVAD_FLAGS  VadFlags;
			MM_GRAPHICS_VAD_FLAGS GraphicsVadFlags;
			MM_SHARED_VAD_FLAGS   SharedVadFlags;
		}Flags;
		
	}u1;
	
	PVOID EventList;                        /*0x038*/

}MMVAD_SHORT, *PMMVAD_SHORT;



typedef struct _MMADDRESS_NODE
{
	ULONG64 u1;
	struct _MMADDRESS_NODE* LeftChild;
	struct _MMADDRESS_NODE* RightChild;
	ULONG64 StartingVpn;
	ULONG64 EndingVpn;
}MMADDRESS_NODE, *PMMADDRESS_NODE;

typedef struct _MMEXTEND_INFO     // 2 elements, 0x10 bytes (sizeof) 
{
	/*0x000*/     UINT64       CommittedSize;
	/*0x008*/     ULONG32      ReferenceCount;
	/*0x00C*/     UINT8        _PADDING0_[0x4];
}MMEXTEND_INFO, *PMMEXTEND_INFO;
struct _SEGMENT
{
	struct _CONTROL_AREA* ControlArea;
	ULONG TotalNumberOfPtes;
	ULONG SegmentFlags;
	ULONG64 NumberOfCommittedPages;
	ULONG64 SizeOfSegment;
	union
	{
		struct _MMEXTEND_INFO* ExtendInfo;
		void* BasedAddress;
	}u;
	ULONG64 SegmentLock;
	ULONG64 u1;
	ULONG64 u2;
	PVOID* PrototypePte;
	ULONGLONG ThePtes[0x1];
};

typedef struct _EX_FAST_REF
{
	union
	{
		PVOID Object;
		ULONG_PTR RefCnt : 3;
		ULONG_PTR Value;
	};
} EX_FAST_REF, *PEX_FAST_REF;

typedef struct _CONTROL_AREA                      // 17 elements, 0x80 bytes (sizeof) 
{
	/*0x000*/     struct _SEGMENT* Segment;
	union                                         // 2 elements, 0x10 bytes (sizeof)  
	{
		/*0x008*/         struct _LIST_ENTRY ListHead;              // 2 elements, 0x10 bytes (sizeof)  
		/*0x008*/         VOID*        AweContext;
	};
	/*0x018*/     UINT64       NumberOfSectionReferences;
	/*0x020*/     UINT64       NumberOfPfnReferences;
	/*0x028*/     UINT64       NumberOfMappedViews;
	/*0x030*/     UINT64       NumberOfUserReferences;
	/*0x038*/     ULONG32 u;                     // 2 elements, 0x4 bytes (sizeof)   
	/*0x03C*/     ULONG32 u1;                    // 2 elements, 0x4 bytes (sizeof)   
	/*0x040*/     struct _EX_FAST_REF FilePointer;              // 3 elements, 0x8 bytes (sizeof)   
                  // 4 elements, 0x8 bytes (sizeof)   
}CONTROL_AREA, *PCONTROL_AREA;


typedef struct _SUBSECTION_
{
	struct _CONTROL_AREA* ControlArea;

}SUBSECTION,*PSUBSECTION;



typedef struct _MMVAD
{
	MMVAD_SHORT Core;
	union                 /*0x040*/ 
	{
		UINT32 LongFlags2;
		//现在用不到省略
		MMVAD_FLAGS2 VadFlags2;

	}u2;
	PSUBSECTION Subsection;               /*0x048*/
	PVOID FirstPrototypePte;        /*0x050*/  
	PVOID LastContiguousPte;        /*0x058*/  
	LIST_ENTRY ViewLinks;           /*0x060*/   
	PEPROCESS VadsProcess;          /*0x070*/   
	PVOID u4;                       /*0x078*/ 
	PVOID FileObject;               /*0x080*/ 
}MMVAD, *PMMVAD;


typedef struct _RTL_AVL_TREE         // 1 elements, 0x8 bytes (sizeof) 
{
	/*0x000*/     struct _RTL_BALANCED_NODE* Root;
}RTL_AVL_TREE, *PRTL_AVL_TREE;

typedef struct _VAD_INFO_
{
	ULONG_PTR pVad;
	ULONG_PTR startVpn;
	ULONG_PTR endVpn;
	ULONG_PTR pFileObject;
	ULONG_PTR flags;
}VAD_INFO, *PVAD_INFO;

typedef struct _ALL_VADS_
{
	ULONG nCnt;
	VAD_INFO VadInfos[1];
}ALL_VADS, *PALL_VADS;


typedef struct _MMSECTION_FLAGS                        // 27 elements, 0x4 bytes (sizeof) 
{
	/*0x000*/     UINT32       BeingDeleted : 1;                     // 0 BitPosition                   
	/*0x000*/     UINT32       BeingCreated : 1;                     // 1 BitPosition                   
	/*0x000*/     UINT32       BeingPurged : 1;                      // 2 BitPosition                   
	/*0x000*/     UINT32       NoModifiedWriting : 1;                // 3 BitPosition                   
	/*0x000*/     UINT32       FailAllIo : 1;                        // 4 BitPosition                   
	/*0x000*/     UINT32       Image : 1;                            // 5 BitPosition                   
	/*0x000*/     UINT32       Based : 1;                            // 6 BitPosition                   
	/*0x000*/     UINT32       File : 1;                             // 7 BitPosition                   
	/*0x000*/     UINT32       AttemptingDelete : 1;                 // 8 BitPosition                   
	/*0x000*/     UINT32       PrefetchCreated : 1;                  // 9 BitPosition                   
	/*0x000*/     UINT32       PhysicalMemory : 1;                   // 10 BitPosition                  
	/*0x000*/     UINT32       ImageControlAreaOnRemovableMedia : 1; // 11 BitPosition                  
	/*0x000*/     UINT32       Reserve : 1;                          // 12 BitPosition                  
	/*0x000*/     UINT32       Commit : 1;                           // 13 BitPosition                  
	/*0x000*/     UINT32       NoChange : 1;                         // 14 BitPosition                  
	/*0x000*/     UINT32       WasPurged : 1;                        // 15 BitPosition                  
	/*0x000*/     UINT32       UserReference : 1;                    // 16 BitPosition                  
	/*0x000*/     UINT32       GlobalMemory : 1;                     // 17 BitPosition                  
	/*0x000*/     UINT32       DeleteOnClose : 1;                    // 18 BitPosition                  
	/*0x000*/     UINT32       FilePointerNull : 1;                  // 19 BitPosition                  
	/*0x000*/     ULONG32      PreferredNode : 6;                    // 20 BitPosition                  
	/*0x000*/     UINT32       GlobalOnlyPerSession : 1;             // 26 BitPosition                  
	/*0x000*/     UINT32       UserWritable : 1;                     // 27 BitPosition                  
	/*0x000*/     UINT32       SystemVaAllocated : 1;                // 28 BitPosition                  
	/*0x000*/     UINT32       PreferredFsCompressionBoundary : 1;   // 29 BitPosition                  
	/*0x000*/     UINT32       UsingFileExtents : 1;                 // 30 BitPosition                  
	/*0x000*/     UINT32       PageSize64K : 1;                      // 31 BitPosition                  
}MMSECTION_FLAGS, *PMMSECTION_FLAGS;


typedef struct _SECTION                          // 9 elements, 0x40 bytes (sizeof) 
{
	/*0x000*/     struct _RTL_BALANCED_NODE SectionNode;       // 6 elements, 0x18 bytes (sizeof) 
	/*0x018*/     UINT64       StartingVpn;
	/*0x020*/     UINT64       EndingVpn;
	/*0x028*/     union {
		PCONTROL_AREA   ControlArea;
		PVOID   FileObject;

	                      }u1;                   // 4 elements, 0x8 bytes (sizeof)  
	/*0x030*/     UINT64       SizeOfSection;
	/*0x038*/     union {
		ULONG32 LongFlags;
		MMSECTION_FLAGS Flags;
	}u;                    // 2 elements, 0x4 bytes (sizeof)  
	struct                                       // 3 elements, 0x4 bytes (sizeof)  
	{
		/*0x03C*/         ULONG32      InitialPageProtection : 12; // 0 BitPosition                   
		/*0x03C*/         ULONG32      SessionId : 19;             // 12 BitPosition                  
		/*0x03C*/         ULONG32      NoValidationNeeded : 1;     // 31 BitPosition                  
	};
}SECTION, *PSECTION;



PMMVAD MiLocateAddress(IN PEPROCESS pEprocess, IN PVOID VirtualAddress);
BOOLEAN EnumProcessVad(ULONG Pid, PALL_VADS pBuffer, ULONG nCnt);
VOID EnumVad(PMMVAD Root, PALL_VADS pBuffer, ULONG nCnt);