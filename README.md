# Asteroids

This is a "game" I wrote in 2010, with some minor changes to make it compile
using stable libraries. It's really just a demo for a 2D vector graphics engine
and a particle system.

It's not particularly well written - if I was to spend time rewriting it, I
would do a few things differently:

- There are a lot of cyclic dependencies, both at the module and class level. I
  would try to extricate this into a strict heirarchy (probably using some
  dependency inversion).
- Most of the `shared_ptr`s should just be `unique_ptr`s or even direct data members.
- Every compilation unit includes a bunch of headers via global.hpp, but most
  of these go unused in any one unit. Should import exactly what I need in each
  unit.
- Replace remaining uses of for(;;)-style loops over iterators with range-for.
- Look for opportunities to rewrite complex loops using STL algorithms.

## Dependencies

You'll need Allegro 5, including the development headers, along with the usual
g++ toolchain. To install these dependencies on Ubuntu, just run `sudo apt
install build-essential liballegro5-dev`.

## Building

1. Install the dependencies.
2. Build the application by typing `make`.
3. Run the application by typing `./asteroids`.

## Controls
Arrow keys rotate/accelerate. Spacebar fires an asteroid. Use the asteroids to destroy the asteroids.
