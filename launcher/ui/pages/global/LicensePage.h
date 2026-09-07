#pragma once

#include <QWidget>
#include "ui/pages/BasePage.h"

class QLineEdit;
class QLabel;
class QPushButton;

class LicensePage : public QWidget, public BasePage
{
public:
    explicit LicensePage(QWidget* parent = nullptr);

    QString displayName() const override { return tr("License"); }
    QIcon icon() const override;
    QString id() const override { return "license"; }

    void load();
    void save();
    void retranslate() override;

private:
    void onValidateClicked();
    void onResetClicked();

    QLineEdit* m_keyEdit;
    QLabel* m_statusLabel;
    QPushButton* m_validateButton;
    QPushButton* m_resetButton;
    QString m_currentKey;
};
