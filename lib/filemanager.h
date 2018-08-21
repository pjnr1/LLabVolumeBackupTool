#ifndef LLABVOLUMEUPBACKER_FILEMANAGER_H
#define LLABVOLUMEUPBACKER_FILEMANAGER_H

#include <QString>
#include <QDir>
#include <QFile>
#include <QCryptographicHash>

class FileManager : public QObject {
    Q_OBJECT

public:
    double getProgress();
    unsigned long getSizeToCopy(bool recursive = true);
    unsigned long getSizeToCopy(const QString &source, bool recursive = true);
    bool startCopy();
    bool startCopy(const QString &source, const QString &dest);
    bool startClean();
    bool startClean(const QString &source);
    bool copy(const QString &source, const QString &dest, bool recursive = true);
    bool remove(const QString &source);
    bool copyFile(const QString& source, const QString& dest, bool override = true);
    bool fileExists(const QString& path);
    bool isFolder(const QString& path);

    void setSource(const QString& path);
    void setDestination(const QString& path);

private:

    QByteArray getCheckSum(const QString &fileName,
                           QCryptographicHash::Algorithm hashAlgorithm);


    void calcSizeToCopy(const QString &source, bool recursive = true);

    unsigned long m_totalFileSize = 0;
    unsigned long m_copiedSoFar = 0;

    QString m_source;
    QString m_destination;
};


#endif //LLABVOLUMEUPBACKER_FILEMANAGER_H
