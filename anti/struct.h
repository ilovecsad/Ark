#pragma once
#include <ntifs.h>




typedef struct _OBJECT_TYPE_INITIALIZER                                                                                                                                                                                                                                                                                             // 32 elements, 0x78 bytes (sizeof) 
{
	/*0x000*/     UINT16       Length;
	union                                                                                                                                                                                                                                                                                                                           // 2 elements, 0x2 bytes (sizeof)   
	{
		/*0x002*/         UINT16       ObjectTypeFlags;
		//struct                                                                                                                                                                                                                                                                                                                      // 2 elements, 0x2 bytes (sizeof)   
		//{
		//	struct                                                                                                                                                                                                                                                                                                                  // 8 elements, 0x1 bytes (sizeof)   
		//	{
		//		/*0x002*/                 UINT8        CaseInsensitive : 1;                                                                                                                                                                                                                                                                                   // 0 BitPosition                    
		//		/*0x002*/                 UINT8        UnnamedObjectsOnly : 1;                                                                                                                                                                                                                                                                                // 1 BitPosition                    
		//		/*0x002*/                 UINT8        UseDefaultObject : 1;                                                                                                                                                                                                                                                                                  // 2 BitPosition                    
		//		/*0x002*/                 UINT8        SecurityRequired : 1;                                                                                                                                                                                                                                                                                  // 3 BitPosition                    
		//		/*0x002*/                 UINT8        MaintainHandleCount : 1;                                                                                                                                                                                                                                                                               // 4 BitPosition                    
		//		/*0x002*/                 UINT8        MaintainTypeList : 1;                                                                                                                                                                                                                                                                                  // 5 BitPosition                    
		//		/*0x002*/                 UINT8        SupportsObjectCallbacks : 1;                                                                                                                                                                                                                                                                           // 6 BitPosition                    
		//		/*0x002*/                 UINT8        CacheAligned : 1;                                                                                                                                                                                                                                                                                      // 7 BitPosition                    
		//	};
		//	struct                                                                                                                                                                                                                                                                                                                  // 2 elements, 0x1 bytes (sizeof)   
		//	{
		//		/*0x003*/                 UINT8        UseExtendedParameters : 1;                                                                                                                                                                                                                                                                             // 0 BitPosition                    
		//		/*0x003*/                 UINT8        Reserved : 7;                                                                                                                                                                                                                                                                                          // 1 BitPosition                    
		//	};
		//};
	}u2;
	/*0x004*/     ULONG32      ObjectTypeCode;
	/*0x008*/     ULONG32      InvalidAttributes;
	/*0x00C*/     struct _GENERIC_MAPPING GenericMapping;                                                                                                                                                                                                                                                                                         // 4 elements, 0x10 bytes (sizeof)  
	/*0x01C*/     ULONG32      ValidAccessMask;
	/*0x020*/     ULONG32      RetainAccess;
	/*0x024*/     enum _POOL_TYPE PoolType;
	/*0x028*/     ULONG32      DefaultPagedPoolCharge;
	/*0x02C*/     ULONG32      DefaultNonPagedPoolCharge;
	/*0x030*/     PVOID DumpProcedure;
	/*0x038*/     PVOID OpenProcedure;
	/*0x040*/     PVOID CloseProcedure;
	/*0x048*/     PVOID DeleteProcedure;
	union                                                                                                                                                                                                                                                                                                                           // 2 elements, 0x8 bytes (sizeof)   
	{
		/*0x050*/         PVOID ParseProcedure;
		/*0x050*/         PVOID ParseProcedureEx;
	}u;
	/*0x058*/     PVOID SecurityProcedure;
	/*0x060*/     PVOID QueryNameProcedure;
	/*0x068*/     PVOID OkayToCloseProcedure;
	/*0x070*/     ULONG32      WaitObjectFlagMask;
	/*0x074*/     UINT16       WaitObjectFlagOffset;
	/*0x076*/     UINT16       WaitObjectPointerOffset;
}OBJECT_TYPE_INITIALIZER, *POBJECT_TYPE_INITIALIZER;

