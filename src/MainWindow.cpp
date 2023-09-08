#include "MainWindow.hpp"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_pUi(new Ui::MainWindow)
{
    m_pUi->setupUi(this);
}

MainWindow::~MainWindow() = default;
