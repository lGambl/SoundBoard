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
    lay->setContentsMargins(5,2,5,2);

    QLabel *lbl = new QLabel(fileName, this);

    // Play button with standard media icon
    QToolButton *play = new QToolButton(this);
    play->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    play->setIconSize(QSize(20,20));
    play->setAutoRaise(true);
    play->setToolTip(tr("Play"));

    // Delete button with standard trash icon
    QToolButton *del = new QToolButton(this);
    del->setIcon(style()->standardIcon(QStyle::SP_TrashIcon));
    del->setIconSize(QSize(20,20));
    del->setAutoRaise(true);
    del->setToolTip(tr("Delete"));

    lay->addWidget(lbl);
    lay->addStretch();
    lay->addWidget(play);
    lay->addWidget(del);

    m_audioOutput = new QAudioOutput(this);
    m_audioOutput->setDevice(SoundItemWidget::audioDevice());
    m_player = new QMediaPlayer(this);
    m_player->setAudioOutput(m_audioOutput);

    connect(play, &QToolButton::clicked, this, [this]() {
        m_player->setSource(QUrl::fromLocalFile(m_fileName));
        m_player->play();
        emit playRequested();
    });
    connect(del, &QToolButton::clicked, this, &SoundItemWidget::deleteRequested);
}
