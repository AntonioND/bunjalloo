/*
  Copyright (C) 2007,2008 Richard Quirk

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

#include <cassert>
#include <libgen.h>
#include <cstdio>
#include "ndspp.h"
#include "config_defs.h"
#include "libnds.h"
#include "BrowseToolbar.h"
#include "BookmarkToolbar.h"
#include "Canvas.h"
#include "Config.h"
#include "Controller.h"
#include "CookieHandler.h"
#include "Document.h"
#include "EditPopup.h"
#include "HtmlDocument.h"
#include "File.h"
#include "FormControl.h"
#include "HrefFinder.h"
#include "Language.h"
#include "InternalVisitor.h"
#include "HtmlElement.h"
#include "Keyboard.h"
#include "Link.h"
#include "LinkHandler.h"
#include "PreferencesToolbar.h"
#include "ProgressBar.h"
#include "ScrollPane.h"
#include "SearchEntry.h"
#include "System.h"
#include "Stylus.h"
#include "TextField.h"
#include "RichTextArea.h"
#include "URI.h"
#include "View.h"
#include "ViewRender.h"
#include "WidgetColors.h"

using namespace std;
const static char * ENTER_URL_TITLE("enter_url");
const static char * EDIT_BOOKMARK_TITLE("edit_bm");
const static char * SAVE_AS_TITLE("save_as");
const static char * ENTER_TEXT_TITLE("enter_text");
const static int STEP(1);
const static char * SEARCH_TEMPLATE = NITRODIR "/docs/search-example.txt";

View::View(Document & doc, Controller & c):
  m_document(doc),
  m_controller(c),
  m_renderer(this),
  m_browseToolbar(new BrowseToolbar(*this)),
  m_bookmarkToolbar(new BookmarkToolbar(*this)),
  m_prefsToolbar(new PreferencesToolbar(*this)),
  m_toolbar(m_browseToolbar),
  m_progress(new ProgressBar(0, 100)),
  m_linkHandler(this),
  m_editPopup(new EditPopup(this)),
  m_cookieHandler(new CookieHandler(this))
{
  m_scrollPane.setTopLevel();
  m_scrollPane.setLocation(0, 0);
  m_scrollPane.setSize(nds::Canvas::instance().width(), nds::Canvas::instance().height());
  m_scrollPane.setScrollIncrement(20);
  m_keyboard.setTopLevel(&m_scrollPane);
  m_keyboard.setTitle(T(ENTER_TEXT_TITLE));
  m_document.registerView(this);
  keysSetRepeat( 10, 5 );
  m_toolbar->setVisible(true);
  string searchFile;
  if (m_controller.config().resource(Config::SEARCHFILE_STR,searchFile))
  {
    if (nds::File::exists(searchFile.c_str()) == nds::File::F_NONE)
    {
      // copy the template file there instead
      Config::copyTemplate(SEARCH_TEMPLATE, searchFile.c_str());
    }
    m_search = new SearchEntry(searchFile);
  }
  m_progress->setSize(250, 10);
  m_progress->setLocation(2, 172);
}

View::~View()
{
  delete m_browseToolbar;
  delete m_bookmarkToolbar;
  delete m_search;
  Stylus::deleteInstance();
}

void View::extractTitle()
{
  // do not store bookmark as the title
  if (m_state == BOOKMARK)
    return;
  // use title text..
  // extract the title from the document
  // only allow bookmarking of html? no, allow any old crap - firefox does.
  // so, extract either the title, or the basename (remove cgi stuff?)
  const HtmlElement * title = m_document.titleNode();
  if (title)
  {
    const std::string &titleText = title->firstChild()->text();
    m_bookmarkTitleUtf8 = titleText;
  }
  else
  {
    URI tmpUri(m_document.uri());
    m_bookmarkTitleUtf8 = tmpUri.fileName();
    m_bookmarkTitleUtf8 = nds::File::base(m_bookmarkTitleUtf8.c_str());
    if (m_bookmarkTitleUtf8.empty())
    {
      m_bookmarkTitleUtf8 = tmpUri.asString();
    }
  }
}

void View::notify()
{
  Document::Status status(m_document.status());

  switch (status) {
    case Document::REDIRECTED:
        m_filenameForProgress.clear();
        break;
    case Document::LOADED_HTML:
      {
        // this is to clear the progress's dirty flag.
        m_progress->paint(m_progress->bounds());
        m_progress->setVisible(false);
        m_filenameForProgress.clear();
        // extract the *current* title
        extractTitle();
        m_renderer.render();
        int pos = m_document.position();
        if (pos == -1)
        {
          // is it relative?
          pos = internalLinkPos();
        }
        m_scrollPane.scrollToPercent(pos);
        //m_scrollPane.scrollToAbsolute(pos);
        m_dirty = true;
        string refresh;
        int refreshTime;
        m_document.refresh(refresh, refreshTime);
        if (not refresh.empty() and refreshTime >= 0)
        {
          m_refreshing = (refreshTime+1) * 30;
        }
        else
        {
          m_refreshing = 0;
        }
      }
      break;
    case Document::INPROGRESS:
      {
        m_progress->setMaximum(100);
        m_progress->setMinimum(0);
        if (not m_document.historyEnabled()) // FIXME: should be "downloading embedded images"
        {
          tick(); // ?
          return;
        }
        unsigned int pc = m_document.percentLoaded();
        m_progress->setValue(pc);
        if (m_filenameForProgress.empty())
        {
          const URI &u(m_controller.downloadingFile());
          m_filenameForProgress = nds::File::base(u.fileName().c_str());
          if (m_filenameForProgress.empty())
          {
            m_filenameForProgress = "index";
          }
        }
        char buffer[10];
        string s(m_filenameForProgress);
        snprintf(buffer, sizeof(buffer), " %d%%", pc);
        s += buffer;
        m_progress->setVisible();
        m_keyboard.forceRedraw();
        m_scrollPane.forceRedraw();
      }
      break;
    case Document::HAS_HEADERS:
      {
        if (not m_saveAsEnabled)
          return;
        switch (m_document.htmlDocument()->mimeType())
        {
          case HtmlParser::OTHER:
            {
              if (m_state == BROWSE and m_controller.downloadingFile() != m_document.uri())
              {
                if (m_controller.downloadQueueIsEmpty())
                {
                  m_controller.stop();
                }
                break;
              }
              saveAs();
              m_state = SAVE_DOWNLOADING;
            }
            break;

          case HtmlParser::UNINITIALISED:
          default:
            // can see it
            break;
        }
      }
    default:
      break;
  }
}

void View::enterUrl()
{
  m_addressBar.setText(m_document.uri());
  m_keyboard.setTitle(T(ENTER_URL_TITLE));
  m_keyboard.editText(&m_addressBar);
  m_toolbar->setVisible(false);
  m_state = ENTER_URL;
  m_dirty = true;
}

void View::editBookmark()
{
  m_addressBar.setText(m_editPopup->details());
  m_keyboard.setTitle(T(EDIT_BOOKMARK_TITLE));
  m_keyboard.editText(&m_addressBar);
  m_toolbar->setVisible(false);
  m_state = EDIT_BOOKMARK;
  m_dirty = true;
}

void View::setToolbar(Toolbar * toolbar)
{
  m_toolbar->setVisible(false);
  m_toolbar = toolbar;
  m_toolbar->setVisible(true);
}

void View::endBookmark()
{
  m_document.clearConfigHistory();
  m_document.setHistoryEnabled(false);
  m_controller.clearReferer();
  if (m_linkHref.empty())
    m_linkHref = m_document.uri();
  m_document.setHistoryEnabled(true);
  m_state = BROWSE;

  setToolbar(m_browseToolbar);
  m_renderer.setUpdater(0);
}

void View::bookmarkUrl()
{
  // Add a new entry to the bookmarks file at DATADIR/userdata/bookmarks.html
  setToolbar(m_bookmarkToolbar);
  m_state = BOOKMARK;
  showBookmarkPage();
}

void View::showBookmarkPage()
{
  m_document.setHistoryEnabled(false);

  URI bookmarksUrl(Config::BOOKMARK_URL);
  assert(bookmarksUrl.server() == "bunjalloo"); // The file must be in DATADIR
  std::string bookmarksPath(std::string(DATADIR) + bookmarksUrl.fileName());

  if (nds::File::exists(bookmarksPath.c_str()) == nds::File::F_NONE)
  {
    // create it
    nds::File bookmarks;
    // doesn't exist, so write out the header...
    bookmarks.open(bookmarksPath.c_str(), "w");
    if (not bookmarks.is_open()) {
      // that's unpossible!
      return;
    }
    const static string header("<meta http-equiv='Content-Type' content='text/html; charset=utf-8'><title>Bookmarks</title>\n");
    bookmarks.write(header.c_str(), header.length());
  }
  m_controller.doUri(bookmarksUrl);
  m_document.setHistoryEnabled(true);
}

void View::editConfig()
{
  // show a config page with values for the configuration :-/
  URI configFile("config://index.html");
  m_controller.doUri(configFile);
}

void View::bookmarkCurrentPage()
{
  {
    URI bookmarksUrl(Config::BOOKMARK_URL);
    assert(bookmarksUrl.server() == "bunjalloo"); // The file must be in DATADIR
    std::string bookmarksPath(std::string(DATADIR) + bookmarksUrl.fileName());

    nds::File bookmarks;
    // OK - add to the bm file
    bookmarks.open(bookmarksPath.c_str(), "a");
    if (bookmarks.is_open())
    {
      // write out "<a href=%1>%2</a>" 1=href 2=title
      string href("<a href='");
      bookmarks.write(href.c_str(), href.length());
      const string & uri(m_document.uri());
      bookmarks.write(uri.c_str(), uri.length());
      href = "'>";
      bookmarks.write(href.c_str(), href.length());
      // output titleText to bookmarks
      bookmarks.write(m_bookmarkTitleUtf8.c_str(), m_bookmarkTitleUtf8.length());
      href = "</a><br>\n";
      bookmarks.write(href.c_str(), href.length());
    }
  }
  showBookmarkPage();
}

void View::addCookie()
{
  // add cookie for the current page.
  // endBookmark() // doesn't work
  m_document.clearConfigHistory();
  m_document.setHistoryEnabled(true);
  URI uri(m_document.uri());
  m_document.setHistoryEnabled(false);
  if (uri.protocol() == URI::HTTPS_PROTOCOL or
      uri.protocol() == URI::HTTP_PROTOCOL)
  {
    m_renderer.clear();
    m_cookieHandler->setMode(CookieHandler::ADD_MODE);
    m_cookieHandler->show();
  }
}

void View::editCookie()
{
  // edit the list of cookies
  m_renderer.clear();
  m_cookieHandler->setMode(CookieHandler::EDIT_MODE);
  m_cookieHandler->show();
}

void View::preferences()
{
  // clicked preferences button.
  // This should allow:
  // 1) Homepage (?) issues 30, 38
  // 2) Cookie file edit, issue 36
  // 3) Add current server to cookie file (+edit?), issue 36
  // 4) shortcut file edition (Add field?)

  // Solution: new toolbar! PreferencesToolbar.cpp
  // back
  // edit cookie file
  // add cookie + edit
  // add shortcut + edit
  setToolbar(m_prefsToolbar);
  editConfig();
}

void View::makeNiceFileName(std::string & fileName)
{
  string tmp = m_document.htmlDocument()->getContentDisposition();
  if (not tmp.empty())
  {
    fileName = tmp;
  }
  else if (fileName.empty())
  {
    fileName = "index.html";
  }
  else
  {
    // strip off any cgi stuff.
    size_t pos = fileName.find('?');
    if (pos != string::npos)
    {
      fileName = fileName.substr(0,pos);
    }
  }
}

void View::setSaveAsEnabled(bool enabled)
{
  m_saveAsEnabled = enabled;
}

void View::saveAs()
{
  // save file as ???
  URI uri(m_document.uri());
  // FIXME: use Content-Disposition header, if available.
  string fileName(nds::File::base(uri.fileName().c_str()));
  makeNiceFileName(fileName);

  m_addressBar.setText(fileName);
  m_keyboard.setTitle(T(SAVE_AS_TITLE));
  m_keyboard.editText(&m_addressBar);
  m_toolbar->setVisible(false);
  m_state = SAVE_CURRENT_FILE;
  m_dirty = true;
}

void View::updateInput()
{
  m_keyState.initialise(
      keysDownRepeat(),
      keysDown(),
      keysHeld(),
      keysUp());

  touchPosition tp;
  touchRead(&tp);

  Stylus::TouchType touchType =
      Stylus::keysToTouchType(m_keyState.isHeld(KEY_TOUCH), m_keyState.isUp(KEY_TOUCH));

  Stylus::instance()->update(touchType, m_keyState.isRepeat(KEY_TOUCH),
                             tp.px, tp.py+SCREEN_HEIGHT);
}

enum {
  HANDY_RIGHT,
  HANDY_LEFT,
  HANDY_UP,
  HANDY_DOWN,
  HANDY_B,
  HANDY_X,
  HANDY_Y,
  HANDY_A
};
// Keys for right handed people
static unsigned short s_righthanded[] = {
  KEY_RIGHT,
  KEY_LEFT,
  KEY_UP,
  KEY_DOWN,
  KEY_B,
  KEY_X,
  KEY_Y,
  KEY_A
};

// Keys for left handed people
static unsigned short s_lefthanded[] = {
  KEY_B,
  KEY_X,
  KEY_Y,
  KEY_A,
  KEY_RIGHT,
  KEY_LEFT,
  KEY_UP,
  KEY_DOWN
};

static unsigned short *s_currenthand(0);

unsigned short handy2key(int val)
{
  return s_currenthand[val];
}

void View::browse()
{
  bool lefty(false);
  if (m_controller.config().resource(Config::LEFTY,lefty))
  {
    s_currenthand = lefty?s_lefthanded:s_righthanded;
  }
  /* Default to my handedness */
  if (!s_currenthand)
    s_currenthand = s_righthanded;

  updateInput();

  if (not m_keyboard.visible())
  {
    if (m_keyState.isRepeat(handy2key(HANDY_A))) {
      enterUrl();
    }
    if (m_keyState.isRepeat(KEY_SELECT)) {
      m_toolbar->cyclePosition();
    }
    if (m_keyState.isRepeat(handy2key(HANDY_DOWN))) {
      // scroll down ...
      m_scrollPane.down();
    }
    if (m_keyState.isRepeat(handy2key(HANDY_UP))) {
      // scroll up ...
      m_scrollPane.up();
    }
    if (m_keyState.isRepeat(handy2key(HANDY_RIGHT))) {
      // scroll down ...
      m_scrollPane.pageDown();
    }
    if (m_keyState.isRepeat(handy2key(HANDY_LEFT))) {
      // scroll up ...
      m_scrollPane.pageUp();
    }
    if (m_keyState.isRepeat(KEY_L)) {
      if (m_toolbar == m_browseToolbar)
        m_controller.previous();
    }
    if (m_keyState.isRepeat(KEY_R)) {
      if (m_toolbar == m_browseToolbar)
        m_controller.next();
    }

    if (m_keyState.isRepeat(handy2key(HANDY_Y))) {
      bookmarkUrl();
    }
    if (m_keyState.isRepeat(handy2key(HANDY_X))) {
      preferences();
      editConfig();
    }
    if (m_keyState.isRepeat(handy2key(HANDY_B))) {
      stopOrReload();
    }
  }

  if (m_keyState.isRepeat(KEY_START)) {
    // render the node tree
    m_document.dumpDOM();
  }

  // change to keys actually down, not repeating
  Stylus * stylus(Stylus::instance());
  if (stylus->touchType() != Stylus::NOTHING)
  {
    m_dirty = m_keyboard.visible() and m_keyboard.dirty();
    if (not m_dirty) {
      m_dirty = m_scrollPane.dirty();
      if (m_dirty)
      {
        m_document.setPosition( m_scrollPane.currentPosition());
      }
    }
    m_toolbar->setVisible(!m_keyboard.visible());
  }
  // else --- add drag gestures, etc..
  if (m_scrollPane.visible() and m_dirty)
  {
    m_document.setPosition( m_scrollPane.currentPosition());
  }

  if (m_refreshing > 0)
  {
    m_refreshing--;
    if (m_refreshing == 0)
    {
      int tmp;
      m_document.refresh(m_linkHref, tmp);
    }
  }
}

