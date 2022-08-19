# Styleblit watercolor

[Demo](https://timostrating.github.io/styleblit-watercolor/) and [thesis](https://www.researchgate.net/publication/362805950_Real-time_example-based_style_transfer_of_watercolor_for_3D_games)

### thesis abstract
The digital revolution has made it possible that many traditional art forms, such as watercolor can now be expressed digitally. Despite the many available techniques to synthesis watercolor, this style sees only limited use in real-time 3D applications, such as video games. The aim of this study was to find ways to improve this. Where previous work has focused on artistic control over watercolor and taking whole 3D objects as input, we set out to explore how style transfer relates to previous watercolor research and how fast style transfer can be improved to work well for watercolor art in the setting of video games. StyleBlit is currently considered the state of the art in fast style transfer and results in the best visual results when it is presented with example images with high frequency details with no big repeating patterns. This description does not match with watercolor art and can also change its visual appearance based on how it is used in a real-world painting. We show that by extending StyleBlit with the idea of blending different example images, a more coherent visual result is shown that can mix colors, using a GPU implementation of Mixbox, in the same way real paint would be used. This makes more styles, such as watercolor, that lay outside the suggested example images with high frequency details, better suitable for the fast patch basted style transfer algorithm StyleBlit. While this type of blending is not as fast as HSV or Linear blending, we show that it would be fast enough to be implemented in the setting of a video game and improve the temporally coherency of the visual result.

## Unity version

https://github.com/timostrating/styleblit-watercolor/releases

## OpenGL version

[Emscpriten](https://emscripten.org/) + [GLFW3](https://www.glfw.org/) + [GLAD](https://www.glfw.org/) + [GLM](https://github.com/g-truc/glm/releases) + [ImGui](https://github.com/ocornut/imgui) + [stb](https://github.com/nothings/stb) + [personal unit test code](https://github.com/timostrating/unity_test)

<p align="center">
  <img src="https://imgur.com/xSN7Wvy.png" alt="image" width=>
</p>

## Setup
OPTIONAL: Setup [Emscpriten](https://emscripten.org/)
1. To manage the dependencies I use submodules. So get them by recursively cloning this repo or run  `$ git submodule update --init --recursive` after you cloned it
2. Download [GLAD with opengl es 3.2](http://glad.dav1d.de/#profile=core&specification=gl&api=gl%3Dnone&api=gles1%3Dnone&api=gles2%3D3.2&api=glsc2%3Dnone&language=c&loader=on) and place the unzipped files in `./external/glad` 

## Building it

Build the (optional) Emscpriten version

`$ cd build/emscripten` <br/>
`$ emconfigure cmake ./` <br/>
`$ make` <br/>
`$ emrun out/styleblit_watercolor.html` <br/>


Or build the normal desktop version

`$ cd build/desktop` <br/>
`$ cmake ./` <br/>
`$ make` <br/>

## Inspired by

https://github.com/hilkojj/cpp-GAME-utils <br/>
https://github.com/hilkojj/cpp-GAME-utils-template

https://github.com/emscripten-core/emscripten/tree/incoming/tests

https://www.amazon.com/Computer-Graphics-OpenGL-Donald-Hearn/dp/0130153907
