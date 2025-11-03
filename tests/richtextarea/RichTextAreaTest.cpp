#include <gtest/gtest.h>
#include <memory.h>
#include "Font.h"
#include "libnds.h"
#include "sans.h"
#include "FontFactory.h"
#include "TextAreaFactory.h"
#define private public
#include "RichTextArea.h"

class RichTextAreaTest: public testing::Test
{
  protected:
    RichTextArea *m_text;
    Font *m_font;
    void SetUp()
    {
      m_font = FontFactory::create(reinterpret_cast<const unsigned char*>(_binary_sans_set_bin_start),
          reinterpret_cast<const unsigned char*>(_binary_sans_map_bin_start));
      TextAreaFactory::setFont(m_font);
      m_text = static_cast<RichTextArea*>(TextAreaFactory::create(TextAreaFactory::TXT_RICH));
    }

    void TearDown()
    {
      delete m_text;
      delete m_font;
    }
};

TEST_F(RichTextAreaTest, check_lineAt)
{
  m_text->appendText("line 1\n");
  m_text->appendText("line 2\n");
  m_text->appendText("line 3\n");
  m_text->appendText("line 4\n");
  int result = m_text->lineAt(m_font->height());
  EXPECT_EQ(1, result);

  result = m_text->lineAt(m_font->height() + 1);
  EXPECT_EQ(1, result);

  result = m_text->lineAt(m_font->height() - 1);
  EXPECT_EQ(0, result);

  result = m_text->lineAt(m_font->height() + 2);
  EXPECT_EQ(1, result);

  result = m_text->lineAt((m_font->height() * 2) - 1);
  EXPECT_EQ(1, result);

  result = m_text->lineAt((m_font->height() * 2));
  EXPECT_EQ(2, result);

  result = m_text->lineAt((m_font->height() * 2) + 1);
  EXPECT_EQ(2, result);
}
