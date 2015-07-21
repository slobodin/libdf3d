libdf3d
=======
Simple C++ game framework aimed for mobile platforms.

#### Currently supported
* desktop platforms: windows, linux, mac os x
* android (partially)
* windows phone (partially)

#### Compiler
* The project heavily uses c++11 and c++14 features.

#### Dependencies
* Windows: boost at least 1.55.0 (df3d uses header only stuff)
* Linux and MacOS: TODO list of libraries

Built and included to the project
* freetype http://www.freetype.org/
* OpenAL

Included with the source
* librocket GUI library http://librocket.com/ (submodule)
* bullet physics library http://bulletphysics.org/
* SPARK particle engine http://sourceforge.net/projects/sparkengine/
* libogg http://xiph.org/ogg/
* libvorbis http://xiph.org/vorbis/
* glm math library http://glm.g-truc.net/
* jsoncpp library https://github.com/open-source-parsers/jsoncpp
* glfw

Compile
=======

CMake works now.

Features
========
Feature list will be added soon.

More documentation will be added soon.

### TODO

* gui should block touches
* locking vertex buffer (glMapBuffer, glUnmapBuffer)
* use 16bit indices when possible
* remove unnessesary unbinds.
* pvrtc, etc1
* LOTS OF STUFF!!!

Test example will be added soon.

Games
========

Example of a game powered by this engine:
http://youtu.be/yeCutM78UpQ
