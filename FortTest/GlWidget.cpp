#include "GlWidget.h"
#include "ConfigReader.h"

GlWidget::GlWidget(QWidget* pParent_)
{
    ConfigReader configReader;

    m_nTextureHeight = configReader.getInt("bufferSize");
    m_nLineWidth = configReader.getInt("width");
    m_nFrequency = configReader.getInt("frequency");
}

GlWidget::~GlWidget()
{
}

void GlWidget::initializeGL()
{
    initializeOpenGLFunctions();
    initialize();
}

void GlWidget::paintGL()
{
    if (!updateTexture())
        return;

    m_program->bind();

    glEnableVertexAttribArray(m_posAttr);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(m_posAttr);

    m_program->release();
}

void GlWidget::resizeGL(int nWidth_, int nHeight_)
{
    glViewport(0, 0, nWidth_, nHeight_);
}

void GlWidget::initialize()
{
    //m_timePoint = std::chrono::steady_clock::now();

    static const char* vertexShaderSource =
        "#version 330 core\n"
        "layout (location = 0) in vec2 posAttr;\n"
        "uniform int m_nLineWidth;\n"
        "out vec2 TexCoord;\n"
        "void main() {\n"
        "   gl_Position = vec4(posAttr, 1.0, 1.0);\n"
        "   TexCoord = 1.0 - (posAttr + 1.0) / 2.0;\n"
        "   TexCoord.y = TexCoord.y / m_nLineWidth;\n"
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    setLineWidth(15);

    setTextureSize(1024, 256);

    m_program->release();
}

bool GlWidget::updateTexture()
{
    if (!m_dataProvider.isReady())
        return false;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, (GLsizei)m_nTextureWidth, (GLsizei)m_dataProvider.getDataCount(DataProvider::A), GL_RED, GL_FLOAT, m_dataProvider.getDataBuffer(DataProvider::A));
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, (GLint)m_dataProvider.getDataCount(DataProvider::A), (GLsizei)m_nTextureWidth, (GLsizei)m_dataProvider.getDataCount(DataProvider::B), GL_RED, GL_FLOAT, m_dataProvider.getDataBuffer(DataProvider::B));

    m_dataProvider.dataReceived();

    return true;
}

void GlWidget::setLineWidth(int nLineWidth_)
{
    m_program->setUniformValue("m_nLineWidth", (GLint)nLineWidth_);
}

void GlWidget::setTextureSize(int nWidht_, int nHeight_)
{
    m_nTextureWidth = nWidht_;
    m_nTextureHeight = nHeight_;

    m_dataProvider.setBufferDimention(m_nTextureWidth, m_nTextureHeight);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)m_nTextureWidth, (GLsizei)m_nTextureHeight, 0, GL_RED, GL_FLOAT, nullptr);
}