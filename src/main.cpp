#define DBG_MACRO_NO_WARNING
#include <dbg.h>

#include "Application.h"

const std::string SHADER_DIR = "assets/shaders/";
const std::string TEXTURE_DIR = "assets/textures/";
const std::string MODELS_DIR = "assets/models/";

float fov = 45.0f;

int main() {
    Application app;
    while (app.isRunning()) app.mainLoop();
    return 0;
}
