Pertotman
=========
A clone of Pacman done with SDL-OpenGL, including 3 difficulties, dynamic texture and level loading and savegames.
Default texture uses heads of teachers and students at Gymnázium Boženy Němcové, Hradec Králové, Czech Republic.

Klon hry Pacman, vytvořený pomocí SDL-OpenGL, se 3 úrovněmi obtížnosti, dynamickým načítáním textur, levelů a
možností ukládaní pozice. Výchozí textura používá hlavy učitelů a žáků na Gymnáziu Boženy Němcové, Hradec Králové.

Prerequisites
-------------

These are prerequisites tested with the current version. It may or may not work with other versions.

- libsdl >= 1.2.25
- libsdl_image >= 1.2.12
- libsdl_ttf >= 1.2 - also works with libsdl2_ttf >= 2.8.1
- libopengl >= ??? - probably >= 3
- libfreetype >= 2.8
- libzlib >= 1.2.11
- libpng >= 1.6.34

Building and running
--------------------

```sh
$ cmake .
$ make
$ ./bin/pertotman
```
