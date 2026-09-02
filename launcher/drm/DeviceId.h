#pragma once

#include <QString>
#include <QUuid>
#include <QSettings>
#include <QNetworkInterface>
#include <QCryptographicHash>
#include <QSysInfo>
#include <QFile>
#include <QIODevice>

#ifdef Q_OS_WIN32
#include <windows.h>
#endif

#ifdef Q_OS_MACOS
#include <IOKit/IOKitLib.h>
#include <CoreFoundation/CoreFoundation.h>
#endif

class DeviceId {
public:
    static QString get() {
        QSettings settings("RealmsLauncher", "Device");
        QString id = settings.value("device_id").toString();

        if (!id.isEmpty() && id.length() >= 8)
            return id;

#ifdef Q_OS_WIN32
        QSettings reg(
            "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Cryptography",
            QSettings::NativeFormat
        );

        id = reg.value("MachineGuid").toString();

        if (!id.isEmpty()) {
            settings.setValue("device_id", id);
            return id;
        }
#endif

#ifdef Q_OS_MACOS
        io_service_t platformExpert =
            IOServiceGetMatchingService(
                kIOMainPortDefault,
                IOServiceMatching("IOPlatformExpertDevice")
            );

        if (platformExpert) {
            CFTypeRef uuid =
                IORegistryEntryCreateCFProperty(
                    platformExpert,
                    CFSTR("IOPlatformUUID"),
                    kCFAllocatorDefault,
                    0
                );

            if (uuid && CFGetTypeID(uuid) == CFStringGetTypeID()) {
                char buffer[256] = {};

                if (CFStringGetCString(
                        static_cast<CFStringRef>(uuid),
                        buffer,
                        sizeof(buffer),
                        kCFStringEncodingUTF8
                    )) {

                    id = QString::fromUtf8(buffer).trimmed();

                    CFRelease(uuid);
                    IOObjectRelease(platformExpert);

                    if (!id.isEmpty()) {
                        settings.setValue("device_id", id);
                        return id;
                    }
                } else {
                    CFRelease(uuid);
                }
            }

            IOObjectRelease(platformExpert);
        }
#endif

#ifdef Q_OS_LINUX
        QFile machineId("/etc/machine-id");

        if (machineId.open(QIODevice::ReadOnly)) {
            id = QString::fromUtf8(machineId.readAll()).trimmed();

            if (!id.isEmpty()) {
                settings.setValue("device_id", id);
                return id;
            }
        }
#endif

        // Fallback: combine available network interface information.
        QString mac;

        for (const QNetworkInterface &iface :
             QNetworkInterface::allInterfaces()) {

            if ((iface.flags() & QNetworkInterface::IsUp) &&
                !(iface.flags() & QNetworkInterface::IsLoopBack)) {

                mac = iface.hardwareAddress();

                if (!mac.isEmpty())
                    break;
            }
        }

        if (!mac.isEmpty()) {
            id = QCryptographicHash::hash(
                (QSysInfo::machineUniqueId().toHex() + mac.toUtf8()),
                QCryptographicHash::Sha256
            ).toHex().left(32);

            settings.setValue("device_id", id);
            return id;
        }

        // Ultimate fallback: random UUID.
        id = QUuid::createUuid()
                 .toString(QUuid::WithoutBraces)
                 .remove('-');

        settings.setValue("device_id", id);
        return id;
    }

    static QString getDeviceName() {
#ifdef Q_OS_WIN32
        wchar_t buffer[MAX_COMPUTERNAME_LENGTH + 1];
        DWORD size = sizeof(buffer) / sizeof(wchar_t);

        if (GetComputerNameW(buffer, &size))
            return QString::fromWCharArray(buffer);
#endif

        return QSysInfo::machineHostName();
    }
};
