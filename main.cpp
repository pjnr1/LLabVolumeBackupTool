#include <QApplication>
#include <gui/mainwindow.h>

int main(int argc, char** argv) {
    Q_INIT_RESOURCE(icons);

    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("LLab Volume Backup Tool");
    w.show();

    return a.exec();
}