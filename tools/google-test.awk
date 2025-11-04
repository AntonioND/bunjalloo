#!/usr/bin/awk -f

# Script to convert unit tests written for CppUnit to google-test syntax. It is
# not exhaustive and only covered the parts of CppUnit I used in the tests
# written for Bunjalloo.

/#include <cppunit\/extensions\/HelperMacros.h>/ {
  print $1" <gtest/gtest.h>"
  next
}
/CPPUNIT_NS::TestFixture/ {
  sub("CPPUNIT_NS::TestFixture", "testing::Test");
}
/CPPUNIT_TEST/ {
  next
}
/tearDown/ {
  sub("tearDown", "TearDown");
}
/setUp/ {
  sub("setUp", "SetUp");
}
/void test|private:/ {
  next
}
/public:/ {
  print "  protected:"
  next
}

/void [A-Z].*::test.*\(\)/ {
   print gensub(/void ([^:]+)::test([^(]+)\(\)/, "TEST_F(\\1, \\2)", 1)
   next
}

/CPPUNIT_ASSERT_MESSAGE/ {
  print gensub(/CPPUNIT_ASSERT_MESSAGE\((".*"), (.*)\);/, "EXPECT_TRUE(\\2) << \\1;", 1);
  next
}

/CPPUNIT_ASSERT_EQUAL_MESSAGE/ {
  print gensub(/CPPUNIT_ASSERT_EQUAL_MESSAGE\((".*"), ([^,]+), (.*)\);/, "EXPECT_EQ(\\2, \\3) << \\1;", 1);
  next
}

/CPPUNIT/ {
  sub("CPPUNIT_ASSERT_EQUAL", "EXPECT_EQ");
  sub("CPPUNIT_ASSERT", "EXPECT_TRUE");
}

1
