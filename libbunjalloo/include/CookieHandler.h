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
#ifndef CookieHandler_h_seen
#define CookieHandler_h_seen

#include "ButtonListener.h"
class ButtonGroup;
class RadioButton;
class CheckBox;
class View;
class CookieHandler: public ButtonListener
{
  public:
    CookieHandler(View * view);

    enum Mode_t
    {
      ADD_MODE,
      EDIT_MODE
    };
    void setMode(Mode_t mode);
    void show();

    /** Tick a frame. 
     * @return true if needs a redraw.
     */
    bool tick();

    virtual void pressed(ButtonI * button);
  private:
    View & m_view;
    ButtonGroup * m_group;
    RadioButton * m_siteButton;
    RadioButton * m_allButton;
    ButtonI * m_ok;
    ButtonI * m_cancel;
    ButtonI * m_deleteSelected;
    ButtonI * m_editSelected;
    std::vector<CheckBox*> m_checkboxes;
    Mode_t m_mode;
    bool m_redrawEdit;

    void initAdd();
    void showAdd();
    void initEdit();
    void showEdit();

    void acceptAdd();
    void acceptEdit();

    void removeSelected();
};
#endif