typedef struct _MY_OBJECT_TYPE                   // 12 elements, 0xD8 bytes (sizeof) 
{
	/*0x000*/     struct _LIST_ENTRY TypeList;              // 2 elements, 0x10 bytes (sizeof)  
	/*0x010*/     struct _UNICODE_STRING Name;              // 3 elements, 0x10 bytes (sizeof)  
	/*0x020*/     VOID*        DefaultObject;
	/*0x028*/     UINT8        Index;
	/*0x029*/     UINT8        _PADDING0_[0x3];
	/*0x02C*/     ULONG32      TotalNumberOfObjects;
	/*0x030*/     ULONG32      TotalNumberOfHandles;
	/*0x034*/     ULONG32      HighWaterNumberOfObjects;
	/*0x038*/     ULONG32      HighWaterNumberOfHandles;
	/*0x03C*/     UINT8        _PADDING1_[0x4];
	/*0x040*/     struct _OBJECT_TYPE_INITIALIZER TypeInfo; // 32 elements, 0x78 bytes (sizeof) 
	/*0x0B8*/     EX_PUSH_LOCK TypeLock;            // 7 elements, 0x8 bytes (sizeof)   
	/*0x0C0*/     ULONG32      Key;
	/*0x0C4*/     UINT8        _PADDING2_[0x4];
	/*0x0C8*/     struct _LIST_ENTRY CallbackList;          // 2 elements, 0x10 bytes (sizeof)  
}MY_OBJECT_TYPE, *PMY_OBJECT_TYPE;


typedef struct _SYSTEM_HANDLE_TABLE_ENTRY_INFO {
	ULONG           ProcessId;
	UCHAR ObjectTypeIndex;
	UCHAR HandleAttributes;
	USHORT HandleValue;
	PVOID                Object;
	ACCESS_MASK          GrantedAccess;
} SYSTEM_HANDLE_TABLE_ENTRY_INFO, *PSYSTEM_HANDLE_TABLE_ENTRY_INFO;

typedef struct _SYSTEM_HANDLE_INFORMATION {
	ULONG NumberOfHandles;
	SYSTEM_HANDLE_TABLE_ENTRY_INFO Handles[1];
} SYSTEM_HANDLE_INFORMATION, *PSYSTEM_HANDLE_INFORMATION;





///////////////////////////////////////////////////////////////////////////////
// https://bbs.pediy.com/thread-259664.htm
	/*
Pool tag	Object type
TmTx	_KTRANSACTION
TmRm	_KRESOURCEMANAGER
TmEn	_KENLISTMENT
TmTm	_KTM           事务管理器是什么（TM）？
	*/
typedef struct _KTMOBJECT_NAMESPACE_LINK // 2 elements, 0x28 bytes (sizeof) 
{
	/*0x000*/     struct _RTL_BALANCED_LINKS Links;    // 5 elements, 0x20 bytes (sizeof) 
	/*0x020*/     UINT8        Expired;
	/*0x021*/     UINT8        _PADDING0_[0x7];
}KTMOBJECT_NAMESPACE_LINK, *PKTMOBJECT_NAMESPACE_LINK;

typedef enum _KTM_STATE  // 6 elements, 0x4 bytes
{
	KKtmUninitialized = 0 /*0x0*/,
	KKtmInitialized = 1 /*0x1*/,
	KKtmRecovering = 2 /*0x2*/,
	KKtmOnline = 3 /*0x3*/,
	KKtmRecoveryFailed = 4 /*0x4*/,
	KKtmOffline = 5 /*0x5*/
}KTM_STATE, *PKTM_STATE;



typedef struct _KTMOBJECT_NAMESPACE // 5 elements, 0xA8 bytes (sizeof)  
{
	/*0x000*/     struct _RTL_AVL_TABLE Table;    // 11 elements, 0x68 bytes (sizeof) 
	/*0x068*/     struct _KMUTANT Mutex;          // 10 elements, 0x38 bytes (sizeof) 
	/*0x0A0*/     UINT16       LinksOffset;
	/*0x0A2*/     UINT16       GuidOffset;
	/*0x0A4*/     UINT8        Expired;
	/*0x0A5*/     UINT8        _PADDING0_[0x3];
}KTMOBJECT_NAMESPACE, *PKTMOBJECT_NAMESPACE;

