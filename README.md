# GalaxyEngine - A simple 3D game engine using LISP as its scripting language

![image](galaxy-engine-logo.jpg)

## Introduction

GalaxyEngine is an open source 3D game engine that uses LISP as its native scripting language.

The purpose of GalaxyEngine is to allow development of simple 3D games using an easy-to-use integrated development environment where everything is easily customizable.

## Building and running GalaxyEngine

Right now, GalaxyEngine only runs on macOS. However, the code is portable across three platforms: macOS, Windows, and Linux.

This is because GalaxyEngine is written in C++ and uses the Vulkan 3D graphics API and the glfw windowing library. Both of these are available across all three platforms.

Stay tuned for updates as they become available!

### Requirements

* Currently, an Apple computer running macOS
* The Vulkan 3D SDK
* The glfw windowing library
* The glm matrix math library

### Building

* Stay tuned!

### Running

* Stay tuned!

## Using GalaxyEngine

GalaxyEngine is a work in progress. Please check back for further updates.

## Version

Current version: 0.21 September 16, 2024 - Initializes blank glfw window at 1200 x 600, initializes Vulkan layer, and selects the first physical GPU device to use, initializes Logical Device on top of it, creates draw surface to connect Vulkan with glfw window, sets up swap chain for triple buffering, sets up image views, sets up pipeline layout, framebuffers, and command buffers, and draws a single triangle. 

But before it displays the triangle through Vulkan, it starts running a very simple LISP interpreter (thanks to Anthony Hay) that's just over 100 lines of code.

The LISP interpreter currently runs at the beginning of the program execution, after the window is loaded but before the triangle is drawn. Future versions will run the LISP interpreter continuously inside the main loop.


