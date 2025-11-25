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
//#include <iostream>
#include <assert.h>
#include <cstdlib>
#include <cstdio>
#include <ctype.h>
#include <algorithm>
#include "Entity.h"
#include "HtmlElement.h"
#include "HtmlParser.h"
#include "ParameterSet.h"
#include "string_utils.h"
#include "ISO_8859_1.h"
#include "utf8.h"
#include "File.h"
#include "Delete.h"

using namespace std;

const char * HtmlParser::IMAGE_JPEG_STR = "image/jpeg";
const char * HtmlParser::IMAGE_JPG_STR = "image/jpg"; // some people...

//! The private implementation of the parser.
class HtmlParserImpl
{

  public:
    enum TokeniserState {
      BOM,
      DATA,
      ENTITY_DATA,
      TAG_OPEN,
      CLOSE_TAG_OPEN,
      TAG_NAME,
      BEFORE_ATTRIBUTE_NAME,
      ATTRIBUTE_NAME,
      AFTER_ATTRIBUTE_NAME,
      BEFORE_ATTRIBUTE_VALUE,
      ATTRIBUTE_VALUE_DOUBLE_QUOTE,
      ATTRIBUTE_VALUE_SINGLE_QUOTE,
      ATTRIBUTE_VALUE_UNQUOTED,
      ENTITY_IN_ATTRIBUTE_VALUE,
      BOGUS_COMMENT,
      MARKUP_DECLARTION_OPEN,
      COMMENT,
      COMMENT_DASH,
      COMMENT_END,
      DOCTYPE,
      BEFORE_DOCTYPE_NAME,
      DOCTYPE_NAME,
      AFTER_DOCTYPE_NAME,
      BOGUS_DOCTYPE,
    };

    enum TagType {
      START,
      END,
      START_END
    };

    HtmlParserImpl(HtmlParser & self)
      : m_self(self) ,
      m_encoding(HtmlParser::UTF8_ENCODING)
    {}

    void initialise(const char * data, unsigned int length);

    void fire();
    void debug();

    inline void setEncoding(HtmlParser::Encoding enc)
    {
      m_encoding = enc;
    }

    inline HtmlParser::Encoding encoding()
    {
      return m_encoding;
    }

    inline unsigned int value() const
    {
      return m_value;
    }

    inline const char * position() {
      return m_position;
    }
    inline const char * end() {
      return m_end;
    }

    void reset();

    inline void setContentModel(HtmlParser::ContentModel model)
    {
      m_contentModel = model;
    }

    inline void setContentDisposition(const std::string & disposition)
    {
      m_contentDisposition = disposition;
    }

    const std::string & getContentDisposition() const
    {
      return m_contentDisposition;
    }

    void refresh(std::string & refresh, int & time) const;
    void setRefresh(const std::string & refresh, int time);
    void setCacheFile(const std::string & filename);

  private:
    static const unsigned int NBSP = 160;
    const char * m_input;
    const char * m_position;
    const char * m_lastPosition;
    const char * m_end;
    unsigned int m_value;
    HtmlParser & m_self;
    HtmlParser::Encoding m_encoding;
    //! last state is for the entity in tokenise
    TokeniserState m_lastState;
    TokeniserState m_state;
    HtmlParser::ContentModel m_contentModel;
    string m_lastStartTagToken;
    TagType m_tagTokenType;
    string m_currentTagToken;
    //! All the attributes for this tag.
    AttributeVector m_tagAttributes;
    //! Current attribute to be added to this tag..
    Attribute* m_attribute;
    string m_commentToken;
    string m_doctypeToken;
    //! Sometimes the chunking screws us over.
    string m_leftOvers;
    string m_contentDisposition;

    //! URL for refresh header.
    string m_refresh;
    //! time for refresh header.
    int m_refreshTime;

    bool m_doctypeTokenIsError;

    //! Name of the html file in the cache to save to.
    string m_cacheFilename;

    void next();
    //! rewind one character to reconsume it.
    void rewind();
    //! Emit a character...
    void emit(unsigned int toEmit);
    void emitDoctype(string & token, bool isError);
    void emitTagToken();
    void emitComment();
    unsigned int consumeEntity();
    // deal with wonky chunkers
    void setLeftovers();

    string asUnconsumedCharString(int amount);
    void consume(int amount);
    void addAttribute();
    void handleBom();
    void handleData();
    void handleEntityData();
    void handleTagOpen();
    void handleCloseTagOpen();
    void handleTagName();
    void handleBeforeAttributeName();
    void handleAttributeName();
    void handleAfterAttributeName();
    void handleBeforeAttributeValue();
    void handleAttributeValueQuote();
    void handleAttributeValueUnquoted();
    void handleEntityInAttributeValue();
    void handleBogusComment();
    void handleMarkupDeclarationOpen();
    void handleComment();
    void handleCommentDash();
    void handleCommentEnd();
    void handleDoctype();
    void handleBeforeDoctypeName();
    void handleDoctypeName();
    void handleAfterDoctypeName();
    void handleBogusDoctype();

