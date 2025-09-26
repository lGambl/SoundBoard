#include "audiomixer.h"

#include <QAudioOutput>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMediaDevices>
#include <QMediaPlayer>
#include <QUrl>

class AudioMixer::PlaybackInstance : public QObject {
    Q_OBJECT
public:
    PlaybackInstance(const QString &filePath, const QAudioDevice &device, float volume, QObject *parent = nullptr)
        : QObject(parent)
        , m_originalPath(filePath)
        , m_resolvedPath(resolvePath(filePath))
        , m_audioOutput(new QAudioOutput(this))
        , m_player(new QMediaPlayer(this))
    {
        m_audioOutput->setDevice(device);
        m_audioOutput->setVolume(volume);
        m_player->setAudioOutput(m_audioOutput);

        QString errorDetail;
        m_valid = prepareSource(m_resolvedPath, &errorDetail);
        if (!m_valid) {
            qWarning() << "[AudioMixer] Unable to prepare file" << m_resolvedPath << ":" << (errorDetail.isEmpty() ? QStringLiteral("Unknown error") : errorDetail);
            QMetaObject::invokeMethod(this, [this]() { emit finished(this); }, Qt::QueuedConnection);
            return;
        }

        m_player->setSource(m_sourceUrl);

        connect(m_player, &QMediaPlayer::mediaStatusChanged, this, &PlaybackInstance::onStatusChanged);
        connect(m_player, &QMediaPlayer::errorOccurred, this, &PlaybackInstance::onError);
    }

    bool isValid() const { return m_valid; }
    QString filePath() const { return m_originalPath; }

    void start()
    {
        if (!m_valid) {
            qWarning() << "[AudioMixer] Attempted to start invalid playback instance for" << m_originalPath;
            return;
        }

        if (m_player->source() != m_sourceUrl)
            m_player->setSource(m_sourceUrl);

        m_player->setPosition(0);
        m_player->play();
    }

    void stop()
    {
        if (!m_valid)
            return;

        m_player->stop();
        m_player->setPosition(0);
    }

    void setVolume(float volume)
    {
        m_audioOutput->setVolume(volume);
    }

    void setDevice(const QAudioDevice &device)
    {
        m_audioOutput->setDevice(device);
    }

signals:
    void finished(AudioMixer::PlaybackInstance *self);

private slots:
    void onStatusChanged(QMediaPlayer::MediaStatus status)
    {
        if (status == QMediaPlayer::EndOfMedia) {
            emit finished(this);
        } else if (status == QMediaPlayer::InvalidMedia) {
            qWarning() << "Failed to load media" << m_player->source();
            emit finished(this);
        }
    }

    void onError(QMediaPlayer::Error /*error*/, const QString &errorString)
    {
        qWarning() << "Audio playback error" << errorString << "for" << m_player->source();
        emit finished(this);
    }

private:
    static QString resolvePath(const QString &path)
    {
        QFileInfo info(path);
        QString canonical = info.canonicalFilePath();
        if (!canonical.isEmpty())
            return canonical;
        return info.absoluteFilePath();
    }

    bool prepareSource(const QString &absolutePath, QString *errorString)
    {
        QFileInfo info(absolutePath);
        if (!info.exists() || !info.isFile()) {
            if (errorString)
                *errorString = QStringLiteral("File not found");
            return false;
        }

#ifdef Q_OS_WIN
        QString native = QDir::toNativeSeparators(absolutePath);
        if (!native.startsWith(QStringLiteral("\\\\?\\")) && native.length() >= 260)
            native = QStringLiteral("\\\\?\\") + native;
        QFile probe(native);
#else
        QFile probe(absolutePath);
#endif
        if (!probe.open(QIODevice::ReadOnly)) {
            if (errorString)
                *errorString = probe.errorString();
            return false;
        }
        probe.close();

        const QUrl url = QUrl::fromLocalFile(info.absoluteFilePath());
        if (!url.isValid()) {
            if (errorString)
                *errorString = QStringLiteral("Invalid file URL");
            return false;
        }

        m_sourceUrl = url;
        return true;
    }

    QString m_originalPath;
    QString m_resolvedPath;
    QAudioOutput *m_audioOutput;
    QMediaPlayer *m_player;
    QUrl m_sourceUrl;
    bool m_valid = false;
};

AudioMixer &AudioMixer::instance()
{
    static AudioMixer mixer;
    return mixer;
}

AudioMixer::AudioMixer(QObject *parent)
    : QObject(parent)
    , m_audioDevice(QMediaDevices::defaultAudioOutput())
{
}

void AudioMixer::setAudioDevice(const QAudioDevice &device)
{
    m_audioDevice = device;
    cleanupFinishedPlayers();
    qDebug() << "[AudioMixer] Output switched to" << m_audioDevice.description();
    for (auto &entry : m_activePlayers) {
        if (entry)
            entry->setDevice(m_audioDevice);
    }
}

void AudioMixer::play(const QString &filePath, float volume)
{
    cleanupFinishedPlayers();
    qDebug() << "Playing file:" << filePath << "at volume:" << volume << "on device:" << m_audioDevice.description();
    auto *instance = new PlaybackInstance(filePath, m_audioDevice, volume, this);
    if (!instance->isValid()) {
        instance->deleteLater();
        return;
    }
    connect(instance, &PlaybackInstance::finished, this, &AudioMixer::removeFinishedInstance);
    m_activePlayers.push_back(instance);
    instance->start();
}

void AudioMixer::stop(const QString &filePath)
{
    cleanupFinishedPlayers();

    for (auto it = m_activePlayers.begin(); it != m_activePlayers.end();) {
        if (*it && (*it)->filePath() == filePath) {
            (*it)->stop();
            (*it)->deleteLater();
            it = m_activePlayers.erase(it);
        } else {
            ++it;
        }
    }
}

void AudioMixer::setVolume(const QString &filePath, float volume)
{
    cleanupFinishedPlayers();

    for (auto &entry : m_activePlayers) {
        if (entry && entry->filePath() == filePath)
            entry->setVolume(volume);
    }
}

void AudioMixer::stopAll()
{
    for (auto &entry : m_activePlayers) {
        if (entry) {
            entry->stop();
            entry->deleteLater();
        }
    }
    m_activePlayers.clear();
}

void AudioMixer::removeFinishedInstance(PlaybackInstance *instance)
{
    for (auto it = m_activePlayers.begin(); it != m_activePlayers.end(); ++it) {
        if (*it == instance) {
            (*it)->deleteLater();
            m_activePlayers.erase(it);
            break;
        }
    }
}

void AudioMixer::cleanupFinishedPlayers()
{
    for (auto it = m_activePlayers.begin(); it != m_activePlayers.end();) {
        if (!*it)
            it = m_activePlayers.erase(it);
        else
            ++it;
    }
}

#include "audiomixer.moc"