void View::stopOrReload()
{
  if (document().status() == Document::LOADED_PAGE)
  {
    controller().reload();
  }
  else
  {
    controller().stop();
  }
}

void View::pressed(ButtonI * button)
{
  if (m_form)
    return;

  FormControl * formControl = (FormControl*)button;
  m_form = formControl;
}

void View::linkClicked(Link * link)
{
  // there are types of link
  // 1) Anchor only
  // 2) Image only
  // 3) Image and anchor

  bool isAnchor = link->eventType() == Link::STATE_LINK;
  bool isImage  = link->color() == WidgetColors::LINK_IMAGE;

  m_controller.setReferer(m_document.uri());
  if (isAnchor and not isImage)
  {
    m_linkHref = link->href();
  }
  else if (isImage and not isAnchor)
  {
    // image link?
    m_linkHref = link->src();
  }
  else // isAnchor and isImage
  {
    Stylus * stylus(Stylus::instance());
    m_linkHandler.setLink(link);
    m_linkHandler.setLocation(stylus->lastX(), stylus->lastY());
    m_linkHandler.setVisible();
  }
}

void View::linkPopup(Link * link)
{
  // if bookmark page, delete bookmark?
  URI uri(m_document.uri());
  if (m_state == BOOKMARK)
  {
    // delete the link from the bookmark... ulp

    HrefFinder visitor(link->href());
    HtmlElement * root((HtmlElement*)m_document.rootNode());
    root->accept(visitor);
    if (visitor.found())
    {
      HtmlElement * el(visitor.element());
      Stylus * stylus(Stylus::instance());
      m_editPopup->setElement(el);
      m_editPopup->setLocation(stylus->lastX(), stylus->lastY());
      m_editPopup->setVisible();
    }
  }
}

