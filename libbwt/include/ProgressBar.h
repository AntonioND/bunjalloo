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
#ifndef ProgressBar_h_seen
#define ProgressBar_h_seen

#include "Component.h"

/** Implement the typical funny coloured bar that tells us how long 
 * we are going to have to wait for something to finish
 */
class ProgressBar: public Component
{
  public:
    ProgressBar(int min, int max);

    void setValue(int val);
    unsigned int value() const;
    void setMinimum(int mini);
    unsigned int minimum() const;
    void setMaximum(int maxi);
    unsigned int maximum() const;

    virtual void paint(const nds::Rectangle & clip);

    virtual bool stylusUp(const Stylus * stylus) { return false; }
    virtual bool stylusDownFirst(const Stylus * stylus) { return false; }
    virtual bool stylusDownRepeat(const Stylus * stylus) { return false; }
    virtual bool stylusDown(const Stylus * stylus) { return false; }
  private:
    unsigned int m_min;
    unsigned int m_max;
    unsigned int m_value;

    DISALLOW_COPY_AND_ASSIGN(ProgressBar);
};
#endif
