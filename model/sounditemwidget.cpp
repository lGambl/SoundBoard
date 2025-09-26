#include "sounditemwidget.h"
#include "audiomixer.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QStyle>
#include <QSize>
#include <algorithm>

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

    QToolButton *keyBind = new QToolButton(this);
    keyBind->setIcon(style()->standardIcon(QStyle::SP_DialogYesButton));
    keyBind->setIconSize(QSize(20, 20));
    keyBind->setAutoRaise(true);
    keyBind->setToolTip(tr("Set Key"));
    keyBind->setText("Set Key");

    lay->addWidget(lbl);
    lay->addStretch();
    lay->addWidget(play);
    lay->addWidget(stop);
    lay->addWidget(del);
    lay->addWidget(keyBind);

    setLayout(lay);
    setMinimumSize(300, 50);
    adjustSize();

    connect(play, &QToolButton::clicked, this, [this]() {
        AudioMixer::instance().play(m_fileName, m_volume / 100.0f);
        emit playRequested();
    });

    connect(stop, &QToolButton::clicked, this, &SoundItemWidget::stop);
    connect(del, &QToolButton::clicked, this, &SoundItemWidget::deleteRequested);
    connect(keyBind, &QToolButton::clicked, this, &SoundItemWidget::keyBindRequested);
}

void SoundItemWidget::setVolume(float volume)
{
    m_volume = std::clamp(volume, 0.0f, 100.0f);
    AudioMixer::instance().setVolume(m_fileName, m_volume / 100.0f);
}

void SoundItemWidget::stop()
{
    AudioMixer::instance().stop(m_fileName);
}

void SoundItemWidget::play()
{
    AudioMixer::instance().play(m_fileName, m_volume / 100.0f);
    emit playRequested();
}

void SoundItemWidget::setAudioDevice(const QAudioDevice &device)
{
    AudioMixer::instance().setAudioDevice(device);
}

QAudioDevice SoundItemWidget::audioDevice()
{
    return AudioMixer::instance().audioDevice();
}
