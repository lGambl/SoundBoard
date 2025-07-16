#ifndef KEYBINDDIALOG_H
#define KEYBINDDIALOG_H

#include <QDialog>

class QLabel;
class QPushButton;

class KeyBindDialog : public QDialog {
    Q_OBJECT
public:
    explicit KeyBindDialog(QWidget *parent = nullptr);
    int getKey() const { return m_key; }

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void startListening();

private:
    QLabel *label;
    QPushButton *setKeyButton;
    int m_key = 0;
    bool listening = false;
};

#endif // KEYBINDDIALOG_H
