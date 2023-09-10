#include "MainWindow.hpp"
#include <QFileDialog>
#include <QMessageBox>
#include <QStyle>
#include <QTime>
#include "ui_MainWindow.h"

#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);
    m_ui->m_play->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    m_ui->m_forward->setIcon(style()->standardIcon(QStyle::SP_MediaSeekForward));
    m_ui->m_rewind->setIcon(style()->standardIcon(QStyle::SP_MediaSeekBackward));
    connect(m_ui->m_openAction, &QAction::triggered, this, &MainWindow::onOpenFile);
    connect(m_ui->m_forward, &QPushButton::clicked, this, &MainWindow::onForward);
    connect(m_ui->m_play, &QPushButton::clicked, this, &MainWindow::onPlay);
    connect(m_ui->m_canvas, &GLWindow::updateTime, this, &MainWindow::onTimeUpdate);
    connect(m_ui->m_canvas, &GLWindow::onFileLoadedFailure, this, &MainWindow::onFileLoadFailure);
    connect(m_ui->m_canvas, &GLWindow::onFileLoaded, this, &MainWindow::onFileLoaded);
    connect(this, &MainWindow::changeVideoMode, m_ui->m_canvas, &GLWindow::onVideoModeChanged);
}

MainWindow::~MainWindow() = default;

constexpr std::string_view home = "C:\\";

void MainWindow::onOpenFile()
{
    const std::string filename = QFileDialog::getOpenFileName(this,
                                                              tr("Open File"),
                                                              home.data(),
                                                              tr("Depth image (*.oni)"))
                                     .toStdString();
    m_ui->m_canvas->setUpdatesEnabled(false);
    m_ui->m_canvas->loadFile(filename);
}

void MainWindow::onForward()
{
    const bool active = m_ui->m_canvas->updatesEnabled();
    if (active)
        return;

    m_ui->m_canvas->updateGL();
}

void MainWindow::onPlay()
{
    const bool active = m_ui->m_canvas->updatesEnabled();

    if (active) {
        m_ui->m_play->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        m_ui->m_canvas->setUpdatesEnabled(false);
    } else {
        m_ui->m_play->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
        m_ui->m_canvas->setUpdatesEnabled(true);
    }
}

void MainWindow::onTimeUpdate(std::uint64_t timestamp)
{
    m_ui->label->setText(QTime::fromMSecsSinceStartOfDay(timestamp).toString("hh:mm:ss.zzz"));
}

void MainWindow::onFileLoaded() {}

void MainWindow::onFileLoadFailure(const std::string &filename)
{
    QMessageBox msg;
    msg.setText(QString("Unable to load file: \"%1\"").arg(QString::fromStdString(filename)));
    msg.show();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_1:
        emit changeVideoMode(GLWindow::VideoMode::ColorChannel);
        break;
    case Qt::Key_2:
        emit changeVideoMode(GLWindow::VideoMode::DepthChannel);
        break;
    }
}
