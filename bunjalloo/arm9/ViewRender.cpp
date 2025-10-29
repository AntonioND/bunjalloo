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
#include "ViewRenderHeaders.h"

using namespace std;
const static char * NOT_VIEWABLE("not_view");

ViewRender::ViewRender(View * self):
  m_self(self),
  m_textArea(0),
  m_zipViewer(new ZipViewer(*self)),
  m_updater(0),
  m_lastElement(0)
{
  m_self->document().registerView(this);
}

// Add a link to a "something" - see ImageComponent::addLink and
// RichTextArea::addLink for details
template<typename T, typename A>
void ViewRender::addLink(T *component, A &a)
{
  if (not m_hrefForLink.empty())
  {
    component->addLink(m_hrefForLink, a);
  }
}

RichTextArea * ViewRender::textArea()
{
  if (m_textArea == 0)
  {
    m_textArea = (RichTextArea*)TextAreaFactory::create(TextAreaFactory::TXT_RICH);
    m_textArea->setSize(nds::Canvas::instance().width()-7, m_textArea->font().height());
    m_textArea->addLinkListener(m_self);
    bool parseNewline(m_self->m_document.htmlDocument()->mimeType() == HtmlDocument::TEXT_PLAIN);
    m_textArea->setParseNewline(parseNewline);
    m_richTextAreas.push_back(m_textArea);
  }
  else
  {
    if (m_pendingNewline)
    {
      m_textArea->insertNewline();
    }
  }
  m_pendingNewline = false;
  addLink(m_textArea, m_hrefViewed);
  return m_textArea;
}

static std::string extractImageText(const HtmlElement * element, bool hasAltText)
{
  const std::string & altText = element->attribute("alt");
  // NO! It might not be a HtmlImageElement - could be a HtmlInputElement
  if (hasAltText) {
    return altText;
  }
  const std::string & srcText = element->attribute("src");
  if (not srcText.empty())
  {
    const char * bname = nds::File::base(srcText.c_str());
    string bnamestr(bname);
    bnamestr = "["+ bnamestr+"]";
    return bnamestr;
  }
  return "[IMG]";
}

void ViewRender::setBgColor(const HtmlElement * body)
{
  std::string bgcolor = body->attribute("bgcolor");
  if (not bgcolor.empty())
  {
    unsigned int rgb8 = ((HtmlBodyElement*)body)->bgColor();
    nds::Color col( ((rgb8 >> 16)&0xff)/8, ((rgb8 >> 8)&0xff)/8, (rgb8&0xff)/8);
    /** FIXME - BWT changes
    m_self->m_textArea->setBackgroundColor(col);
    - semi fixed, but not perfect
    */
    m_self->m_scrollPane->setBackgroundColor(col);
  }
}

static std::string uri2filename(const URI &uri) {
  std::string filename;
  switch (uri.protocol())
  {
    case URI::FILE_PROTOCOL:
    case URI::CONFIG_PROTOCOL:
      filename = uri.fileName();
      break;
    case URI::HTTPS_PROTOCOL:
    case URI::HTTP_PROTOCOL:
      filename = Cache::CACHE_DIR;
      filename += "/";
      filename += uri.crc32();
      break;
    default:
      break;
  }
  return filename;
}

void ViewRender::doImage(const std::string & imgStr,
    const std::string & src)
{
  bool show(false);
  m_self->controller().config().resource(Config::SHOW_IMAGES,show);
  if (show)
  {
    // configured to show images inline
    if (not src.empty())
    {
      URI uri(m_self->document().uri());
      const URI &imgUri(uri.navigateTo(src));
      const std::string &filename(uri2filename(imgUri));
      if (filename.empty())
        return;
      nds::Image *image = new nds::Image(filename.c_str());
      ImageComponent *imageComponent = new ImageComponent(image, m_box, &m_self->document());
      add(imageComponent);
      addLink(imageComponent, m_self);
      m_self->controller().queueUri(imgUri);
    }
  }
  else
  {
    // the old image display - shows a text value only + clickage
    if (not imgStr.empty())
    {
      textArea()->addImage(src);
      textArea()->appendText(imgStr);
      textArea()->endImage();
    }
  }
}

