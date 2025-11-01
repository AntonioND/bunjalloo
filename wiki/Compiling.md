# Compiling from source

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
