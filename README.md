# nbody

Nbody, A completely unscientific N body simulator written as a learning project.
Started out as a simple orbit sim for my A level computing coursework, written
in common lisp. I then adapted it to work as an N body simulator. Since then I
have been rewriting it in C, again just as a learning experience.

## Building 

Requires SDL2, and a C99 compiler. Tested only on my Arch system using GCC.
Easiest way to build is
```
make
```
but just look at the make file and figure it out if you don't want to use make.

## Usage
```
./nbody [OPTION]...

Options:
 -w, --width 		Width of the window
 -l, --height 		Height of the window
 -f, --fullscreen 	Sets window to fullscreen
 -g, --generate 	Randomly generates given number of bodies
 -c, --centre 		Centres renders onto the centre of mass

Examples:
./nbody -fg 128 	Generates 128 bodies and runs fullscreen
```
## TODO

* ~~Get the physics working~~ DONE
* Add input from file/stdin
* ~~Command line args~~ DONE
* rendering actual circles
* Multithreading
