Slay the Dragons
================
A simple 2D game played with two analog sticks/nubs.
        
The goal is to create a game that will run on the [Pyra Handheld](https://pyra-handheld.com/), but also on PCs with x86 architecture.
        
Compile and Run the Source Code
-------------------------------
The game is written in C/C++ and uses SDL 2, SDL_image 2 and SDL_mixer 2.
      
### Getting Started on Debian Linux and Code::Blocks
  * Install Code::Blocks IDE  
    `sudo apt-get install codeblocks`
  * Install SDL 2  
    `sudo apt-get install libsdl2-dev`
  * Install SDL 2 extension library for loading png images  
    `sudo apt-get install libsdl2-image-dev`
  * Install SDL 2 extension library for music and sounds  
    `sudo apt-get install libsdl2-mixer-dev`
  * Start Code::Blocks and create an empty project
  * Add all files provided by this project
  * Go to `Project` > `Properties...` > `Project settings` > `Project's build options...` > `Linker settings`
  * Add the following `Other linker options`:  
    `-lSDL2 -lSDL2_image -lSDL2_mixer`
        
### Getting Started on Windows and Visual Studio
  * Install Visual Studio Community, including the workload `Desktop development with C++`
  * Create a Microsoft account or login with an existing account
  * Download and extract SDL 2 development libraries for Visual C++
  * Download and extract SDL_image 2 development libraries for Visual C++
  * Download and extract SDL_mixer 2 development libraries for Visual C++
  * Start Visual Studio and create an empty project
  * Add all files provided by this project
  * In the project properties go to `Configuration Properties` > `VC++ Directories`
  * Add all extracted SDL subfolders named `inlcude` to `Include Directories` (SDL 2, SDL_image 2 and SDL_mixer 2)
  * Add all extracted SDL subfolders named `lib/x86` to `Library Directories` (SDL 2, SDL_image 2 and SDL_mixer 2)
  * In the project properties go to `Configuration Properties` > `Linker` > `Input`
  * Add the following `Additional Dependencies`:
      * `SDL2.lib`
      * `SDL2main.lib`
      * `SDL2_image.lib`
      * `SDL2_mixer.lib`
  * Copy the extracted SDL `.dll` files into the project's root path (SDL 2, SDL_image 2 and SDL_mixer 2)
        
Authors
-------
  * Timo Scheit - *Initial work*
        
License
-------
This project is licensed under the MIT License - see the LICENSE.txt file
for details.
        
Acknowledgments
---------------
  * [SDL](https://www.libsdl.org/) (cross-platform development library)
  * [Lazy Foo' Productions](http://lazyfoo.net/) (SDL tutorial)
  * [LMMS](https://lmms.io/) (audio workstation)
