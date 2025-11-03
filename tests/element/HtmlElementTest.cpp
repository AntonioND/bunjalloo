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
#include <gtest/gtest.h>
#include "HtmlElement.h"
#include "HtmlTextAreaElement.h"
#include "HtmlMetaElement.h"
#include "HtmlOptionElement.h"
#include "HtmlInputElement.h"
#include "HtmlAnchorElement.h"
#include "ElementFactory.h"

using namespace std;

class HtmlElementTest : public testing::Test
{
  protected:
    HtmlElement * m_element;
    HtmlElement * m_clone;

    void TearDown() {
      delete m_element;
      delete m_clone;
    }

    void SetUp() {
      m_element = 0;
      m_clone = 0;
    }
};

TEST_F(HtmlElementTest, 0)
{
  m_element = ElementFactory::create("html");
  EXPECT_TRUE( m_element != 0);
}

TEST_F(HtmlElementTest, Attributes)
{
  m_element = ElementFactory::create("html");

  string expected("en");
  m_element->setAttribute("lang", expected);
  string result = m_element->attribute("lang");
  EXPECT_EQ( expected, result);

  expected = "newId";
  m_element->setAttribute("id", expected);
  result = m_element->attribute("id");
  EXPECT_EQ( expected, result);

  expected = "Test title";
  m_element->setAttribute("title", expected);
  result = m_element->attribute("title");
  EXPECT_EQ( expected, result);

  expected = "fichero";
  m_element->setAttribute("dir", expected);
  result = m_element->attribute("dir");
  EXPECT_EQ( expected, result);

  // try setting some unsupported attribute
  m_element->setAttribute("test", expected);
  result = m_element->attribute("test");
  expected = "";
  EXPECT_EQ( expected, result);


}

TEST_F(HtmlElementTest, Append)
{
  m_element = ElementFactory::create("html");
  HtmlElement * head = ElementFactory::create("head");
  m_element->append(head);
  EXPECT_TRUE( m_element->hasChildren());
}

TEST_F(HtmlElementTest, Isa)
{
  m_element = ElementFactory::create("html");
  EXPECT_TRUE( m_element->isa("html"));
  delete m_element;
  m_element = ElementFactory::create("head");
  EXPECT_TRUE( m_element->isa("head"));
}


TEST_F(HtmlElementTest, Anchor)
{
  m_element = ElementFactory::create("a");
  EXPECT_TRUE( m_element->isa("a"));
  HtmlAnchorElement * a = dynamic_cast<HtmlAnchorElement*>(m_element);
  EXPECT_TRUE( a != 0 );
}


TEST_F(HtmlElementTest, Clone)
{
  m_element = ElementFactory::create("a");
  m_element->setAttribute("href", "http://localhost");
  m_element->setAttribute("id", "myElement");
  m_clone = m_element->clone();
  EXPECT_TRUE( m_clone != 0);
  EXPECT_TRUE( m_clone->isa("a"));
  EXPECT_EQ(m_element->attribute("href"),
      m_clone->attribute("href"));
  EXPECT_EQ(m_element->attribute("id"),
      m_clone->attribute("id"));
}


TEST_F(HtmlElementTest, Parent)
{
  // add a node, check its parent is the right one
  m_element = ElementFactory::create("html");
  HtmlElement * child = ElementFactory::create("head");
  m_element->append(child);

  EXPECT_TRUE( child->parent() == m_element);
}

TEST_F(HtmlElementTest, AppendText)
{
  // parentNode -> add text, add node, add text 
  // check the text nodes are correct
  m_element = ElementFactory::create("body");
  m_element->appendText('T');
  EXPECT_TRUE(m_element->hasChildren());
  HtmlElement * text = m_element->firstChild();
  string expected("#TEXT");
  EXPECT_EQ(expected, text->tagName());
  HtmlElement * a = ElementFactory::create("a");
  a->appendText('L');
  m_element->append(a);
  EXPECT_EQ(a, m_element->lastChild());
  EXPECT_TRUE(a->hasChildren());
  m_element->appendText('2');
  EXPECT_EQ(expected, m_element->lastChild()->tagName());

}

TEST_F(HtmlElementTest, Meta)
{
  // create a meta node, then check the type
  m_element = ElementFactory::create("meta");
  EXPECT_TRUE( m_element->isa("meta"));
  HtmlMetaElement * meta = dynamic_cast<HtmlMetaElement*>(m_element);
  EXPECT_TRUE( meta != 0 );

  string expected("content-type");
  m_element->setAttribute("http-equiv", "content-type");
  EXPECT_EQ(expected, m_element->attribute("http-equiv"));
}

