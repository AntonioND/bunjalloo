# Compiling from source

## Requirements

### General

- [DevkitArm](http://sourceforge.net/project/showfiles.php?group_id=114505&package_id=124207)
  release `r24`
- [libnds](http://sourceforge.net/project/showfiles.php?group_id=114505&package_id=151608) - the
  latest version
- [SDL library](http://www.libsdl.org/) - to compile the PC version
  - optional, use `--without-sdl` to disable. This isn't recommended as testing is much easier with
    SDL enabled
- [Python](http://www.python.org/), a Unix-like environment(tested on Ubuntu 7.10, partly tested on
  Windows Vista with MinGW from the [msysgit project](http://code.google.com/p/msysgit/))
- [waf](http://waf.googlecode.com/) - the configuration and build system (included in the source
  tarballs from the download section, version 1.5.X for git)

### Chaos

In addition to the above, you will also need the following to compile Chaos:

- [Sox](http://sox.sourceforge.net/)

### Bunjalloo

In addition to the General requirements above, you will also need the following to compile
Bunjalloo:

- [libfat](http://sourceforge.net/project/showfiles.php?group_id=114505&package_id=197264) - the
  latest version
- [dswifi](http://sourceforge.net/project/showfiles.php?group_id=114505&package_id=199021) - the
  latest version
- [Python Imaging Library (PIL)](http://www.pythonware.com/products/pil/) to compile and change the
  fonts (optional)
- [CppUnit](http://cppunit.sourceforge.net/cppunit-wiki) for unit tests
  - optional use `--without-cppunit` to disable
- libpng, zlib, gif_lib (part of libungif), libjpeg and matrixssl.

The libraries can be downloaded, unpacked, built and installed for the NDS using [this helper
script in the /tools directory](http://github.com/quirky/bunjalloo/tree/master/tools/download-libs.sh).
They are added to the $DEVKITPRO/libnds directory. This script uses subversion and patch, which you
may have to install. There are precompiled NDS libraries available in the
[download area](http://code.google.com/p/quirkysoft/downloads/list) too, to save you time. Install
them to $DEVKITPRO/libnds and you'll be up and running in no time, hopefully.

For the Linux test version:

- use the `-dev` versions that your OS vendor supplies for zlib, libjpeg, libgif and libpng.
- Run `make install` for matrixssl and the unzip (minizip) library to install to /usr/local/.

On Windows, use `--without-sdl` and cross your fingers.

## Steps

1. Install the tools as per the instructions on their respective websites
2. Set the DEVKITPRO environment variable to point to the place you installed devkitARM and libnds
3. Ensure the other tools are in your PATH
4. Check out the code into a workspace and compile using [waf's](http://waf.googlecode.com/)
   autoconf emulation: `git clone git://github.com/quirky/bunjalloo.git` `./configure make`.
   Alternatively, run directly with waf if the configure script fails (e.g. on Windows): `waf
   configure` `waf build` This is for the `next` branch, which has a homogeneous build system,
   `master` [is messier](http://code.google.com/p/quirkysoft/source/browse/wiki/Compiling.wiki?r=874#42).
   Source code can be downloaded if you don't have git installed.
  - [Download source code releases](http://code.google.com/p/quirkysoft/downloads/list?can=2&q=label%3Asource)
    from the download section.
  - [Download a snapshot of the next branch](http://github.com/quirky/bunjalloo/tarball/next) from
    Github.

## Sending patches

This is a mini how-to for sending patches. After making your changes, testing them out, committing
them locally, etc, run: `git format-patch -M HEAD^` Where `HEAD^` is whatever commit your patch
corresponds to. This generates the `*.patch` files to send. Once you have done this, let me know
about the change by doing one of the following:

- [Add an issue](http://code.google.com/p/quirkysoft/issues/entry) with the change description and
  attach your patch file.
- Post the patch with a description of what it does to the
  [mailing list](http://groups.google.com/group/bunjalloo-discussion)
- Send me an email - my address is **richard.quirk at gmail.com** - attach your patch file.
- The source code is hosted on [GitHub](http://github.com/quirky/bunjalloo/tree/master) so if you
  are feeling brave, create a fork and send me a pull request there. There is a [complete guide to
  working on forks](http://github.com/guides/fork-a-project-and-submit-your-modifications) and I'd
  recommend anyone interested in hacking on the code to set it up this way.

Thanks!
