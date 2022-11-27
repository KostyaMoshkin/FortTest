#include "DataProvider.h"

#include <chrono>

static void dataCollector(float* pData_, int nBufferWidth_, const int& nCurrentPosition_)
{
    auto start = std::chrono::steady_clock::now();


    while (true)
    {
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;
        if (elapsed_seconds.count() > 0.2)
        {
            start = std::chrono::steady_clock::now();
            for (int i = 0; i < nBufferWidth_; ++i)
                pData_[i] = nCurrentPosition_ % 8;
        }
    }
}

DataProvider::DataProvider()
{
    m_vData.resize(2);

    m_vData[BufferName::A] = nullptr;
    m_vData[BufferName::B] = nullptr;

    m_vCount.resize(2);

    m_vCount[BufferName::A] = 0;
    m_vCount[BufferName::B] = 0;
}

DataProvider::~DataProvider()
{
}

void DataProvider::setBufferDimention(int nWidth_, int nHeight_)
{
    m_nBufferWidth = nWidth_;
    m_nBufferHeight = nHeight_;

    m_vBuffer.resize(m_nBufferWidth * m_nBufferHeight);
    memset(m_vBuffer.data(), 0, m_vBuffer.size() * sizeof(float));

    m_nCurrentPosition = m_nBufferHeight - 1;

    setNewPortion(nullptr);
}

void DataProvider::setNewPortion(float* pData_)
{
    m_nCurrentPosition = m_nCurrentPosition > 0 ? --m_nCurrentPosition : m_nBufferHeight - 1;

    for (int i = 0; i < m_nBufferWidth; ++i)
        m_vBuffer[i + m_nCurrentPosition * m_nBufferWidth] = m_nCurrentPosition % 8;

    //memcpy(&m_vBuffer[m_nCurrentPosition * m_nBufferWidth], pData_, m_nBufferWidth * sizeof(float));

    m_vData[BufferName::A] = &m_vBuffer[m_nCurrentPosition * m_nBufferWidth];
    m_vCount[BufferName::A] = m_nBufferHeight - m_nCurrentPosition;

    m_vData[BufferName::B] = &m_vBuffer[0];
    m_vCount[BufferName::B] = m_nCurrentPosition;

    m_bDataReady = true;
}

float* DataProvider::getDataBuffer(BufferName bufferName_)
{
    return m_vData[bufferName_];
}

int DataProvider::getDataCount(BufferName bufferName_)
{
    return m_vCount[bufferName_];
}
