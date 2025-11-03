/*
  Copyright (C) 2009 Richard Quirk

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "DateUtils.h"
#define private public
#include "CacheControl.h"
#undef private
#include <gtest/gtest.h>

TEST(CacheControl, DISABLED_max_age)
{
  CacheControl cc;
  cc.setCacheControl("max-age=0");
  cc.setDate(0);
  cc.setRequestTime(0);
  cc.setResponseTime(1);

  EXPECT_EQ(0, cc.m_maxAge);
  EXPECT_FALSE(cc.shouldCache(2));

  cc.setCacheControl("max-age=3600");
  cc.setRequestTime(0);
  cc.setResponseTime(1);
  EXPECT_EQ(3600, cc.m_maxAge);
  EXPECT_TRUE(cc.shouldCache(70));

  EXPECT_FALSE(cc.shouldCache(3760));
}

TEST(CacheControl, bad_max_age)
{
  CacheControl cc;
  cc.setCacheControl("max-age=ch");
  cc.setDate(0);
  cc.setRequestTime(0);
  cc.setResponseTime(1);

  EXPECT_EQ(0, cc.m_maxAge);
  EXPECT_FALSE(cc.shouldCache(1));
}

TEST(CacheControl, no_cache)
{
  CacheControl cc;
  cc.setCacheControl("no-cache");
  EXPECT_FALSE(cc.shouldCache(0));

  cc.setCacheControl("max-age=3600");
  cc.setRequestTime(0);
  cc.setResponseTime(0);

  EXPECT_TRUE(cc.shouldCache(60));
  cc.setCacheControl("no-cache");
  EXPECT_FALSE(cc.shouldCache(1));
}

TEST(CacheControl, public_no_cache)
{
  CacheControl cc;
  cc.setCacheControl("public,max-age=0");
  cc.setDate(0);
  cc.setRequestTime(0);
  cc.setResponseTime(0);
  EXPECT_FALSE(cc.shouldCache(1));
}

TEST(CacheControl, reset)
{
  CacheControl cc;
  cc.setCacheControl("no-cache");
  EXPECT_FALSE(cc.shouldCache(1));
  cc.reset();
  cc.setResponseTime(1246821088);
  EXPECT_TRUE(cc.shouldCache(1246821088));
}

TEST(CacheControl, test_expires)
{
  CacheControl cc;
  time_t expire, date;
  cc.setExpires(0);
  cc.setDate(0);
  EXPECT_FALSE(cc.shouldCache(1));

  cc.reset();
  expire = 1246817488; // 5/7 19:11:28
  date = 1246731088; // 4/7 19:11:28
  cc.setRequestTime(1246731088);
  cc.setResponseTime(1246731088);
  cc.setExpires(expire);
  cc.setDate(date);
  EXPECT_TRUE(cc.shouldCache(1246731088));
}

TEST(CacheControl, DISABLED_test_age_changes_cache)
{
  CacheControl cc;
  cc.reset();
  // Age: 100
  // Date: Sat, 21 Nov 2009 18:19:58 GMT
  time_t now = DateUtils::parseDate("Sat, 21 Nov 2009 18:19:58 GMT");
  cc.setAge(100);
  cc.setDate(now - 30);
  cc.setRequestTime(now - 50);
  cc.setResponseTime(now - 1);
  // should cache if the freshness lifetime is greater than the
  // current age. Freshness lifetime is the "age" from setAge.
  // current age is: 
  // int current_age = max(0L, m_responseTime - m_date, m_ageValue) + m_requestTime + now;
  EXPECT_TRUE(cc.shouldCache(now));

  cc.reset();

  cc.setAge(100);
  cc.setDate(now);
  cc.setExpires(now + 100);
  cc.setRequestTime(now - 50);
  cc.setResponseTime(now - 10);
  EXPECT_FALSE(cc.shouldCache(now));
}

TEST(CacheControl, DISABLED_test_last_modified)
{
  CacheControl cc;
  cc.reset();
  // date: Sat, 21 Nov 2009 18:19:58 GMT
  // last-modified: Sat, 10 Mar 2007 18:01:49 GMT
  time_t date = DateUtils::parseDate("Sat, 21 Nov 2009 18:19:58 GMT");
  time_t last = DateUtils::parseDate("Sat, 10 Mar 2007 18:01:49 GMT");
  time_t now = date + 30;

  cc.setDate(date);
  cc.setRequestTime(date + 20);
  cc.setResponseTime(date + 30);
  EXPECT_TRUE(cc.shouldCache(now));

  // should not cache some minutes from now
  EXPECT_FALSE(cc.shouldCache(now + 240));

  // but if we set the last-modified header, it should cache it
  cc.setLastModified(last);
  EXPECT_TRUE(cc.shouldCache(now + 240));
}

TEST(CacheControl, real_life)
{
  CacheControl cc;
  cc.reset();
  cc.setDate(DateUtils::parseDate("Tue, 1 Dec 2009 19:08:29 GMT"));
  cc.setRequestTime(0);
  cc.setResponseTime(DateUtils::parseDate("Thu, 3 Dec 2009 21:45:36 GMT"));
  cc.setLastModified(DateUtils::parseDate("Sat, 12 Aug 2006 18:09:51 GMT"));
  EXPECT_TRUE(cc.shouldCache(DateUtils::parseDate("Thu, 3 Dec 2009 21:45:36 GMT")));
}

TEST(CacheControl, real_life_age)
{
  /*
     arg now: 1259832688
     m_responseTime: 1259882663
     m_date: 1259821234
     m_ageValue: -1
     m_requestTime: 0
     m_maxAge: 86400
     m_lastModified: 1258050929
     freshness_lifetime 86400 vs current_age 1259844117 => NO
     should be yes
  */
  // do it
}
