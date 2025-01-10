#include "gtest/gtest.h"
#include "GAIABoardCommandLibrary.h"

TEST(GAIABoardCommandLibraryTest, idn)
{
    EXPECT_EQ(GAIABoardCommandLibrary::idn(), "#IDN?\n");
}
TEST(GAIABoardCommandLibraryTest, enable)
{
    EXPECT_EQ(GAIABoardCommandLibrary::enable(0), "#ENABLE 0\n");
}
TEST(GAIABoardCommandLibraryTest, getvg)
{
    EXPECT_EQ(GAIABoardCommandLibrary::getvg(0), "#GETVG 0\n");
}
TEST(GAIABoardCommandLibraryTest, getvd)
{
    EXPECT_EQ(GAIABoardCommandLibrary::getvd(0), "#GETVD 0\n");
}
TEST(GAIABoardCommandLibraryTest, getid)
{
    EXPECT_EQ(GAIABoardCommandLibrary::getid(0), "#GETID 0\n");
}
TEST(GAIABoardCommandLibraryTest, getref)
{
    EXPECT_EQ(GAIABoardCommandLibrary::getref(0), "#GETREF 0\n");
}
TEST(GAIABoardCommandLibraryTest, getemp)
{
    EXPECT_EQ(GAIABoardCommandLibrary::getemp(0), "#GETEMP 0\n");
}
TEST(GAIABoardCommandLibraryTest, name)
{
    EXPECT_EQ(GAIABoardCommandLibrary::name(), "#NAME?\n");
}