    void appendValueToAttributeName();
    void appendValueToAttributeValue();
};

void HtmlParserImpl::next()
{
  m_lastPosition = m_position;
  if (m_encoding == HtmlParser::UTF8_ENCODING) {
    // Get a UTF-8 codepoint. If there is a decoding error and the pointer
    // doesn't advance, advance it by one character to prevent hangs.
    // TODO: Ideally this would be in a thread and it would just wait for more
    // bytes to be available.
    const char *lastPosition = m_position;
    m_value = utf8::next(m_position, m_end);
    if (lastPosition == m_position)
      m_position++;
  } else {
    m_value = ISO_8859_1::decode((*m_position)&0xff);
    m_position++;
  }
}

void HtmlParserImpl::reset()
{
  m_attribute = 0;
  m_tagAttributes.clear();
  m_state = BOM;
  m_contentModel = HtmlParser::PCDATA;
  m_encoding = HtmlParser::UTF8_ENCODING;
  m_leftOvers.clear();
  m_contentDisposition.clear();
  m_refresh.clear();
  m_refreshTime = -1;
}

void HtmlParserImpl::initialise(const char * data, unsigned int length)
{
  m_input = data;
  m_position = data;
  m_lastPosition = data;
  m_end = data+length;
  m_value = 0;
  if (not m_cacheFilename.empty())
  {
    nds::File f;
    f.open(m_cacheFilename.c_str(), "a");
    f.write(data, length);
  }
}


void HtmlParserImpl::rewind()
{
  m_position = m_lastPosition;
}

void HtmlParserImpl::emit(unsigned int toEmit)
{
  static unsigned int last = 0;
  if (last == '\r' and toEmit == '\n') {
    last = toEmit;
    return;
  }
  // collapse multiple concurrent blanks into a single space.
  last = toEmit;
  if (toEmit == '\r') {
    toEmit = '\n';
  }
  //if (toEmit == '\t')// or toEmit == NBSP)
  //{
  //  toEmit = ' ';
  //}
  m_self.handleData(toEmit);
}

void HtmlParserImpl::emitDoctype(string & token, bool isError)
{
  // cout << "DOCTYPE: " << token << (isError?" (Erroneous)":" OK") << endl;
}

void HtmlParserImpl::handleTagOpen()
{
  assert(m_contentModel == HtmlParser::PCDATA or m_contentModel == HtmlParser::RCDATA or m_contentModel == HtmlParser::CDATA);
  if (m_contentModel == HtmlParser::RCDATA or m_contentModel == HtmlParser::CDATA)
  {
    next();
    if (m_value == '/') {
      m_state = CLOSE_TAG_OPEN;
    }
    else
    {
      rewind();
      emit('<');
      m_state = DATA;
    }
  }
  else if (m_contentModel == HtmlParser::PCDATA)
  {
    next();
    if (isalpha(m_value))
    {
      m_tagTokenType = START;
      m_currentTagToken = ::tolower(m_value);
      m_state = TAG_NAME;
    }
    else {
      switch (m_value) {
        case '!':
          m_state = MARKUP_DECLARTION_OPEN;
          break;
        case '/':
          m_state = CLOSE_TAG_OPEN;
          break;
        case '>':
          emit('>');
          m_state = DATA;
          break;
        case '?':
          m_state = BOGUS_COMMENT;
          break;
        default:
          // Parse error.
          emit('<');
          m_state = DATA;
          break;
      };
    }
  }
}

void HtmlParserImpl::setLeftovers()
{
  m_leftOvers = asUnconsumedCharString(m_end - m_position);
  //printf("leftovers %s\n", m_leftOvers.c_str());
  m_position = m_end;
}

