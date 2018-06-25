#include <QTimer>
#include <QLineEdit>
#include <QMessageBox>
#include "volumeswidget.h"
#include "ui_volumeswidget.h"

VolumesWidget::VolumesWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VolumesWidget)
{
    ui->setupUi(this);

    // Setup labels and texts
    ui->groupBox->setTitle("Select volumes");
    ui->label_4->setText("Filter");
    // Setup backup button
    ui->backupButton_2->setText("Start Backup");
    connect(ui->backupButton_2, &QPushButton::released, this, &VolumesWidget::initiateBackup);
    // Setup refresh button
    ui->refreshButton_2->setText("Refresh List");
    connect(ui->lineEdit_2, &QLineEdit::textEdited, this, &VolumesWidget::filterList);

    // Setup listWidget
    m_list = ui->listWidget_2;
    m_list->setFlow(QListView::TopToBottom);
    m_list->setIconSize(QSize(32,32));
    m_list->setSelectionMode(QAbstractItemView::MultiSelection);

    // Setup helpers
    m_vm = new VolumeManager();
    connect(ui->refreshButton_2, &QPushButton::released, this, &VolumesWidget::refreshMountedVolumes);

    // Setup progress bar
    ui->progressBar->setRange(0,100);
    ui->progressBar->setEnabled(false);

    auto m_timer = new QTimer();
    m_timer->setInterval(250);
    connect(m_timer, &QTimer::timeout, this, &VolumesWidget::updateProgressBar);
    m_timer->start();

}

VolumesWidget::~VolumesWidget()
{
    delete ui;
}

void VolumesWidget::refreshMountedVolumes()  {
    m_volumes = m_vm->getMountedVolumes();

    m_list->clear();
    m_listMap.clear();
    for (auto& v : m_volumes) {
        auto v_item = new QListWidgetItem();

        if (v.name().startsWith("SD_")) {
            // If volume has LLab sd-card prefix "SD_", set sd-card icon
            v_item->setIcon(QIcon(":/icons/sd-card.svg"));
        } else {
            // else, set default harddrive icon
            v_item->setIcon(QIcon(":/icons/harddrive.svg"));
        }
        m_listMap.insert(std::make_pair(v_item,&v));
        v_item->setText(v.name());
        v_item->setTextAlignment(Qt::AlignLeft);
        m_list->addItem(v_item);
    }
}

void VolumesWidget::initiateBackup() {
    if (m_list->selectedItems().empty()) {
        displayNoVolumeSelectedWarning();
        return;
    }
    QFileInfo fileInfo = QFileInfo(m_backupBasePath);
    if (!fileInfo.exists()) {
        displayBackupPathNotFoundWarning();
        return;
    }
    if (!fileInfo.isWritable()) {
        displayBackupPathNotWritableWarning();
        return;
    }

    if (displayAreYouSureToRunQuestion()) {
        m_backupInProgress = true;
        m_time.start();
        for (auto const& v : m_list->selectedItems()) {
            auto it = m_listMap.find(v);
            if (it != m_listMap.end()) {
                auto vol = it->second;
                m_fileManagers.emplace_back(new FileManager());
                m_fileManagers.front()->getSizeToCopy(vol->rootPath());
                QFuture<void> future = QtConcurrent::run(m_fileManagers.front(), &FileManager::copy, vol->rootPath(),
                                                         m_backupBasePath, true);
                m_futureList.append(future);
            }
        }
    }
}

void VolumesWidget::filterList(QString filterString) {
    hideAll();
    QList<QListWidgetItem*> matches = ( m_list->findItems(filterString, Qt::MatchFlag::MatchContains) );
    for (auto item : matches) {
        item->setHidden(false);
    }
}

void VolumesWidget::hideAll() {
    for(int row(0); row < m_list->count(); row++) {
        m_list->item(row)->setHidden(true);
    }
}


void VolumesWidget::updateProgressBar() {
    if (m_backupInProgress) {
        ui->progressBar->setEnabled(true);
        double progress = 0.0;
        // Get progress from all workers
        for (auto const& fileManager : m_fileManagers) {
            progress += fileManager->getProgress();
        }
        // Normalize
        progress /= m_fileManagers.size();
        // Get percentage
        progress *= 100;
        // Update progressbar
        ui->progressBar->setValue(progress);

        // If done backing up
        if (progress >= 100) {
            // Make sure all done
            waitForFinish();
            // Reset states
            m_backupInProgress = false;
            ui->progressBar->setEnabled(false);
            // Get time elapsed
            auto timeElapsed = m_time.elapsed();
            // Display messagebox
            QMessageBox::information(this, "Done.", getBackupDoneMsg(timeElapsed), QMessageBox::Ok);
        }
    }
}

void VolumesWidget::waitForFinish() {
    while (true) {
        bool c = false;
        for (auto const& f : m_futureList) {
            c = !f.isFinished();
        }
        if (!c) {
            break;
        }
    }
}

void VolumesWidget::setBackupPath(const QString &path) {
    m_backupBasePath = path;
}

QString VolumesWidget::getBackupDoneMsg(int timeElapsed) {
    QString message = "";
    message += "Done backing up.\n";
    message += "Time elapsed: ";
    if (timeElapsed > 60000) {
        message += QString::number(m_time.elapsed() / 60000);
        message += " minutes and ";
        message += QString::number((m_time.elapsed() / 1000) % 60);
        message += " seconds.\n";
    } else if (timeElapsed > 1000) {
        message += QString::number(m_time.elapsed() / 1000);
        message += " seconds.\n";
    } else {
        message += QString::number(m_time.elapsed());
        message += " milliseconds.\n";
    }
    return message;
}

int VolumesWidget::displayBackupPathNotFoundWarning() {
    QString message = "";
    message += "Can't run backup!\n";
    message += "Destination path is unavailable..";
    message += "Path: ";
    message += m_backupBasePath + "\n";
    message += "Check your settings!";

    QMessageBox* msgBox = new QMessageBox(
            QMessageBox::Warning,
            "Backup path not valid",
            message,
            QMessageBox::Ok,
            this,
            Qt::Sheet);

    return msgBox->exec();
}

int VolumesWidget::displayBackupPathNotWritableWarning() {
    QString message = "";
    message += "Can't run backup!\n";
    message += "Destination path is not writable..";
    message += "Path: ";
    message += m_backupBasePath + "\n";
    message += "Check your settings and write-permissions!";

    QMessageBox* msgBox = new QMessageBox(
            QMessageBox::Warning,
            "Backup path not valid",
            message,
            QMessageBox::Ok,
            this,
            Qt::Sheet);

    return msgBox->exec();
}

int VolumesWidget::displayAreYouSureToRunQuestion() {
    QString message = "";
    message += "Destination path is: ";
    message += m_backupBasePath;
    message += "\n";
    message += "Are you sure you want to run the backup?";

    QMessageBox* msgBox = new QMessageBox(
            QMessageBox::Question,
            "Run backup?",
            message,
            QMessageBox::Yes | QMessageBox::No,
            this,
            Qt::Sheet);

    return msgBox->exec();
}

int VolumesWidget::displayNoVolumeSelectedWarning() {
    QString message = "";
    message += "Can't run backup!\n";
    message += "No source/volumes has been selected..\n";
    message += "Make sure you have selected the volumes\n";
    message += "that you want to backup.";

    QMessageBox* msgBox = new QMessageBox(
            QMessageBox::Warning,
            "Backup path not valid",
            message,
            QMessageBox::Ok,
            this,
            Qt::Sheet);

    return msgBox->exec();
}


