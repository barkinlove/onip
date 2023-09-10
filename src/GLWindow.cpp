#include "GLWindow.hpp"
#include <QTimer>
#include "OniDevice.hpp"
#include "OniVideoStream.hpp"
#include <GL/glut.h>

#include <cmath>
#include <iostream>
#include <ranges>
#include <set>

GLWindow::GLWindow(QWidget *parent)
    : QGLWidget(parent)
    , m_loaded(false)
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

// it was too late when I found out that openni::RGB888Pixel has no overloaded arithmetical operators

void applySobel(const openni::VideoFrameRef *frame)
{
    //    std::vector<openni::RGB888Pixel> grayScaledImg;
    //    std::size_t imgSize = frame->getHeight() * frame->getWidth();
    //    grayScaledImg.resize(imgSize);
    //    auto frameBuffer = static_cast<const openni::RGB888Pixel *>(frame->getData());
    //    const size_t size = frame->getStrideInBytes() / sizeof(openni::RGB888Pixel);

    //    for (int y = 0; y < frame->getHeight(); ++y) {
    //        for (int x = 0; x < frame->getWidth(); ++x) {
    //            const openni::RGB888Pixel *currentPixel = frameBuffer + y * size + x;
    //            size_t idx = y * size + x;
    //            std::uint32_t sum = 0;
    //            sum += currentPixel->r;
    //            sum += currentPixel->g;
    //            sum += currentPixel->b;
    //            std::uint8_t avg = sum / 3;
    //            grayScaledImg[idx] = openni::RGB888Pixel{avg, avg, avg};
    //        }
    //    }
    //    std::vector<openni::RGB888Pixel> bufferX;
    //    std::vector<openni::RGB888Pixel> bufferY;
    //    std::vector<openni::RGB888Pixel> gx;
    //    std::vector<openni::RGB888Pixel> gy;
    //    bufferX.resize(imgSize);
    //    bufferY.resize(imgSize);
    //    gx.resize(imgSize);
    //    gy.resize(imgSize);
    //    for (int y = 1; y < frame->getHeight() - 1; ++y) {
    //        for (int x = 0; x < frame->getWidth(); ++x) {
    //            const openni::RGB888Pixel *currentPixel = frameBuffer + y * size + x;
    //            size_t idx = y * size + x;
    //            bufferX[idx] = grayScaledImg[idx + 1] - grayScaledImg[idx - 1];
    //            bufferY[idx] = 1 * grayScaledImg[idx] + 2 * grayScaledImg[idx] + 1 * grayScaledImg[idx];
    //        }
    //    }
    //    for (int y = 1; y < frame->getHeight() - 1; ++y) {
    //        for (int x = 0; x < frame->getWidth(); ++x) {
    //            const openni::RGB888Pixel *currentPixel = frameBuffer + y * size + x;
    //            size_t idx = y * size + x;
    //            // checkout indicies later, but imho it shuould work
    //            gx[idx] = 1 * bufferX[idx] + 2 * bufferX[idx] + 1 * bufferX[idx];
    //            gy[idx] = bufferY[idx + 1] - bufferY[idx - 1];
    //        }
    //    }

    //    float threshold = 0.1f;
    //    std::vector<openni::RGB888Pixel> intensity;
    //    std::vector<opnnni::RGB888Pixel> direction;
    //    intensity.resize(imgSize);
    //    direction.resize(imgSize);
    //    float maxIntense = 0.f, minIntense = 1000000.f;
    //    for (int i = 0; i < imgSize; ++i) {
    //        float x = gx[i];
    //        float y = gy[i];
    //        intensity[i] = std::sqrt(x * x + y * y);
    //        maxIntense = std::max(maxIntense, intensity[i]);
    //        minIntense = std::min(minIntense, intensity[i]);
    //        direction[i] = std::atan2(x, y);
    //    }
    //    static const s_pi = std::acos(-1.f);
    //    float value = 0.f;
    //    for (int i = 0; i < imgSize; ++i) {
    //        float hue = direction[i] * 180.f / s_pi + 180.f;
    //        if (maxIntense == minIntense) {
    //            value = 0.f;
    //        } else {
    //            float t = 255 * (intensity[i] - minIntense) / (maxIntense - minIntense);
    //            value = t > threshold ? t : 0;
    //        }
    //        float saturation = l = value;

    //        // hls to rgb conversion
    //    }
    //    return grayScaledImg;
}