void HtmlParserImpl::handleCloseTagOpen()
{
  assert(m_contentModel == HtmlParser::PCDATA or m_contentModel == HtmlParser::RCDATA or m_contentModel == HtmlParser::CDATA);
  if (m_contentModel == HtmlParser::RCDATA or m_contentModel == HtmlParser::CDATA)
  {
    bool parseError(true);
    string nextFew = asUnconsumedCharString(m_lastStartTagToken.length()+1);
    if (nextFew.empty())
    {
      setLeftovers();
      return;
    }
    if (nextFew.substr(0, nextFew.length()-1) == m_lastStartTagToken)
    {
      // check next
      int check = nextFew[nextFew.length()-1];
      switch (check)
      {
        case 0x0009:
        case 0x000A:
        case 0x000B:
        case 0x000C:
        case 0x0020:
        case 0x003E: // >
        case 0x002F: // /
        case 0x003C: // <
        case EOF:
          parseError = false;
          break;
        default:
          break;
      }
    }
    if (parseError)
    {
      emit('<');
      emit('/');
      m_state = DATA;
      return;
    }
  }
  // PCDATA  ( or CDATA with matching token)
  m_contentModel = HtmlParser::PCDATA;
  next();
  if (::isalpha(m_value))
  {
    m_tagTokenType = END;
    m_currentTagToken = ::tolower(m_value);
    m_state = TAG_NAME;
  }
  else
  {
    switch (m_value) {
      case '>':
        // parse error
        m_state = DATA;
        break;
      case (unsigned int)EOF:
        emit('<');
        emit('/');
        rewind();
        m_state = DATA;
        break;
      default:
        // parse error..
        m_state = BOGUS_COMMENT;
        break;
    }
  }
}

void HtmlParserImpl::emitTagToken()
{
  m_lastStartTagToken = "";
  switch (m_tagTokenType)
  {
    case START:
      // cout << "<" << m_currentTagToken << ">" << endl;
      m_self.handleStartTag(m_currentTagToken, m_tagAttributes);
      m_lastStartTagToken = m_currentTagToken;
      break;
    case END:
      // cout << " </" << m_currentTagToken << ">" << endl;
      m_self.handleEndTag(m_currentTagToken);
      break;
    case START_END:
      // cout << " <" << m_currentTagToken << "/>" << endl;
      m_self.handleStartEndTag(m_currentTagToken, m_tagAttributes);
      break;
  }
  for_each( m_tagAttributes.begin(), m_tagAttributes.end(), delete_ptr());
  m_tagAttributes.clear();
  delete m_attribute;
  m_attribute = 0;
}
void HtmlParserImpl::emitComment()
{
  // nothing at all.
}

void HtmlParserImpl::handleTagName()
{
  next();
    switch (m_value)
    {
      case '/':
        // parse error if this is not a permitted slash
        if (m_tagTokenType == START) {
          m_tagTokenType = START_END;
        }
      case 0x0009:
      case 0x000A:
      case 0x000B:
      case 0x000C:
      case 0x0020:
        m_state = BEFORE_ATTRIBUTE_NAME;
        break;
      case '<':
      case (unsigned int)EOF:
        // parse error - but carry on.
      case '>':
        emitTagToken();
        m_state = DATA;
        break;
      default:
        if (::isalpha(m_value)) {
          m_value = ::tolower(m_value);
        }
        m_currentTagToken += m_value;
        break;
    }
}

void HtmlParserImpl::addAttribute()
{
  if (m_attribute) {
    m_tagAttributes.push_back(m_attribute);
  }
  m_attribute = 0;
}

void HtmlParserImpl::handleBeforeAttributeName()
{
  next();
  switch (m_value)
  {
    case '/':
      // parse error if this is not a permitted slash
      if (m_tagTokenType == START) {
        m_tagTokenType = START_END;
      }
    case 0x0009:
    case 0x000A:
    case 0x000B:
    case 0x000C:
    case 0x0020:
      m_state = BEFORE_ATTRIBUTE_NAME;
      break;
    case '<':
    case (unsigned int)EOF:
      // parse error - carry on, reconsume in data state.
      rewind();
    case '>':
      emitTagToken();
      m_state = DATA;
      break;
    default:
      if (::isupper(m_value))
      {
        m_value = ::tolower(m_value);
      }
      addAttribute();
      m_attribute = new Attribute;
      m_attribute->name = m_value;
      m_attribute->value.clear();
      m_state = ATTRIBUTE_NAME;
      break;
  }
}

void HtmlParserImpl::appendValueToAttributeName()
{
  if (m_attribute != 0)
    utf8::unchecked::append(m_value, back_inserter(m_attribute->name));
}

void HtmlParserImpl::appendValueToAttributeValue()
{
  if (m_attribute != 0)
    utf8::unchecked::append(m_value, back_inserter(m_attribute->value));
}

void HtmlParserImpl::handleAttributeName()
{
  next();
  bool shouldEmit(false);
  switch (m_value) {
    case 0x0009:
    case 0x000A:
    case 0x000B:
    case 0x000C:
    case 0x0020:
      m_state = AFTER_ATTRIBUTE_NAME;
      break;
    case '=':
      m_state = BEFORE_ATTRIBUTE_VALUE;
      break;
    case '<':
    case (unsigned int)EOF:
      // parse error - carry on, reconsume in data state. FALL THROUGH!
      rewind();
    case '>':
      shouldEmit = true;
      m_state = DATA;
      break;

    case '/':
      // parse error if this is not a permitted slash
      m_state = BEFORE_ATTRIBUTE_NAME;
      break;
    default:
      {
        if (::isupper(m_value))
        {
          m_value = ::tolower(m_value);
        }
        appendValueToAttributeName();
      }
      break;
  }
  if (m_state != ATTRIBUTE_NAME) {
    AttributeVector::iterator it(m_tagAttributes.begin());
    for (; it != m_tagAttributes.end(); ++it)
    {
      if ((*it)->name == m_attribute->name)
      {
        // discard attribute...
        delete m_attribute;
        m_attribute = 0;
        break;
      }
    }
  }
  if (shouldEmit) {
    emitTagToken();
  }
}

