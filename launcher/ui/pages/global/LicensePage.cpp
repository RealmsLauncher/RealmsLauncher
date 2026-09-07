#include "LicensePage.h"
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QMessageBox>
#include <QCoreApplication>
#include "drm/LicenseManager.h"
#include "settings/INISettingsObject.h"
#include "Application.h"   // for global APPLICATION pointer

LicensePage::LicensePage(QWidget* parent) : BasePage(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);

    QLabel* explanation = new QLabel(this);
    explanation->setWordWrap(true);
    explanation->setText(tr(
        "This launcher is whitelisted and intended only for authorized users.\n"
        "If you obtained this launcher without permission, it will not function.\n"
        "\n"
        "Enter your license key below to validate and unlock the launcher.\n"
        "Without a valid key, the application will not start."
    ));
    layout->addWidget(explanation);

    // Key input row
    QHBoxLayout* keyLayout = new QHBoxLayout();
    QLabel* keyLabel = new QLabel(tr("License Key:"), this);
    m_keyEdit = new QLineEdit(this);
    m_keyEdit->setEchoMode(QLineEdit::Password);
    keyLayout->addWidget(keyLabel);
    keyLayout->addWidget(m_keyEdit);
    layout->addLayout(keyLayout);

    // Buttons row
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    m_validateButton = new QPushButton(tr("Validate"), this);
    m_resetButton = new QPushButton(tr("Reset License"), this);
    buttonLayout->addWidget(m_validateButton);
    buttonLayout->addWidget(m_resetButton);
    layout->addLayout(buttonLayout);

    m_statusLabel = new QLabel(this);
    m_statusLabel->setWordWrap(true);
    layout->addWidget(m_statusLabel);

    layout->addStretch();

    connect(m_validateButton, &QPushButton::clicked, this, &LicensePage::onValidateClicked);
    connect(m_resetButton, &QPushButton::clicked, this, &LicensePage::onResetClicked);
}

QIcon LicensePage::icon() const
{
    return QIcon::fromTheme("security");
}

void LicensePage::load()
{
    m_currentKey = m_settings->get("LicenseKey", "").toString();
    m_keyEdit->setText(m_currentKey);
    m_statusLabel->clear();
}

void LicensePage::save()
{
    QString newKey = m_keyEdit->text().trimmed();
    if (newKey != m_currentKey)
    {
        LicenseManager* lm = APPLICATION->licenseManager();
        if (!newKey.isEmpty() && lm->validate(newKey))
        {
            m_settings->set("LicenseKey", newKey);
            m_currentKey = newKey;
            lm->startMonitoring(newKey);
            m_statusLabel->setText(tr("✓ License key is valid and saved."));
        }
        else
        {
            m_statusLabel->setText(tr("✗ Invalid license key. Please try again."));
            // revert to previous key
            m_keyEdit->setText(m_currentKey);
        }
    }
}

void LicensePage::retranslate()
{
    // handled via tr()
}

void LicensePage::onValidateClicked()
{
    QString key = m_keyEdit->text().trimmed();
    LicenseManager* lm = APPLICATION->licenseManager();

    if (key.isEmpty()) {
        m_statusLabel->setText(tr("Please enter a license key."));
        return;
    }

    // Perform server validation
    if (lm->validate(key))
    {
        m_statusLabel->setText(tr("✓ License key is valid. Saving and starting monitoring..."));
        m_settings->set("LicenseKey", key);
        m_currentKey = key;
        lm->startMonitoring(key);
    }
    else
    {
        m_statusLabel->setText(tr("✗ Invalid license key. Please check and try again."));
    }
}

void LicensePage::onResetClicked()
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        tr("Reset License"),
        tr("Are you sure you want to clear the license key?\n"
           "The launcher will close after resetting."),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
    );

    if (reply == QMessageBox::Yes)
    {
        // Clear key from settings
        m_settings->set("LicenseKey", "");
        m_currentKey.clear();
        m_keyEdit->clear();
        m_statusLabel->setText(tr("License key cleared. The launcher will now close."));

        // Stop monitoring
        LicenseManager* lm = APPLICATION->licenseManager();
        if (lm)
            lm->stopMonitoring();

        // Close the application
        QCoreApplication::quit();
    }
}
