#ifndef SOUNDITEMWIDGET_H
#define SOUNDITEMWIDGET_H

#include <QWidget>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QMediaDevices>
#include <QAudioDevice>
class QLabel;
class QPushButton;

class SoundItemWidget : public QWidget {
    Q_OBJECT
public:
    explicit SoundItemWidget(const QString &fileName, QWidget *parent = nullptr);

    static void setAudioDevice(const QAudioDevice &device) { m_audioDevice = device; }
    static QAudioDevice audioDevice() { return m_audioDevice; }

signals:
    void playRequested();
    void deleteRequested();

public slots:
    void stop() { m_player->stop(); }
    void setVolume(float volume);        // ← new

private:
    QString m_fileName;
    QMediaPlayer *m_player;
    QAudioOutput *m_audioOutput;
    static QAudioDevice m_audioDevice;
    float m_volume = 1.0f; // Default volume
};

#endif // SOUNDITEMWIDGET_H
