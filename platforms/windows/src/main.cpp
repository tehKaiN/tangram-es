#include "glfwApp.h"
#include "windowsPlatform.h"
#include "log.h"
#include "map.h"
#include <memory>
#include <signal.h>
#include <stdlib.h>

using namespace Tangram;

int main(int argc, char* argv[]) {

    auto platform = std::make_shared<WindowsPlatform>();

    // Create the windowed app.
    GlfwApp::create(platform, 1024, 768);
    GlfwApp::parseArgs(argc, argv);

    // Loop until the user closes the window
    GlfwApp::run();

    // Clean up.
    GlfwApp::destroy();

}