void GLWindow::drawImg()
{
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
}

void GLWindow::beforeDraw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glOrtho(0.f, width(), height(), 0.f, -1.f, 1.f);
    glLoadIdentity();

    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
}

void GLWindow::paintGL()
{
    if (!m_loaded)
        return;

    std::int32_t streamReadyIdx = 0;
    openni::Status status = openni::OpenNI::waitForAnyStream(m_streams.data(),
                                                             m_streams.size(),
                                                             &streamReadyIdx);
    if (status != openni::STATUS_OK)
        return;

    openni::VideoStream *stream = m_streams[streamReadyIdx];
    openni::VideoFrameRef *frame = &m_frames[streamReadyIdx];

    stream->readFrame(frame);

    if (!frame->isValid())
        return;

    m_currentFrameIdx = frame->getFrameIndex();

    beforeDraw();

    bool colorChannel = m_currentMode == VideoMode::ColorChannel
                        && stream->getSensorInfo().getSensorType() == openni::SENSOR_COLOR;

    bool applySobel = false;
    if (colorChannel) {
        std::vector<openni::RGB888Pixel> sobelProcessed;
        if (m_sobel) {
            //            sobelProcessed = applySobel(frame);
        }
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
    drawImg();
    emit frameRendered(frame);
}

void GLWindow::closeOni()
{
    for (auto stream : m_streams) {
        stream->destroy();
    }
    if (m_device.isValid()) {
        m_device.close();
    }
}

void GLWindow::loadFile(const std::string &filename)
{
    const bool active = updatesEnabled();
    setUpdatesEnabled(false);
    closeOni();

    if (!m_device.open(filename.data())) {
        emit onFileLoadedFailure(filename);
        return;
    }

    m_colorStream.create(m_device, openni::SENSOR_COLOR);
    m_depthStream.create(m_device, openni::SENSOR_DEPTH);
    m_streams.push_back(m_colorStream.get());
    m_streams.push_back(m_depthStream.get());
    m_loaded = m_colorStream.get()->isValid() || m_depthStream.get()->isValid();
    if (m_loaded) {
        std::int32_t frameNumber = m_device.get().getPlaybackControl()->getNumberOfFrames(
            m_colorStream.ref());
        emit onFileLoaded();
        emit updateTimeline(frameNumber);
    } else
        emit onFileLoadedFailure(filename);
}

void GLWindow::onVideoModeChanged(VideoMode mode)
{
    m_currentMode = mode;
}

void GLWindow::onSobelCheckBoxPressed()
{
    m_sobel != m_sobel;
}

void GLWindow::onSeek(bool forward)
{
    const openni::VideoStream &stream = m_currentMode == VideoMode::ColorChannel
                                            ? m_colorStream.ref()
                                            : m_depthStream.ref();
    m_currentFrameIdx = forward ? ++m_currentFrameIdx : --m_currentFrameIdx;
    m_device.get().getPlaybackControl()->seek(stream, m_currentFrameIdx);
    paintGL();
}

void GLWindow::onSetFrame(std::int32_t frameIdx)
{
    const openni::VideoStream &stream = m_currentMode == VideoMode::ColorChannel
                                            ? m_colorStream.ref()
                                            : m_depthStream.ref();
    m_device.get().getPlaybackControl()->seek(stream, frameIdx);
    //    QTimer::singleShot(0, this, SLOT(update()));
}