typedef struct _KTM                                   // 33 elements, 0x3C0 bytes (sizeof) 
{
	/*0x000*/     ULONG32      cookie;
	/*0x004*/     UINT8        _PADDING0_[0x4];
	/*0x008*/     struct _KMUTANT Mutex;                            // 10 elements, 0x38 bytes (sizeof)  
	/*0x040*/     enum _KTM_STATE State;
	/*0x044*/     UINT8        _PADDING1_[0x4];
	/*0x048*/     struct _KTMOBJECT_NAMESPACE_LINK NamespaceLink;   // 2 elements, 0x28 bytes (sizeof)   
	/*0x070*/     struct _GUID TmIdentity;                          // 4 elements, 0x10 bytes (sizeof)   
	/*0x080*/     ULONG32      Flags;
	/*0x084*/     ULONG32      VolatileFlags;
	/*0x088*/     struct _UNICODE_STRING LogFileName;               // 3 elements, 0x10 bytes (sizeof)   
	/*0x098*/     struct _FILE_OBJECT* LogFileObject;
	/*0x0A0*/     VOID*        MarshallingContext;
	/*0x0A8*/     VOID*        LogManagementContext;
	/*0x0B0*/     struct _KTMOBJECT_NAMESPACE Transactions;         // 5 elements, 0xA8 bytes (sizeof)   
	/*0x158*/     struct _KTMOBJECT_NAMESPACE ResourceManagers;     // 5 elements, 0xA8 bytes (sizeof)   
	/*0x200*/     struct _KMUTANT LsnOrderedMutex;                  // 10 elements, 0x38 bytes (sizeof)  
	/*0x238*/     struct _LIST_ENTRY LsnOrderedList;                // 2 elements, 0x10 bytes (sizeof)   
	/*0x248*/     union _LARGE_INTEGER CommitVirtualClock;          // 4 elements, 0x8 bytes (sizeof)    
	/*0x250*/     struct _FAST_MUTEX CommitVirtualClockMutex;       // 5 elements, 0x38 bytes (sizeof)   
	/*0x288*/     ULONG_PTR BaseLsn;                           // 2 elements, 0x8 bytes (sizeof)    
	/*0x290*/     ULONG_PTR CurrentReadLsn;                    // 2 elements, 0x8 bytes (sizeof)    
	/*0x298*/     ULONG_PTR LastRecoveredLsn;                  // 2 elements, 0x8 bytes (sizeof)    
	/*0x2A0*/     VOID*        TmRmHandle;
	/*0x2A8*/     struct _KRESOURCEMANAGER* TmRm;
	/*0x2B0*/     struct _KEVENT LogFullNotifyEvent;                // 1 elements, 0x18 bytes (sizeof)   
	/*0x2C8*/     struct _WORK_QUEUE_ITEM CheckpointWorkItem;       // 3 elements, 0x20 bytes (sizeof)   
	/*0x2E8*/     ULONG_PTR CheckpointTargetLsn;               // 2 elements, 0x8 bytes (sizeof)    
	/*0x2F0*/     struct _WORK_QUEUE_ITEM LogFullCompletedWorkItem; // 3 elements, 0x20 bytes (sizeof)   
	/*0x310*/     struct _ERESOURCE LogWriteResource;               // 17 elements, 0x68 bytes (sizeof)  
	/*0x378*/     ULONG32      LogFlags;
	/*0x37C*/     LONG32       LogFullStatus;
	/*0x380*/     LONG32       RecoveryStatus;
	/*0x384*/     UINT8        _PADDING2_[0x4];
	/*0x388*/     ULONG_PTR LastCheckBaseLsn;                  // 2 elements, 0x8 bytes (sizeof)    
	/*0x390*/     struct _LIST_ENTRY RestartOrderedList;            // 2 elements, 0x10 bytes (sizeof)   
	/*0x3A0*/     struct _WORK_QUEUE_ITEM OfflineWorkItem;          // 3 elements, 0x20 bytes (sizeof)   
}KTM, *PKTM;


////////////////////////////////////////////////////////////////////////////////

