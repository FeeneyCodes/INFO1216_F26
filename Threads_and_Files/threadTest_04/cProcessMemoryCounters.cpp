#include "cProcessMemoryCounters.h"
#include <sstream>

cProcessMemoryCounters::cProcessMemoryCounters()
{
	this->UpdateMemoryCounterInfo();
}

bool cProcessMemoryCounters::UpdateMemoryCounterInfo(void)
{
	this->m_bDataIsValid = false;

	// https://learn.microsoft.com/en-us/windows/win32/api/psapi/nf-psapi-getprocessmemoryinfo
	// Set size of structure in bytes
	this->m_PMCInfo.cb = sizeof(this->m_PMCInfo);

	HANDLE hThisProcess = GetCurrentProcess();

	if ( GetProcessMemoryInfo( hThisProcess, 
	                           (PPROCESS_MEMORY_COUNTERS)(&(this->m_PMCInfo)), 
	                           this->m_PMCInfo.cb) )
	{
		this->m_bDataIsValid = true;
	}
	else
	{
		// Returned zero (0), an error condition
		this->m_lastErrorCode = GetLastError();



		// Get this error in human readable text
		LPWSTR lpBufferText = 0;

		DWORD bufferSize = FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,	// DWORD dwFlags,
		                                  0,						// [in, optional] LPCVOID lpSource (ignored with Message_From_System)
		                                  this->m_lastErrorCode,	// [in] DWORD dwMessageId
		                                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),	// [in] DWORD dwLanguageId (0 = don't care or built in)
		                                  (LPWSTR)&lpBufferText,	// [out] LPCVOID lpSource
		                                  0,						// [in] DWORD nSize (0 = allocated by system)
		                                  NULL);					// [in, optional] va_list* Arguments
		if (bufferSize == 0)
		{
			// Something went wrong calling FormatMessage()
			this->m_lastErrorString = "Something went wrong calling FormatMessage()";
		}
		else
		{
			// Convert the wide Unicode to std::string
			int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, lpBufferText, (int)bufferSize, NULL, 0, NULL, NULL);
			std::string strTo(sizeNeeded, 0);
			WideCharToMultiByte(CP_UTF8, 0, lpBufferText, (int)bufferSize, &strTo[0], sizeNeeded, NULL, NULL);

			this->m_lastErrorString = std::string(strTo, sizeNeeded);
		}
	}

	return this->bAreMemoryCountersValid();
}

// True if the last call to Update() worked
bool cProcessMemoryCounters::bAreMemoryCountersValid(void)
{
	return this->m_bDataIsValid;
}

std::string cProcessMemoryCounters::getLastErrorString(void)
{
	return this->m_lastErrorString;
}

DWORD cProcessMemoryCounters::getLastErrorCode(void)
{
	return this->m_lastErrorCode;
}

DWORD cProcessMemoryCounters::getPageFaultCount(void)
{
	return this->m_PMCInfo.PageFaultCount;
}

SIZE_T cProcessMemoryCounters::getPeakWorkingSetSize(void)
{
	return this->m_PMCInfo.PeakWorkingSetSize;
}

SIZE_T cProcessMemoryCounters::getWorkingSetSize(void)
{
	return this->m_PMCInfo.WorkingSetSize;
}

SIZE_T cProcessMemoryCounters::getQuotaPeakPagedPoolUsage(void)
{
	return this->m_PMCInfo.QuotaPeakPagedPoolUsage;
}

SIZE_T cProcessMemoryCounters::getQuotaPagedPoolUsage(void)
{
	return this->m_PMCInfo.QuotaPagedPoolUsage;
}

SIZE_T cProcessMemoryCounters::getQuotaPeakNonPagedPoolUsage(void)
{
	return this->m_PMCInfo.QuotaPeakNonPagedPoolUsage;
}

SIZE_T cProcessMemoryCounters::getQuotaNonPagedPoolUsage(void)
{
	return this->m_PMCInfo.QuotaNonPagedPoolUsage;
}

