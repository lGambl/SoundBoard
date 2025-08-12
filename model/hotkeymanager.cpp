#include "hotkeymanager.h"
#include <QKeySequence>

HotkeyManager::HotkeyManager(QObject *parent) : QObject(parent) {}

HotkeyManager::~HotkeyManager() {
    unregisterAll();
}

void HotkeyManager::registerHotkey(int key, int index) {
    if (key == 0) return;
    auto hotkey = new QHotkey(QKeySequence(key), true, this);
    if (!hotkey) {
        qWarning("[HotkeyManager] QHotkey allocation failed for key %d", key);
        return;
    }
    if (!hotkey->isRegistered()) {
        qWarning("[HotkeyManager] QHotkey not registered for key %d", key);
        hotkey->deleteLater();
        return;
    }
    if (!connect(hotkey, &QHotkey::activated, this, [this, index]() {
        emit hotkeyPressed(index);
    })) {
        qWarning("[HotkeyManager] Failed to connect QHotkey activated signal for key %d", key);
        hotkey->deleteLater();
        return;
    }
    hotkeys.append(hotkey);
}

void HotkeyManager::registerStopAllHotkey(int key) {
    if (key == 0) return;
    if (stopAllHotkey) {
        stopAllHotkey->deleteLater();
        stopAllHotkey = nullptr;
    }
    stopAllHotkey = new QHotkey(QKeySequence(key), true, this);
    if (!stopAllHotkey) {
        qWarning("[HotkeyManager] stopAll QHotkey allocation failed for key %d", key);
        return;
    }
    if (!stopAllHotkey->isRegistered()) {
        qWarning("[HotkeyManager] stopAll QHotkey not registered for key %d", key);
        stopAllHotkey->deleteLater();
        stopAllHotkey = nullptr;
        return;
    }
    if (!connect(stopAllHotkey, &QHotkey::activated, this, [this]() {
        emit stopAllPressed();
    })) {
        qWarning("[HotkeyManager] Failed to connect stopAll QHotkey activated signal for key %d", key);
        stopAllHotkey->deleteLater();
        stopAllHotkey = nullptr;
        return;
    }
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