typedef struct _KRESOURCEMANAGER_COMPLETION_BINDING // 4 elements, 0x28 bytes (sizeof) 
{
	/*0x000*/     struct _LIST_ENTRY NotificationListHead;        // 2 elements, 0x10 bytes (sizeof) 
	/*0x010*/     VOID*        Port;
	/*0x018*/     UINT64       Key;
	/*0x020*/     PEPROCESS BindingProcess;
}KRESOURCEMANAGER_COMPLETION_BINDING, *PKRESOURCEMANAGER_COMPLETION_BINDING;

typedef enum _KRESOURCEMANAGER_STATE  // 3 elements, 0x4 bytes
{
	KResourceManagerUninitialized = 0 /*0x0*/,
	KResourceManagerOffline = 1 /*0x1*/,
	KResourceManagerOnline = 2 /*0x2*/
}KRESOURCEMANAGER_STATE, *PKRESOURCEMANAGER_STATE;
typedef struct _KRESOURCEMANAGER                                   // 20 elements, 0x250 bytes (sizeof) 
{
	/*0x000*/     struct _KEVENT NotificationAvailable;                          // 1 elements, 0x18 bytes (sizeof)   
	/*0x018*/     ULONG32      cookie;
	/*0x01C*/     enum _KRESOURCEMANAGER_STATE State;
	/*0x020*/     ULONG32      Flags;
	/*0x024*/     UINT8        _PADDING0_[0x4];
	/*0x028*/     struct _KMUTANT Mutex;                                         // 10 elements, 0x38 bytes (sizeof)  
	/*0x060*/     struct _KTMOBJECT_NAMESPACE_LINK NamespaceLink;                // 2 elements, 0x28 bytes (sizeof)   
	/*0x088*/     struct _GUID RmId;                                             // 4 elements, 0x10 bytes (sizeof)   
	/*0x098*/     struct _KQUEUE NotificationQueue;                              // 5 elements, 0x40 bytes (sizeof)   
	/*0x0D8*/     struct _KMUTANT NotificationMutex;                             // 10 elements, 0x38 bytes (sizeof)  
	/*0x110*/     struct _LIST_ENTRY EnlistmentHead;                             // 2 elements, 0x10 bytes (sizeof)   
	/*0x120*/     ULONG32      EnlistmentCount;
	/*0x124*/     UINT8        _PADDING1_[0x4];
	/*0x128*/     PVOID* NotificationRoutine;
	/*0x130*/     VOID*        Key;
	/*0x138*/     struct _LIST_ENTRY ProtocolListHead;                           // 2 elements, 0x10 bytes (sizeof)   
	/*0x148*/     struct _LIST_ENTRY PendingPropReqListHead;                     // 2 elements, 0x10 bytes (sizeof)   
	/*0x158*/     struct _LIST_ENTRY CRMListEntry;                               // 2 elements, 0x10 bytes (sizeof)   
	/*0x168*/     struct _KTM* Tm;
	/*0x170*/     struct _UNICODE_STRING Description;                            // 3 elements, 0x10 bytes (sizeof)   
	/*0x180*/     struct _KTMOBJECT_NAMESPACE Enlistments;                       // 5 elements, 0xA8 bytes (sizeof)   
	/*0x228*/     struct _KRESOURCEMANAGER_COMPLETION_BINDING CompletionBinding; // 4 elements, 0x28 bytes (sizeof)   
}KRESOURCEMANAGER, *PKRESOURCEMANAGER;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct _SEP_TOKEN_PRIVILEGES // 3 elements, 0x18 bytes (sizeof) 
{
	/*0x000*/     UINT64       Present;
	/*0x008*/     UINT64       Enabled;
	/*0x010*/     UINT64       EnabledByDefault;
}SEP_TOKEN_PRIVILEGES, *PSEP_TOKEN_PRIVILEGES;

typedef struct _SEP_AUDIT_POLICY               // 2 elements, 0x1F bytes (sizeof) 
{
	/*0x000*/     struct _TOKEN_AUDIT_POLICY AdtTokenPolicy; // 1 elements, 0x1E bytes (sizeof) 
	/*0x01E*/     UINT8        PolicySetStatus;
}SEP_AUDIT_POLICY, *PSEP_AUDIT_POLICY;



