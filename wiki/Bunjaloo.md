# Bunjalloo

Please use the [mailing list](http://groups.google.com/group/bunjalloo-discussion)
for comments.

## Installation

### DLDI Patching

Bunjalloo v0.2 and upwards requires [DLDI](http://chishm.drunkencoders.com/DLDI/)
patching. The steps to install are:

- Unzip the [downloaded zip file](http://code.google.com/p/quirkysoft/downloads/list)
  to the root of your flash card.
- Patch bunjalloo.nds with the [DLDI](http://chishm.drunkencoders.com/DLDI/) tool.

DLDI is needed becuase the font files are no longer encrusted into the binary.
Instead they are read from the slot-1 media. Your flash card should have the
following directory structure:

```
|-- bunjalloo.nds
`-- data/
    `-- bunjalloo
        |-- config.ini
        |-- docs
        |   |-- copying
        |   `-- warranty
        |-- fonts
        |   |-- sans.img
        |   |-- sans.map
        |   |-- sans.pal
        |   `-- toolbar.png
        `-- user
            |-- ckallow.lst
            `-- search.cfg
```

The distribution zip file already contains these folders. Unzipping it to the
root folder of the flash card should create the appropriate structure. In fact
the bunjalloo.nds file can be placed in any directory, it doesn't have to go in
the root of the card - what matters is the **data** directory.

- `toolbar.png` is the image used for the buttons. It is an 8-bit color-mapped
  PNG image.
- `ckallow.lst` contains URLs of sites that allow cookies.
- `search.cfg` contains search keyword shortcuts. The "%s" is replaced with the
  search text

### Earlier Version

Bunjalloo v0.1 (Deprecated) used DLDI for viewing local files (those with a
`file://` URL) only. To view remote `http://` URLs, no patching was required.
Bunjalloo 0.1 could be run using Wireless Multiboot, without an attached
cartridge.

## Controls

Action     | What it does
-----------+--------------
Start      | Enter a URL (show keyboard)
Select     |
Up/Down    | Scroll screen up and down
Left/Right | Scroll screen a page at a time
R          | Move forwards in the history
L          | Move backwards in the history
Touch      | Follow link, enter data in forms, press keys

## Edit Bookmarks

In order to edit a bookmark, first select the bookmark icon at the bottom of the
screen.

https://wiki.quirkysoft.googlecode.com/hg/images/bunjalloo-main.png

Next, **hold the stylus down** on the bookmark you want to edit for a couple of
seconds.

https://wiki.quirkysoft.googlecode.com/hg/images/bunjalloo-press.png

This causes a secret menu to appear! Select Edit or Delete from the menu. Delete
is irreversible I'm afraid, so be careful.

https://wiki.quirkysoft.googlecode.com/hg/images/bunjalloo-menu.png

Editing the bookmark will lead you to the keyboard input screen. The first line
is the URL of the bookmark, the other lines are the text shown for this marker.

## Edit Prefs

Changing the options is done via the option menu. To open this menu, click the
icon shown below - it looks like a spanner and screwdriver.

https://wiki.quirkysoft.googlecode.com/hg/images/bunjalloo-prefs-main.png

The icons shown on this menu are, from left to right:

https://wiki.quirkysoft.googlecode.com/hg/images/bunjalloo-prefs.png

- Return to the browser menu
- Edit the cookie permissions list
- Add the current domain/server to the allowed cookie list
- Edit the search list (unimplemented!)
- Add to the search list (unimplemented!)
- Edit the options

Clicking on each active icon, apart from the return one of course, takes you to
a new screen with more self-explanatory options.

## About Bunjalloo

Bunjalloo is a HTTP user agent (web browser) for the Nintendo DS. It has been
implemented from scratch [using the whatwg user agent design rules](http://www.whatwg.org/).

https://wiki.quirkysoft.googlecode.com/hg/images/bunjalloo-ds.png

## Availability

Bunjalloo is freely available under the GNU General Public License. See
COPYRIGHT and http://code.google.com/p/quirkysoft/ for further details.
Bunjalloo can be downloaded from the
[download section](http://code.google.com/p/quirkysoft/downloads/list) of this
site.

## Changing the font

Not available in 0.1. If you want to use a different font, then you will need
the contents of the
[bunjalloo/tools](http://quirkysoft.googlecode.com/svn/trunk/bunjalloo/tools/)
directory. Run the convert.sh script passing the True Type Font name and the
point size desired. It should generate 3 files, the raw image data, the palette
and the mappings. Copy these into the `/data/bunjalloo/fonts` directory.

To add extra ranges of UTF-8 glyphs, then **all** the ranges required must be
passed on the command line after the size. By default the glyphs included are:
`0x1f-0x7f, 0xa0-0x100, 0x20AC-0x20AD, 8211-8212, 8216-8218, 8220-8221,
8226-8226` These are used if no ranges are given. Here `0x1f-0x7f` means "the
range 0x1f (31) to 0x7f (127) inclusive". This covers all ASCII characters, the
extra latin ones (accented a's and e's, etc) and some common symbols - the Euro
sign, copyright and so on.

The font name is defined in `/data/bunjalloo/config.ini`. Change the line
font=fonts/vera to indicate the basename of the new font. For example, to
generate a new 12 point font from the font MyFont.ttf, one could do this:

```
./convert.sh MyFont.ttf 12 cp myfont.* /media/disk/data/bunjalloo/fonts/
sed -i 's/vera/myfont/' /media/disk/data/bunjalloo/config.ini
```

The output font name is always all-lowercase.
