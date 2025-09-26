#ifndef SOUNDITEMWIDGET_H
#define SOUNDITEMWIDGET_H

#include <QWidget>
#include <QAudioDevice>
class QLabel;
class QPushButton;
class QMainWindow;

class SoundItemWidget : public QWidget {
    Q_OBJECT
public:
    explicit SoundItemWidget(QString filePath, QString displayName, QWidget *parent);

    static void setAudioDevice(const QAudioDevice &device);
    static QAudioDevice audioDevice();

signals:
    void playRequested();
    void deleteRequested();
    void editRequested();
    void keyBindRequested();

public slots:
    void stop();
    void setVolume(float volume);
    void play();

private:
    QString m_fileName;
    QString m_displayName;
    float m_volume = 100.0f;
};

#endif // SOUNDITEMWIDGET_H