typedef struct _SEP_CACHED_HANDLES_TABLE       // 2 elements, 0x10 bytes (sizeof) 
{
	/*0x000*/   ULONG_PTR Lock;                 // 7 elements, 0x8 bytes (sizeof)  
	/*0x008*/     struct _RTL_DYNAMIC_HASH_TABLE* HashTable;
}SEP_CACHED_HANDLES_TABLE, *PSEP_CACHED_HANDLES_TABLE;
typedef struct _DEVICE_MAP                               // 7 elements, 0x48 bytes (sizeof) 
{
	/*0x000*/     struct _OBJECT_DIRECTORY* DosDevicesDirectory;
	/*0x008*/     struct _OBJECT_DIRECTORY* GlobalDosDevicesDirectory;
	/*0x010*/     VOID*        DosDevicesDirectoryHandle;
	/*0x018*/     LONG32       ReferenceCount;
	/*0x01C*/     ULONG32      DriveMap;
	/*0x020*/     UINT8        DriveType[32];
	/*0x040*/     PVOID* ServerSilo;
}DEVICE_MAP, *PDEVICE_MAP;

typedef struct _SEP_LOGON_SESSION_REFERENCES                               // 17 elements, 0xC0 bytes (sizeof) 
{
	/*0x000*/     struct _SEP_LOGON_SESSION_REFERENCES* Next;
	/*0x008*/     struct _LUID LogonId;                                                  // 2 elements, 0x8 bytes (sizeof)   
	/*0x010*/     struct _LUID BuddyLogonId;                                             // 2 elements, 0x8 bytes (sizeof)   
	/*0x018*/     INT64        ReferenceCount;
	/*0x020*/     ULONG32      Flags;
	/*0x024*/     UINT8        _PADDING0_[0x4];
	/*0x028*/     struct _DEVICE_MAP* pDeviceMap;
	/*0x030*/     VOID*        Token;
	/*0x038*/     struct _UNICODE_STRING AccountName;                                    // 3 elements, 0x10 bytes (sizeof)  
	/*0x048*/     struct _UNICODE_STRING AuthorityName;                                  // 3 elements, 0x10 bytes (sizeof)  
	/*0x058*/     struct _SEP_CACHED_HANDLES_TABLE CachedHandlesTable;                   // 2 elements, 0x10 bytes (sizeof)  
	/*0x068*/     ULONG_PTR SharedDataLock;                                   // 7 elements, 0x8 bytes (sizeof)   
	///*0x070*/     struct _AUTHZBASEP_CLAIM_ATTRIBUTES_COLLECTION* SharedClaimAttributes;
	///*0x078*/     struct _SEP_SID_VALUES_BLOCK* SharedSidValues;
	///*0x080*/     struct _OB_HANDLE_REVOCATION_BLOCK RevocationBlock;                    // 3 elements, 0x20 bytes (sizeof)  
	///*0x0A0*/     PVOID* ServerSilo;
	///*0x0A8*/     struct _LUID SiblingAuthId;                                            // 2 elements, 0x8 bytes (sizeof)   
	///*0x0B0*/     struct _LIST_ENTRY TokenList;                                          // 2 elements, 0x10 bytes (sizeof)  
}SEP_LOGON_SESSION_REFERENCES, *PSEP_LOGON_SESSION_REFERENCES;


