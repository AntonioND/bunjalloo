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
#ifndef ButtonI_h_seen
#define ButtonI_h_seen

class ButtonListener;
/** An abstract button class that handles selection and setting callback
 * listeners.*/
class ButtonI
{
  public:

    /** Set the initial state of the class.*/
    ButtonI(): m_selected(false),m_listener(0){}

    /** End of lifetime.*/
    virtual ~ButtonI() {}

    /** Set the selection state.
     * @param selected the boolean flag for selection.
     */
    inline void setSelected(bool selected=true)
    {
      m_selected = selected;
    }

    /** Get the selection state.
     * @return true if this object is selected, false otherwise.
     */
    inline bool selected() const
    {
      return m_selected;
    }

    /** Set the callback ButtonListener for this ButtonI.
     * @param listener the listener to use.
     */
    inline void setListener(ButtonListener * listener)
    {
      m_listener = listener;
    }
    /** Get the current callback ButtonListener for this ButtonI.
     * @return the currently set listener.
     */
    inline ButtonListener * listener() const
    {
      return m_listener;
    }

    /** Remove the current callback ButtonListener.
     * @param listener remove this listener. (not fully implemented - just removes the only listener)
     */
    inline void removeListener(ButtonListener * listener)
    {
      m_listener = 0;
    }

  private:
    bool m_selected;
    ButtonListener * m_listener;

};
#endif
