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

private:
    void keyPressEvent(QKeyEvent *event);

private slots:
    void onOpenFile();
    void onForward();
    void onPlay();
    void onTimeUpdate(std::uint64_t timestamp);
    void onFileLoaded();
    void onFileLoadFailure(const std::string &filename);

private:
    std::unique_ptr<Ui::MainWindow> m_ui;
    bool m_inAction = false;
};
