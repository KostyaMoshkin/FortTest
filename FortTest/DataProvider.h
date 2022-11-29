#pragma once

#include <QTimer>

#include <windows.h>
#include <mmsystem.h>

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

	int m_nCurrentPosition = 0;

	bool m_bDataReady = false;

	std::vector<float> m_vBuffer;
	std::vector<float> m_vfSpectr;

	std::vector<float *> m_vData;
	std::vector<int> m_vCount;

	QTimer* m_timer;

	WAVEFORMATEX m_wfex;
	WAVEHDR m_waveHdr;

public:
	DataProvider();
	~DataProvider();

public:
	void init(int nFrequency_);
	void AudioInit();

	void setBufferDimention(int nWidth_, int nHeight_);
	void setNewPortion();

	float* getDataBuffer(BufferName bufferName_);
	int getDataCount(BufferName bufferName_);

	bool isReady() { return m_bDataReady; }
	void dataReceived() { m_bDataReady = false; }
};