void ViewRender::clearRadioGroups()
{
  FormGroupMap::iterator it(m_radioGroup.begin());
  for (; it != m_radioGroup.end(); ++it)
  {
    delete it->second;
  }
  m_radioGroup.clear();
}

void ViewRender::clear()
{
  m_self->m_scrollPane->removeChildren();
  clearRadioGroups();
  m_textArea = 0;
  m_richTextAreas.clear();
  m_pendingNewline = false;
  m_box = new BoxLayout();
  m_box->setSize(249, 192);
}

void ViewRender::renderImage()
{
  URI uri(m_self->m_document.uri());
  string filename;
  if (uri.protocol() == URI::FILE_PROTOCOL)
  {
    filename = uri.fileName();
  }
  else
  {
    filename = m_self->m_controller.cache()->fileName(m_self->m_document.uri());
  }
  nds::Image * image(0);
  if (not filename.empty())
  {
    image = new nds::Image(filename.c_str(),
        (nds::Image::ImageType)m_self->m_document.htmlDocument()->mimeType());
  }
  ImageComponent * imageComponent = new ImageComponent(image, m_box, &m_self->document());
  m_self->m_scrollPane->add(imageComponent);
}

bool ViewRender::hasImage()
{
  HtmlDocument::MimeType mimeType = m_self->m_document.htmlDocument()->mimeType();
  return (mimeType == HtmlDocument::IMAGE_PNG
      or mimeType == HtmlDocument::IMAGE_GIF
      or mimeType == HtmlDocument::IMAGE_JPEG);
}

void ViewRender::render()
{
  const HtmlElement * root = m_self->m_document.rootNode();
  bool useScrollPane(false);

  if (m_updater)
  {
    clear();
    m_updater->show();
    useScrollPane = true;
  }
  else
  {
    HtmlDocument::MimeType mimeType = m_self->m_document.htmlDocument()->mimeType();
    if (hasImage())
    {
      useScrollPane = true;
    }
    else if (mimeType == HtmlParser::ZIP)
    {
      clear();
      URI uri(m_self->m_document.uri());
      string filename;
      if (uri.protocol() == URI::FILE_PROTOCOL)
      {
        filename = uri.fileName();
      }
      else
      {
        filename = m_self->m_controller.cache()->fileName(m_self->m_document.uri());
      }
      m_zipViewer->setFilename(filename);
      m_zipViewer->show();
      useScrollPane = true;
    }
    else if (mimeType == HtmlParser::OTHER)
    {
      clear();
      textArea()->appendText(T(NOT_VIEWABLE));
      textArea()->appendText(": ");
      textArea()->appendText(m_self->m_document.htmlDocument()->mimeTypeValue());
      useScrollPane = false;
    }
    else if (mimeType == HtmlParser::TEXT_PLAIN)
    {
      clear();
      textArea()->appendText(m_self->document().htmlDocument()->data());
      useScrollPane = true;
    }
    else
    {
      clear();
      if (not root->isa(HtmlConstants::HTML_TAG))
        return;
      if (not root->hasChildren())
        return;
      doTitle(m_self->m_document.titleNode());

      HtmlElement * body = (HtmlElement*)root->lastChild();
      if (body->hasChildren())
      {
        body->accept(*this);
      }
      useScrollPane = true;
    }
  }
  done(useScrollPane);
}

void ViewRender::done(bool resetScroller)
{
  pushTextArea();
  m_self->m_scrollPane->add(m_box);
  if (resetScroller)
  {
    m_self->resetScroller();
  }
}

void ViewRender::setUpdater(Updater * updater)
{
  delete m_updater;
  m_updater = updater;
}

