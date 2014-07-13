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
* boost at least 1.55.0 (df3d uses header only stuff except of boost::python)
* python 2.7

Built and included to the project
* librocket GUI library http://librocket.com/
* freetype http://www.freetype.org/
* OpenAL

Included with the source
* bullet physics library http://bulletphysics.org/
* SPARK particle engine http://sourceforge.net/projects/sparkengine/
* libogg http://xiph.org/ogg/
* libvorbis http://xiph.org/vorbis/
* glm math library http://glm.g-truc.net/
* jsoncpp library https://github.com/open-source-parsers/jsoncpp

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

Test example will be added soon.
