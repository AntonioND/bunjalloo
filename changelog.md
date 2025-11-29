# Bunjaloo Changelog

## Version DEV (XXXX-XX-XX)

- User changes:

  - Lists (unordered and ordered) are now supported. They aren't perfect, but
    they make lots of websites a lot more readable than before.
  - The system that lays out elements on the screen has been heavily reworked,
    which was needed to support lists. Some websites display more spaces than
    before in some places.
  - Links of the type `//yourdomain.com/about/example.html` now work. They are
    absolute links that contain everything except for the schema (HTTP, HTTPS,
    etc), which is preserved.
  - The download queue used to stop before some files were fully downloaded.
    This prevented some images from being displayed, for example.
  - MIME type `text/xhtml` is now allowed. It's parsed the same way as HTML.
  - Editable text areas were very buggy. The caret jumped around when editing
    text around the edges, adding and deleting characters could make the
    application crash. This has been fixed.
  - Combo boxes with entries that were too long displayed text outside of the
    box, on the right (only after selecting a too long entry). The text is now
    clipped correctly.
  - The font Bitstream Vera has been replaced by DejaVu, which is based on
    Bitstream Vera and adds a lot of additional characters. The license text has
    been replaced as well. Note that it wasn't possible to keep using the old
    TTF file because the converted font wasn't created using `convertftf`, but a
    conversion script that was removed before version 0.8.0.
  - Charset detection has been improved. Some websites, like Wikipedia, were
    using `meta` tags to communicate the encoding, and they were ignored by the
    parser.
  - Websites are now considered to be encoded in UTF-8 instead of ISO 8859-1.
    This is needed because some websites may communicate the encoding in ways
    that Bunjalloo doesn't understand, and most websites are in UTF-8 anyway.
  - ISO 8859-1 decoding has been improved. Replacement characters are printed
    when invalid encodings are found.
  - The home page of the browser has been simplified.
  - Websites that ended with an incomplete UTF-8 character would hang the HTML
    parser. This has been fixed.
  - Version 0.11.0 partially fixed a bug where combo boxes would remain open
    after going forwards or backwards in the browsing history, for example. This
    has now been fixed properly.
  - The SDL application will exit instead of crashing if the inittialization of
    SDL fails.

