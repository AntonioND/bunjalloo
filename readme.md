# Bunjalloo

This is Bunjalloo, the open source Nintendo DS web browser. It was originally
developed by Richard Quirk until 2010. This repository contains a working fork
that you can build with modern BlocksDS.

Bunjalloo has been implemented from scratch [using the whatwg user agent design
rules](http://www.whatwg.org/).

This fork can take advantage of the additional RAM, CPU power, and WPA2 support
of DSi. It also uses modern cryptographic libraries to support modern TLS
protocols.

Bunjalloo can also be built using SDL so that it's easier to develop it and
debug it.

## Setup

In order to install it in your Nintendo DS or DSi, copy `bunjalloo.nds` to your
SD card. The contents of `sdroot` must be copied to the root of your SD card.

## Controls

- Start: Enter a URL
- Up/Down: Scroll screen up and down
- Left/Right: Scroll screen a page at a time
- R: Move forwards in the history
- L: Move backwards in the history
- Click stylus: Follow link, enter data in forms, press keys.
- Hold stylus: Scroll.

## Features

- Bookmark support.
- Cookie and image support.
- Optional CA certificate checks.

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

## User data

The files in `sdroot/data/bunjalloo/user` contain some useful information:

- `bookmarks.html`: List of bookmarks.
- `ckallow.lst`: URLs of sites that allow cookies.
- `search.cfg`: Search keyword shortcuts. `%s` is replaced by the search text.

## Credits

Richard Quirk (quirkysoft): Original author of Bunjalloo (http://code.google.com/p/quirkysoft)

Antonio Niño Díaz: Port to BlocksDS.
