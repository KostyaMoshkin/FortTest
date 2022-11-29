#include "GlWidget.h"
#include "ConfigReader.h"

GlWidget::GlWidget(QWidget* pParent_)
{
    ConfigReader configReader;

    m_nTextureHeight = configReader.getInt("bufferSize");
    m_nLineWidth = configReader.getInt("width");
    m_nFrequency = configReader.getInt("frequency");

    m_dataProvider.init(m_nFrequency);

    m_timer = new QTimer(this);
    m_timer->start(1000 / 5 / m_nFrequency);
    connect(m_timer, &QTimer::timeout, this, &GlWidget::update);
}

GlWidget::~GlWidget()
{
    delete m_timer;
    delete m_program;
}

void GlWidget::initializeGL()
{
    initializeOpenGLFunctions();
    initialize();
}

void GlWidget::resizeGL(int nWidth_, int nHeight_)
{
    glViewport(0, 0, nWidth_, nHeight_);

    m_program->bind();
    m_program->setUniformValue("m_fScreenRatio", (GLfloat)nHeight_ / m_nTextureHeight);
    m_program->release();
}

void GlWidget::initialize()
{
    static const char* vertexShaderSource =
        "#version 330 core\n"
        "layout (location = 0) in vec2 posAttr;\n"
        "uniform int m_nLineWidth;\n"
        "uniform float m_fScreenRatio;\n"
        "out vec2 TexCoord;\n"
        "void main() {\n"
        "   gl_Position = vec4(posAttr, 1.0, 1.0);\n"
        "   TexCoord = (posAttr + 1.0) / 2.0;\n"
        "   TexCoord.y = (1.0 - TexCoord.y) * m_fScreenRatio / m_nLineWidth;\n"
        "}\n";

    static const char* fragmentShaderSource =
        "#version 330 core\n"
        "uniform sampler2D ourTexture;\n"
        "in vec2 TexCoord;\n"
        "out vec4 FragColor;\n"
        "void main() {\n"
        "   float fAmplitude = texture(ourTexture, TexCoord).r;\n"
        "   FragColor = vec4(fAmplitude, fAmplitude, fAmplitude, 1);\n"
        "}\n";

    m_program = new QOpenGLShaderProgram(this);
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    m_program->link();
    m_program->bind();

    m_posAttr = m_program->attributeLocation("posAttr");
    Q_ASSERT(m_posAttr != -1);

    static const GLfloat vertices[] = {
         -1.0f,  -1.0f,
         -1.0f,  +1.0f,
         +1.0f,  +1.0f,
         +1.0f,  +1.0f,
         +1.0f,  -1.0f,
         -1.0f,  -1.0f
    };

    glVertexAttribPointer(m_posAttr, 2, GL_FLOAT, GL_FALSE, 0, vertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenTextures(1, &m_texture);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    setLineWidth(m_nLineWidth);

    setTextureSize(512);

    m_program->release();
}

void GlWidget::update()
{
    if (!m_dataProvider.isReady())
        return;

    QOpenGLWidget::update(0, 0, size().width(), size().height());
}

void GlWidget::paintGL()
{
    updateTexture();

    m_program->bind();

    glEnableVertexAttribArray(m_posAttr);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(m_posAttr);

    m_program->release();
}

void GlWidget::updateTexture()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, (GLsizei)m_nTextureWidth, (GLsizei)m_dataProvider.getDataCount(DataProvider::A), GL_RED, GL_FLOAT, m_dataProvider.getDataBuffer(DataProvider::A));
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, (GLint)m_dataProvider.getDataCount(DataProvider::A), (GLsizei)m_nTextureWidth, (GLsizei)m_dataProvider.getDataCount(DataProvider::B), GL_RED, GL_FLOAT, m_dataProvider.getDataBuffer(DataProvider::B));

    m_dataProvider.dataReceived();
}

void GlWidget::setLineWidth(int nLineWidth_)
{
    m_program->setUniformValue("m_nLineWidth", (GLint)nLineWidth_);
}

void GlWidget::setTextureSize(int nWidht_)
{
    m_nTextureWidth = nWidht_;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)m_nTextureWidth, (GLsizei)m_nTextureHeight, 0, GL_RED, GL_FLOAT, nullptr);

    m_dataProvider.setBufferDimention(m_nTextureWidth, m_nTextureHeight);
}