void View::keyboard()
{
  updateInput();
}

void View::tick()
{
  scanKeys();
  switch (m_state)
  {
    case BROWSE:
      browse();
      break;
    case ENTER_URL:
    case EDIT_BOOKMARK:
    case SAVE_CURRENT_FILE:
    case SAVE_DOWNLOADING:
      keyboard();
      break;
    case BOOKMARK:
      // have a page of bookmarks or what?
      // need to be able to
      // 1) jump to bookmark
      // 2) edit bookmark
      // 3) add current page easily
      // 4) add any page
      // for now cheap solution: have an Add button and change the Toolbar state.
      browse();
      break;
  }
  m_dirty |= m_keyboard.tick();
  m_dirty |= m_cookieHandler->tick();
  m_dirty |= m_progress->visible() and m_progress->dirty();
  m_dirty |= m_scrollPane.visible() and m_scrollPane.dirty();
  m_toolbar->tick();
  m_toolbar->updateIcons();

  if (m_dirty) {
    const static nds::Rectangle clip(0, 0, nds::Canvas::instance().width(), nds::Canvas::instance().height());
    m_scrollPane.paint(clip);
    m_keyboard.paint(clip);
    m_linkHandler.paint(clip);
    m_editPopup->paint(clip);
    m_progress->paint(m_progress->bounds());
    nds::Canvas::instance().endPaint();
    m_dirty = false;
    m_scrollPane.setVisible(not m_keyboard.visible());
  }

  if (m_state != BROWSE and not m_keyboard.visible())
  {
    m_keyboard.setTitle(T(ENTER_TEXT_TITLE));
  }
  if (m_state == ENTER_URL and not m_keyboard.visible()) {
    m_toolbar->setVisible(true);
    doEnterUrl();
  }

  if (m_state == EDIT_BOOKMARK and not m_keyboard.visible()) {
    m_toolbar->setVisible(true);
    doEditBookmark();
  }

  if (( m_state == SAVE_CURRENT_FILE or m_state == SAVE_DOWNLOADING )
    and not m_keyboard.visible()) {
    m_toolbar->setVisible(true);
    doSaveAs();
  }

  // clicked a link:
  if (not m_linkHref.empty()) {
    URI uri(m_document.uri());
    if (m_state == BOOKMARK)
    {
      endBookmark();
      uri = m_linkHref;
    }
    string tmp(m_linkHref);
    m_linkHref = "";
    // cout << "Navigated to " << m_linkHref << endl;
    // TODO - "navigate or download"..
    m_controller.doUri( uri.navigateTo(tmp).asString() );
  }

  if (m_form)
  {
    URI uri(m_document.uri());
    FormControl * tmp = m_form;
    m_form = 0;

    tmp->input(m_controller, uri);
    m_state = BROWSE;
  }
}

