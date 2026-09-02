#pragma once

#include <QObject>
#include <QTimer>
#include <QProcess>
#include <QJsonObject>
#include <QSet>

#ifdef Q_OS_WIN32
#ifdef NODE_TEXT
#undef NODE_TEXT
#endif
#ifdef NODE_DOCUMENT
#undef NODE_DOCUMENT
#endif
#include <windows.h>
#endif

class LicenseManager : public QObject {
    Q_OBJECT
public:
    explicit LicenseManager(QObject *parent = nullptr);
    ~LicenseManager();

    void addTargetJavaSession(const QString &sessionId);
    void removeTargetJavaSession(const QString &sessionId);
    void startMonitoring(const QString &licenseKey);
    void stopMonitoring();

private slots:
    void performCheck();
    void onSshFinished(int exitCode, QProcess::ExitStatus status);
    void onSshError(QProcess::ProcessError error);

private:
    void killJavaProcesses();
    void showFatalErrorAndExit(const QString &reason);
    void handleInvalidLicense(const QString &reason);
    QString writeTempPrivateKey();

#ifdef Q_OS_WIN32
    QString getProcessCommandLine(DWORD pid);
#endif

    QTimer *m_timer = nullptr;
    QProcess *m_sshProcess = nullptr;
    QString m_licenseKey;
    QString m_deviceId;
    QString m_deviceName;
    QSet<QString> m_targetSessionIds;
    int m_failureCount = 0;
    const int MAX_FAILURES = 3;
    bool m_isRunning = false;
};
