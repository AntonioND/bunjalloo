# Bunjalloo

This is Bunjalloo, the open source Nintendo DS web browser. It was originally
developed by Richard Quirk until 2010 and then abandoned. This repository
contains a working fork that you can build with modern BlocksDS

This fork can take advantage of the additional RAM, CPU power, and WPA2 support
of DSi.

## Features

- Cookie and image support.

## Controls

- Start: Enter a URL
- Up/Down: Scroll screen up and down
- Left/Right: Scroll screen a page at a time
- R: Move forwards in the history
- L: Move backwards in the history
- Click stylus: Follow link, enter data in forms, press keys.
- Hold stylus: Scroll.

## Build

Clone this repository and run:

```sh
make -j`nproc`
```

Copy `bunjalloo.nds` to your flashcard and the folder `sdroot/` to the root of
your SD card (either in your flashcard or your DSi). The `data` folder inside
`sdroot` must be in the root of your SD card.

## Credits

Richard Quirk (quirkysoft): Author of Bunjalloo (http://code.google.com/p/quirkysoft)
