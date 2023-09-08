#include "Application.hpp"
#include <openni2/OpenNI.h>

Application::Application(std::int32_t argc, char *argv[])
    : QApplication(argc, argv)
{
    openni::OpenNI::initialize();
}

Application::~Application()
{
    openni::OpenNI::shutdown();
}

std::int32_t Application::run()
{
    m_window.show();
    return exec();
}
