#include <iostream>
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