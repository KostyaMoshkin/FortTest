#pragma once
#include <QTimer>

#include <vector>

class DataProvider : public QObject
{
public:
	enum BufferName {
		A = 0,
		B,
	};

private:
	int m_nFrequency = 1;

	int m_nBufferWidth = 0;
	int m_nBufferHeight = 0;
	std::vector<float> m_vBuffer;

	std::vector<float *> m_vData;
	std::vector<int> m_vCount;

	int m_nCurrentPosition = 0;

	bool m_bDataReady = false;

	QTimer* timer;

public:
	DataProvider();
	~DataProvider();

public:
	void setBufferDimention(int nWidth_, int nHeight_);
	void setFrequency(int nFrequency_) { m_nFrequency = nFrequency_; }
	void setNewPortion();

	float* getDataBuffer(BufferName bufferName_);
	int getDataCount(BufferName bufferName_);

	bool isReady() { return m_bDataReady; }
	void dataReceived() { m_bDataReady = false; }
};