void HtmlParserImpl::handleAfterAttributeName()
{
  next();
  switch (m_value) {
    case 0x0009:
    case 0x000A:
    case 0x000B:
    case 0x000C:
    case 0x0020:
      m_state = AFTER_ATTRIBUTE_NAME;
      break;
    case '=':
      m_state = BEFORE_ATTRIBUTE_VALUE;
      break;
    case '<':
    case (unsigned int)EOF:
      // parse error - carry on, reconsume in data state. FALL THROUGH!
      rewind();
    case '>':
      emitTagToken();
      m_state = DATA;
      break;
    case '/':
      // parse error if this is not a permitted slash
      m_state = BEFORE_ATTRIBUTE_NAME;
      break;
    default:
      {
        if (::isupper(m_value))
        {
          m_value = ::tolower(m_value);
        }
        addAttribute();
        m_attribute = new Attribute;
        m_attribute->name=m_value;
        m_attribute->value.clear();
        m_state = ATTRIBUTE_NAME;
      }
      break;
  }

}

void HtmlParserImpl::handleBeforeAttributeValue()
{
  next();
  switch (m_value) {
    case 0x0009:
    case 0x000A:
    case 0x000B:
    case 0x000C:
    case 0x0020:
      m_state = BEFORE_ATTRIBUTE_VALUE;
      break;
    case '"':
      m_state = ATTRIBUTE_VALUE_DOUBLE_QUOTE;
      break;
    case '\'':
      m_state = ATTRIBUTE_VALUE_SINGLE_QUOTE;
      break;
    case '&':
      m_state = ATTRIBUTE_VALUE_UNQUOTED;
      break;

    case '<':
    case (unsigned int)EOF:
      // parse error - carry on, reconsume in data state. FALL THROUGH!
      rewind();
    case '>':
      emitTagToken();
      m_state = DATA;
      break;
    default:
      {
        appendValueToAttributeValue();
        m_state = ATTRIBUTE_VALUE_UNQUOTED;
      }
      break;
  }
}

void HtmlParserImpl::handleAttributeValueQuote()
{
  next();
  unsigned int quote(ATTRIBUTE_VALUE_DOUBLE_QUOTE==m_state?'"':'\'');
  if ( m_value == quote)
  {
    m_state = BEFORE_ATTRIBUTE_NAME;
    // append m_attribute to list ?? TODO
    addAttribute();
  }
  else {
    switch (m_value)
    {
      case '&':
        m_lastState = m_state;
        m_state = ENTITY_IN_ATTRIBUTE_VALUE;
        break;
      case (unsigned int)EOF:
        emitTagToken();
        rewind();
        m_state = DATA;
        break;
      default:
        appendValueToAttributeValue();
        break;
    }
  }
}

void HtmlParserImpl::handleAttributeValueUnquoted()
{
  next();
  switch (m_value) {
    case 0x0009:
    case 0x000A:
    case 0x000B:
    case 0x000C:
    case 0x0020:
      m_state = BEFORE_ATTRIBUTE_NAME;
      addAttribute();
      break;
    case '&':
      m_lastState = m_state;
      m_state = ENTITY_IN_ATTRIBUTE_VALUE;
      break;
    case '<':
    case (unsigned int)EOF:
      // parse error - emit token but redo this value.
      rewind();
    case '>':
      addAttribute();
      emitTagToken();
      m_state = DATA;
      break;
    default:
      appendValueToAttributeValue();
      break;
  }
}

static int getMatches(const char * userChar)
{
  for (int i = 0; i < ENTITY_COUNT; ++i)
  {
    const char * name = s_entity[i].name;
    const char * tmp = userChar;
    bool found(true);
    // since they are in alphabetical order, can bail as soon as we get too far
    if (*tmp < *name) {
      break;
    }
    while (*name != 0 and *tmp != 0 and found) {
      if (*name != *tmp) {
        found = false;
      }
      name++;
      tmp++;
    }
    if (found and (*name == 0)) {
      return i;
    }
    if (found and (*name != 0) and (*tmp == 0)) {
      return -2;
    }
  }
  return -1;
}


