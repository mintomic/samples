This repository contains some data structures and sample applications which demonstrate low-level lock-free concepts using [Mintomic](http://mintomic.github.io/).

Right now, there is only one sample application: ArrayOfItems.

[CMake](http://www.cmake.org/cmake/resources/software.html) is required.

## Build Instructions

The following steps work in Windows, Linux and MacOS.

First, clone this repository as you normally would.

    $ git clone https://github.com/mintomic/samples

The repository contains Mintomic as a submodule. If you cloned using the above command line, you'll need to manually update the submodule. Otherwise, the `mintomic/` subdirectory will be empty, and things won't build.

    $ cd samples
    $ git submodule update --init

Next, descend into the `projects/arrayofitems/` subdirectory and run CMake as you normally would. It's customary to run it from a subdirectory named `build/`.

    $ cd projects/arrayofitems
    $ mkdir build
    $ cd build
    $ cmake ..

If the above CMake command line does not generate the project type you want, try again using CMake's `-G` option, or use the CMake GUI.

To generate an Xcode project for iOS devices, use the following CMake command line:

    $ cmake -DCMAKE_TOOLCHAIN_FILE=../../../mintomic/cmake/iOS.cmake -G "Xcode" ..

After running CMake, you will be left with some project/solution files for your IDE, or some Unix Makefiles. You should know what to do at this point. Remember to select ArrayOfItems as the Startup Project (Visual Studio) or the Active Scheme (Xcode).

It's highly recommended to build and run the Release configuration. Select this in your IDE the usual way, or if you're working with Unix Makefiles, specify the build type on the CMake command line:

    $ cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ..
