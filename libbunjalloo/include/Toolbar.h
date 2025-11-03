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
#ifndef Toolbar_h_seen
#define Toolbar_h_seen

#include <vector>
#include "Sprite.h"
#include "ViewI.h"
#include "StylusListener.h"

class View;

class Toolbar: public ViewI, public StylusListener
{
  public:
    /** Position on screen of the Toolbar. */
    enum Position
    {
      TOP,    //!< Horizontal layout at the top of the screen.
      BOTTOM, //!< Horizontal layout at the bottom of the screen.
      LEFT,   //!< Vertical layout on the left hand side of the screen.
      RIGHT   //!< Vertical layout on the right hand side of the screen.
    };

    /** Icon positions in the toolbar image. */
    enum ToolbarIcon
    {
      ICON_BACK = 0,
      ICON_FORWARD,
      ICON_STOP,
      ICON_REFRESH,

      ICON_BACK_DISABLE,
      ICON_FORWARD_DISABLE,
      ICON_PREFS,
      ICON_SPINNER,

      ICON_BOOKMARK,
      ICON_GO_URL,
      ICON_SEARCH,
      ICON_SAVE_AS,

      ICON_NOT_CONNECTED,
      ICON_CONNECTED,
      ICON_CONNECT_ERROR,
      ICON_HIDE_LEFT,
      ICON_ADD_BOOKMARK,

      ICON_COOKIE,
      ICON_ADD_COOKIE,
      ICON_ADD_SEARCH,
    };

    static const int TILES_PER_ICON;
    static const int TOOLBAR_X;
    static const int TOOLBAR_X_RIGHT;
    static const int TOOLBAR_X_LEFT;
    static const int TOOLBAR_SEP;

    /** Create a Toolbar.
     * @param view The View that will handle user input.
     * @param entries Number of Sprite entries to be shown.
     */
    Toolbar(View & view, int entries);

    /** End of Toolbar life. */
    virtual ~Toolbar();

    /** Get the visibility of this Toolbar.
     * @return true if the Toolbar is visible, false otherwise.
     */
    bool visible() const;

    /** Set the visibility of this Toolbar.
     * @param visible The new visibility state.
     */
    virtual void setVisible(bool visible);

    /** Callback for each frame animation. */
    virtual void tick() = 0;

    /** Sub classes should implement this method to update the button sprites
     * with the right images. */
    virtual void updateIcons() = 0;

    /** Cycle the TB position top -> right -> bottom -> left -> top. */
    void cyclePosition();

    /** Show an on-screen cursor.
     * @param x The cursor x position.
     * @param y The cursor y position.
     * @param cursorid The index of the sprite to use for this cursor.
     */
    void showCursor(int x, int y, int cursorid);

    /** Hide the cursor, making it invisible. */
    void hideCursor();

    //! Implement ViewI
    virtual void notify();

    /** Get the on screen Position of the Toolbar.
     * @return The position of the Toolbar. 
     * @see Position
     */
    Position position() const;

    virtual bool stylusUp(const Stylus * stylus);
    virtual bool stylusDownFirst(const Stylus * stylus);
    virtual bool stylusDownRepeat(const Stylus * stylus);
    virtual bool stylusDown(const Stylus * stylus);

  protected:
    /** The View item helps handle user input.  */
    View & m_view;
    typedef std::vector<nds::Sprite * > SpriteVector;
    //! Vector that holds the hardware sprites shown in this Toolbar.
    SpriteVector m_sprites;

    /** Handle the pressing of button.
     * @param i The index of the button pressed.
     */
    virtual void handlePress(int i) = 0;

    /** Do the layout of the buttons. */
    virtual void layout();

  private:
    // visible - ie. is showing
    bool m_visible;
    nds::Sprite *  m_cursorSprite;
    int m_touchedIndex;

    Position m_position;

    int touchedIndex(int x, int y) const;

};
#endif
