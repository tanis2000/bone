# Bone, the Binocle project generator

[![Build Status](https://travis-ci.com/tanis2000/bone.svg?branch=master)](https://travis-ci.com/tanis2000/bone)

Bone is a project generator and build tool for the Binocle engine, a 2D/3D videogame and application engine.

The ultimate goal of Bone is to relieve the developer from all the tedious tasks like configuring CMake for different architectures, create build scripts, etc...

The idea isn't new and it can feel like yet another wrapper around CMake, and it actually is like that :-P
[Fips](https://github.com/floooh/fips) is a good alternative if you want to manage your C/C++ project on your own.
The actual idea came from the [snow](https://github.com/snowkit/snow) and [lime](https://github.com/openfl/lime) tools. They are pretty similar but written in JavaScript and Haxe. I definitely used those as a source of inspiration.

Bone is far from being finished, but it's good enough for my usage and can probably be of use to others as well. I can always use a hand to support more target architectures, so feel free to submit PR or open issues as needed.

## Create a new project

Bone supports both the C version and C++ version of Binocle. As of the time I'm writing this, only the C version is available in the wild. The C++ version is still under closed source and will eventually be released in the future for historic reasons.

Let's say that we want to create a Binocle C application called `project` in the `./project` folder.

```sh
bone init --engine binocle-c project ./project
```

This will create the `./project` folder and initialize a new Binocle C project called `project` with a skeleton of an application.

## Build a project

To build a project just do:

```sh
bone build ./project
```

This will build the application for the hosting architecture. You can force the architecture by specifying the architecture from the following list:

- mac
- windows
- linux
- ios
- android
- web

i.e. if you want to build for `ios` just do:

```sh
bone build ./project ios
```

## Run a project

You can easily run a project with the following command:

```sh
bone run ./project
```

Just like with the `build` command, you can specify a target architecture.
i.e. if you want to run the `ios` version, just do:

```sh
bone run ./project ios
```

## Update a project

From time to time I update the CMake scripts to reflect changes in development environments and to improve the user experience. In that cas you can `update` your project.

Please make sure to backup your work before doing so as it can break your build scripts.

To update your scripts, just run:

```sh
bone update ./project
```

## Upgrade a project

If you want to upgrade to the latest version of Binocle, just run the following command:

```sh
bone upgrade ./project
```

Binocle is a submodule of your project so it's just like doing a remote pull from the Binocle repository.

## LICENSE

MIT License

Copyright (c) 2019 Valerio Santinelli

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