TEST_F(HtmlElementTest, Remove)
{
  // create a node, add another, check removal
  m_element = ElementFactory::create("body");
  m_element->appendText('T');
  HtmlElement * a = ElementFactory::create("a");
  HtmlElement * b = ElementFactory::create("b");
  m_element->append(a);
  m_element->append(b);
  size_t expected(3);
  EXPECT_EQ(expected, m_element->children().size());

  m_element->remove(a);
  expected = 2;
  EXPECT_EQ(expected, m_element->children().size());

  string expectedText("#TEXT");
  EXPECT_EQ(expectedText, m_element->firstChild()->tagName());
  expectedText = "b";
  EXPECT_EQ(expectedText, m_element->lastChild()->tagName());
  delete a;
}

TEST_F(HtmlElementTest, Img)
{
  // create a node, add another, check removal
  m_element = ElementFactory::create("img");
  m_element->setAttribute("src", "image-src");
  string expected("image-src");
  EXPECT_EQ(expected, m_element->attribute("src"));

  expected = "alternate text";
  m_element->setAttribute("alt", expected);
  EXPECT_EQ(expected, m_element->attribute("alt"));
  expected = "minombre";
  m_element->setAttribute("name", expected);
  EXPECT_EQ(expected, m_element->attribute("name"));
}

TEST_F(HtmlElementTest, PreviousSibling)
{
  m_element = ElementFactory::create("ul");
  HtmlElement * li = ElementFactory::create("li");
  HtmlElement * text = ElementFactory::create("#TEXT");
  m_element->append(text);
  m_element->append(li);
  const HtmlElement * ps = m_element->previousSibling(li);
  EXPECT_TRUE(0 != ps);
  EXPECT_TRUE(text == ps);
}

TEST_F(HtmlElementTest, NextSibling)
{
  m_element = ElementFactory::create("ul");
  HtmlElement * li = ElementFactory::create("li");
  HtmlElement * text = ElementFactory::create("#TEXT");
  m_element->append(text);
  m_element->append(li);
  const HtmlElement * ps = m_element->nextSibling(text);
  EXPECT_TRUE(0 != ps);
  EXPECT_TRUE(li == ps);
}

TEST_F(HtmlElementTest, OptionElement)
{
  m_element = ElementFactory::create("option");
  m_element->setAttribute("value", "a");
  m_element->setAttribute("label", "b");
  HtmlOptionElement * opt = dynamic_cast<HtmlOptionElement*>(m_element);
  EXPECT_TRUE(opt != 0);
  EXPECT_TRUE(not opt->selected());
  m_element->setAttribute("selected", "yes");
  EXPECT_TRUE(opt->selected());
  
  string expected("a"); 
  EXPECT_EQ( expected , m_element->attribute("value"));
  expected = "b"; 
  EXPECT_EQ( expected , m_element->attribute("label"));
}


TEST_F(HtmlElementTest, ElementsByTagName)
{
  m_element = ElementFactory::create("form");
  HtmlElement * select = ElementFactory::create("select");
  HtmlElement * option = ElementFactory::create("option");
  HtmlElement * option2 = ElementFactory::create("option");
  HtmlElement * p = ElementFactory::create("p");
  m_element->append(select);
  select->append(option);
  select->append(p);
  select->append(option2);

  size_t expected(1);
  EXPECT_EQ( expected , m_element->elementsByTagName("select").size());
  expected = 2;
  EXPECT_EQ( expected , select->elementsByTagName("option").size());


}

TEST_F(HtmlElementTest, TextAreaElement)
{
  m_element = ElementFactory::create("textarea");
  HtmlTextAreaElement * element = (HtmlTextAreaElement*)m_element;
  m_element->setAttribute("rows", "10");
  m_element->setAttribute("cols", "80");
  int expected = 10;
  int result = element->rows();
  EXPECT_EQ( expected, result);
  expected = 80;
  result = element->cols();
  EXPECT_EQ( expected, result);
}

TEST_F(HtmlElementTest, InputElement)
{
  m_element = ElementFactory::create("input");
  HtmlInputElement * element = (HtmlInputElement*)m_element;
  EXPECT_TRUE(not element->hasAltText());

  std::string altText = "alt text";
  std::string srcText = "image.png";
  m_element->setAttribute("src", srcText);
  m_element->setAttribute("alt", altText);
  EXPECT_TRUE(altText == m_element->attribute("alt"));
  EXPECT_TRUE(srcText ==  m_element->attribute("src"));
}