SIZE_T cProcessMemoryCounters::getPagefileUsage(void)
{
	return this->m_PMCInfo.PagefileUsage;
}

SIZE_T cProcessMemoryCounters::getPeakPagefileUsage(void)
{
	return this->m_PMCInfo.PeakPagefileUsage;
}

SIZE_T cProcessMemoryCounters::getPrivateUsage(void)
{
	return this->m_PMCInfo.PrivateUsage;
}

SIZE_T cProcessMemoryCounters::getPrivateWorkingSetSize(void)

{
	return this->m_PMCInfo.PrivateWorkingSetSize;
}

ULONG64 cProcessMemoryCounters::getSharedCommitUsage(void)

{
	return this->m_PMCInfo.SharedCommitUsage;
}


std::string cProcessMemoryCounters::getCounterSummaryString(void)
{
	std::stringstream ssInfo;

	// https://learn.microsoft.com/en-us/windows/win32/api/psapi/ns-psapi-process_memory_counters_ex2

	//	PageFaultCount: The number of page faults.
	ssInfo << "Page Fault Count: " << this->m_PMCInfo.PageFaultCount << std::endl;

	//	PeakWorkingSetSize: The peak working set size, in bytes.
	ssInfo << "Peak Working Set Size: " << this->m_PMCInfo.PeakWorkingSetSize << " bytes" << std::endl;

	//	WorkingSetSize: The current working set size, in bytes
	ssInfo << "Working Set Size: " << this->m_PMCInfo.WorkingSetSize << " bytes" << std::endl;

	//	QuotaPeakPagedPoolUsage: The peak paged pool usage, in bytes.
	ssInfo << "Quota Peak Paged Pool Usage: " << this->m_PMCInfo.QuotaPeakPagedPoolUsage << " bytes" << std::endl;

	//	QuotaPagedPoolUsage: The current paged pool usage, in bytes.
	ssInfo << "Quota Paged Pool Usage: " << this->m_PMCInfo.QuotaPagedPoolUsage << " bytes" << std::endl;

	//	QuotaPeakNonPagedPoolUsage: The peak non - paged pool usage, in bytes.
	ssInfo << "Quota PeakNon Paged Pool Usage: " << this->m_PMCInfo.QuotaPeakNonPagedPoolUsage << " bytes" << std::endl;

	//	QuotaNonPagedPoolUsage: The current non - paged pool usage, in bytes.
	ssInfo << "Quota Non Paged Pool Usage: " << this->m_PMCInfo.QuotaNonPagedPoolUsage << " bytes" << std::endl;

	//	PagefileUsage: The Commit Charge value in bytes for this process.
	//		            Commit Charge is the total amount of private memory that 
	//		            the memory manager has committed for a running process.
	ssInfo << "Pagefile Usage (Commit Charge): " << this->m_PMCInfo.PagefileUsage << " bytes" << std::endl;

	//	PeakPagefileUsage: The peak value in bytes of the Commit Charge during the lifetime of this process.
	ssInfo << "Peak Pagefile Usage: " << this->m_PMCInfo.PeakPagefileUsage << " bytes" << std::endl;

	//	PrivateUsage: Same as PagefileUsage.The Commit Charge value in bytes for this process.
	//		            Commit Charge is the total amount of private memory that the memory manager 
	//		            has committed for a running process.
	ssInfo << "Private Usage (Commit Charge): " << this->m_PMCInfo.PrivateUsage << " bytes" << std::endl;

	//	PrivateWorkingSetSize: The current private working set size, in bytes.
	ssInfo << "Private Working Set Size: " << this->m_PMCInfo.PrivateWorkingSetSize << " bytes" << std::endl;

	//	SharedCommitUsage: The current shared commit usage, in bytes.
	ssInfo << "Shared Commit Usage: " << this->m_PMCInfo.SharedCommitUsage << " bytes" << std::endl;

	return ssInfo.str();
}
	
