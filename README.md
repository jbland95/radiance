Radiance
========

Radiance is video art software for VJs. It supports beat detection, animated GIFs, YouTube video, and OpenGL shader effects.

[![Build Status](https://travis-ci.org/zbanks/radiance.svg?branch=master)](https://travis-ci.org/zbanks/radiance)

You can download this software for MacOS from [radiance.video](https://radiance.video). There are no prebuilt Linux packages, but you can build it yourself fairly easily.

Screenshots
-----------

![screenshot](https://i.imgur.com/hgdTxPU.png)

![GIF Screencapture](https://i.imgur.com/I4qnMQo.gif)

[Example effects](https://radiance.video/library/)

Build
-----

### Dependencies

- `Qt 5.6`
- `SDL2-TTF`
- `PortAudio`
- `FFTW3`
- `libsamplerate`

### Optional Dependencies
- `libmpv`
- `rtmidi`

Install dependencies on Ubuntu:

    $ sudo apt-add-repository ppa:beineri/opt-qt591-trusty
    $ sudo apt-get update
    $ sudo apt-get install qt59base qt59multimedia qt59quickcontrols qt59imageformats qt59quickcontrols2 qt59script libfftw3-dev libsamplerate0-dev libasound2-dev libmpv-dev
    $ git clone https://github.com/EddieRingle/portaudio    # build & install
    $ git clone https://github.com/thestk/rtmidi            # build & install

Note: you may need to install portaudio & rtmidi from git as above

### Building Radiance

    git clone https://github.com/zbanks/radiance
    git submodule update --init
    cd radiance
    mkdir build
    cd build
    cmake .. # -DCMAKE_PREFIX_PATH=/opt/qt59/ -DCMAKE_BUILD_TYPE=Debug
    make
    ./radiance          # Qt UI
    ./radiance_cli      # Command line GIF generator

If you `git pull` changes, make sure you also do `git submodule update` to pull in changes to `BTrack/`.

### youtube-dl

Radiance uses `libmpv` to load videos, which can optionally use `youtube-dl` to stream videos from YouTube and many other sites. Since `youtube-dl` updates frequently, we have avoided bundling it with Radiance. Instead, on Linux:

    sudo pip install youtube-dl

or on Mac:

    brew install youtube-dl

You can then load YouTube videos into Radiance by typing `youtube:search terms` into the pattern loader box.

Keyboard Shortcuts
------------------

### Tiles
- `j` - Decrement selected slider by 10%
- `k`  - Increment selected slider by 10%
- `` ` ``, `0-9` - Set selected slider. `` ` `` = 0%; `1` = 10%; `5` = 50%; `0` = 100%
- `Delete` - Remove a tile
- `Enter` - Set a tile as output (then click `Show Output` to fullscreen it)
- `Ctrl` + `` ` ``, `0-9` - Assign slider to MIDI knob
- `r` - Reload node

### Other
- `q` - Close output window
- `:` - Load node

Shader Effects
--------------

Radiance generates video from a set of connected "VideoNodes." Most commonly, these nodes are based on OpenGL fragment shaders, but can also be static images, GIFs, or videos. Each node takes one or more inputs and produces exactly one output.

Each OpenGL fragment shader node is described by a single `.glsl` file in [`resources/library/effects`](https://github.com/zbanks/radiance/tree/master/resources/library/effects).

For the most part, these files are plain GLSL describing the fragment shader. This is similar to setups on shadertoy.com or glslsandbox.com . Each shader defines a function `void main(void)` which sets a pixel color `vec4 fragColor` for a given coordinate `vec2 uv`. `fragColor` is an RGBA color with pre-multiplied alpha: so white with 40% opacity is encoded as `vec4(0.4, 0.4, 0.4, 0.4)`. The coodinate `uv` has `x` and `y` values in the range `[0.0, 1.0]` with `vec2(0., 0.)` corresponding to the lower-left corner.

The shader also has access to its input(s) as textures through the `iInput` (or `iInputs[]`) uniforms.

In addition, each shader has access to additional uniforms which are documented in [`resources/effects/*.glsl`](https://github.com/zbanks/radiance/tree/master/resources/library/effects). The most important is `iIntensity`, which is a value in the range `[0.0, 1.0]` which is mapped to a slider in the UI that the user controls. There is a limitation of having *exactly one* input slider per effect: this is intentional to reduce the cognitive overhead on the end user. Other variables include information about the current audio or time.

### Invariants

Each shader must follow these properties:

* The `fragColor` set by each shader must be a valid, pre-multiplied alpha, RGBA tuple. Each component of the `vec4` must be in the range `[0.0, 1.0]`, and the RGB components must be less than or equal to the A component. (See `afixhighlight` for a shader that will highlight errors here in pink)
* Identity: the shader must pass through its first input completely unchanged when `iIntensity == 0.` This means that inserting a new shader should not effect the output until it's intensity is increased.


### Multi-buffer Shader Effects

Some effects cannot be accomplished with a single fragment shader pass. An effect can consist of a series of shaders, separated by `#buffershader`. Each shader renders to a texture in `iChannels[]` (e.g. the first renders to `iChannels[0]`). The shaders are rendered in backwards-order, so the last shader is rendered first. Only the output of the first shader is displayed -- the other buffers persist between frames but are not exposed to other nodes.

An example that uses this feature is [`foh.glsl`](https://github.com/zbanks/radiance/blob/master/resources/library/effects/foh.glsl). This implements an (exponential) "first-order hold" - it samples the input texture on a multiple of the beat and stores it in `iChannel[1]`.



Copyright & License
-------------------

Released under the MIT/X11 License. Copyright 2016 Zach Banks and Eric Van Albert.

