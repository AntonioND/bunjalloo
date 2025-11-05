# Building Bunjalloo

## Nintendo DS

Install [BlocksDS](https://blocksds.skylyrac.net/docs/).

Clone this repository and run:

```sh
make nds -j`nproc`
```

Copy `bunjalloo/bunjalloo.nds` to your flashcard and the folder `sdroot/` to the
root of your SD card (either in your flashcard or your DSi). The `data` folder
inside `sdroot` must be in the same folder as the `bunjalloo.nds` ROM.

## Linux

Install [BlocksDS](https://blocksds.skylyrac.net/docs/). You will need it for
some tools.

Install the dependencies:

```sh
sudo apt install libsdl-dev
```

Clone this repository and run:

```sh
make sdl -j`nproc`
```

Then simply run the following command from the root of the repository:

```sh
./bunjalloo/bunjalloo_sdl.elf
```

## melonDS

If you want to run this in melonDS you will need a SD image. Unfortunately you
will need to add some file to `sdroot` (preferably a few tens of megabytes) that
you remove later. This is required because Bunjalloo will use the SD card to
store some user settings, cookies, cached pages, etc.

Run:

```sh
cp myfile sdroot/
make -f bunjalloo/Makefile sdimage

mkdir fs
sudo mount image.bin fs
rm fs/myfile
sudo umount fs
rmdir fs
```
