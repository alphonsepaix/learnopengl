#include "Application.h"

int main() {
    Application app;
    while (app.isRunning()) app.mainLoop();
    return 0;
}