unsigned int HtmlParserImpl::consumeEntity()
{
  // 8.2.3.1 web apps 1.0
  // we are now just after an &
  const char * start = m_position;
  next();
  if (m_value == '#')
  {
    // check for xX, hex, parse numbers
    next();
    typedef int(*FilterFunction)(int);
    FilterFunction filter(::isdigit);
    string entity;
    unsigned int minLength = 0;
    if (::tolower(m_value) == 'x')
    {
      filter = ::isxdigit;
      entity += "0x";
      minLength = 2;
    } else {
      rewind();
    }
    while (m_position != m_end)
    {
      next();
      if (filter(m_value))
      {
        entity += m_value;
      } else {
        break;
      }
    }
    if (entity.length() > minLength)
    {
      // make sure we catch silly pages that use &#151; and the like
      // instead of &mdash; 
      long value = ::strtol(entity.c_str(), 0, 0);
      if (m_encoding == HtmlParser::ISO_ENCODING)
      {
        value = ISO_8859_1::decode(value);
      }
      return value;
      //return ::strtol(entity.c_str(), 0, 0);
    }
    else {
      // unconsume.
      m_position = start;
      return 0;
    }
  }
  else
  {
    // parse until ';' or "forever" then check the string against the table of 259 entities.
    // TODO
    rewind();

    // check that it isn't nbsp - that is very common
    if (asUnconsumedCharString(5) == "nbsp;") {
      //cout << "Entity shortcut - nbsp." << endl;
      consume(5);
      return NBSP;
    }
    const char * start = m_position;
    unsigned int found(0);
    // the longest entity is 10 characters.
    char sofar[12];
    int index = 0;
    while ((m_position != m_end) and index < 11)
    {
      next();
      if (m_value == ';') {
        break;
      }
      else {
        sofar[index] = m_value;
        index++;
        sofar[index] = 0;
        int match = getMatches(sofar);
        if (match == -1) {
          // no possible matches, break
          break;
        }
        if (match >= 0) {
          // exact match
          found = s_entity[match].value;
        }
        if (match == -2) {
          // partial match found. keep going.
        }

      }
    }
    if (m_value != ';')
    {
      rewind();
    }
    if (not found) {
      m_position = start;
      return 0;
    }
    //cout << "Entity found " << (void*)found  << " = " << (char) found<< endl;
    return found;
  }
  return 0;
}

void HtmlParserImpl::handleEntityInAttributeValue()
{
  unsigned int value = consumeEntity();
  if (m_attribute) {
    if (value == 0)
    {
      m_attribute->value += '&';
    }
    else
    {
      utf8::unchecked::append(value, back_inserter(m_attribute->value));
    }
  }
  m_state = m_lastState;
}

void HtmlParserImpl::handleBogusComment()
{
  assert(m_contentModel == HtmlParser::PCDATA);
  while (m_position != m_end) {
    next();
    if (m_value == '>' or (int)m_value == EOF) {
      emitComment();
      break;
    }
    m_commentToken += m_value;
  }
  m_state = DATA;
}

void HtmlParserImpl::consume(int amount)
{
  for (int i = 0; i < amount; ++i)
    next();
}

string HtmlParserImpl::asUnconsumedCharString(int amount)
{
  string charString;
  if (not m_leftOvers.empty())
  {
    charString.append(m_leftOvers, 0, amount);
    amount -= charString.length();
    m_leftOvers.erase(0,amount);
  }
  if (amount and  ((m_position + amount) <= m_end)) {
    charString.append(m_position, amount);
    transform(charString.begin(), charString.end(), charString.begin(), ::tolower);
  }
  return charString;
}

void HtmlParserImpl::handleMarkupDeclarationOpen()
{
  assert(m_contentModel == HtmlParser::PCDATA);

  string next2 = asUnconsumedCharString(2);
  if (next2.empty())
  {
    setLeftovers();
    return;
  }
  if (next2 == "--")
  {
    consume(2);
    // comment state.
    m_commentToken = "";
    m_state = COMMENT;
  }
  else
  {
    string next7 = asUnconsumedCharString(7);
    if (next7.empty())
    {
      setLeftovers();
      return;
    }
    if (next7 == "doctype")
    {
      consume(7);
      m_state = DOCTYPE;
    }
    else
    {
      // parse error
      m_state = BOGUS_COMMENT;
    }
  }
}
void HtmlParserImpl::handleComment()
{
  next();
  switch (m_value) {
    case '-':
      m_state = COMMENT_DASH;
      break;
    case (unsigned int)EOF:
      emitComment();
      rewind();
      m_state = DATA;
      break;
    default:
      m_commentToken += m_value;
      break;
  }
}
void HtmlParserImpl::handleCommentDash()
{
  next();
  switch (m_value) {
    case '-':
      m_state = COMMENT_END;
      break;
    case (unsigned int)EOF:
      emitComment();
      rewind();
      m_state = DATA;
      break;
    default:
      m_commentToken += '-';
      m_commentToken += m_value;
      m_state = COMMENT;
      break;
  }
}
void HtmlParserImpl::handleCommentEnd()
{
  next();
  switch (m_value) {
    case (unsigned int)EOF:
      rewind();
    case '>':
      emitComment();
      m_state = DATA;
      break;
    case '-':
      // parse error, keep going..
      m_commentToken += '-';
      break;
    default:
      // you've blown it... try parsing again
      m_commentToken += '-';
      m_commentToken += '-';
      m_commentToken += m_value;
      m_state = COMMENT;
      break;
  }
}

