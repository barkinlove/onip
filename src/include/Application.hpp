#pragma once
#include <QApplication>
#include "MainWindow.hpp"
#include <cstdint>

class Application : public QApplication
{
public:
    Application(std::int32_t argc, char *argv[]);
    ~Application();

    std::int32_t run();

private:
    MainWindow m_window;
};
