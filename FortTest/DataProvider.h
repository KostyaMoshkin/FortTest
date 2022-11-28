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
	std::vector<float> m_vBuffer;

	std::vector<float> m_vfSpectr;

	std::vector<float *> m_vData;
	std::vector<int> m_vCount;

	int m_nCurrentPosition = 0;

	bool m_bDataReady = false;

	QTimer* timer;


	WAVEFORMATEX m_wfex;
	WAVEHDR m_waveHdr;


public:
	DataProvider();
	~DataProvider();

public:
	void setBufferDimention(int nWidth_, int nHeight_);
	void init(int nFrequency_);
	void setNewPortion();
	void AudioInit();

	float* getDataBuffer(BufferName bufferName_);
	int getDataCount(BufferName bufferName_);

	bool isReady() { return m_bDataReady; }
	void dataReceived() { m_bDataReady = false; }
};