void ViewRender::doTitle(const std::string & str)
{
  HtmlElement * newElement = ElementFactory::create(HtmlConstants::TITLE_TAG);
  HtmlElement * text = ElementFactory::create(HtmlConstants::TEXT);
  text->text() = str;
  newElement->append(text);
  doTitle(newElement);
  delete newElement;
}

void ViewRender::doTitle(const HtmlElement * title)
{
  if (title)
  {
    m_textArea = (RichTextArea*)TextAreaFactory::create(TextAreaFactory::TXT_RICH);
    m_textArea->setCentred();
    m_textArea->setOutlined();
    m_textArea->setSize(nds::Canvas::instance().width()-7, m_textArea->font().height());
    HtmlElement * titleText = title->firstChild();
    if (titleText)
    {
      visit(*titleText);
    }
    m_self->m_scrollPane->add(m_textArea);
    m_textArea = 0;
  }
}

void ViewRender::renderSelect(const HtmlElement * selectElement)
{
  Select * select = new Select(const_cast<HtmlElement*>(selectElement));
  select->init();
  add(select);
}

void ViewRender::pushTextArea()
{
  if (m_textArea) {
    m_box->add(m_textArea);
    m_textArea = 0;
  }
}

void ViewRender::insertNewline()
{
  m_pendingNewline = true;
  if (m_textArea == 0)
  {
    m_box->insertNewline();
    m_pendingNewline = false;
  }
}

void ViewRender::add(Component *component)
{
  pushTextArea();
  if (m_pendingNewline)
  {
    m_box->insertNewline();
    m_pendingNewline = false;
  }
  m_box->add(component);
}

// FIXME - where should this go?
static const int MAX_SIZE(SCREEN_WIDTH-7);
static const int MIN_SIZE(8);

void ViewRender::renderInput(const HtmlElement * inputElement)
{
  string sizeText = inputElement->attribute("size");
  int size(0);
  if (not sizeText.empty())
  {
    size = strtol(sizeText.c_str(), 0, 0);
    size *= 8; // FIXME! should be: textArea->font().height(); ?
  }
  if (size > MAX_SIZE)
    size = MAX_SIZE;

  HtmlInputElement::InputType type = ((const HtmlInputElement*)inputElement)->inputType();
  switch (type)
  {
    case HtmlInputElement::IMAGE:
    case HtmlInputElement::SUBMIT:
      {
        bool hasAltText = ((HtmlInputElement*)inputElement)->hasAltText();
        FormControl * submitButton = new FormControl(inputElement,
            type == HtmlInputElement::SUBMIT?(
            inputElement->attribute("value").empty()?
            "Submit" : inputElement->attribute("value")):
            extractImageText(inputElement, hasAltText)
            );
        submitButton->setListener(m_self);
        // m_textArea = 0;
        if (size > MIN_SIZE)
        {
          submitButton->setSize(size, submitButton->preferredSize().h);
        }
        /* m_self->m_scrollPane->add(submitButton); */
        add(submitButton);
      }
      break;
    case HtmlInputElement::TEXT:
      {
        InputText * text = new InputText(const_cast<HtmlElement*>(inputElement));
        // m_textArea = 0;
        if (size <= 0)
          size = MIN_SIZE*8; // FIXME! textArea->font().height();
        text->setListener(m_self->m_keyboard);
        text->setSize(size, text->preferredSize().h);
        /*m_self->m_scrollPane->add(text);*/
        add(text);
      }
      break;
    case HtmlInputElement::PASSWORD:
      {
        // TODO - password field should show *** instead of real text.
        PasswordField * text = new PasswordField(const_cast<HtmlElement*>(inputElement));
        // m_textArea = 0;
        if (size <= 0)
          size = MIN_SIZE*8; // FIXME! textArea->font().height();
        text->setListener(m_self->m_keyboard);
        text->setSize(size, text->preferredSize().h);
        // m_self->m_scrollPane->add(text);
        add(text);
      }
      break;
    case HtmlInputElement::CHECKBOX:
      {
        // m_textArea = 0;
        FormCheckBox * checkbox = new FormCheckBox(const_cast<HtmlElement*>(inputElement));
        add(checkbox);
      }
      break;
    case HtmlInputElement::RADIO:
      {
        // see if there is a RadioGroup with this name
        std::string name = inputElement->attribute("name");
        // FIXME - get the group.
        // m_textArea = 0;
        RadioButton * radio = new RadioButton;
        add(radio);
        if (not name.empty())
        {
          FormGroupMap::iterator it(m_radioGroup.find(name));
          if (it != m_radioGroup.end())
          {
            it->second->add(radio, const_cast<HtmlElement*>(inputElement));
          }
          else
          {
            FormRadio * fr = new FormRadio;
            fr->add(radio, const_cast<HtmlElement*>(inputElement));
            m_radioGroup[name] = fr;
          }
        }
      }
      break;
    case HtmlInputElement::HIDDEN:
      break;
  }
}

