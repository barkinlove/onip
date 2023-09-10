#include "MainWindow.hpp"
#include <QFileDialog>
#include <QGLWidget>
#include <QMessageBox>
#include <QStyle>
#include <QTime>
#include <QTimer>
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
    // TODO: remove this when sobel operator will be introduced
    m_ui->m_sobelCheckBox->setDisabled(true);
    connect(m_ui->m_openAction, &QAction::triggered, this, &MainWindow::onOpenFile);
    connect(m_ui->m_forward, &QPushButton::clicked, this, &MainWindow::onForward);
    connect(m_ui->m_play, &QPushButton::clicked, this, &MainWindow::onPlay);
    connect(m_ui->m_canvas, &GLWindow::frameRendered, this, &MainWindow::onTimeUpdate);
    connect(m_ui->m_canvas, &GLWindow::onFileLoadedFailure, this, &MainWindow::onFileLoadFailure);
    connect(m_ui->m_canvas, &GLWindow::onFileLoaded, this, &MainWindow::onFileLoaded);
    connect(this, &MainWindow::changeVideoMode, m_ui->m_canvas, &GLWindow::onVideoModeChanged);
    connect(m_ui->m_sobelCheckBox,
            &QCheckBox::clicked,
            m_ui->m_canvas,
            &GLWindow::onSobelCheckBoxPressed);
    connect(this, &MainWindow::seek, m_ui->m_canvas, &GLWindow::onSeek);
    connect(m_ui->m_canvas, &GLWindow::updateTimeline, this, &MainWindow::onTimelineInit);
    connect(m_ui->m_canvas, &GLWindow::frameRendered, this, &MainWindow::onTimelineUpdate);
    connect(m_ui->m_timeline, &QSlider::sliderMoved, m_ui->m_canvas, &GLWindow::onSetFrame);
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
    if (filename.empty())
        return;
    m_ui->m_canvas->loadFile(filename);
}

void MainWindow::onForward()
{
    const bool active = m_ui->m_canvas->updatesEnabled();
    if (active)
        return;
    emit seek(true);
}

void MainWindow::onRewind()
{
    const bool active = m_ui->m_canvas->updatesEnabled();
    if (active)
        return;
    emit seek(false);
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

void MainWindow::onTimeUpdate(const openni::VideoFrameRef *frame)
{
    // testing
    m_ui->label->setText(
        QTime::fromMSecsSinceStartOfDay(frame->getTimestamp()).toString("hh:mm:ss.zzz"));
}

void MainWindow::onFileLoaded()
{
    m_ui->m_play->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
}

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
    case Qt::Key_Space:
        onPlay();
        break;
    }
}

void MainWindow::onTimelineInit(std::uint32_t frameNumber)
{
    m_ui->m_timeline->setMaximum(frameNumber);
}

void MainWindow::onTimelineUpdate(const openni::VideoFrameRef *frame)
{
    m_ui->m_timeline->setValue(frame->getFrameIndex());
}
