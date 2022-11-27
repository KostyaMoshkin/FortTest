#pragma once

#include <vector>

class DataProvider
{
public:
	enum BufferName {
		A = 0,
		B,
	};

private:

	int m_nBufferWidth = 0;
	int m_nBufferHeight = 0;
	std::vector<float> m_vBuffer;

	std::vector<float *> m_vData;
	std::vector<int> m_vCount;

	int m_nCurrentPosition = 0;

	bool m_bDataReady = false;

public:
	DataProvider();
	~DataProvider();

public:
	void setBufferDimention(int nWidth_, int nHeight_);
	void setNewPortion(float *pData_);

	float* getDataBuffer(BufferName bufferName_);
	int getDataCount(BufferName bufferName_);

	bool isReady() { return m_bDataReady; }
	void dataReceived() { m_bDataReady = false; }
};

