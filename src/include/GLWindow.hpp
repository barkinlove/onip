#pragma once

#include <QGLWidget>
#include <QTime>
#include "OniDevice.hpp"
#include "OniVideoStream.hpp"

class GLWindow : public QGLWidget
{
    Q_OBJECT
public:
    explicit GLWindow(QWidget *parent = nullptr);

    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void loadFile(const std::string &filename);
    void setActive(bool active) noexcept;
    bool isActive() const noexcept;

    enum class VideoMode { ColorChannel, DepthChannel };

public slots:
    void onVideoModeChanged(VideoMode mode);

signals:
    void updateTime(std::uint64_t timestamp);
    void onFileLoadedFailure(const std::string &filename);
    void onFileLoaded();

private:
    OniDevice m_device;
    OniVideoStream m_colorStream;
    OniVideoStream m_depthStream;
    std::vector<openni::VideoStream *> m_streams;
    std::vector<openni::VideoFrameRef> m_frames;
    bool m_loaded;
    bool m_finished;
    std::unique_ptr<QTimer> m_timer;
    QTime m_playbackTime;
    VideoMode m_currentMode;
};
