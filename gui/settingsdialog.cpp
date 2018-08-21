#include <iostream>
#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    // Set settings-file path
    c_settingsPath = QCoreApplication::applicationDirPath() + "/../Resources/settings";

    m_backupPath = ui->lineEdit;
    m_backupPath->setEnabled(false);

    m_cleanAfterCopy = ui->cleanCheckBox;

    updateSettingsGui();


    connect(ui->pushButton, &QPushButton::clicked, this, &SettingsDialog::onSetBackUpPathClicked);

}

SettingsDialog::~SettingsDialog() {
    delete ui;
}

QString SettingsDialog::getBackupPath() {
    return m_backupPath->text();;
}

bool SettingsDialog::getCleanAfterCopy() {
    return m_cleanAfterCopy->isChecked();
}

void SettingsDialog::onSetBackUpPathClicked() {
    auto dirName = QFileDialog::getExistingDirectory(this, "Select folder..", "/",
                                                     QFileDialog::ShowDirsOnly);
    if (!dirName.isNull()) {
        m_backupPath->setText(dirName);
    }
}

void SettingsDialog::updateSettingsGui() {
    QFile file(c_settingsPath);
    file.open(QIODevice::ReadOnly);
    QDataStream in(&file);    // read the data serialized from the file
    QString str;
    bool cleanCheckBox;
    in >> str;
    in >> cleanCheckBox;
    m_backupPath->setText(str);
    m_cleanAfterCopy->setChecked(cleanCheckBox);
}

void SettingsDialog::updateSettings() {
    QFile file(c_settingsPath);
    file.open(QIODevice::WriteOnly);
    QDataStream out(&file);   // we will serialize the data into the file
    out << m_backupPath->text();
    out << m_cleanAfterCopy->isChecked();
}

int SettingsDialog::exec() {
    updateSettingsGui();


    auto res = QDialog::exec();
    if (res) {
        updateSettings();
    }

    return res;
}

