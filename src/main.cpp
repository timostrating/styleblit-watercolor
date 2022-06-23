//
// Created by sneeuwpop on 16-6-19.
//

// PROFILING requires the gcc flag -finstrument-functions
#define PROFILING 0
#define AUTOMATIC_PROFILING 0
#define TESTING 0
#define PERFORMANCE_TEST 0

#include "screens/triangle_test_screen.cpp"
#include "screens/imgui_test_screen.cpp"
#include "screens/model_test_screen.cpp"
#include "screens/model_indices_test_screen.cpp"
#include "screens/quad_model_test_screen.cpp"
#include "screens/color_test_screen.cpp"
#include "screens/tmp_screen.cpp"
//#include "screens/blend_linear_screen.cpp"
//#include "screens/blend_km_screen.cpp"
//#include "screens/test_runner_screen.cpp"
#include "util/game.h"

#include "util/external/unit_test.hpp"
#include "util/debug/profiler.h"
#include "util/debug/tester.h"

#define STB_IMAGE_IMPLEMENTATION
#include "util/external/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "util/external/std_image_write.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "util/external/stb_image_resize.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "util/external/tiny_obj_loader.h"

int main(int argc, char* argv[])
{
    PROFILE_FUNCTION();

    PROFILER_BEGIN_SESSION("Main", "Profile.json");
    if (!GAME::init())
        return -1;

    TmpScreen screen;
    GAME::setScreen(&screen);

#if TESTING == 1
    RUN_ALL_TESTS();
#endif

    GAME::run();
    PROFILER_END_SESSION();
    return 0;
}