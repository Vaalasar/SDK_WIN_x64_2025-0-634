# Introduction

This is a simple example demonstrating the implementation of native Overlays with the SketchUp Live C API.
It implements a simple overlay that draws a user-selected image on top of the SketchUp view.

There are 2 subfolders:
* `cpp`: contains the native code which uses the overlay C API.
* `ruby`: contains the Ruby code which implements the SketchUp extension as the entry point.

# To build the C++ code:
 * Using your favorite CMake configure tool (`Visual Studio`, `CLion`, `cmake-gui`) open the `cpp` folder.
 * Select a multi-config generator. (i.e. `Ninja Multi-Config`, `Visual Studio`, `Xcode`).
 * Set the `SKETCHUP_API_INCLUDE_PATH` CMake variable to the path containing SketchUp API headers.
   On Windows, this will be the `\headers` folder inside the SDK package.
   On macOS, it should point to the directory containing `SketchUpAPI.framework`.
 * Set the `SKETCHUP_API_LIBRARY_PATH` CMake variable to the path containing SketchUp API libraries.
   On Windows, this will be the the SDK package subfolder containing `sketchup.lib`.
   On macOS, it should point to the SketchUp.app bundle subdirectory containing the executable,
   e.g. `/Applications/SketchUp 2024/SketchUp.app/Contents/MacOS`.

Build the "Debug" configuration. This should create a `ruby/code/ex_hello_c_overlay/bin/Debug/ex_hello_overlay.dll` (or `.so` on macOS).
 
# To run the extension:
 * Copy `ruby/load_ex_hello_c_overlay.rb` to `C:\Users\<user_name>\AppData\Roaming\SketchUp\SketchUp 2024\SketchUp\Plugins` (or the macOS equivalent).
 * Update the `$LOAD_PATH` in `load_ex_hello_c_overlay.rb` to point to the `ruby\code` folder. It should look something like
 ```ruby
$LOAD_PATH << 'C:\SU_SDK_Samples\Overlays\ruby\code'
require 'ex_hello_c_overlay.rb'
 ```
  * Run SketchUp 2024 or later. The extension should be loaded and it should add a command under the Extensions menu.
