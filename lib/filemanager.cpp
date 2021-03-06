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
    bool result = QFile::copy(source, dest);
    if (result) {
        emit log("Copied: " + source + " to " + dest);
    } else {
        emit log("Error copying: " + source + " to " + dest);
    }
    return result;
}

/**
 * Recursive copy
 * @param source
 * @param dest
 * @return
 */
bool FileManager::copy(const QString &source, const QString &dest) {
    auto dir = QDir(source);
    QFileInfoList files = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);

    // Create destination folder if it doesn't exist
    if (!isFolder(dest)) {
        dir.mkdir(dest);
    }
    for(const auto& f : files) {
        if (isFolder(f.filePath())) {
            if (m_recursive) {
                copy(f.filePath(), dest + "/" +  f.fileName());
            }
        } else {
            copyFile(f.filePath(), dest + "/" +  f.fileName(), true);
            m_copiedSoFar += f.size();
        }
    }
    return true;
}

bool FileManager::remove(const QString &source) {
    return false;
}

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

void FileManager::calcSizeToCopy(const QString &source) {
    auto dir = QDir(source);
    QFileInfoList files = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);
    for(const auto& f : files) {
        if (isFolder(f.filePath())) {
            if (m_recursive) {
                calcSizeToCopy(f.filePath());
            }
        } else {
            m_totalFileSize += f.size();
        }
    }
}

unsigned long FileManager::getSizeToCopy() {
    m_totalFileSize = 0;
    calcSizeToCopy(m_source);
    return m_totalFileSize;
}

unsigned long FileManager::getSizeToCopy(const QString &source) {
    m_totalFileSize = 0;
    calcSizeToCopy(source);
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
        return copy(source, newDest);
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
        QFile::remove(f.filePath());
        emit log("Deleted: " + f.filePath());
    }
    return true;
}

bool FileManager::startClean() {
    return startClean(m_source);
}

bool FileManager::recursive() {
    return m_recursive;
}

void FileManager::recursive(bool r) {
    m_recursive = r;
}

void FileManager::setSource(const QString& path) {
    m_source = path;
}

void FileManager::setDestination(const QString& path) {
    m_destination = path;
}

QString FileManager::getDestination() {
    return m_destination;
}
QString FileManager::getSource() {
    return m_source;
}