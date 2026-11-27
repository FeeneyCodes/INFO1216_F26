#pragma once

// Written by: Michael Feeney mfeeney (at) fanshawec (dot) ca 
// November 2026

#include <string>
#include <Windows.h>
// For the "Process Memory Counters" (what task manager sees)
// https://learn.microsoft.com/en-us/windows/win32/api/psapi/ns-psapi-process_memory_counters_ex2
// 

#include <Psapi.h>	

class cProcessMemoryCounters
{
public:
	cProcessMemoryCounters();

	bool UpdateMemoryCounterInfo(void);
	// True if the last call to Update() worked
	bool bAreMemoryCountersValid(void);
	// If call didn't work, this might have more information
	std::string getLastErrorString(void);
	DWORD getLastErrorCode(void);


	//	PageFaultCount: The number of page faults.
	DWORD getPageFaultCount(void);
//	PeakWorkingSetSize: The peak working set size, in bytes.
	SIZE_T getPeakWorkingSetSize(void);
	//	WorkingSetSize: The current working set size, in bytes
	SIZE_T getWorkingSetSize(void);
	//	QuotaPeakPagedPoolUsage: The peak paged pool usage, in bytes.
	SIZE_T getQuotaPeakPagedPoolUsage(void);
	//	QuotaPagedPoolUsage: The current paged pool usage, in bytes.
	SIZE_T getQuotaPagedPoolUsage(void);
	//	QuotaPeakNonPagedPoolUsage: The peak non - paged pool usage, in bytes.
	SIZE_T getQuotaPeakNonPagedPoolUsage(void);
	//	QuotaNonPagedPoolUsage: The current non - paged pool usage, in bytes.
	SIZE_T getQuotaNonPagedPoolUsage(void);
	//	PagefileUsage: The Commit Charge value in bytes for this process.
	//		            Commit Charge is the total amount of private memory that 
	//		            the memory manager has committed for a running process.
	SIZE_T getPagefileUsage(void);
	//	PeakPagefileUsage: The peak value in bytes of the Commit Charge during the lifetime of this process.
	SIZE_T getPeakPagefileUsage(void);
	//	PrivateUsage: Same as PagefileUsage.The Commit Charge value in bytes for this process.
	//		            Commit Charge is the total amount of private memory that the memory manager 
	//		            has committed for a running process.
	SIZE_T getPrivateUsage(void);
	//	PrivateWorkingSetSize: The current private working set size, in bytes.
	SIZE_T getPrivateWorkingSetSize(void);
	//	SharedCommitUsage: The current shared commit usage, in bytes.
	ULONG64 getSharedCommitUsage(void);

	std::string getCounterSummaryString(void);
private:
	PROCESS_MEMORY_COUNTERS_EX2 m_PMCInfo;

	bool m_bDataIsValid = false;
	std::string m_lastErrorString = "";
	DWORD m_lastErrorCode = 0;
};

