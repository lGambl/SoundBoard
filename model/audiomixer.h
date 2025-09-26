#ifndef AUDIOMIXER_H
#define AUDIOMIXER_H

#include <QAudioDevice>
#include <QObject>
#include <QPointer>
#include <QString>
#include <QVector>

class AudioMixer : public QObject {
    Q_OBJECT
public:
    static AudioMixer &instance();

    void setAudioDevice(const QAudioDevice &device);
    QAudioDevice audioDevice() const { return m_audioDevice; }

    void play(const QString &filePath, float volume);
    void setVolume(const QString &filePath, float volume);
    void stop(const QString &filePath);
    void stopAll();

private:
    class PlaybackInstance;

    explicit AudioMixer(QObject *parent = nullptr);

    void removeFinishedInstance(PlaybackInstance *instance);
    void cleanupFinishedPlayers();

    QVector<QPointer<PlaybackInstance>> m_activePlayers;
    QAudioDevice m_audioDevice;
};

#endif // AUDIOMIXER_H
