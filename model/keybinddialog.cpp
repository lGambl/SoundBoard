#include "keybinddialog.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QKeyEvent>

KeyBindDialog::KeyBindDialog(QWidget *parent)
    : QDialog(parent), m_key(0), listening(false)
{
    setWindowTitle("Set Key Binding");
    auto *layout = new QVBoxLayout(this);
    label = new QLabel("Click 'Set Key' and then press a key.", this);
    setKeyButton = new QPushButton("Set Key", this);
    layout->addWidget(label);
    layout->addWidget(setKeyButton);
    connect(setKeyButton, &QPushButton::clicked, this, &KeyBindDialog::startListening);
    setLayout(layout);
}

void KeyBindDialog::startListening() {
    label->setText("Press any key...");
    listening = true;
    setKeyButton->setEnabled(false);
    setFocus(Qt::OtherFocusReason);
}

void KeyBindDialog::keyPressEvent(QKeyEvent *event) {
    if (listening) {
        m_key = event->key();
        label->setText(QString("Key set: %1").arg(QKeySequence(m_key).toString()));
        listening = false;
        setKeyButton->setEnabled(true);
        accept();
    } else {
        QDialog::keyPressEvent(event);
    }
}
