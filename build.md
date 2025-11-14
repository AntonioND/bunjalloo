# Building Bunjalloo

## Nintendo DS

Install [BlocksDS](https://blocksds.skylyrac.net/docs/).

Clone this repository and run:

```sh
make nds -j`nproc`
```

Now copy `bunjalloo/bunjalloo.nds` to your flashcard.

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

If you want to run this in melonDS you will need a SD image with some free
space to store some user settings, cookies, cached pages, etc.

Look for a file of a few tens of megabytes and run the following commands:

```sh
mkdir sdroot/
cp myfile sdroot/
make -f bunjalloo/Makefile sdimage

mkdir fs
sudo mount image.bin fs
rm fs/myfile
sudo umount fs
rmdir fs
```
