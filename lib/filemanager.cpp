#include <iostream>
#include <QtCore/qdatetime.h>
#include "filemanager.h"

/**
 * Simple copy-function for prototyping
 * @param source
 * @param dest
 * @return
 */
bool FileManager::copyFile(const QString &source, const QString &dest, bool override) {
    if (!fileExists(source)) {
        return false;
    } else if (fileExists(dest)) {
        if (override) {
            QFile::remove(dest);
        } else {
            return false;
        }
    }
    QFile::copy(source, dest);
    auto hashAlgo = QCryptographicHash::Algorithm::Md5;
    return (getCheckSum(source, hashAlgo) == getCheckSum(dest, hashAlgo));

}

/**
 * Recursive copy
 * @param source
 * @param dest
 * @return
 */
bool FileManager::copy(const QString &source, const QString &dest, bool recursive) {
    auto dir = QDir(source);
    QFileInfoList files = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);

    // Create destination folder if it doesn't exist
    if (!isFolder(dest)) {
        dir.mkdir(dest);
    }
    for(const auto& f : files) {
        if (isFolder(f.filePath())) {
            if (recursive) {
                copy(f.filePath(), dest + "/" +  f.fileName(), recursive);
            }
        } else {
            copyFile(f.filePath(), dest + "/" +  f.fileName());
            m_copiedSoFar += f.size();
        }
    }
    return true;
}

bool FileManager::remove(const )

/**
 * Check if path exists and is a file
 * @param path
 * @return
 */
bool FileManager::fileExists(const QString& path){
    QFileInfo check_file(path);
    return check_file.exists() && check_file.isFile();
}

/**
 * Check if path exists and is a directory
 * @param path
 * @return
 */
bool FileManager::isFolder(const QString& path) {
    QFileInfo check_file(path);
    return check_file.exists() && check_file.isDir();
}

/**
 * Get checksum of file
 * @param fileName
 * @param hashAlgorithm
 * @return
 */
QByteArray FileManager::getCheckSum(const QString &fileName, QCryptographicHash::Algorithm hashAlgorithm) {
    QFile f(fileName);
    if (f.open(QFile::ReadOnly)) {
        QCryptographicHash hash(hashAlgorithm);
        if (hash.addData(&f)) {
            return hash.result();
        }
    }
    return QByteArray();
}

void FileManager::calcSizeToCopy(const QString &source, bool recursive) {
    auto dir = QDir(source);
    QFileInfoList files = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);
    for(const auto& f : files) {
        if (isFolder(f.filePath())) {
            if (recursive) {
                calcSizeToCopy(f.filePath(), recursive);
            }
        } else {
            m_totalFileSize += f.size();
        }
    }
}

unsigned long FileManager::getSizeToCopy(bool recursive) {
    m_totalFileSize = 0;
    calcSizeToCopy(m_source, recursive);
    return m_totalFileSize;
}

unsigned long FileManager::getSizeToCopy(const QString &source, bool recursive) {
    m_totalFileSize = 0;
    calcSizeToCopy(source, recursive);
    return m_totalFileSize;
}

double FileManager::getProgress() {
    if (m_totalFileSize == 0) {
        return -1;
    } else {
        return (double) m_copiedSoFar / (double) m_totalFileSize;
    }
}

bool FileManager::startCopy(const QString &source, const QString &dest) {
    auto dir = QDir(source);
    auto dateTime = QDateTime::currentDateTime();
    auto newDest = dest + "/" + dateTime.toString("yyyyMMdd_HHmm") + "_" +  dir.dirName();

    dir.mkdir(newDest);
    if (isFolder(newDest)) {
        return copy(source,newDest,true);
    } else {
        return false;
    }
}

bool FileManager::startCopy() {
    return startCopy(m_source, m_destination);
}

bool FileManager::startClean(const QString &source) {
    auto dir = QDir(source);
    QFileInfoList files = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);
    for(const auto& f : files) {
        dir.remove(f.filePath());
    }
    return true;
}

bool FileManager::startClean() {
    return startClean(m_source);
}

void FileManager::setSource(const QString& path) {
    m_source = path;
}

void FileManager::setDestination(const QString& path) {
    m_destination = path;
}