typedef struct _TOKEN                                                        // 48 elements, 0x498 bytes (sizeof) 
{
	/*0x000*/     struct _TOKEN_SOURCE TokenSource;                                        // 2 elements, 0x10 bytes (sizeof)   
	/*0x010*/     struct _LUID TokenId;                                                    // 2 elements, 0x8 bytes (sizeof)    
	/*0x018*/     struct _LUID AuthenticationId;                                           // 2 elements, 0x8 bytes (sizeof)    
	/*0x020*/     struct _LUID ParentTokenId;                                              // 2 elements, 0x8 bytes (sizeof)    
	/*0x028*/     union _LARGE_INTEGER ExpirationTime;                                     // 4 elements, 0x8 bytes (sizeof)    
	/*0x030*/     struct _ERESOURCE* TokenLock;
	/*0x038*/     struct _LUID ModifiedId;                                                 // 2 elements, 0x8 bytes (sizeof)    
	/*0x040*/     struct _SEP_TOKEN_PRIVILEGES Privileges;                                 // 3 elements, 0x18 bytes (sizeof)   
	/*0x058*/     struct _SEP_AUDIT_POLICY AuditPolicy;                                    // 2 elements, 0x1F bytes (sizeof)   
	/*0x077*/     UINT8        _PADDING0_[0x1];
	/*0x078*/     ULONG32      SessionId;
	/*0x07C*/     ULONG32      UserAndGroupCount;
	/*0x080*/     ULONG32      RestrictedSidCount;
	/*0x084*/     ULONG32      VariableLength;
	/*0x088*/     ULONG32      DynamicCharged;
	/*0x08C*/     ULONG32      DynamicAvailable;
	/*0x090*/     ULONG32      DefaultOwnerIndex;
	/*0x094*/     UINT8        _PADDING1_[0x4];
	/*0x098*/     struct _SID_AND_ATTRIBUTES* UserAndGroups;
	/*0x0A0*/     struct _SID_AND_ATTRIBUTES* RestrictedSids;
	/*0x0A8*/     VOID*        PrimaryGroup;
	/*0x0B0*/     ULONG32*     DynamicPart;
	/*0x0B8*/     struct _ACL* DefaultDacl;
	/*0x0C0*/     enum _TOKEN_TYPE TokenType;
	/*0x0C4*/     enum _SECURITY_IMPERSONATION_LEVEL ImpersonationLevel;
	/*0x0C8*/     ULONG32      TokenFlags;
	/*0x0CC*/     UINT8        TokenInUse;
	/*0x0CD*/     UINT8        _PADDING2_[0x3];
	/*0x0D0*/     ULONG32      IntegrityLevelIndex;
	/*0x0D4*/     ULONG32      MandatoryPolicy;
	/*0x0D8*/     struct _SEP_LOGON_SESSION_REFERENCES* LogonSession;
	/*0x0E0*/     struct _LUID OriginatingLogonSession;                                    // 2 elements, 0x8 bytes (sizeof)    
	/*0x0E8*/     struct _SID_AND_ATTRIBUTES_HASH SidHash;                                 // 3 elements, 0x110 bytes (sizeof)  
	/*0x1F8*/     struct _SID_AND_ATTRIBUTES_HASH RestrictedSidHash;                       // 3 elements, 0x110 bytes (sizeof)  
	
	///*0x308*/     struct _AUTHZBASEP_SECURITY_ATTRIBUTES_INFORMATION* pSecurityAttributes;
	///*0x310*/     VOID*        Package;
	///*0x318*/     struct _SID_AND_ATTRIBUTES* Capabilities;
	///*0x320*/     ULONG32      CapabilityCount;
	///*0x324*/     UINT8        _PADDING3_[0x4];
	///*0x328*/     struct _SID_AND_ATTRIBUTES_HASH CapabilitiesHash;                        // 3 elements, 0x110 bytes (sizeof)  
	///*0x438*/     struct _SEP_LOWBOX_NUMBER_ENTRY* LowboxNumberEntry;
	///*0x440*/     struct _SEP_CACHED_HANDLES_ENTRY* LowboxHandlesEntry;
	///*0x448*/     struct _AUTHZBASEP_CLAIM_ATTRIBUTES_COLLECTION* pClaimAttributes;
	///*0x450*/     VOID*        TrustLevelSid;
	///*0x458*/     struct _TOKEN* TrustLinkedToken;
	///*0x460*/     VOID*        IntegrityLevelSidValue;
	///*0x468*/     struct _SEP_SID_VALUES_BLOCK* TokenSidValues;
	///*0x470*/     struct _SEP_LUID_TO_INDEX_MAP_ENTRY* IndexEntry;
	///*0x478*/     struct _SEP_TOKEN_DIAG_TRACK_ENTRY* DiagnosticInfo;
	///*0x480*/     struct _SEP_CACHED_HANDLES_ENTRY* BnoIsolationHandlesEntry;
	///*0x488*/     VOID*        SessionObject;
	///*0x490*/     UINT64       VariablePart;
	
}TOKEN, *PTOKEN;



