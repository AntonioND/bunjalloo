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
#ifndef ZipViewer_h_seen
#define ZipViewer_h_seen

#include <string>
#include <vector>
#include "ButtonListener.h"
#include "ZipFile.h"

class CheckBox;
class View;

class ZipViewer: public ButtonListener, public ExtractListener
{
  public:
    ZipViewer(View & view);

    void setFilename(const std::string & filename);

    void unzip();
    void show();
    virtual void pressed(ButtonI * button);

    virtual bool extract(const char * name);
    virtual void before(const char * name);
    virtual void after(const char * name);

  private:
    View & m_view;
    std::string m_filename;
    // no need to delete these - the text area they get added to deals with
    // memory management.
    ButtonI * m_unzip;
    std::vector<CheckBox*> m_checkboxes;
    int m_fileCount;
    size_t m_index;
};
#endif
