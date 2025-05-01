#include "sounditemwidget.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QUrl>
#include <QMediaDevices>

// static audio device initialization
typedef QAudioDevice AudioDeviceAlias; // dummy to avoid type issues
QAudioDevice SoundItemWidget::m_audioDevice = QMediaDevices::defaultAudioOutput();

SoundItemWidget::SoundItemWidget(const QString &fileName, QWidget *parent)
    : QWidget(parent)
    , m_fileName(fileName)
{
    auto *lay = new QHBoxLayout(this);
    lay->setContentsMargins(5,2,5,2);
    QLabel *lbl = new QLabel(fileName, this);
    QPushButton *play = new QPushButton("▶", this);
    QPushButton *del  = new QPushButton("✖", this);
    lay->addWidget(lbl);
    lay->addStretch();
    lay->addWidget(play);
    lay->addWidget(del);

    // set up media player/output
    m_audioOutput = new QAudioOutput(this);
    m_audioOutput->setDevice(SoundItemWidget::audioDevice());
    m_player = new QMediaPlayer(this);
    m_player->setAudioOutput(m_audioOutput);

    // on-click: load & play, then notify
    connect(play, &QPushButton::clicked, this, [this]() {
        m_player->setSource(QUrl::fromLocalFile(m_fileName));
        m_player->play();
        emit playRequested();
    });

    connect(del, &QPushButton::clicked, this, &SoundItemWidget::deleteRequested);
}
