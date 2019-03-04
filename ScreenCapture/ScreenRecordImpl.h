#pragma once


#include <Windows.h>
#include <atomic>
#include <QObject>
#include <QString>
#include <QMutex>
#include <condition_variable>

#ifdef	__cplusplus
extern "C"
{
#endif
struct AVFormatContext;
struct AVCodecContext;
struct AVCodec;
struct AVFifoBuffer;
struct AVAudioFifo;
struct AVFrame;
struct SwsContext;
struct SwrContext;
#ifdef __cplusplus
};
#endif

class ScreenRecordImpl : public QObject
{
	Q_OBJECT
private:
	enum RecordState {
		NotStarted,
		Started,
		Paused,
		Stopped,
		Unknown,
	};
public:
	ScreenRecordImpl(QObject * parent = Q_NULLPTR);
	void Init(const QVariantMap& map);

	private slots :
	void Start();
	void Pause();
	void Stop();

private:
	//��fifobuf��ȡ����Ƶ֡��д������������ã������ļ�
	void MuxThreadProc();
	//����Ƶ��������ȡ֡��д��fifobuf
	void ScreenRecordThreadProc();
	//����Ƶ��������ȡ֡��д��fifobuf
	void SoundRecordThreadProc();
	int OpenVideo();
	int OpenAudio();
	int OpenOutput();
	QString GetSpeakerDeviceName();
	QString GetMicrophoneDeviceName();
	AVFrame* AllocAudioFrame(AVCodecContext* c, int nbSamples);
	void InitVideoBuffer();
	void InitAudioBuffer();
	void FlushVideoDecoder();
	void FlushVideoEncoder();
	void FlushAudioDecoder();
	void FlushAudioEncoder();
	void Release();

private:
	QString				m_filePath;
	int					m_width;
	int					m_height;
	int					m_fps;
	int					m_bitRate;

	int m_vIndex;		//������Ƶ������
	int m_aIndex;		//������Ƶ������
	int m_vOutIndex;	//�����Ƶ������
	int m_aOutIndex;	//�����Ƶ������
	AVFormatContext		*m_vFmtCtx;
	AVFormatContext		*m_aFmtCtx;
	AVFormatContext		*m_oFmtCtx;
	AVCodecContext		*m_vDecodeCtx;
	AVCodecContext		*m_aDecodeCtx;
	AVCodecContext		*m_vEncodeCtx;
	AVCodecContext		*m_aEncodeCtx;
	SwsContext			*m_swsCtx;
	SwrContext			*m_swrCtx;
	AVFifoBuffer		*m_vFifoBuf;
	AVAudioFifo			*m_aFifoBuf;

	AVFrame				*m_vOutFrame;
	uint8_t				*m_vOutFrameBuf;
	int					m_vOutFrameSize;

	int					m_nbSamples;
	RecordState			m_state;
	std::condition_variable m_cvNotPause;	//�������ͣ��ʱ�������ɼ��̹߳���
	std::mutex				m_mtxPause;
	std::condition_variable m_cvVBufNotFull;
	std::condition_variable m_cvVBufNotEmpty;
	std::mutex				m_mtxVBuf;
	std::condition_variable m_cvABufNotFull;
	std::condition_variable m_cvABufNotEmpty;
	std::mutex				m_mtxABuf;
};