# Asteroids

This is a "game" I wrote in 2010. It's really just a basic 2D engine with a fun particle system. 

## Building

Unfortunately, this was written using an unstable version of liballegro, so it's a bit of a pain to build.

1. Build and install liballegro version 4.9.18 and update ldconfig.
    * (An aside: in order to get this version of liballegro to build against a modern libpng, I had to add the line `#include <zlib.h>` to `addons/image/png.c`.
2. Try a build: `make`
3. If it fails, make sure the Makefile is pointing at the right headers and shared libraries.
4. Run it with ./asteroids.

## Controls
Arrow keys rotate/accelerate. Spacebar fires an asteroid. Use the asteroids to destroy the asteroids.
