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
#include "Palette.h"
#include "Canvas.h"
#include "TextArea.h"
#include "ProgressBar.h"

ProgressBar::ProgressBar(int min, int max): Component(),
  m_min(min),
  m_max(max),
  m_value(min)
{
}

void ProgressBar::setValue(int val)
{
  m_value = val;
  m_dirty = true;
}

unsigned int ProgressBar::value() const
{
  return m_value;
}

void ProgressBar::setMinimum(int mini)
{
  m_min = mini;
  m_dirty = true;
}

unsigned int ProgressBar::minimum() const
{
  return m_min;
}

void ProgressBar::setMaximum(int maxi)
{
  m_max = maxi;
  m_dirty = true;
}

unsigned int ProgressBar::maximum() const
{
  return m_max;
}

void ProgressBar::paint(const nds::Rectangle & clip)
{
  if (not visible() or not dirty())
    return;
  m_dirty = false;
  // fill the bg rectangle in the text container to the desired colour
  nds::Canvas::instance().setClip(clip);
  // fill to %
  // min = x
  // max = x+w
  // val = x + ( (w*val) / (max-min))
  unsigned int range = clip.w;
  if (m_max > m_min)
  {
     range = (clip.w * m_value) / (m_max-m_min);
  }
  nds::Rectangle progress(clip.x+1,clip.y+1, range, clip.h);
  nds::Canvas::instance().fillRectangle(progress.x, progress.y,
      progress.w, progress.h, nds::Color(0,29,10));
}

