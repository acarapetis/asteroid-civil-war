# Asteroids

This is a "game" I wrote in 2010, with some minor changes to make it compile
using stable libraries. It's really just a demo for a 2D vector graphics engine
and a particle system.

I've spent a few hours in 2022 cleaning it up:

- Now builds with a stable version of Allegro
- Untangled some cyclic dependencies (the module dependencies now form a DAG)
- Replaced libboost with C++17 standard library
- Replaced a lot of ugly iteration with ranged-for or STL algorithms

It's still not particularly good code:

- It leans heavily on a crutch made of `shared_ptr`s. Most of these could be
  `unique_ptr`s or even direct data members.
- There are some complex loops I didn't take the time to fully recomprehend -
  these can probably be simplified by decomposition into STL algorithms.

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
