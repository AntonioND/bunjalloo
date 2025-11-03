/*
  Copyright (C) 2008 Richard Quirk

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
#include <algorithm>
#include "Button.h"
#include "CheckBox.h"
#include "Document.h"
#include "File.h"
#include "Language.h"
#include "ProgressBar.h"
#include "RichTextArea.h"
#include "View.h"
#include "ViewRender.h"
#include "ZipViewer.h"

using std::vector;
using std::string;

ZipViewer::ZipViewer(View & view):
  m_view(view),
  m_unzip(0),
  m_fileCount(0),
  m_index(0)
{
}

void ZipViewer::setFilename(const std::string & filename)
{
  m_filename = filename;
  m_unzip = 0;
  m_fileCount = 0;
  // don't delete here - the textArea already handles that
  m_checkboxes.clear();
}

void ZipViewer::show()
{
  ViewRender *renderer(m_view.renderer());
  renderer->doTitle(nds::File::base(m_view.document().uri().c_str()));
  ZipFile file(this);
  file.open(m_filename.c_str());

  vector<string> contents;
  file.list(contents);
  m_fileCount = contents.size();
  if (not contents.empty())
  {
    // add an "unzip" button!
    Button * unzipButton = new Button(T("unzip"));
    unzipButton->setListener(this);
    renderer->add(unzipButton);
    m_unzip = unzipButton;
    renderer->textArea()->appendText(T(" "));

    renderer->insertNewline();
  }

  for (vector<string>::const_iterator it(contents.begin()); it != contents.end(); ++it)
  {
    const string & u(*it);
    CheckBox * cb = new CheckBox;
    cb->setSelected();
    renderer->add(cb);
    m_checkboxes.push_back(cb);
    renderer->textArea()->appendText(u);
    renderer->insertNewline();
  }
}

void ZipViewer::unzip()
{
  ProgressBar & progressBar(m_view.progressBar());
  progressBar.setMaximum(m_fileCount);
  progressBar.setMinimum(0);
  progressBar.setVisible();
  m_index = 0;
  progressBar.setValue(m_index);
  ZipFile file(this);
  // unzip the file
  file.open(m_filename.c_str());
  if (file.is_open())
  {
    file.extract();
  }
}

void ZipViewer::pressed(ButtonI * button)
{
  if (button == m_unzip)
  {
    // unzip the file
    unzip();
  }
  ProgressBar & progressBar(m_view.progressBar());
  progressBar.setVisible(false);
  m_view.tick();
}

void ZipViewer::before(const char * name)
{
  m_view.tick();
}

void ZipViewer::after(const char * name)
{
  m_index++;
  ProgressBar & progressBar(m_view.progressBar());
  progressBar.setValue(m_index);
  m_view.tick();
}


bool ZipViewer::extract(const char * name)
{
  if (m_checkboxes.size() > m_index)
  {
    return m_checkboxes[m_index]->selected();
  }
  return true;
}
