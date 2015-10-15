#include "gtest/gtest.h"

#include <stdlib.h>
#include <IRA>
#include "FitsTools.h"
#include "File.h"
#include "TestSchema.h"

#include "FitsTools_test.i"

#include "File_test.i"

using namespace NewFitsWriterTest;


TEST_F(NewFitsWriter_FitsTools, FitsTools_filePrepare){
	EXPECT_TRUE(FitsTools_filePrepare());
}

TEST_F(NewFitsWriter_File, File_opening){
	EXPECT_TRUE(File_opening());
}

TEST_F(NewFitsWriter_File, File_creation){
	EXPECT_TRUE(File_creation());
}

TEST_F(NewFitsWriter_File, File_getTable){
	EXPECT_TRUE(File_getTable());
}

TEST_F(NewFitsWriter_File, File_writeTableColumns){
	EXPECT_TRUE(File_writeTableColumns());
}

TEST_F(NewFitsWriter_File, File_writeTableColumnsFast){
	EXPECT_TRUE(File_writeTableColumnsFast());
}


