#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "volumeswidget.h"
#include "settingsdialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    VolumesWidget* m_volumesWidget;
    SettingsDialog* m_settingsDialog;

    void updateSettings();

private slots:
    void showSettings();
    void exitApplication();
};

#endif // MAINWINDOW_H