void ViewRender::renderTextArea(const HtmlElement * textAreaElement)
{
  FormTextArea * text = new FormTextArea(const_cast<HtmlElement*>(textAreaElement));
  text->setListener(m_self->m_keyboard);
  m_textArea = 0;
  add(text);
}

void ViewRender::notify()
{
  Document::Status status(m_self->document().status());
  static int progressId(0);

  switch (status)
  {
    case Document::LOADED_ITEM:
    case Document::LOADED_HTML:
      {
        progressId = 0;
        m_self->m_scrollPane->setSize(nds::Canvas::instance().width(), nds::Canvas::instance().height());
      }
      break;
    case Document::INPROGRESS:
      {
        if (hasImage() and m_self->controller().downloadingFile() == m_self->document().uri())
        {
          if (progressId == 0)
          {
            clear();
            renderImage();
            m_self->resetScroller();
          }
        }
        progressId++;
      }
      break;
    default:
      if (hasImage() and m_self->controller().downloadingFile() == m_self->document().uri())
      {
        clear();
        renderImage();
      }
      progressId = 0;
      break;
  }
}

// Visitor implementation
void ViewRender::begin(HtmlAnchorElement & element)
{
  const std::string & href(element.attribute("href"));
  m_hrefViewed = false;
  m_hrefForLink.clear();
  if (not href.empty())
  {
    URI newUri = URI(m_self->m_document.uri()).navigateTo(href);
    m_hrefViewed = m_self->m_controller.cache()->contains(newUri, false);
    m_hrefForLink = href;
  }
  else
  {
    textArea()->addLink(href);
  }
}

bool ViewRender::visit(HtmlAnchorElement & element)
{
  return true;
}
void ViewRender::end(HtmlAnchorElement & element)
{
  if (m_textArea)
  {
    m_textArea->endLink();
  }
  m_hrefForLink.clear();
}

void ViewRender::begin(HtmlBlockElement & element)
{
  if (element.isBlock())
  {
    textArea()->insertNewline();
  }
}
bool ViewRender::visit(HtmlBlockElement & element)
{
  return true;
}
void ViewRender::end(HtmlBlockElement & element)
{
  if (element.isBlock())
  {
    textArea()->insertNewline();
  }
}

void ViewRender::begin(HtmlBodyElement & element)
{
}
bool ViewRender::visit(HtmlBodyElement & element)
{
  return true;
}
void ViewRender::end(HtmlBodyElement & element)
{
}

