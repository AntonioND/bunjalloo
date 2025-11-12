# Bunjalloo user guide

## Edit Bookmarks

In order to edit a bookmark, open the bookmarks menu. Next, **hold the stylus
down** on the bookmark you want to edit for a couple of seconds.

This causes a secret menu to appear! Select `Edit` or `Delete` from the menu.
`Delete` is irreversible so be careful.

Editing the bookmark will lead you to the keyboard input screen. The first line
is the URL of the bookmark, the other lines are the text shown for this marker.

## Change settings

Changing the settings is done via the settings menu. To open this menu, click on
the icon that looks like a spanner and screwdriver.

The icons shown on this menu are, from left to right:

- Return to the browser menu
- Edit the cookie permissions list
- Add the current domain/server to the allowed cookie list
- Edit the search list (unimplemented!)
- Add to the search list (unimplemented!)
- Edit the options

Clicking on each active icon, apart from the return one of course, takes you to
a new screen with more self-explanatory options.

## Using CA certificates

The CA certificates used by Bunjalloo are found in the SD card in
`sdroot/data/bunjalloo/cert/cacert.pem`. The certificates in this file have been
obtained from [here](https://curl.se/docs/caextract.html), and they originate
from Firefox.

The settings menu lets you enable or disable the CA certificate check in HTTPS
connections. They are disabled by default because they require a few hundred KBs
of RAM (which is a lot for a DS). Note that changing this setting requires a
reboot to take effect.

If you want to use different certificates you can replace the `cacert.pem` file
by your own, or you can add your own file to this folder. Then, open file
`data/bunjalloo/config.ini` and modify the line `cafile=cert/cacert.pem` to
point to the file with your certificates. If `config.ini` doesn't exist, boot
Bunjalloo once so that it gets created.

## SDL screen settings

When using the SDL build of Bunjalloo you can adjust some settings using
environment variables:

- `BUNJALLOO_SCREEN_GAP`: Distance in pixels between the two screens. If this
  isn't zero a black rectangle of this height will be drawn between the screens
  to simulate the gap in a real console. A distance of about 20-40 looks ok.
- `BUNJALLOO_SCREEN_SCALE`: The size of the screens (and the gap between
  screens) is scaled by this value. It can only be an integer number without
  decimals.
- `BUNJALLOO_SLOT2_RAM_SIZE`: Size of the emulated Slot-2 RAM device used to
  store image data.

You can adjust the settings on Linux, for example, like this:

```sh
BUNJALLOO_SCREEN_GAP=40 BUNJALLOO_SCREEN_SCALE=2 ./bunjalloo_sdl.elf
```

## User data

The files in `sdroot/data/bunjalloo/user` contain some useful information:

- `bookmarks.html`: List of bookmarks.
- `ckallow.lst`: URLs of sites that allow cookies.
- `search.cfg`: Search keyword shortcuts. `%s` is replaced by the search text.

## Changing the font

If you want to use a different font, then you will need the contents of the
`tools/convertftf` directory.

Install the dependencies of `convertftf`:

```sh
sudo apt install libfreetype-dev
```

Build `convertftf` by running this from the root of the repository:

```sh
make tools
```

Run the `convertftf` tool passing the True Type Font name and the point size
desired. It should generate 2 files: the map data and the set data. Copy them
into the `sdroot/data/bunjalloo/fonts` directory.

To add extra ranges of UTF-8 glyphs, then **all** the ranges required must be
passed on the command line after the size. By default the glyphs included are:
`0x1f-0x7f, 0xa0-0x100, 0x20AC-0x20AD, 8211-8212, 8216-8218, 8220-8221,
8226-8226` These are used if no ranges are given. Here `0x1f-0x7f` means "the
range 0x1f (31) to 0x7f (127) inclusive". This covers all ASCII characters, the
extra latin ones (accented a's and e's, etc) and some common symbols - the Euro
sign, copyright and so on.

The font name is defined in `sdroot/data/bunjalloo/config.ini`. Change the line
`font=fonts/sans` to indicate the basename of the new font. For example, to
generate a new 12 point font from the font `MyFont.ttf`, one could do this:

```
tools/convertftf/convertftf --map=myfont.map --set=myfont.set --size=12 MyFont.ttf
mv myfont.* sdroot/data/bunjalloo/fonts/
sed -i 's/sans/myfont/' sdroot/data/bunjalloo/config.ini
```

The output font name is always all-lowercase.
