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
#include <QDateTime>

namespace Ui {
class VolumesWidget;
}

class VolumesWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VolumesWidget(QWidget *parent = nullptr);
    ~VolumesWidget();

    void setBackupPath(const QString& path);
    void setCleanAfterCopy(bool b);

private:
    Ui::VolumesWidget *ui;
    QListWidget* m_list;
    std::map<QListWidgetItem*,QStorageInfo*> m_listMap;
    VolumeManager* m_vm;

    QProgressDialog* m_progressDialog;

    QList<QStorageInfo> m_volumes;
    bool m_backupInProgress = false;
    QString m_backupBasePath;
    bool m_cleanAfterCopy;

    std::vector<FileManager*> m_fileManagers;
    QList<QFuture<void>> m_futureList;

    void waitForFinish();
    void cancelBackup();
    QString getBackupDoneMsg(int timeElapsed);
    QString getCancelledMsg();
    QTime m_time;

    int displayBackupPathNotFoundWarning();
    int displayBackupPathNotWritableWarning();
    int displayAreYouSureToRunQuestion();
    int displayNoVolumeSelectedWarning();
    int displaySourcePathNotWritableWarning();

private slots:
    void refreshMountedVolumes();
    void initiateBackup();
    void filterList(QString filterString);
    void updateProgressBar();
};

#endif // VOLUMESWIDGET_H
