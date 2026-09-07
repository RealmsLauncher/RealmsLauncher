#include "LicenseManager.h"
#include "DeviceId.h"
#include <QApplication>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QTemporaryFile>
#include <QFile>
#include <QProcess>

#ifdef Q_OS_WIN32
#include <windows.h>
#include <tlhelp32.h>
#include <winternl.h>
#endif

// =============================================================
// PRIVATE KEY (Ed25519) – LOCKED TO CHECK ONLY, NOT PLACEHOLDER
// =============================================================
static const char *EMBEDDED_PRIVATE_KEY =
    "-----BEGIN OPENSSH PRIVATE KEY-----\n"
    "b3BlbnNzaC1rZXktdjEAAAAABG5vbmUAAAAEbm9uZQAAAAAAAAABAAAAMwAAAAtzc2gtZW\n"
    "QyNTUxOQAAACAvPh7HCztDhT2ddfZp5EUuClC68kJ7q/gYYUG3r7A3wAAAAKBxkVhbcZFY\n"
    "WwAAAAtzc2gtZWQyNTUxOQAAACAvPh7HCztDhT2ddfZp5EUuClC68kJ7q/gYYUG3r7A3wA\n"
    "AAAEApsV9MSCzOFxZo7ddFcnrGUA7YGintNhvBKcPrZmSA7y8+HscLO0OFPZ119mnkRS4K\n"
    "ULryQnur+BhhQbevsDfAAAAAG2Rwc1wyMTE4NjAzMUBXSU4tMUg4NTI1MzMyQwEC\n"
    "-----END OPENSSH PRIVATE KEY-----\n";

LicenseManager::LicenseManager(QObject *parent)
    : QObject(parent), m_timer(new QTimer(this)), m_sshProcess(nullptr),
      m_deviceId(DeviceId::get()), m_deviceName(DeviceId::getDeviceName()),
      m_failureCount(0), m_isRunning(false)
{
    connect(m_timer, &QTimer::timeout, this, &LicenseManager::performCheck);
}

LicenseManager::~LicenseManager()
{
    stopMonitoring();
}

void LicenseManager::addTargetJavaSession(const QString &sessionId)
{
    if (!sessionId.isEmpty()) {
        m_targetSessionIds.insert(sessionId);
        qDebug() << "Added session:" << sessionId << "Total sessions:" << m_targetSessionIds.size();
    }
}

void LicenseManager::removeTargetJavaSession(const QString &sessionId)
{
    if (!sessionId.isEmpty() && m_targetSessionIds.remove(sessionId)) {
        qDebug() << "Removed session:" << sessionId << "Remaining sessions:" << m_targetSessionIds.size();
    }
}

void LicenseManager::startMonitoring(const QString &licenseKey)
{
    if (m_isRunning) return;
    m_licenseKey = licenseKey;
    m_failureCount = 0;
    m_isRunning = true;
    performCheck();
    m_timer->start(12000);
}

void LicenseManager::stopMonitoring()
{
    m_isRunning = false;
    m_timer->stop();
    if (m_sshProcess) {
        if (m_sshProcess->state() == QProcess::Running)
            m_sshProcess->kill();
        m_sshProcess->deleteLater();
        m_sshProcess = nullptr;
    }
}

void LicenseManager::performCheck()
{
    if (!m_isRunning) return;
    if (m_sshProcess && m_sshProcess->state() == QProcess::Running)
        return;

    QString keyPath = writeTempPrivateKey();
    if (keyPath.isEmpty()) {
        m_failureCount++;
        if (m_failureCount >= MAX_FAILURES) {
            m_timer->stop();
            handleInvalidLicense("Failed to write SSH key");
        }
        return;
    }

    QString sshCommand = QString("check %1 %2 %3")
        .arg(m_licenseKey)
        .arg(m_deviceId)
        .arg(m_deviceName);

    QStringList sshArgs;
    sshArgs << "-o" << "ConnectTimeout=5"
            << "-o" << "StrictHostKeyChecking=accept-new"
            << "-i" << keyPath
            << "realmsnetwork@ssh-realmsnetwork.alwaysdata.net"
            << sshCommand;

    m_sshProcess = new QProcess(this);
    connect(m_sshProcess, &QProcess::finished, this, [this, keyPath](int exitCode, QProcess::ExitStatus status) {
        QFile::remove(keyPath);
        onSshFinished(exitCode, status);
    });
    connect(m_sshProcess, &QProcess::errorOccurred, this, &LicenseManager::onSshError);
    m_sshProcess->start("ssh", sshArgs);
}

