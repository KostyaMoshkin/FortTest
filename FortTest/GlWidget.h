#pragma once

#include <QtOpenGLWidgets/QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>

#include "DataProvider.h"

class GlWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT

	GLint m_posAttr = 0;

	QOpenGLShaderProgram* m_program;

	QTimer* m_timer;

	DataProvider m_dataProvider;

	unsigned int m_texture;

	int m_nFrequency;

	int m_nLineWidth;

	int m_nTextureWidth;
	int m_nTextureHeight;

public:
	GlWidget(QWidget* pParent_ = nullptr);
	~GlWidget();

protected:
	void initializeGL() override;
	void paintGL() override;
	void resizeGL(int nWidth_, int nHeight_) override;

protected:
	void initialize();
	void updateTexture();
	void update();

	void setLineWidth(int nLineWidth_);
	void setTextureSize(int nWidht_);
};

