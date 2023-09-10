#pragma once

#include <QKeyEvent>
#include <QMainWindow>
#include "GLWindow.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow final : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void changeVideoMode(GLWindow::VideoMode mode);
    void seek(bool forward);

private:
    void keyPressEvent(QKeyEvent *event);

private slots:
    void onOpenFile();
    void onForward();
    void onRewind();
    void onPlay();
    void onTimeUpdate(const openni::VideoFrameRef *frame);
    void onFileLoaded();
    void onFileLoadFailure(const std::string &filename);
    void onTimelineInit(std::uint32_t frameNumber);
    void onTimelineUpdate(const openni::VideoFrameRef *frame);

private:
    std::unique_ptr<Ui::MainWindow> m_ui;
};
