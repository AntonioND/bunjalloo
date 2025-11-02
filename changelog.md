# Bunjaloo Changelog

## Version DEV

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

## Version 0.9.0

- Migrated to BlocksDS from devkitARM. Now Bunjalloo can take advantage of the
  Wifi driver in DSi mode, with WPA2 support and the additional RAM of DSi.
- The old versions of libpng, libjpeg and giflib have been replaced by libplum.
- MatrixSsl isn't currently maintained and it has been replaced by Mbed TLS.
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

## Changes 0.8

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

## Changes 0.7.7

- Spaces at the start of elements change
  ([issue 112](https://code.google.com/p/quirkysoft/issues/detail?id=112))
- Fix relative query links
  ([issue 110](https://code.google.com/p/quirkysoft/issues/detail?id=110))
- Tabs were shown as an invalid character, instead of a space
- Warranty shown as html

## Changes 0.7.6

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

## Changes 0.7.5

- Improvements to caching - logging in to GMail works again
  ([issue 99](https://code.google.com/p/quirkysoft/issues/detail?id=99))
- Clicking preference icon goes straight to preferences
- Fix encoding problems
  ([issue 98](https://code.google.com/p/quirkysoft/issues/detail?id=98))
- Fixed lots of non-ascii character keyboard bugs
- Fix configuration changes that use escapable % characters
- you may have to manually fix the download path in your configuration settings

## Changes 0.7.4

- Fixed freezing after only a few page loads
  ([Issue 97](https://code.google.com/p/quirkysoft/issues/detail?id=97))

## Changes 0.7.3

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

## Changes 0.7.2

- Update code for devkitARM r24
- Italian localisation courtesy of Paolo Ferraresi

Automatic DLDI patching of downloaded NDS files from within zips no longer
works. I'll remove the option in the next release. I can no longer test
patching, as my card's firmware auto patches anyway. If this is a feature you
really need... er.. sorry!

## Changes 0.7.1

- Fix url parsing stuff so Google Images searches show
- Added \ to the keyboard * Add ABXY functions (A=URL, B=Stop, X=edit config,
  Y=Bookmark)
- Add left-right handedness switch for d-pad and ABXY
- Fix for [issue 74](https://code.google.com/p/quirkysoft/issues/detail?id=74),
  bookmarks have incorrect title

## Changes 0.7

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

## Changes 0.6.3 (tiny release to get back into the groove :-)

- [Issue 60](https://code.google.com/p/quirkysoft/issues/detail?id=60): Fix the
  flashing when downloading files

## Changes 0.6.2

- Fix SSL connections (https)
- Add sleep mode when closing the lid

## Changes 0.6.1

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

## Changes 0.6

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

## Changes 0.5.4

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

## Changes 0.5.3.1

- Fix clicking URL on url/image menu, as reported in
  [issue 44](https://code.google.com/p/quirkysoft/issues/detail?id=44)

## Changes 0.5.3

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

## Changes 0.5.2 (a very minor release)

- The only user-visible change is the use of the Independent JPEG Group's JPEG
  library instead of the jpegd library.

## Changes 0.5.1

- No longer fails when SSL certificate chain validation does not pass.
- Fix for SSL links stopping early in some cases.
- Add context information above the keyboard.

## Changes 0.5

- Add HTTPS for proxy and direct connections.
- Make cookies work more thoroughly.
- Bug fixes (add name to .nds again, content refreshing fix, tags after `</body>`)
- Update to dswifi 0.3.4

## Changes 0.4-beta1 to 0.4

- Show small pages and images at the top of the lower screen.
- Make right/left on the D-pad do a page down/up.
- Add keyword searches from url entry. (g, y and w for Google, Yahoo and
  Wikipedia)
- Add page title rendering
- Add "save as" option
- Remember the scroll position of previously visited pages.
- Fix crash when showing a form after no `</form>` tag on last page.

## Changes from version 0.3.6 to 0.4-beta1

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

## Bug fixes in version 0.3.6

- Fix newline bug in header parser for servers that send \n instead of \r\n
- Remove the accidental inclusion of cache dir
  ([issue 33](https://code.google.com/p/quirkysoft/issues/detail?id=33))

## Bug fixes in version 0.3.5

- Crash in the HTML parser from
  [Issue 29](https://code.google.com/p/quirkysoft/issues/detail?id=29)

## Bug fixes in version 0.3.4

- Crash deleting text when editing a URL
  ([issue 28](https://code.google.com/p/quirkysoft/issues/detail?id=28))
- Do not initialise sound system as it isn't used.

## Bug fixes in version 0.3.3

- Background colour removed (fixes
  [issue 26](https://code.google.com/p/quirkysoft/issues/detail?id=26)). It
  wasn't fully implemented and caused some pages to be unreadable.

## Bug fixes in version 0.3.2

- Cancel URL no longer reloads page (fixes
  [issue 25](https://code.google.com/p/quirkysoft/issues/detail?id=25))

## Bug fixes in version 0.3.1

- Regression in charset handling corrected.
- Potential crash when text area scrolls off the top of the screen fixed.

## Changes in version 0.3

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

## Changes in version 0.2.1

- Bug fix release. The only difference to 0.2 was a fix for
  [Supercard CF compatibility](http://code.google.com/p/quirkysoft/issues/detail?id=1&can=1).

## Changes in version 0.2

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
