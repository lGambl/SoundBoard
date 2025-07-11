#include "sounditemwidget.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QStyle>
#include <QSize>
#include <QUrl>
#include <QMediaDevices>
#include <QDebug>

typedef QAudioDevice AudioDeviceAlias;
QAudioDevice SoundItemWidget::m_audioDevice = QMediaDevices::defaultAudioOutput();

SoundItemWidget::SoundItemWidget(QString filePath, QString displayName, QWidget *parent)
    : QWidget(parent)
    , m_fileName(filePath)
    , m_displayName(displayName)
{
    auto *lay = new QHBoxLayout(this);
    lay->setSpacing(10);
    lay->setAlignment(Qt::AlignCenter);

    QLabel *lbl = new QLabel(m_displayName, this);
    lbl->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

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

    setLayout(lay);
    setMinimumSize(300, 50);
    adjustSize();

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
