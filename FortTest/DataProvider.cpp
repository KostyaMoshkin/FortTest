#include "DataProvider.h"

#include <thread>
#include <chrono>
#include <mutex>

#include <QDebug>

std::mutex mutexSpectr;

const double TwoPi = 6.283185307179586;

static void FFTAnalysis(unsigned _int16* AVal, float* FTvl, int Nvl, int Nft) {
    int i, j, n, m, Mmax, Istp;
    float Tmpr, Tmpi, Wtmp, Theta;
    float Wpr, Wpi, Wr, Wi;
    float* Tmvl;

    n = Nvl * 2;
    Tmvl = new float[n];

    for (i = 0; i < n; i += 2) {
        Tmvl[i] = 0;
        Tmvl[i + 1] = AVal[i / 2];
    }

    i = 1; j = 1;
    while (i < n) {
        if (j > i) {
            Tmpr = Tmvl[i]; Tmvl[i] = Tmvl[j]; Tmvl[j] = Tmpr;
            Tmpr = Tmvl[i + 1]; Tmvl[i + 1] = Tmvl[j + 1]; Tmvl[j + 1] = Tmpr;
        }
        i = i + 2; m = Nvl;
        while ((m >= 2) && (j > m)) {
            j = j - m; m = m >> 1;
        }
        j = j + m;
    }

    Mmax = 2;
    while (n > Mmax) {
        Theta = -TwoPi / Mmax; Wpi = sin(Theta);
        Wtmp = sin(Theta / 2); Wpr = Wtmp * Wtmp * 2;
        Istp = Mmax * 2; Wr = 1; Wi = 0; m = 1;

        while (m < Mmax) {
            i = m; m = m + 2; Tmpr = Wr; Tmpi = Wi;
            Wr = Wr - Tmpr * Wpr - Tmpi * Wpi;
            Wi = Wi + Tmpr * Wpi - Tmpi * Wpr;

            while (i < n - Mmax) {
                j = i + Mmax;
                Tmpr = Wr * Tmvl[j] - Wi * Tmvl[j - 1];
                Tmpi = Wi * Tmvl[j] + Wr * Tmvl[j - 1];

                Tmvl[j] = Tmvl[i] - Tmpr; Tmvl[j - 1] = Tmvl[i - 1] - Tmpi;
                Tmvl[i] = Tmvl[i] + Tmpr; Tmvl[i - 1] = Tmvl[i - 1] + Tmpi;
                i = i + Istp;
            }
        }

        Mmax = Istp;
    }

    for (i = 0; i < Nft; i++) {
        j = i * 2;
        FTvl[i] = 2 * sqrt(pow(Tmvl[j], 2) + pow(Tmvl[j + 1], 2)) / Nvl / (5 * 1024);
    }

    delete[]Tmvl;
}

static void MicrophonReader(WAVEFORMATEX wfex_, WAVEHDR waveHdr_, std::vector<float>& vfSpectr_)
{
    HWAVEIN hWaveIn;

    std::vector<unsigned _int16> vSoundBuffer(vfSpectr_.size());

    waveHdr_.lpData = (LPSTR)vSoundBuffer.data();

    while (true)
    {
        waveInOpen(&hWaveIn, WAVE_MAPPER, &wfex_, 0L, 0L, WAVE_FORMAT_DIRECT);
        waveInPrepareHeader(hWaveIn, &waveHdr_, sizeof(WAVEHDR));
        waveInAddBuffer(hWaveIn, &waveHdr_, sizeof(WAVEHDR));
        waveInStart(hWaveIn);

        while (waveInUnprepareHeader(hWaveIn, &waveHdr_, sizeof(WAVEHDR)) == WAVERR_STILLPLAYING)
            Sleep(20);

        waveInClose(hWaveIn);

        std::lock_guard<std::mutex> guard(mutexSpectr);
        {
            FFTAnalysis(vSoundBuffer.data(), vfSpectr_.data(), vSoundBuffer.size(), vfSpectr_.size());
        }
    }
}

//------------------------------------------------------------------------------------------

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
    delete m_timer;
}

void DataProvider::init(int nFrequency_) 
{ 
    m_nFrequency = nFrequency_; 

    AudioInit();
}

void DataProvider::AudioInit()
{
    const float fPeriod = 1.0f / m_nFrequency;

    const int SampleRate = 4 * 16 * 1024;

    m_wfex.wFormatTag = WAVE_FORMAT_PCM;
    m_wfex.nChannels = 2;
    m_wfex.wBitsPerSample = 16;
    m_wfex.nSamplesPerSec = SampleRate;
    m_wfex.nAvgBytesPerSec = (DWORD)SampleRate * m_wfex.nChannels * m_wfex.wBitsPerSample / 8;
    m_wfex.nBlockAlign = (WORD)m_wfex.nChannels * m_wfex.wBitsPerSample / 8;
    m_wfex.cbSize = 0;

    m_waveHdr.dwBufferLength = (DWORD)SampleRate * 2 * fPeriod * m_wfex.nChannels;
    m_waveHdr.dwBytesRecorded = 0;
    m_waveHdr.dwUser = 0L;
    m_waveHdr.dwFlags = 0L;
    m_waveHdr.dwLoops = 0L;

    m_vfSpectr.resize(m_wfex.nSamplesPerSec * m_wfex.nChannels * 1.0 / m_nFrequency);

    std::thread micThread(MicrophonReader, m_wfex, m_waveHdr, std::ref( m_vfSpectr));
    micThread.detach();
}

void DataProvider::setBufferDimention(int nWidth_, int nHeight_)
{
    m_nBufferWidth = nWidth_;
    m_nBufferHeight = nHeight_;

    m_vBuffer.resize(m_nBufferWidth * m_nBufferHeight, 0);

    m_nCurrentPosition = m_nBufferHeight - 1;

    m_timer = new QTimer(this);
    m_timer->start(1000 / m_nFrequency);
    connect(m_timer, &QTimer::timeout, this, &DataProvider::setNewPortion);
}

void DataProvider::setNewPortion()
{
    m_nCurrentPosition = m_nCurrentPosition > 0 ? --m_nCurrentPosition : m_nBufferHeight - 1;

    memcpy(&m_vBuffer[m_nCurrentPosition * m_nBufferWidth], m_vfSpectr.data(), m_nBufferWidth * sizeof(float));

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
