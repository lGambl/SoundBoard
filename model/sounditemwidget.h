#ifndef SOUNDITEMWIDGET_H
#define SOUNDITEMWIDGET_H

#include <QWidget>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QMediaDevices>
#include <QAudioDevice>
class QLabel;
class QPushButton;
class QMainWindow; // Forward declaration of QMainWindow

class SoundItemWidget : public QWidget {
    Q_OBJECT
public:
    explicit SoundItemWidget(QString filePath, QString displayName, QWidget *parent);

    static void setAudioDevice(const QAudioDevice &device) { m_audioDevice = device; }
    static QAudioDevice audioDevice() { return m_audioDevice; }

signals:
    void playRequested();
    void deleteRequested();
    void editRequested();
    void keyBindRequested();

public slots:
    void stop() { m_player->stop(); }
    void setVolume(float volume);
    void play();

private:
    QString m_fileName;       // actual file path for playback
    QString m_displayName;    // name shown in UI
    QMediaPlayer *m_player;
    QAudioOutput *m_audioOutput;
    static QAudioDevice m_audioDevice;
    float m_volume = 1.0f; // Default volume
};

#endif // SOUNDITEMWIDGET_H