void HtmlParserImpl::handleDoctype()
{
  next();
  if (isWhitespace(m_value))
  {
    m_state = BEFORE_DOCTYPE_NAME;
  }
  else
  {
    // Parse error. Reconsume the current character.
    rewind();
    m_state = BEFORE_DOCTYPE_NAME;
  }
}

void HtmlParserImpl::handleBeforeDoctypeName()
{
  next();
  if (isWhitespace(m_value))
  {
    m_state = BEFORE_DOCTYPE_NAME;
  }
  else
  {
    if (islower(m_value)) {
      m_doctypeTokenIsError = true;
      m_doctypeToken = ::toupper(m_value);
      m_state = DOCTYPE_NAME;
    }
    else
    {
      string empty("");
      switch (m_value)
      {
        case '>':
          emitDoctype(empty, true);
          m_state = DATA;
          break;
        case (unsigned int)EOF:
          emitDoctype(empty, true);
          m_state = DATA;
          rewind();
          break;
        default:
          m_doctypeTokenIsError = true;
          m_doctypeToken = ::toupper(m_value);
          m_state = DOCTYPE_NAME;
          break;
      }
    }
  }
}
void HtmlParserImpl::handleDoctypeName()
{
  next();
  if (isWhitespace(m_value))
  {
    m_state = AFTER_DOCTYPE_NAME;
  }
  else
  {
    if (::islower(m_value))
    {
      m_doctypeTokenIsError = true;
      m_doctypeToken += ::toupper(m_value);
    }
    else {
      switch (m_value)
      {
        case '>':
          emitDoctype(m_doctypeToken, m_doctypeTokenIsError);
          m_state = DATA;
          break;
        case (unsigned int)EOF:
          emitDoctype(m_doctypeToken, m_doctypeTokenIsError);
          m_state = DATA;
          rewind();
          break;
        default:
          m_doctypeToken += m_value;
          break;
      }
    }
    m_doctypeTokenIsError = (m_doctypeToken != "HTML");
  }
}

void HtmlParserImpl::handleAfterDoctypeName()
{
  next();
  if (isWhitespace(m_value))
  {
    // stay in this state
    m_state = AFTER_DOCTYPE_NAME;
  }
  else
  {
    switch (m_value)
    {
      case '>':
        emitDoctype(m_doctypeToken, m_doctypeTokenIsError);
        m_state = DATA;
        break;
      case (unsigned int)EOF:
        emitDoctype(m_doctypeToken, m_doctypeTokenIsError);
        m_state = DATA;
        rewind();
        break;
      default:
        m_doctypeTokenIsError = true;
        m_state = BOGUS_DOCTYPE;
        break;
    }
  }

}

void HtmlParserImpl::handleBogusDoctype()
{
  next();
  switch (m_value)
  {
    case '>':
      emitDoctype(m_doctypeToken, m_doctypeTokenIsError);
      m_state = DATA;
      break;
    case (unsigned int)EOF:
      emitDoctype(m_doctypeToken, m_doctypeTokenIsError);
      m_state = DATA;
      rewind();
      break;
    default:
      break;
  }
}

void HtmlParserImpl::handleBom()
{
  const unsigned char * bom = (const unsigned char *)m_position;
  if (bom[0] == 0xef and bom[1] == 0xbb and bom[2] == 0xbf)
  {
    setEncoding(HtmlParser::UTF8_ENCODING);
  }
  m_state = DATA;
}

void HtmlParserImpl::handleData()
{
  // consume next character.
  next();
  if ( m_value == '&'
      and (m_contentModel == HtmlParser::PCDATA
        or m_contentModel == HtmlParser::RCDATA))
  {
    // entity data state.
    m_state = ENTITY_DATA;
  }
  else if ( m_value == '<'
      and m_contentModel != HtmlParser::PLAINTEXT)
  {
    // tag open state
    m_state = TAG_OPEN;
  }
  else
  {
    // anything else. ??
    emit(m_value);
  }
}
void HtmlParserImpl::handleEntityData()
{
  assert(m_contentModel != HtmlParser::CDATA);
  unsigned int value = consumeEntity();
  if (value == 0)
  {
    emit('&');
  }
  else
  {
    emit(value);
  }
  m_state = DATA;
}

