# VZX Containers

These are C++ container classes used in the [VZX Music Visualizer](https://www.vzx-visualizer.com/)

## Malloc container

It is a specialized container aimed at graphics / game engines:
- it can allocate large numbers of items without running the constructor
- convenient, type-safe access of memory-mapped data (the "volatile" mode)

## New-based container

It is a container based around new. Unlike std::vector, it does not throw exceptions
and also allows easy reading from memory-mapped "volatile" data.
