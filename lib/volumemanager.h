//
// Created by Jens Lindahl on 11/06/2018.
//

#ifndef LLABVOLUMEUPBACKER_VOLUMEMANAGER_H
#define LLABVOLUMEUPBACKER_VOLUMEMANAGER_H

#include <QStorageInfo>
#include <iostream>
#include "filemanager.h"

class VolumeManager {
public:

    VolumeManager() = default;

    QList<QStorageInfo> getMountedVolumes() {
        QList<QStorageInfo> volumes;
        for (const auto& v : QStorageInfo::mountedVolumes()) {
            if (v.isValid() && v.isReady() && !v.isRoot()) {
                if (!v.isReadOnly()) {
                    volumes.append(v);
                }
            }
        }
        return volumes;
    }

};


#endif //LLABVOLUMEUPBACKER_VOLUMEMANAGER_H
