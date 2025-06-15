#include "sounditemwidget.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QStyle>
#include <QSize>
#include <QUrl>
#include <QMediaDevices>

typedef QAudioDevice AudioDeviceAlias;
QAudioDevice SoundItemWidget::m_audioDevice = QMediaDevices::defaultAudioOutput();

SoundItemWidget::SoundItemWidget(const QString &fileName, QWidget *parent)
    : QWidget(parent)
    , m_fileName(fileName)
{
    auto *lay = new QHBoxLayout(this);
    lay->setContentsMargins(5, 2, 5, 2);

    QLabel *lbl = new QLabel(fileName, this);

    QToolButton *play = new QToolButton(this);
    play->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    play->setIconSize(QSize(20, 20));
    play->setAutoRaise(true);
    play->setToolTip(tr("Play"));

    QToolButton *stop = new QToolButton(this);
    stop->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    stop->setIconSize(QSize(20, 20));
    stop->setAutoRaise(true);
    stop->setToolTip(tr("Stop"));

    QToolButton *del = new QToolButton(this);
    del->setIcon(style()->standardIcon(QStyle::SP_TrashIcon));
    del->setIconSize(QSize(20, 20));
    del->setAutoRaise(true);
    del->setToolTip(tr("Delete"));

    lay->addWidget(lbl);
    lay->addStretch();
    lay->addWidget(play);
    lay->addWidget(stop);
    lay->addWidget(del);

    m_audioOutput = new QAudioOutput(this);
    m_audioOutput->setDevice(SoundItemWidget::audioDevice());

    m_player = new QMediaPlayer(this);
    m_player->setAudioOutput(m_audioOutput);
    m_player->setSource(QUrl::fromLocalFile(m_fileName));

    connect(m_player, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::InvalidMedia) {
            qWarning("Failed to load media: %s", qUtf8Printable(m_fileName));
        }
    });

    connect(play, &QToolButton::clicked, this, [this]() {
        m_player->stop();
        m_player->setPosition(0);
        m_player->play();
        emit playRequested();
    });

    connect(stop, &QToolButton::clicked, this, &SoundItemWidget::stop);

    connect(del, &QToolButton::clicked, this, &SoundItemWidget::deleteRequested);
}

void SoundItemWidget::setVolume(float volume) {
    m_volume = volume;
    if (m_audioOutput)
        m_audioOutput->setVolume(m_volume);
}
