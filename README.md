# Raycaster
A simple recasting engine I made to experiment with 3D computer graphics.

It just works by testing for camera-ray intersections with triangles.
It accepts certain types of .obj files as inputs, but all other parameters, like shaders and resolution, are in the soruce code as constants, which require recompilation to be changed.

It runs on a single thread in the processor, which means that it is very slow, which is not something I care a lot about because I never intended for this project to be particularly useful. It is more of a learning exercize for myself.

As of now, there are a few different properties objects can have:

* Flat Shaded (No bounce rays at all)

* Reflective Shaded (All incoming rays are reflected off of the surface.)

* Diffuse Shaded (Incoming rays are scattered randomly)




Features it does not have yet, but that I plan on adding at some point:

* Refraction Shader (Like glass)

* Parameters .txt file

* Object scaling and rotation

* Preformance improvements

* (Non sky) Light Sources

* Image Textures

* Multithreadding or GPU support