void HtmlParserImpl::debug()
{
#if 0
  const char * stuff[] = {
     "DATA",
     "ENTITY_DATA",
     "TAG_OPEN",
     "CLOSE_TAG_OPEN",
     "TAG_NAME",
     "BEFORE_ATTRIBUTE_NAME",
     "ATTRIBUTE_NAME",
     "AFTER_ATTRIBUTE_NAME",
     "BEFORE_ATTRIBUTE_VALUE",
     "ATTRIBUTE_VALUE_DOUBLE_QUOTE",
     "ATTRIBUTE_VALUE_SINGLE_QUOTE",
     "ATTRIBUTE_VALUE_UNQUOTED",
     "ENTITY_IN_ATTRIBUTE_VALUE",
     "BOGUS_COMMENT",
     "MARKUP_DECLARTION_OPEN",
     "COMMENT",
     "COMMENT_DASH",
     "COMMENT_END",
     "DOCTYPE",
     "BEFORE_DOCTYPE_NAME",
     "DOCTYPE_NAME",
     "AFTER_DOCTYPE_NAME",
     "BOGUS_DOCTYPE",
  };
  //cout << stuff[m_state] << " ";
#ifndef ARM9
  wcout << (wchar_t)m_value << endl;
#endif
#endif
}

void HtmlParserImpl::fire()
{
  //debug();
  switch (m_state) {
    case BOM:
      {
        // check BOM for UTF-8
        handleBom();
      }
      break;
    case DATA:
      {
        handleData();
      }
      break;
    case ENTITY_DATA:
      {
        handleEntityData();
      }
      break;
    case TAG_OPEN:
      {
        handleTagOpen();
      }
      break;
    case CLOSE_TAG_OPEN:
      {
        handleCloseTagOpen();
      }
      break;
    case TAG_NAME:
      {
        handleTagName();
      }
      break;
    case BEFORE_ATTRIBUTE_NAME:
      {
        handleBeforeAttributeName();
      }
      break;
    case ATTRIBUTE_NAME:
      {
        handleAttributeName();
      }
      break;
    case AFTER_ATTRIBUTE_NAME:
      {
        handleAfterAttributeName();
      }
      break;
    case BEFORE_ATTRIBUTE_VALUE:
      {
        handleBeforeAttributeValue();
      }
      break;
    case ATTRIBUTE_VALUE_DOUBLE_QUOTE:
    case ATTRIBUTE_VALUE_SINGLE_QUOTE:
      {
        handleAttributeValueQuote();
      }
      break;
    case ATTRIBUTE_VALUE_UNQUOTED:
      {
        handleAttributeValueUnquoted();
      }
      break;
    case ENTITY_IN_ATTRIBUTE_VALUE:
      {
        handleEntityInAttributeValue();
      }
      break;
    case BOGUS_COMMENT:
      {
        handleBogusComment();
      }
      break;
    case MARKUP_DECLARTION_OPEN:
      {
        handleMarkupDeclarationOpen();
      }
      break;
    case COMMENT:
      {
        handleComment();
      }
      break;
    case COMMENT_DASH:
      {
        handleCommentDash();
      }
      break;
    case COMMENT_END:
      {
        handleCommentEnd();
      }
      break;
    case DOCTYPE:
      {
        handleDoctype();
      }
      break;
    case BEFORE_DOCTYPE_NAME:
      {
        handleBeforeDoctypeName();
      }
      break;
    case DOCTYPE_NAME:
      {
        handleDoctypeName();
      }
      break;
    case AFTER_DOCTYPE_NAME:
      {
        handleAfterDoctypeName();
      }
      break;
    case BOGUS_DOCTYPE:
      {
        handleBogusDoctype();
      }
      break;
/*default:
      next();
      break;*/
  };
}

void HtmlParserImpl::setCacheFile(const std::string & filename)
{
  nds::File f;
  if (not filename.empty())
  {
    f.open(filename.c_str(), "w");
  }
  m_cacheFilename.clear();
  if (f.is_open())
  {
    m_cacheFilename = filename;
  }
}

HtmlParser::HtmlParser():
  m_mimeType(UNINITIALISED),
  m_details(new HtmlParserImpl(*this))
{
}

HtmlParser::~HtmlParser()
{
  delete m_details;
}

void HtmlParser::feed(const char * data, unsigned int length)
{
  m_details->initialise(data, length);
  if (mimeType() == HtmlParser::TEXT_HTML
      or mimeType() == HtmlParser::UNINITIALISED)
  {
    while (m_details->position() < m_details->end()) {
      m_details->fire();
    }
  }
  else
  {
    handleBinaryData(data, length);
  }
}

