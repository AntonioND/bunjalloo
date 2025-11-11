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
#ifndef ScrollPane_h_seen
#define ScrollPane_h_seen

#include "Component.h"
#include "StylusListener.h"
#include "ScrollBar.h"
#include "Palette.h"
#include "Sprite.h"

class ScrollBar;
namespace nds { class Sprite; }

/** A scrollable view of many Components. The view may be larger than the
 * screen and this class handles the positioning of all the child widgets. */
class ScrollPane: public Component
{

  public:
    /** Set a pop up menu that will override the display and input handling.
     * @param popup the menu item to show.
     */
    static void setPopup(Component * popup);

    /** Remove a pop up menu.
     * @param popup the menu item to show, if the current menu is not this one,
     * then nothing is done.
     */
    static void removePopup(Component * popup);

    /** Initialise the ScrollPane.*/
    ScrollPane();

    /** Delete the ScrollPanes child components.*/
    ~ScrollPane();

    /** Set the incremental amount that each up/down press moves.
     * @param scrollIncrement the amount to scroll in pixels.
     */
    void setScrollIncrement(int scrollIncrement);

    /** Get the current scroll increment.
     * @return the scroll increment in pixels.
     */
    int scrollIncrement() const;

    /** Move the contents a screen up */
    void screenUp();
    /** Move the contents a screen down */
    void screenDown();

    void pageUp();
    void pageDown();
    /** Scroll the view up one amount.  */
    void up();
    /** Scroll the view down one amount.  */
    void down();
    /** Scroll the view up one block (half screen/bounds size).  */
    void upBlock();
    /** Scroll the view down one block (half screen/bounds size).  */
    void downBlock();

    /** Set if the child layout should be stretched horizontally to fill the space available.
     * @param s the flag to use for setting the stretchiness.
     */
    void setStretchChildren(bool s=true);

    /** Scroll to a "percentage" (0 = 0%, 256 = 100%) of the screen.
     * @param value the percentage of the screen to scroll to, represented in fixed point.
     */
    void scrollToPercent(int value);

    /** Get the current position as a fixed point percentage.
     * @return the percentage of the screen scrolled.
     */
    int currentPosition() const;

    /** Set this to be the top level ScrollPane. The top level pane is special
     * as its scroll bar does not fill the whole height, but is shown only on
     * the bottom screen. The top level ScrollPane also handles the pop-up menu.
     * @param topLevel true if this is the top level scroll pane. false
     * otherwise. ScrollPanes are initialised to not be the top level one.
     */
    void setTopLevel(bool topLevel=true);

    /** Get the current top level state.
     * @return true if this is the top level ScrollPane.
     */
    bool topLevel() const;

    /** Set the background fill colour shown between child Components.
     * @param color the color to use.
     */
    void setBackgroundColor(unsigned short color);
    unsigned short backgroundColor() const;

    bool scrollBarHit(int x, int y);

    // From Component
    virtual void paint(const nds::Rectangle & clip);
    virtual void setSize(unsigned int w, unsigned int h);
    virtual void setLocation(int x, int y);

    virtual bool stylusUp(const Stylus * stylus);
    virtual bool stylusDownFirst(const Stylus * stylus);
    virtual bool stylusDownRepeat(const Stylus * stylus);
    virtual bool stylusDown(const Stylus * stylus);

    void forceRedraw();

    int visibleHeight() const;

  private:
    static Component * s_popup;
    int m_scrollIncrement { 1 };
    int m_distanceScrolled { 0 };
    bool m_topLevel { false };
    bool m_canScrollUp { false };
    bool m_canScrollDown { false};
    ScrollBar m_scrollBar { };
    unsigned short m_backgroundColour { nds::Color(31,31,31) };
    bool m_stretchChildren { false };
    bool m_touchedMe { false };

    nds::Sprite m_scrollAnywhereSprite;
    nds::ObjectPalette m_scrollAnywherePalette;

    void layoutChildren();
    void calculateScrollBar();

    void showScrollBar(const nds::Rectangle & clip);
    void adjustScroll(int & scrollIncrement);
    void adjustScrollUp(int & scrollIncrement);
    enum ScrollType
    {
      SCROLL_NO_ADJUST,
      SCROLL_WITH_ADJUST,
    };
    void up(ScrollType type);
    void down(ScrollType type);
    bool anywhereScroll(const Stylus *stylus);
    void showScrollAnywhere(const Stylus *stylus);
    void hideScrollAnywhere();
    bool isScrollBarShowing() const;

    DISALLOW_COPY_AND_ASSIGN(ScrollPane);
};

#endif
