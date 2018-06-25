#include <QApplication>
#include <QDebug>
#include <QStorageInfo>
#include <gui/mainwindow.h>
#include <QRegularExpression>

int main(int argc, char** argv) {
    Q_INIT_RESOURCE(icons);

    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("LLab Volume Backup Tool");
    w.show();

    return a.exec();
}