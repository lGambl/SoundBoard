#include "hotkeymanager.h"
#include <QKeySequence>

HotkeyManager::HotkeyManager(QObject *parent) : QObject(parent) {}

HotkeyManager::~HotkeyManager() {
    unregisterAll();
}

void HotkeyManager::registerHotkey(int key, int index) {
    if (key == 0) return;
    auto hotkey = new QHotkey(QKeySequence(key), true, this);
    connect(hotkey, &QHotkey::activated, this, [this, index]() {
        emit hotkeyPressed(index);
    });
    hotkeys.append(hotkey);
}

void HotkeyManager::registerStopAllHotkey(int key) {
    if (key == 0) return;
    stopAllHotkey = new QHotkey(QKeySequence(key), true, this);
    connect(stopAllHotkey, &QHotkey::activated, this, [this]() {
        emit stopAllPressed();
    });
}

void HotkeyManager::unregisterAll() {
    for (auto hk : hotkeys) {
        if (hk) hk->deleteLater();
    }
    hotkeys.clear();
    if (stopAllHotkey) {
        stopAllHotkey->deleteLater();
        stopAllHotkey = nullptr;
    }
}
