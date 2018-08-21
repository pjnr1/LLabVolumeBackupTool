
#ifndef LLABVOLUMEUPBACKER_SETTINGSDIALOG_H
#define LLABVOLUMEUPBACKER_SETTINGSDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QFileDialog>
#include <QCheckBox>


namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();

    QString getBackupPath();
    bool getCleanAfterCopy();

    int exec() override;

private:
    Ui::SettingsDialog *ui;

    QLineEdit* m_backupPath;
    QCheckBox* m_cleanAfterCopy;

    void updateSettingsGui();
    void updateSettings();

    QString c_settingsPath;

private slots:
    void onSetBackUpPathClicked();
};


#endif //LLABVOLUMEUPBACKER_SETTINGSDIALOG_H
