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
#ifndef Updater_h_seen
#define Updater_h_seen

#include <string>
#include "ButtonListener.h"
#include "URI.h"

class Controller;
class Document;
class Button;
class View;

class Updater: public ButtonListener
{
  public:
    Updater(Controller & c, Document & d, View & view);
    void init();

    virtual void pressed(ButtonI * button);

    void show();

    bool valid() const;

  private:
    Controller & m_controller;
    Document & m_document;
    View & m_view;
    std::string m_newVersion;
    Button * m_ok;
    Button * m_cancel;

    URI m_downloadUrl;
    enum UpdaterState
    {
      START,
      GOT_INI,
      GOT_ZIP,
      DO_UPDATE,
      INI_FAIL,
      ZIP_FAIL,
      CANCELLED,
      SUCCESS
    };
    UpdaterState m_state;

    void getZip();
    void doUpdate();
    void iniFail();
    void askUpdate();
    void doTitle();
    void addOk();
    void addCancel();
    void alreadyGotLatest();
};
#endif