void ViewRender::begin(HtmlElement & element)
{
  if (element.isa(HtmlConstants::UL_TAG) or element.isa(HtmlConstants::OL_TAG))
  {
    // FIXME!!
    /** m_self->m_textArea->increaseIndent(); */
    //if (not element.isBlock() and element.parent()->isa(HtmlConstants::LI_TAG))
    textArea()->insertNewline();
  }
  else if (element.isa(HtmlConstants::LI_TAG)) {
    const HtmlElement * prev(element.parent()->previousSibling(&element));
    if (prev and prev->isa(HtmlConstants::TEXT)) // TODO - remove #TEXT and so forth
    {
      textArea()->insertNewline();
    }
  }
}

bool ViewRender::visit(HtmlElement & element)
{
  if (not element.text().empty() and element.isa(HtmlConstants::TEXT))
  {
    const std::string &et(element.text());
    textArea()->appendText(et.c_str());
  }
  else if (element.isa(HtmlConstants::BR_TAG))
  {
    insertNewline();
  }
  return true;
}
void ViewRender::end(HtmlElement & element)
{
  if (element.isa(HtmlConstants::LI_TAG))
  {
    ElementList::const_iterator it(element.children().begin());
    ElementList::const_iterator end(element.children().end());
    bool hasBlock(false);
    for (; it != end; ++it)
    {
      if ( (*it)->isa(HtmlConstants::UL_TAG) or (*it)->isBlock()) {
        hasBlock = true;
        break;
      }
    }
    if (not hasBlock)
      textArea()->insertNewline();
  }
  else if (element.isa(HtmlConstants::P_TAG) or (element.tagName()[0] == 'h' and (element.tagName()[1] >= '1' and element.tagName()[1] <= '6')))
  {
    textArea()->insertNewline();
  }
}

void ViewRender::begin(HtmlFormElement & element)
{
  pushTextArea();
  m_box->insertNewline();
  m_pendingNewline = false;
}
bool ViewRender::visit(HtmlFormElement & element)
{
  return true;
}
void ViewRender::end(HtmlFormElement & element)
{
}

void ViewRender::begin(HtmlImageElement & element)
{
}
bool ViewRender::visit(HtmlImageElement & element)
{
  // hurrah for alt text. some people set it to "", which screws up any
  // easy way to display it (see w3m google.com - Google [hp1] [hp2] [hp3]... huh?)
  bool hasAltText = element.hasAltText();
  const std::string &imgText = extractImageText(&element, hasAltText);
  if (m_textArea) {
    m_textArea->endLink();
  }
  doImage(imgText, element.attribute("src"));
  return true;
}
void ViewRender::end(HtmlImageElement & element)
{
}

void ViewRender::begin(HtmlInputElement & element)
{
}
bool ViewRender::visit(HtmlInputElement & element)
{
  if (element.isa(HtmlConstants::SELECT_TAG))
  {
    renderSelect(&element);
    return false;
  }
  else if (element.isa(HtmlConstants::INPUT_TAG))
  {
    renderInput(&element);
  }
  return true;
}
void ViewRender::end(HtmlInputElement & element)
{
}

void ViewRender::begin(HtmlMetaElement & element)
{
}
bool ViewRender::visit(HtmlMetaElement & element)
{
  return true;
}
void ViewRender::end(HtmlMetaElement & element)
{
}

void ViewRender::begin(HtmlOptionElement & element)
{
}
bool ViewRender::visit(HtmlOptionElement & element)
{
  return true;
}
void ViewRender::end(HtmlOptionElement & element)
{
}

void ViewRender::begin(HtmlPreElement & element)
{
  textArea()->setParseNewline(true);
}
bool ViewRender::visit(HtmlPreElement & element)
{
  return true;
}
void ViewRender::end(HtmlPreElement & element)
{
  textArea()->setParseNewline(false);
}

void ViewRender::begin(HtmlTextAreaElement & element)
{
}
bool ViewRender::visit(HtmlTextAreaElement & element)
{
  renderTextArea(&element);
  return false;
}
void ViewRender::end(HtmlTextAreaElement & element)
{
}

void ViewRender::textAreas(std::list<RichTextArea*>& textAreas)
{
  textAreas = m_richTextAreas;
}