void View::doEnterUrl()
{
  m_state = BROWSE;
  string newAddress = m_keyboard.result();
  if (not newAddress.empty() and m_keyboard.selected() == Keyboard::OK)
  {
    // check for search
    string result;
    if (m_search and m_search->checkSearch(newAddress, result))
    {
      newAddress = result;
    }
    m_toolbar->setVisible(true);
    m_controller.clearReferer();
    m_controller.doUri(newAddress);
  }
}

void View::doEditBookmark()
{
  const std::string &value = m_keyboard.result();
  if (not value.empty() and m_keyboard.selected() == Keyboard::OK)
  {
    m_editPopup->postEdit(value);
  }
  bookmarkUrl();
}

void View::doSaveAs()
{
  string fileName = m_keyboard.result();
  if (not fileName.empty() and m_keyboard.selected() == Keyboard::OK)
  {
    m_toolbar->setVisible(true);
    m_controller.saveAs(fileName.c_str(),
        m_state==SAVE_DOWNLOADING?Controller::SAVE_DOWNLOADING:Controller::SAVE_CURRENT_FILE);
  }
  else
  {
    // this does nothing on already loaded pages.
    // cancels save on download in progress stuff
    m_controller.stop();
  }
  m_state = BROWSE;
}

void View::setUpdater(Updater * updater)
{
  m_renderer.setUpdater(updater);
}

