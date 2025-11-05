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
