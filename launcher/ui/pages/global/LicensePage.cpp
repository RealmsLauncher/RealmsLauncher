#include "LicensePage.h"
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QMessageBox>
#include "drm/LicenseManager.h"
#include "settings/INISettingsObject.h"

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

    QHBoxLayout* keyLayout = new QHBoxLayout();
    QLabel* keyLabel = new QLabel(tr("License Key:"), this);
    m_keyEdit = new QLineEdit(this);
    m_keyEdit->setEchoMode(QLineEdit::Password);
    m_validateButton = new QPushButton(tr("Validate"), this);
    keyLayout->addWidget(keyLabel);
    keyLayout->addWidget(m_keyEdit);
    keyLayout->addWidget(m_validateButton);
    layout->addLayout(keyLayout);

    m_statusLabel = new QLabel(this);
    m_statusLabel->setWordWrap(true);
    layout->addWidget(m_statusLabel);

    layout->addStretch();

    connect(m_validateButton, &QPushButton::clicked, this, &LicensePage::onValidateClicked);
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
        if (LicenseManager::validate(newKey))
        {
            m_settings->set("LicenseKey", newKey);
            m_currentKey = newKey;
            m_statusLabel->setText(tr("✓ License key is valid and saved."));
        }
        else
        {
            m_statusLabel->setText(tr("✗ Invalid license key. Please try again."));
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
    if (LicenseManager::validate(key))
    {
        m_statusLabel->setText(tr("✓ License key is valid."));
        m_settings->set("LicenseKey", key);
        m_currentKey = key;
    }
    else
    {
        m_statusLabel->setText(tr("✗ Invalid license key. Please check and try again."));
    }
}
