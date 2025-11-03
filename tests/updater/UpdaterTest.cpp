#include <gtest/gtest.h>
#include <time.h>
#include <memory>
#include <sstream>
#define private public
#include "Controller.h"
#include "Updater.h"
#undef private
#include "File.h"
#include "Cache.h"
#include "Config.h"
#include "Document.h"
#include "Button.h"
#include "HttpClient.h"
#include "HtmlElement.h"
#include "HtmlParser.h"
#include "HtmlDocument.h"
#include "View.h"
#include "FontFactory.h"
#include "TextAreaFactory.h"
#include "libnds.h"
#include "sans.h"

void runServer(int times)
{
  std::stringstream dbg;
  dbg << "python server.py " << times << " &";
  system(dbg.str().c_str());
  // sleep until the server starts. ugh.
  // this is 0.1 secs
  timespec tenth = {0, 100000000};
  nanosleep(&tenth, 0);
}

Controller *createController()
{
  Controller *c(new Controller());
  c->cache()->setUseCache(true);
  c->m_config = new Config();
  c->m_httpClient->setController(c);
  return c;
}

TEST(UpdaterTest, test_can_read_server)
{
  runServer(1);
  std::auto_ptr<Controller> c(createController());
  URI uri("http://localhost:8000/data/version.txt");
  c->doUri(uri);
  EXPECT_EQ(HtmlParser::TEXT_PLAIN, c->m_document->htmlDocument()->mimeType());

  EXPECT_TRUE(c->m_httpClient->hasPage());
  EXPECT_EQ("# update file\nversion=9.9\n\nURL=http://localhost:8000/data/newversion.zip\n",
      c->m_document->htmlDocument()->data());
}

TEST(UpdaterTest, test_updates)
{
  TextAreaFactory::setFont(FontFactory::create(
        (unsigned char*)_binary_sans_set_bin_start,
        (unsigned char*)_binary_sans_map_bin_start));
  std::auto_ptr<Controller> c(createController());
  c->m_document->setUri("http://localhost:8000/data/version.txt");
  View v(*c->m_document, *c.get());
  // server needs to run twice, once for the version.txt
  // then again for the version.zip file.
  runServer(2);
  // set the configuration...
  Config &config(const_cast<Config&>(c->config()));
  config.callback(Config::UPDATE, "http://localhost:8000/data/version.txt");
  Updater updater(*c.get(), *c->m_document, v);
  updater.show();
  EXPECT_EQ(Updater::GOT_INI, updater.m_state);
  EXPECT_EQ(HtmlParser::TEXT_PLAIN, c->m_document->htmlDocument()->mimeType());
  EXPECT_TRUE(c->m_httpClient->hasPage());
  EXPECT_EQ("# update file\nversion=9.9\n\nURL=http://localhost:8000/data/newversion.zip\n",
      c->m_document->htmlDocument()->data());

  // need to show again! this time it should get the zip...
  updater.show();
  EXPECT_EQ(Updater::GOT_ZIP, updater.m_state);
  EXPECT_EQ(HtmlParser::ZIP, c->m_document->htmlDocument()->mimeType());
  EXPECT_TRUE(updater.m_ok != 0);

  // once we get the zip, check that pressing the button works
  updater.pressed(updater.m_ok);
  EXPECT_EQ(Updater::DO_UPDATE, updater.m_state);
  EXPECT_EQ(nds::File::F_REG, nds::File::exists("newversion.txt"));
  nds::File::rmrf("newversion.txt");
  nds::File::rmrf("data/bunjalloo");
}
