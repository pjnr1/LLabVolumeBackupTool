#ifndef VOLUMESWIDGET_H
#define VOLUMESWIDGET_H

#include <QWidget>
#include <lib/volumemanager.h>
#include <QListWidget>
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrent>
#include <map>
#include <QProgressDialog>

namespace Ui {
class VolumesWidget;
}

class VolumesWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VolumesWidget(QWidget *parent = 0);
    ~VolumesWidget();

    void setBackupPath(const QString& path);



private:
    Ui::VolumesWidget *ui;
    QListWidget* m_list;
    std::map<QListWidgetItem*,QStorageInfo*> m_listMap;
    VolumeManager* m_vm;

    QList<QStorageInfo> m_volumes;
    bool m_backupInProgress = false;
    QString m_backupBasePath;

    std::vector<FileManager*> m_fileManagers;
    QList<QFuture<void>> m_futureList;

    void hideAll();
    void waitForFinish();
    QString getBackupDoneMsg(int timeElapsed);

    QTime m_time;

    int displayBackupPathNotFoundWarning();
    int displayBackupPathNotWritableWarning();
    int displayAreYouSureToRunQuestion();
    int displayNoVolumeSelectedWarning();

private slots:
    void refreshMountedVolumes();
    void initiateBackup();
    void filterList(QString filterString);
    void updateProgressBar();
};

#endif // VOLUMESWIDGET_H
