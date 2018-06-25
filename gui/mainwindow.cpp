#include <QEvent>
#include <QCloseEvent>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    // Setup settings dialog
    m_settingsDialog = new SettingsDialog();
    m_settingsDialog->setWindowTitle("Settings");
    m_settingsDialog->setWindowFlags(Qt::Sheet);
    m_settingsDialog->setWindowModality(Qt::WindowModal);

    m_volumesWidget = new VolumesWidget();
    ui->volumesLayout->addWidget(m_volumesWidget);
    updateSettings();

    // Update

    // Setup actions
    ui->actionSettings->setShortcut(QKeySequence::Preferences);
    ui->actionQuit->setShortcut(QKeySequence::Quit);
    connect(ui->actionSettings, &QAction::triggered, this, &MainWindow::showSettings);
    connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::exitApplication);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showSettings() {
    if (m_settingsDialog->exec()) {
        updateSettings();
    }
}

void MainWindow::exitApplication() {
    close();
}

void MainWindow::updateSettings() {
    m_volumesWidget->setBackupPath(m_settingsDialog->getBackupPath());
}