- Dev changes:

  - Components now support having indentation. ViewRender and BoxLayout have
    been modified to support components with indentation, like lists. In a list,
    new text areas are created whenever a new indentation is needed.
  - The HTML renderer and BoxLayout have been modified to support indentation.
  - 3 digit color codes are now supported in the HTML parser. They will be used
    to change the background color, but it isn't supported yet.
  - The HTML formatting of the in-application user documentation has been fixed.
  - The build system has been heavily refactored. Instead of building many
    libraries (libbunjalloo, libbwt, libndspp) and linking them with the final
    binary (Bunjalloo and unit tests), now there's a common makefile that
    generates libbunjalloo and the Bunjalloo binary. Thanks to this the
    makefiles of the tests only have to link one library instead of three. Also,
    it's easier to make changes to Bunjalloo because any changes in the code
    will trigger a rebuild of the final binary (with different libraries the
    Makefile of the binary couldn't detect changes to the libraries).
  - The HTML parser now looks for `noscript` tags explicitly. Previously it
    still displayed the contents inside of them, but now it will be easier to
    change their display style if needed.
  - libvera has been renamed to libfont to be more generic and to not require
    more changes in the future.
  - Some outdated documentation has been cleaned and moved out of the wiki
    folder where they were initially saved.
  - All remaining tests with issues have been fixed. For example, the updater
    tests no longer crash on exit (it was caused due to a call to a pure virtual
    function in the Client class, which has been fixed).
  - There is now some debug code that paints rectangles around website elements
    on the screen. This is useful for debugging only, and it requires a rebuild
    to be enabled.
  - Some hardcoded strings have been replaced by constant names.
  - Bunjalloo now requests UTF-8 encoding before ISO 8859-1 in GET requests.
  - `convertftf` has been improved:

    - It has more error checks.
    - It allows cutting off pixels from the top or bottom of the font so that it
      fits better in the current widgets.
    - Color rounding has been simplified.
    - An additional shell script has been added to convert DejaVu automatically
      instead of having to remember how to run `convertftf` every time.

## Version 0.11.0 (2025-11-16)

When updating to this version you should keep your `data/bunjalloo/user` files
safe, but you should delete all other files and folders from `data/bunjalloo`.
The new version includes all the required files in the NDS ROM and it creates
all temporary and user folders automatically.

- User changes:

  - Slot-2 RAM expansion cartridges (such as the one that came with Opera) is
    now supported. They are used as temporary storage for images.
  - Search engine shortcuts have been updated. Google and Yahoo have been
    removed because they no longer work. The Wikipedia search string has been
    updated, and DuckDuckGo, FrogFind! and GitHub have been added to the list.
  - The URL used by the auto-updater has been fixed, so future versions of
    Bunjalloo can be downloaded by Bunjalloo itself. Note that auto-update only
    seems to work if the Bunjalloo ROM is in the root of the SD card.
  - The NDS ROM of Bunjalloo now contains all the files it needs to run, it
    doesn't need to be distributed with a folder with lots of extra files.
    `data/bunjalloo` is now only used as temporary storage and for user
    configuration storage. Unfortunately, changing the font of the CA
    certificates now requires the user to use `ndstool` to modify the NitroFS
    files. Also, if `data/bunjalloo` doesn't exist at boot it will be created
    automatically.
  - The text box used for keyboard input has been made a lot more stable.
    Sometimes some lines would be too long to fit and they would overflow to the
    next one displaying the last character. Sometimes deleting a character or
    adding a character would make the caret go over the line limit and cause a
    crash. This has been fixed. Also, now it's possible to press the white area
    after the text and the caret will be set to the last line with text instead
    of disappearing.
  - A new protocol called `about://` has been added. This protocol is used for
    built-in pages that aren't stored in the filesystem or downloaded. Also, the
    home page `file://licence` has been moved to a file in NitroFS.
  - The `file://` URI schema support has been improved. Now it supports servers.
    `file://localhost/file.txt` accesses a file at the root of the filesystem,
    `file://bunjalloo/file.txt` accesses a file inside `data/bunjalloo`, and
    `file://nitro/file.txt` accesses a file inside the NitroFS filesystem of
    Bunjalloo.
  - A new internal page called `about://sysinfo` has been added. It shows system
    information such as available RAM. This page can be loaded while browsing a
    website and it will show the memory available after loading that website.
  - Add missing credits and licenses to the homepage of the browser.
  - Implement option that allows user to decide to clear the cache at boot or
    not. The setting was there, but it was unused.
  - Transparency has been fixed in images that don't need to be scaled down to
    fit on the screen.
  - The palette of the scroll touch screen icon is now loaded correctly so that
    it doesn't overlap with the colors used by the toolbar.
  - Combo boxes have been made a lot more stable. Boxes that were too big would
    have a scroll bar outside of the screen (or cause a crash). If a combo box
    was left open and the user, for example, pressed "back" in the toolbar, or
    button Y to open the bookmarks, it would cause a crash. This all has been
    fixed in this version.
  - In the SDL version, allow the user to set the gap between screens and the
    zoom factor with the environment variables `BUNJALLOO_SCREEN_GAP` and
    `BUNJALLOO_SCREEN_SCALE`. `NDS_GAP` is no longer supported.
  - The SDL window now displays "Bunjalloo" as title instead of "SDL
    Application".
  - In the SDL version, keyboard input with the PC input now works in a fluid
    way rather than having to wait for a second or so between each key press.
    Backspace and return keys are also supported now.
  - Some checks have been added to the Canvas class to avoid crashes because of
    writting out bounds.
  - Environment variable `NDS_DLDI` has been renamed to `BUNJALLOO_DLDI` and
    `NDS_LANG` has been renamed to `BUNJALLOO_LANGUAGE`.

- Dev changes:

  - The HTML parser doesn't support partial parsing of data. The code is
    organized to fed it partial data, but the parser has several loops that
    assume that there is more data to be used, and they hang if there isn't more
    data. Instead of modifying the whole parser, a partial fix has been added:
    the HTML parser is now only called when the full website has been
    downloaded. This only fixes well-formed websites, and it will still hang on
    malformed websites.
  - Codebase reorganized. Different libraries are now in different folders and
    can be built independently as static archives. This makes it possible to
    build tests without rebuilding all the codebase for each tests.
  - Fully replace the old build system by a new one. Tools and tests can now be
    built with the new build system. Tests can also be run automatically with
    `make PLAT=SDL run` from the `tests` folder.
  - GoogleTest v1.17.0 has been added to the repository. It is used by the test
    binaries.
  - The instructions to change the TTF font used by Bunjalloo have been updated
    to work with `convertftf`.
  - The crashes in the HtmlParser tests have been fixed.
  - Some tests have been fixed. Some of them were broken during the development
    of versions 0.9.0 and 0.10.0 because the behaviour of some things changed.
  - Most tests can be run automatically with one command from make.
  - Many classes have been refactored to require fewer dynamic instantiations
    and reduce heap fragmentation.
  - Lots of checks have been added to dynamic allocations. This should prevent
    some crashes due to running out of memory.
  - Some memory leaks have been fixed.
  - C++ exceptions have been enabled in DS builds. They were already enabled in
    SDL builds.
  - Some code has been moved to ITCM to increase speed.
  - The home page is now a file in NitroFS rather than being hardcoded in the
    Bunjalloo binary.
  - Cache management has been fixed. The destination (VRAM) was flushed instead
    of the source (main RAM).

## Version 0.10.0 (2025-11-02)

- Fixed SSL connections. Entropy is now gathered correctly and the main thread
  yields every now and then to let the SSL handshake finish.
- Support checking CA certificates when using SSL. This uses a lot of RAM and
  it's a bit slow so it's disabled by default, and there's a new option in the
  settings menu to enable it or disable it. Also, users can provide their own
  sets of CA certificates.
- Fixed the type of position markers when searching strings. It was only valid
  in 32-bit platforms like the DS, so some code wasn't working on the SDL port.
  This prevented the configuration settings page from showing up.
- The DS version now expects the `data/bunjalloo` folder to be in the root of
  the SD, not in the same folder as the NDS ROM.
- The home page has been updated. The page with the licenses used by components
  of Bunjalloo has been split into multiple pages. Also, the licenses of
  BlocksDS libraries are now included as well.
- Support BMP and APNG formats.
- Fix scale down calculations for very big images. Sometimes they were left too
  big in one dimension.
- Display transparent pixels as white instead of black.
- A bad debug check has been removed. This prevented some elements from loading,
  like some (but not all) images.
- Improved handling of errors when receiving packets.
- Restore code that parses dates.
- Fix timeout counter so that it uses seconds instead of retry attempts.
- Zlib has been updated to version 1.3.1.
- Some unused features of Mbed TLS have been disabled to reduce code size.
- The code is now built with `-Os` to save RAM.
- Flush data cache before doing a DMA copy.
- Removed some superfluous waits for VBL.
- Fixed some warnings, silenced some of them.
- Some internal code cleanup.
- Updated instructions in readme.

## Version 0.9.0 (2025-10-22)

- Migrated to BlocksDS from devkitARM. Now Bunjalloo can take advantage of the
  Wifi driver in DSi mode, with WPA2 support and the additional RAM of DSi.
- The old versions of libpng, libjpeg and giflib have been replaced by libplum.
- MatrixSSL isn't currently maintained and it has been replaced by Mbed TLS.
  Switching between HTTP and HTTPS is still a bit unreliable.
- The PC port has been fixed. It builds, some infinite loops have been fixed,
  and images are now displayed correctly.
- Test can't be built at the moment, they will be re-enabled in the future.
- Initialize keyboard state correctly so that the extra symbols aren't displayed
  after the first key press.
- Modify cache CRC32 calculation to include the protocol, not just the rest of
  the address. This lets us retry a website with http and then https if it
  fails, for example. Previously it would just reload the cached error page.
- Added missing copyright notices to the "show copying" page.

## Version 0.8 (2009-12-21)

- Updated to the latest devkitARM, libnds, libfat and libpng
- Fix the updater that was broken in 0.7.7
- Fix some layout randomness (on e.g config screen)
- Change the progress bar to be textless
- Fix for occasional giant whitespaces
- Use last-modified in cache calculations
- Fix flashing when swapping from browser to keyboard
- Show on-screen password fields with circles
- Change the password field to use circles
- Speed up scrolling
- Performance improvements
- Change to Okiwi font generation and rendering
- Fix some major memory leaks

## Version 0.7.7 (2009-10-31)

- Spaces at the start of elements change
  ([issue 112](https://code.google.com/p/quirkysoft/issues/detail?id=112))
- Fix relative query links
  ([issue 110](https://code.google.com/p/quirkysoft/issues/detail?id=110))
- Tabs were shown as an invalid character, instead of a space
- Warranty shown as html

## Version 0.7.6 (2009-07-23)

- Added saving, loading and expiring of cookies
- Fix several cookie-related bugs
- Fix redirects to absolute locations. This fixes mobile Google Reader, amongst
  other things
- Fix crash bug while editing bookmarks when you deleted the newline
- Show the could-not-load error page again
- Show the mime-type on pages that are not supported
- New spinner icon (Thanks to an anonymous reader for that)
- Fake WAP support for google.com/xhtml
- Fix the preferences options for cookie permissions per domain
- Fix appending text to HTML node attribute value (isue 10??)
- Fix crash when trying to connect to an incorrect address
  ([issue 103](https://code.google.com/p/quirkysoft/issues/detail?id=103))
- Escape + on input fields
  ([issue 81](https://code.google.com/p/quirkysoft/issues/detail?id=81))
- Try and improve HTTP connection responsiveness
- Allow user agent switching via "expert" configuration - set useragent=blah in
  config.ini
- Update to the latest versions of 3rd party libraries (matrix SSL 1.8.7,
  libpng 1.2.38, latest libraries from devkitPro)
- Moved the code to Google Code's Mercurial hosting

## Version 0.7.5 (2009-07-08)

- Improvements to caching - logging in to GMail works again
  ([issue 99](https://code.google.com/p/quirkysoft/issues/detail?id=99))
- Clicking preference icon goes straight to preferences
- Fix encoding problems
  ([issue 98](https://code.google.com/p/quirkysoft/issues/detail?id=98))
- Fixed lots of non-ascii character keyboard bugs
- Fix configuration changes that use escapable % characters
- you may have to manually fix the download path in your configuration settings

## Version 0.7.4 (2009-07-02)

- Fixed freezing after only a few page loads
  ([Issue 97](https://code.google.com/p/quirkysoft/issues/detail?id=97))

## Version 0.7.3 (2009-07-01)

- A new NDS banner bitmap by Sam Goldfield
- Add new scrolling method - just hold and drag, sort of
- Page up and down now scroll just 1 screen
- Fix for linked image following a text link, which caused an unclickable text
  link
- Fix crash caused by bad HTML entities
- Fix memory leak when unzipping files
- Fix for some rarer types of PNG image
- Disable button shortcuts when keyboard is visible
- Remove automatic DLDI patching of unzipped NDS files
- Update to devkitPro r26 and libpng 1.2.37

Please read [my blog post](http://quirkygba.blogspot.com/2009/06/ez-flash-vi-and-libnds-136.html)
if you use an EZ Flash Vi as touch screen may not work.

## Version 0.7.2 (2008-12-28)

- Update code for devkitARM r24
- Italian localisation courtesy of Paolo Ferraresi

Automatic DLDI patching of downloaded NDS files from within zips no longer
works. I'll remove the option in the next release. I can no longer test
patching, as my card's firmware auto patches anyway. If this is a feature you
really need... er.. sorry!

## Version 0.7.1 (2008-10-05)

- Fix url parsing stuff so Google Images searches show
- Added \ to the keyboard * Add ABXY functions (A=URL, B=Stop, X=edit config,
  Y=Bookmark)
- Add left-right handedness switch for d-pad and ABXY
- Fix for [issue 74](https://code.google.com/p/quirkysoft/issues/detail?id=74),
  bookmarks have incorrect title

## Version 0.7 (2008-09-02)

- User visible changes

  - Added inline images
  - Add "images" option to config to turn on/off inline images
  - Show images as they download
  - Image: Fix some JPEG and GIF crashes
  - Recognise image/jpg alternate MIME type
  - Fix flickering when scrolling to top or bottom
    ([issue 70](https://code.google.com/p/quirkysoft/issues/detail?id=70))
  - Some small performance improvements
    ([issue 55](https://code.google.com/p/quirkysoft/issues/detail?id=55))
  - Show GPL Licence on splash screen correctly
  - Fix HTTPS pages

- Development stuff:

  - Doxygen cleanups
  - Add debugging options ([google-perftools](http://code.google.com/p/google-perftools/)
    and [gcov](http://gcc.gnu.org/onlinedocs/gcc/Gcov.html))
  - Changed the installation from source
  - Update build system to waf-1.5 (use [waf trunk](http://code.google.com/p/waf))

- Known issues:

  - GIFs are not fully supported - no anims, not all types
  - No way to save images unless they are linked to
  - Show/hide bookmarks still flickers
  - Progress bar doesn't refresh correctly on unzip and save-as

## Version 0.6.3 (2008-07-23)

Tiny release to get back into the groove. :-)

- [Issue 60](https://code.google.com/p/quirkysoft/issues/detail?id=60): Fix the
  flashing when downloading files

## Version 0.6.2 (2008-05-25)

- Fix SSL connections (https)
- Add sleep mode when closing the lid

## Version 0.6.1 (2008-05-10)

- French translation updated by Kikdifferent
- German translation updated by Pascal Braig
- Dutch translation updated by Maikel Steneker
- Fixes to the updater - upgrading automatically from 0.6 doesn't work, it just
  hangs :-(
- When typing in fields, show web page on top screen
- Checks for fatal conditions at boot up (no DLDI, no data dir)
- Fix black lines at the end of some images.
- Compiled with devkitArm r23
- Fix interlaced png images
- Can turn off the wifi connection by clicking the connection icon
- Fix scrolling way off the bottom of the page that sometimes happened

## Version 0.6 (2008-04-18)

- Enhancements

  - Auto-updater; semi-automatic as the user has to push a button :-)
  - Bookmark editing/deleting.
  - Now possible to change sites allowed to set cookies - delete/modify the
    cookie list on the fly.
  - Default download directory is configurable (set to root by default).
  - Run time configuration for language, etc.
  - Send the Accept-Language header with the current language setting.
  - Dutch translation added. Note: Only English and Spanish are fully up to
    date, the rest require some community help plz! :-)
  - Zip file viewer/extractor.
  - Unzip archives and optionally patch any `*.nds` files with DLDI.

- Bug fixes

  - Fix keyboard modifier states - caps lock and shift should appear pressed
    more often.
  - Show — – entities, some iso-8859-1 fixes.
  - Content-Type application/xhtml accepted, makes more sites readable.
  - Use Content-Disposition header as a good guess for downloaded file names.

- For developers

  - Instructions for building are in the source tar.
  - Update libpng to version 1.2.26
  - Updated the script that downloads dependencies so it actually works.
  - Autoconf-like dependency checks added (`./configure && make`)

## Version 0.5.4 (2008-03-04)

- Improved save-as to check mime type.
- Add Referer header.
- Add ~ to the keyboard.
- Add a progress bar.
- Add configurable language support.
- Translations for German and French. Thanks to Pascal Braig and dr.
- Friendlier configuration with no overwriting of files.
- Update giflib, compile jpeg library with better options for a size decrease.
- Add internal page links (e.g. /somepage#internal )
- Add a title to the initial screen.
- Fix crash bug when title is empty.
- Fix combo box slowness.

## Version 0.5.3.1 (2008-02-19)

- Fix clicking URL on url/image menu, as reported in
  [issue 44](https://code.google.com/p/quirkysoft/issues/detail?id=44)

## Version 0.5.3 (2008-02-15)

- Enhancements

  - More visual feedback when touching links, buttons, form elements, etc.
  - Add a basic bookmarking feature.
  - Show visited links in a different colour.

- Minor UI Changes:

  - The auto-hide menu is now manual-hide.
  - Online status and spinner are one icon. Reload/stop are one button.

- Bugs squashed:

  - Stop the image URL menu from going out of bounds.
  - Fix crash bug caused by some URLs with ":" in them.
  - Page position remembered on fwd/back for touch screen scrolling too.

## Version 0.5.2 (2008-01-28)

A very minor release.

- The only user-visible change is the use of the Independent JPEG Group's JPEG
  library instead of the jpegd library.

## Version 0.5.1 (2007-11-17)

- No longer fails when SSL certificate chain validation does not pass.
- Fix for SSL links stopping early in some cases.
- Add context information above the keyboard.

## Version 0.5 (2007-11-11)

- Add HTTPS for proxy and direct connections.
- Make cookies work more thoroughly.
- Bug fixes (add name to .nds again, content refreshing fix, tags after `</body>`)
- Update to dswifi 0.3.4

## Version 0.4 (2007-11-04)

- Show small pages and images at the top of the lower screen.
- Make right/left on the D-pad do a page down/up.
- Add keyword searches from url entry. (g, y and w for Google, Yahoo and
  Wikipedia)
- Add page title rendering
- Add "save as" option
- Remember the scroll position of previously visited pages.
- Fix crash when showing a form after no `</form>` tag on last page.

## Version 0.4-beta1 (2007-10-29)

- Major changes

  - Basic image support added
  - gzip content-encoding supported
  - Cache files to flash card (mostly optional - see config.ini)
  - Minor changes
  - [issue 32](https://code.google.com/p/quirkysoft/issues/detail?id=32) - file
    that defines allowed cookies can now contain `http://` at the start
  - Plain text rendering semi works again (no font changes though)
  - Unicode shrunk to 16 bits internally to use less memory
  - !URIs with spaces are escaped to use "%20"
  - libnds-20071023 compatibility changes

- Build changes

  - Build with devkitArm r21
  - Changed to [scons](http://www.scons.org/) build system

## Version 0.3.6 (2007-10-20)

- Fix newline bug in header parser for servers that send \n instead of \r\n
- Remove the accidental inclusion of cache dir
  ([issue 33](https://code.google.com/p/quirkysoft/issues/detail?id=33))

## Version 0.3.5 (2007-10-7)

- Fix crash in the HTML parser from
  [Issue 29](https://code.google.com/p/quirkysoft/issues/detail?id=29)

## Version 0.3.4 (2007-10-03)

- Fix crash deleting text when editing a URL
  ([issue 28](https://code.google.com/p/quirkysoft/issues/detail?id=28))
- Do not initialise sound system as it isn't used.

## Version 0.3.3 (2007-10-01)

- Background colour removed (fixes
  [issue 26](https://code.google.com/p/quirkysoft/issues/detail?id=26)). It
  wasn't fully implemented and caused some pages to be unreadable.

## Version 0.3.2 (2007-09-29)

- Cancel URL no longer reloads page (fixes
  [issue 25](https://code.google.com/p/quirkysoft/issues/detail?id=25))

## Version 0.3.1 (2007-09-27)

- Regression in charset handling corrected.
- Potential crash when text area scrolls off the top of the screen fixed.

## Version 0.3 (2007-09-24)

- Complete GUI rewrite. Fixes [issue 7](https://code.google.com/p/quirkysoft/issues/detail?id=7)
  and (most of) [issue 5](https://code.google.com/p/quirkysoft/issues/detail?id=5).
- Much better connection handling, fixed
  [issue 9](https://code.google.com/p/quirkysoft/issues/detail?id=9). Uses
  hot-off-the-compiler dswifi 0.3.3 too.
- Better rendering, less multiple blank line problems.
- Completely new keyboard display. Fixed the underscore problem from
  [issue 6](https://code.google.com/p/quirkysoft/issues/detail?id=6).
- Proxy server configuration added.
- Changed license to GPL v3
- Added tool bar (back, forwards, stop, load page, wifi status)
- Easier installation - just unzip to the card, no more "bunjalloo-dist" dir.
- Fixed [issue 6](https://code.google.com/p/quirkysoft/issues/detail?id=6),
  [issue 8](https://code.google.com/p/quirkysoft/issues/detail?id=8),
  [issue 15](https://code.google.com/p/quirkysoft/issues/detail?id=15),
  [issue 16](https://code.google.com/p/quirkysoft/issues/detail?id=16) and
  [issue 18](https://code.google.com/p/quirkysoft/issues/detail?id=18)
- Prior to the final version, several more bugs were squashed!

**Important!** Don't forget to copy the new contents of the `data/bunjalloo`
directory across. The font files have been slightly altered and the new
toolbar.png file is required.

## Version 0.2.1 (2007-08-03)

- Bug fix release. The only difference to 0.2 was a fix for
  [Supercard CF compatibility](http://code.google.com/p/quirkysoft/issues/detail?id=1&can=1).

## Version 0.2 (2007-07-30)

- Removed the built-in font - now **requires** DLDI patching
- Escaping of forms and urls. Now urls with "&" should work as expected.
- Parsing of .. in urls. So href="../whatever" will work
- Added history - left and right go forwards and backwards.
- POST form handling added.
- Long lines wrap
- Spaces added to keyboard - allowing multi-word searches on Google.
- Password fields added - though without text fields and a full keyboard they
  are a bit useless...
- Highlight link when touched.
- Cookies added - uses a whitelist of allowed cookies, all others are refused.
  **Empty** default whitelist for cookies stored in
  `/data/bunjalloo/user/ckallow.lst`
- Configuration file added, to allow for (future) customisation.
- Allow "file:" to "http:" URL navigation, but not the other way around.
- Updated dswifi to 0.3.2 to fix problems on newer DS lites.
