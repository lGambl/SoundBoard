#ifndef HOTKEYMANAGER_H
#define HOTKEYMANAGER_H

#include <QObject>
#include <QVector>
#include <QPointer>
#include <QHotkey>

class HotkeyManager : public QObject {
    Q_OBJECT
public:
    explicit HotkeyManager(QObject *parent = nullptr);
    ~HotkeyManager();

    void registerHotkey(int key, int index);
    void unregisterAll();
    void registerStopAllHotkey(int key);

signals:
    void hotkeyPressed(int index);
    void stopAllPressed();

private:
    QVector<QPointer<QHotkey>> hotkeys;
    QPointer<QHotkey> stopAllHotkey;
};

#endif // HOTKEYMANAGER_H
