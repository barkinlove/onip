#include "GLWindow.hpp"
#include <QTimer>
#include "OniDevice.hpp"
#include "OniVideoStream.hpp"
#include <GL/glut.h>

#include <iostream>
#include <ranges>
#include <set>

GLWindow::GLWindow(QWidget *parent)
    : QGLWidget(parent)
    , m_loaded(false)
    , m_finished(false)
    , m_timer(std::make_unique<QTimer>(this))
    , m_currentMode(VideoMode::ColorChannel)
{
    m_streams.reserve(2);
    m_frames.resize(2);
}

void GLWindow::initializeGL()
{
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    connect(m_timer.get(), SIGNAL(timeout()), this, SLOT(update()));
    m_timer->start(10);
}

void GLWindow::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    updateGL();
}

static std::set<std::uint64_t> timestamps = {};

void GLWindow::paintGL()
{
    if (!m_loaded || m_finished) {
        glClear(GL_COLOR_BUFFER_BIT);
        return;
    }

    std::int32_t streamReadyIdx = 0;
    openni::Status status = openni::OpenNI::waitForAnyStream(m_streams.data(),
                                                             m_streams.size(),
                                                             &streamReadyIdx);
    if (status != openni::STATUS_OK) {
        return;
    }

    openni::VideoStream *stream = m_streams[streamReadyIdx];
    openni::VideoFrameRef *frame = &m_frames[streamReadyIdx];

    stream->readFrame(frame);

    if (std::ranges::any_of(timestamps, [target = frame->getTimestamp()](auto &timestamp) {
            return timestamp == target;
        })) {
        m_finished = true;
        return;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glOrtho(0.f, width(), height(), 0.f, -1.f, 1.f);
    glLoadIdentity();

    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    bool colorChannel = m_currentMode == VideoMode::ColorChannel
                        && stream->getSensorInfo().getSensorType() == openni::SENSOR_COLOR;

    if (colorChannel) {
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RGB,
                     frame->getWidth(),
                     frame->getHeight(),
                     0,
                     GL_RGB,
                     GL_UNSIGNED_BYTE,
                     frame->getData());
    }

    bool depthChannel = m_currentMode == VideoMode::DepthChannel
                        && stream->getSensorInfo().getSensorType() == openni::SENSOR_DEPTH;
    if (depthChannel) {
        int maxDepth = 3500;
        int minDepth = 350;
        std::vector<openni::DepthPixel> correctDepth;
        correctDepth.resize(frame->getHeight() * frame->getWidth());
        auto depthBufferData = static_cast<const openni::DepthPixel *>(frame->getData());
        const size_t size = frame->getStrideInBytes() / sizeof(openni::DepthPixel);

        for (int y = 0; y < frame->getHeight(); ++y) {
            for (int x = 0; x < frame->getWidth(); ++x) {
                const openni::DepthPixel *currentPixel = depthBufferData + y * size + x;
                size_t idx = y * size + x;
                if (*currentPixel) {
                    correctDepth[idx] = maxDepth - (*currentPixel - minDepth);
                }
            }
        }

        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_DEPTH_COMPONENT,
                     frame->getWidth(),
                     frame->getHeight(),
                     0,
                     GL_DEPTH_COMPONENT,
                     GL_SHORT,
                     correctDepth.data());
    }

    glBegin(GL_QUADS);

    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-1.0f, -1.0f, 1.0f);

    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, 1.0f);

    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);

    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-1.0f, 1.0f, 1.0f);

    glEnd();

    timestamps.insert(frame->getTimestamp());
    emit updateTime(frame->getTimestamp());
}

void GLWindow::loadFile(const std::string &filename)
{
    m_playbackTime.restart();
    m_finished = false;

    if (!m_device.open(filename.data())) {
        emit onFileLoadedFailure(filename);
        return;
    }

    m_colorStream.create(m_device, openni::SENSOR_COLOR);
    m_depthStream.create(m_device, openni::SENSOR_DEPTH);
    m_streams.push_back(m_colorStream.get());
    m_streams.push_back(m_depthStream.get());
    m_loaded = m_colorStream.get()->isValid() || m_depthStream.get()->isValid();
    if (m_loaded)
        emit onFileLoaded();
    else
        emit onFileLoadedFailure(filename);
}

void GLWindow::onVideoModeChanged(VideoMode mode)
{
    m_currentMode = mode;
}
