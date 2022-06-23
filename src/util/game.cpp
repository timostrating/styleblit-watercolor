//
// Created by sneeuwpop on 16-6-19.
//

#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
#endif

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "./interfaces/screen.h"
#include "debug/gl_debug.h"
#include "input/keyboard.h"
#include "input/mouse.h"
#include "io/file_watch.h"
#include "game.h"
#include "external/std_image_write.h"
#include "debug/tester.h"
#include "debug/profiler.h"

namespace GAME {
    GLFWwindow *g_window;
    Screen *screen;

//    FileWatch fileWatch = FileWatch();

    enum Constants { SCREENSHOT_MAX_FILENAME = 256 };
    static GLubyte *pixels = NULL;
    static unsigned int nframes = 0;
    static unsigned int g_width = 1920;
    static unsigned int g_height = 1080;
    static unsigned int format_nchannels = 3;

    /* THIS IS A COPY FROM: https://stackoverflow.com/a/14324292/7217653
     *
     * Take screenshot with glReadPixels and save to a file in PPM format.
     *
     * -   filename: file path to save to, without extension
     * -   width: screen width in pixels
     * -   height: screen height in pixels
     * -   pixels: intermediate buffer to avoid repeated mallocs across multiple calls.
     *     Contents of this buffer do not matter. May be NULL, in which case it is initialized.
     *     You must `free` it when you won't be calling this function anymore.
     */
    static void screenshot(const char *filename, unsigned int width, unsigned int height, GLubyte **pixels) {
        size_t i, j, cur;
        FILE *f = fopen(filename, "w");
        fprintf(f, "P3\n%d %d\n%d\n", width, height, 255);
        *pixels = static_cast<GLubyte *>(realloc(*pixels, format_nchannels * sizeof(GLubyte) * width * height));
        glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, *pixels);
        for (i = 0; i < height; i++) {
            for (j = 0; j < width; j++) {
                cur = format_nchannels * ((height - i - 1) * width + j);
                fprintf(f, "%3d %3d %3d ", (*pixels)[cur], (*pixels)[cur + 1], (*pixels)[cur + 2]);
            }
            fprintf(f, "\n");
        }
        fclose(f);
    }

    /**
     * Capturing image at higher performance can be done using the apitrace tool:
     *
     * apt install apitrace
     * apitrace trace ./project
     * apitrace dump-images ./project.trace
     * ffmpeg  -pattern_type glob -i '*.png' -c:v libx264 -vf fps=24 -pix_fmt yuv420p MooieVideo.mp4
     */

#ifdef linux
    void debugInit()
    {
        PROFILE_FUNCTION();
//        fileWatch.addDirectoryToWatch("../assets/shaders", true);
//        fileWatch.startWatchingSync();
    }

    void debugTick()
    {
        PROFILE_FUNCTION();
        if (INPUT::KEYBOARD::pressed(GLFW_KEY_F12)) {
            char filename[SCREENSHOT_MAX_FILENAME];
            snprintf(filename, SCREENSHOT_MAX_FILENAME, "tmp.%05d.ppm", nframes);
            screenshot(filename, g_width, g_height, &pixels);
            nframes++;
//            todo: add a free(pixels); to the destructor
        }
    }
#endif

    bool init()
    {
        PROFILE_FUNCTION();

        /* Initialize the library */
        if (!glfwInit())
            return -1;

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

        glfwWindowHint(GLFW_RESIZABLE , 1);
        /* Create a windowed mode window and its OpenGL context */
        g_window = glfwCreateWindow(g_width, g_height, "PokeWorld", NULL, NULL);
        if (!g_window)
        {
            glfwTerminate();
            return -1;
        }

        /* Make the window's context current */
        glfwMakeContextCurrent(g_window);
        glfwSwapInterval(0); // 1 = Enable vsync

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cerr << "Failed to initialize OpenGL context\n";
            return -1;
        }


        INPUT::KEYBOARD::setup(g_window);
        INPUT::MOUSE::setup(g_window);

        GL_DEBUG::enableGLDebug();
        GL_DEBUG::printContext();

#ifdef linux
        debugInit();
#endif

        return true;
    }


    double prevTime = 0;
    int framesInSecond = 0;
    double timeTowardsSecond = 0;

    void tick()
    {
        PROFILE_FUNCTION();

        double curTime = glfwGetTime();
        double deltaTime = curTime - prevTime;
        ADD_TEST_RESULT(deltaTime);

        framesInSecond++;

//#if PERFORMANCE_TEST == 1
//        if (framesInSecond > 1000) {
//            glfwSetWindowShouldClose(g_window, GLFW_TRUE);
//        }
//#else
//        if ((timeTowardsSecond += deltaTime) > 1)
//        {
//            std::string fps = std::to_string(framesInSecond) + " fps";
//            glfwSetWindowTitle(g_window, fps.c_str());
//            framesInSecond = 0;
//            timeTowardsSecond -= 1;
//        }
//#endif

        if (screen)
            screen->render(deltaTime);

        swapBuffers();

        INPUT::KEYBOARD::lateUpdate();
        INPUT::MOUSE::lateUpdate();

#ifdef linux
        debugTick();
#endif

        prevTime = curTime;
    }

    inline void swapBuffers()
    {
        PROFILE_FUNCTION();
        glfwSwapBuffers(g_window);
        glfwPollEvents();
    }

    void framebufferSizeCallback(GLFWwindow* _, int width, int height)
    {
        PROFILE_FUNCTION();

        g_width = width;
        g_height = height;

        std::cout << "resize: ("<< width <<" " << height << ")\n";

        glViewport(0, 0, width, height);
        if (screen)
            screen->resize(width, height);
    }

    void run()
    {
        PROFILE_FUNCTION();

        prevTime = glfwGetTime();

        glfwSetFramebufferSizeCallback(g_window, framebufferSizeCallback);  // https://www.glfw.org/docs/latest/window_guide.html#window_fbsize

#ifdef __EMSCRIPTEN__ // Main loop
        emscripten_set_main_loop(tick, 0, 1);
#else
        while (!glfwWindowShouldClose(g_window)) { tick(); }
#endif

        glfwDestroyWindow(g_window);
    }

    void setScreen(Screen *newScreen)
    {
        PROFILE_FUNCTION();

        screen = newScreen;
        screen->setup(g_window);
        glViewport(0, 0, g_width, g_height);
        screen->resize(g_width, g_height);
    }
}