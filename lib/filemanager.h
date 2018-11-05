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
    unsigned long getSizeToCopy();
    unsigned long getSizeToCopy(const QString &source);
    bool startCopy();
    bool startCopy(const QString &source, const QString &dest);
    bool startClean();
    bool startClean(const QString &source);
    bool copy(const QString &source, const QString &dest);
    bool remove(const QString &source);
    bool copyFile(const QString& source, const QString& dest, bool override);
    bool fileExists(const QString& path);
    bool isFolder(const QString& path);

    bool recursive();
    void recursive(bool r);
    void setSource(const QString& path);
    void setDestination(const QString& path);

    QString getDestination();
    QString getSource();

signals:
    Q_SIGNAL void log(const QString& text);

private:

    QByteArray getCheckSum(const QString &fileName,
                           QCryptographicHash::Algorithm hashAlgorithm);


    void calcSizeToCopy(const QString &source);

    unsigned long m_totalFileSize = 0;
    unsigned long m_copiedSoFar = 0;

    bool m_recursive = true;
    QString m_source;
    QString m_destination;

};


#endif //LLABVOLUMEUPBACKER_FILEMANAGER_H
