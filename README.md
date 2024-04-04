# kiyosGameEngine
kiyoGameEngine (kge) is a small header only C++17 library, that provides boilerplate code for a small to medium sized games. Currently kge includes a main game loop with a state stack, a simple point class and an entity component system. Each header is independent and only requires the STL and C++17 compatible compiler. It is designed to be used with your favourite graphics/sound library. Current examples build on Windows, Linux and deploy to the web with Emscripten (WebAssembly).

## Install
Just include the kge header files in your project to use kge. Examples and tests build on Ubuntu 20.04 and Windows 10 using Visual Studio. The example requires SFML and CMake. The easiest way to build on windows is to use Conan so it can pull in SFML when using CMake. One way of doing this is to install Chocolatey on Windows and then use Powershell to install CMake and Python, then use Python to install Conan (Make sure Visual Studio installs MSVC).
For the web, install emscripten and then read and run the ./tools/emscriptenBuild.sh script. This builds all the assets for the index.html that will display a working example. Your hosting web server now needs the following headers set to play the sample (as of Dec 2021) but taking out multithreading may change things(let me know!).

Cross-Origin-Embedder-Policy: require-corp
Cross-Origin-Opener-Policy: same-origin

## To use 
Just look at the examples to get up and running with kge, the header files are small and simple and are independent of each other. The templates are digestible, so it shouldn't take too long to work out what's going on. 

