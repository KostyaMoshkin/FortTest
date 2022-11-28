#pragma once

#include <QtOpenGLWidgets/QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>

#include "DataProvider.h"

class GlWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT

	GLint m_posAttr = 0;

	QOpenGLShaderProgram* m_program = nullptr;

	unsigned int m_texture = UINT_MAX;

	int m_nFrequency;

	int m_nLineWidth;

	int m_nTextureWidth;
	int m_nTextureHeight;

	DataProvider m_dataProvider;

	QTimer* timer;

public:
	GlWidget(QWidget* pParent_ = nullptr);
	~GlWidget();

protected:
	void initializeGL() override;
	void paintGL() override;
	void resizeGL(int nWidth_, int nHeight_) override;

	void update();

protected:
	void initialize();
	void updateTexture();
	void setLineWidth(int nLineWidth_);
	void setTextureSize(int nWidht_);

};