void View::resetScroller()
{
  m_scrollPane.setLocation(0,0);
  m_scrollPane.setSize(nds::Canvas::instance().width(), nds::Canvas::instance().height());
  m_scrollPane.setSize(nds::Canvas::instance().width(), nds::Canvas::instance().height());
  m_scrollPane.scrollToPercent(0);
}

ViewRender * View::renderer()
{
  return &m_renderer;
}

int View::internalLinkPos()
{
  URI uri(m_document.uri());
  const string & internal(uri.internalLink());
  if (not internal.empty())
  {
    // do some stuff with internal links
    InternalVisitor visitor(internal);
    HtmlElement * root((HtmlElement*)m_document.rootNode());
    root->accept(visitor);
    if (visitor.found())
    {
      std::list<RichTextArea*> richTextAreas;
      m_renderer.textAreas(richTextAreas);
      int linksFound = 0;
      // search in each RichTextArea until we find the link position
      for (std::list<RichTextArea*>::iterator it(richTextAreas.begin());
          it != richTextAreas.end(); ++it)
      {
        RichTextArea * text(*it);
        int links = text->linkCount();
        if ((linksFound + links) > visitor.index())
        {
          // if (link is in this text area)...
          int linkInText = visitor.index() - linksFound;
          unsigned int linkPos = text->linkPosition(linkInText)  - 192;
          return (linkPos * 256) / (m_scrollPane.visibleHeight());
        }
        linksFound += links;
      }
    }
  }
  return -1;
}
