libdf3d
=======
Simple C++/Python-based game framework aimed for mobile platforms which is in prototype.

#### Currently supported
* windows
* android
* linux is possible

#### Compiler
* windows Visual Studio 2013
* android gcc with c++11 features

#### Dependencies
* SDL2
* SDL2_Image
* boost at least 1.55.0 (df3d uses header only stuff)

Built and included to the project
* librocket (for both windows and android)
* freetype
* bullet physics (windows, android)
* SPARK particle engine (windows, android)
* OpenAL
* libogg, libvorbis
All android makefiles for the above libraries are placed in docs folder

Included with the source
* glm
* jsoncpp

Compile
=======

For now the project contains visual studio solution with library project. Cmake doesn't work.

Features
========
Feature list will be added soon.

More documentation will be added soon.

### TODO

* weak refs in python
* gui should block touches
* locking vertex buffer (glMapBuffer, glUnmapBuffer)
* use 16bit indices when possible
* remove unnessesary unbinds.
* RGB444
* make texture samplers as uniforms.
* pvrtc, etc1
* provide some third-party with source

Python bindings permanently moved to client code. Need shared libraries on android.