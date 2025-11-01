# Bunjalloo

This is Bunjalloo, the open source Nintendo DS web browser. It was originally
developed by Richard Quirk until 2010. This repository contains a working fork
that you can build with modern BlocksDS.

This fork can take advantage of the additional RAM, CPU power, and WPA2 support
of DSi. It also uses modern cryptographic libraries to support modern TLS
protocols.

## Features

- Bookmark support.
- Cookie and image support.
- Optional CA certificate checks.

## Controls

- Start: Enter a URL
- Up/Down: Scroll screen up and down
- Left/Right: Scroll screen a page at a time
- R: Move forwards in the history
- L: Move backwards in the history
- Click stylus: Follow link, enter data in forms, press keys.
- Hold stylus: Scroll.

## Build (NDS)

Install [BlocksDS](https://blocksds.skylyrac.net/docs/).

Clone this repository and run:

```sh
make -j`nproc`
```

Copy `bunjalloo.nds` to your flashcard and the folder `sdroot/` to the root of
your SD card (either in your flashcard or your DSi). The `data` folder inside
`sdroot` must be in the same folder as the `bunjalloo.nds` ROM.

## Build (Linux)

Install [BlocksDS](https://blocksds.skylyrac.net/docs/). You will need it for
some tools.

Install SDL 1:

```sh
sudo apt install libsdl-dev
```

Clone this repository and run:

```sh
make -f Makefile.pc -j`nproc`
```

Then simply run:

```sh
./bunjalloo_sdl.elf
```

## melonDS

If you want to run this in melonDS you will need a SD image. Unfortunately you
will need to add some file to `sdroot` (preferably a few tens of megabytes) that
you remove later. This is required because Bunjalloo will use the SD card to
store some user settings, cookies, cached pages, etc.

Run:

```sh
cp myfile sdroot/
make sdimage

mkdir fs
sudo mount image.bin fs
rm fs/myfile
sudo umount fs
rmdir fs
```

Then you can use this image in melonDS.

## Using CA certificates

Open file `data/bunjalloo/config.ini` and modify the following line
`cafile=cert/cacert_none.pem` to `cafile=cert/cacert.pem`. Loading the
certificates takes a bit of time at boot and it uses a few hundreds of KBs of
RAM at runtime.

If `config.ini` doesn't exist, boot Bunjalloo once so that it gets created.

## Credits

Richard Quirk (quirkysoft): Author of Bunjalloo (http://code.google.com/p/quirkysoft)

Antonio Niño Díaz: Port to BlocksDS.