void HtmlParser::setEncoding(HtmlParser::Encoding enc)
{
  m_details->setEncoding(enc);
}

void HtmlParser::setContentModel(ContentModel newModel)
{
  m_details->setContentModel(newModel);
}

void HtmlParserImpl::setRefresh(const std::string & refresh, int time)
{
  m_refresh = refresh;
  m_refreshTime = time;
}

void HtmlParserImpl::refresh(std::string & refresh, int & time) const
{
  refresh = m_refresh;
  time = m_refreshTime;
}

void HtmlParser::parseCharset(const std::string & value)
{
  if (value == "iso-8859-1")
  {
    setEncoding(HtmlParser::ISO_ENCODING);
  }
  else if (value == "utf-8")
  {
    setEncoding(HtmlParser::UTF8_ENCODING);
  }
}

void HtmlParser::parseContentType(const std::string & value)
{
  ParameterSet paramSet(value);
  if (paramSet.hasParameter("charset"))
  {
    string charset;
    paramSet.parameter("charset", charset);
    parseCharset(charset);
  }

  std::vector<std::string> segments;
  split(value, segments, ";");
  if (not segments.empty())
    setMimeType(segments[0]);

  if (m_mimeType == TEXT_PLAIN)
  {
    setContentModel(PLAINTEXT);
  }
}

void HtmlParser::parseContentDisposition(const std::string & value)
{
  ParameterSet paramSet(value);
  if (paramSet.hasParameter("filename"))
  {
    string filename;
    paramSet.parameter("filename", filename);
    m_details->setContentDisposition(filename);
  }
}

std::string HtmlParser::getContentDisposition() const
{
  return m_details->getContentDisposition();
}

void HtmlParser::setMimeType(const std::string &mt)
{
  m_mimeType = OTHER;
  m_mimeTypeValue = mt;
  // mime type is set to a parameter name...
  if (mt == "text/plain") {
    m_mimeType = TEXT_PLAIN;
  }
  else if (mt == "text/html"
      or mt == "text/xhtml"
      or mt == "application/xhtml+xml"
      or mt == "text/vnd.wap.wml"
      or mt == "application/xhtml") {
    m_mimeType = TEXT_HTML;
  }
  else if (mt == "image/png") {
    m_mimeType = IMAGE_PNG;
  }
  else if (mt == "image/gif") {
    m_mimeType = IMAGE_GIF;
  }
  else if (mt == IMAGE_JPEG_STR or mt == IMAGE_JPG_STR) {
    m_mimeType = IMAGE_JPEG;
  }
  else if (mt == "image/apng") {
    m_mimeType = IMAGE_APNG;
  }
  else if (mt == "image/bmp") {
    m_mimeType = IMAGE_BMP;
  }
  else if (mt == "application/zip") {
    m_mimeType = ZIP;
  }
}

void HtmlParser::parseRefresh(const std::string & value)
{
  // actual refreshing implemented in view.
  ParameterSet refreshSet(value);
  string refresh;
  if (refreshSet.hasParameter("url"))
  {
    refreshSet.parameter("url", refresh);
  }
  if (not refreshSet.keyValueMap().empty())
  {
    // bit of a hack this - adds an empty key for the time.. if no time given, then will be the url, which == 0
    m_details->setRefresh(refresh, strtol(refreshSet.keyValueMap().begin()->first.c_str(), 0, 10));
  }
}

void HtmlParser::refresh(std::string & refreshUrl, int & time) const
{
  m_details->refresh(refreshUrl, time);
}

void HtmlParser::checkMetaTagHttpEquiv(const HtmlElement * meta)
{
  string httpEquiv = meta->attribute("http-equiv");
  string content =  meta->attribute("content");
  if (not httpEquiv.empty() and not content.empty())
  {
    transform(httpEquiv.begin(), httpEquiv.end(), httpEquiv.begin(), ::tolower);
    transform(content.begin(), content.end(), content.begin(), ::tolower);
    if (httpEquiv == "content-type")
    {
      parseContentType(content);
    }
    else if (httpEquiv == "refresh")
    {
      parseRefresh(content);
    }
  }

  string charset =  meta->attribute("charset");
  if (not charset.empty())
  {
    parseCharset(charset);
  }
}

HtmlParser::Encoding HtmlParser::encoding() const
{
  return m_details->encoding();
}

void HtmlParser::setToStart()
{
  m_details->reset();
  m_mimeType = UNINITIALISED;
}

void HtmlParser::setCacheFile(const std::string & filename)
{
  m_details->setCacheFile(filename);
}

HtmlParser::MimeType HtmlParser::mimeType() const
{
  return m_mimeType;
}

std::string HtmlParser::mimeTypeValue() const
{
  return m_mimeTypeValue;
}