QString LicenseManager::writeTempPrivateKey()
{
    QTemporaryFile tempFile;
    tempFile.setAutoRemove(false);
    if (!tempFile.open()) {
        qCritical() << "Cannot create temp file for SSH key";
        return {};
    }
    tempFile.write(EMBEDDED_PRIVATE_KEY);
    tempFile.flush();
    QString path = tempFile.fileName();
    tempFile.close();
    return path;
}

void LicenseManager::onSshFinished(int exitCode, QProcess::ExitStatus status)
{
    if (!m_isRunning) return;

    QByteArray output = m_sshProcess->readAllStandardOutput();
    QByteArray error = m_sshProcess->readAllStandardError();
    m_sshProcess->deleteLater();
    m_sshProcess = nullptr;

    if (exitCode != 0 || status != QProcess::NormalExit) {
        qWarning() << "SSH failed:" << exitCode << error;
        m_failureCount++;
        if (m_failureCount >= MAX_FAILURES) {
            m_timer->stop();
            handleInvalidLicense("SSH connection failed repeatedly");
        }
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(output);
    if (doc.isNull()) {
        qWarning() << "Invalid JSON from server:" << output;
        m_failureCount++;
        if (m_failureCount >= MAX_FAILURES) {
            m_timer->stop();
            handleInvalidLicense("Invalid server response");
        }
        return;
    }

    QJsonObject obj = doc.object();
    QString statusStr = obj.value("status").toString();
    QString reason = obj.value("reason").toString();

    if (statusStr == "OK") {
        m_failureCount = 0;
        qDebug() << "License valid. Owner:" << obj.value("owner").toString();
    } else {
        qWarning() << "License invalid:" << reason;
        m_failureCount++;
        if (m_failureCount >= MAX_FAILURES) {
            m_timer->stop();
            handleInvalidLicense(reason);
        }
    }
}

void LicenseManager::onSshError(QProcess::ProcessError error)
{
    Q_UNUSED(error);
    if (!m_isRunning) return;
    qWarning() << "SSH process error.";
    m_failureCount++;
    if (m_failureCount >= MAX_FAILURES) {
        m_timer->stop();
        handleInvalidLicense("SSH process crashed");
    }
}

#ifdef Q_OS_WIN32
QString LicenseManager::getProcessCommandLine(DWORD pid)
{
    QString result;
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (!hProcess) return result;

    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    if (!ntdll) { CloseHandle(hProcess); return result; }

    typedef NTSTATUS (NTAPI *NtQueryInfoProc)(HANDLE, PROCESSINFOCLASS, PVOID, ULONG, PULONG);
    auto NtQueryInformationProcess = (NtQueryInfoProc)GetProcAddress(ntdll, "NtQueryInformationProcess");
    if (!NtQueryInformationProcess) { CloseHandle(hProcess); return result; }

    PROCESS_BASIC_INFORMATION pbi = {};
    ULONG len = 0;
    NTSTATUS status = NtQueryInformationProcess(hProcess, ProcessBasicInformation, &pbi, sizeof(pbi), &len);
    if (status != 0 || !pbi.PebBaseAddress) { CloseHandle(hProcess); return result; }

    PEB peb = {};
    if (!ReadProcessMemory(hProcess, pbi.PebBaseAddress, &peb, sizeof(peb), nullptr)) {
        CloseHandle(hProcess);
        return result;
    }

    RTL_USER_PROCESS_PARAMETERS params = {};
    if (!ReadProcessMemory(hProcess, peb.ProcessParameters, &params, sizeof(params), nullptr)) {
        CloseHandle(hProcess);
        return result;
    }

    wchar_t *cmdLine = new wchar_t[params.CommandLine.Length / sizeof(wchar_t) + 1];
    if (!ReadProcessMemory(hProcess, params.CommandLine.Buffer, cmdLine,
                           params.CommandLine.Length, nullptr)) {
        delete[] cmdLine;
        CloseHandle(hProcess);
        return result;
    }
    cmdLine[params.CommandLine.Length / sizeof(wchar_t)] = L'\0';
    result = QString::fromWCharArray(cmdLine);
    delete[] cmdLine;
    CloseHandle(hProcess);
    return result;
}
#endif

void LicenseManager::killJavaProcesses()
{
    if (m_targetSessionIds.isEmpty()) {
        qWarning() << "No target sessions – cannot kill Java.";
        return;
    }

#ifdef Q_OS_WIN32
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) return;

    PROCESSENTRY32W pe = {};
    pe.dwSize = sizeof(PROCESSENTRY32W);

    if (Process32FirstW(snapshot, &pe)) {
        do {
            QString exe = QString::fromWCharArray(pe.szExeFile);
            if (exe.compare("javaw.exe", Qt::CaseInsensitive) != 0 &&
                exe.compare("java.exe", Qt::CaseInsensitive) != 0) {
                continue;
            }

            QString cmdLine = getProcessCommandLine(pe.th32ProcessID);
            if (cmdLine.isEmpty()) continue;

            int idx = cmdLine.indexOf("--session");
            if (idx == -1) continue;

            int start = cmdLine.indexOf(' ', idx + 10);
            while (start < cmdLine.size() && cmdLine.at(start) == ' ') start++;
            int end = cmdLine.indexOf(' ', start);
            if (end == -1) end = cmdLine.size();

            QString foundSession = cmdLine.mid(start, end - start);

            if (m_targetSessionIds.contains(foundSession)) {
                HANDLE hProc = OpenProcess(PROCESS_TERMINATE, FALSE, pe.th32ProcessID);
                if (hProc) {
                    qDebug() << "Killing Java process" << pe.th32ProcessID << "with session" << foundSession;
                    TerminateProcess(hProc, 1);
                    CloseHandle(hProc);
                }
            }
        } while (Process32NextW(snapshot, &pe));
    }
    CloseHandle(snapshot);

#else
    for (const QString &session : m_targetSessionIds) {
        QProcess pgrep;
        pgrep.start("pgrep", {"-f", "java.*--session " + session});
        if (!pgrep.waitForFinished(3000)) {
            qWarning() << "pgrep timed out for session" << session;
            continue;
        }
        if (pgrep.exitCode() != 0) continue;
        QByteArray output = pgrep.readAllStandardOutput();
        QList<QByteArray> lines = output.split('\n');
        for (const QByteArray &line : lines) {
            if (line.isEmpty()) continue;
            bool ok;
            int pid = line.trimmed().toInt(&ok);
            if (ok && pid > 0) {
                QProcess killProc;
                killProc.start("kill", {"-9", QString::number(pid)});
                killProc.waitForFinished(1000);
                if (killProc.exitCode() == 0) {
                    qDebug() << "Killed Java process" << pid << "with session" << session;
                } else {
                    qWarning() << "Failed to kill PID" << pid;
                }
            }
        }
    }
#endif
}

void LicenseManager::handleInvalidLicense(const QString &reason)
{
    killJavaProcesses();
    showFatalErrorAndExit(reason);
}

void LicenseManager::showFatalErrorAndExit(const QString &reason)
{
    stopMonitoring();

    QString title = "Realms Launcher";
    QString message;
    if (reason.contains("REVOKED", Qt::CaseInsensitive))
        message = "License has been revoked.";
    else if (reason.contains("EXPIRED", Qt::CaseInsensitive))
        message = "License has expired.";
    else if (reason.contains("DEVICE_MISMATCH", Qt::CaseInsensitive))
        message = "Device mismatch. Please contact support.";
    else
        message = "License is invalid or unreachable.";

#ifdef Q_OS_WIN32
    MessageBoxW(nullptr,
                message.toStdWString().c_str(),
                title.toStdWString().c_str(),
                MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);
#else
    QMessageBox::critical(nullptr, title, message, QMessageBox::Ok);
#endif

    QCoreApplication::quit();
    exit(1);
}
