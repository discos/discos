#include "Summary.h"
#include "SummarySchema.h"

FitsWriter_private::CFile *createSummary()
{
	FitsWriter_private::CFile *file=NULL;
	#define _FILE_LOCAL_FILE file
	#include "FileCreator.h"
	#undef _FILE_LOCAL_FILE
	return file;
}

