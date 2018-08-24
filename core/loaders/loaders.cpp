//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/loaders/loaders.cpp
//! @brief     Implements loader functions.
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/aux/exception.h"
#include "core/raw/rawfile.h"
#include <QStringBuilder> // for ".." % ..

namespace load {
Rawfile loadCaress(const QString& filePath);
Rawfile loadYaml(const QString& filePath);
Rawfile loadMar(const QString& filePath);
Rawfile loadTiffDat(const QString& filePath);
QString loadCaressComment(const QString& filePath);
}

namespace {

// peek at up to maxLen bytes (to establish the file type)
static QByteArray peek(int pos, int maxLen, const QFileInfo& info) {
    QFile file(info.filePath());
    if (file.open(QFile::ReadOnly) && file.seek(pos))
        return file.read(maxLen);
    return QByteArray();
}

// Caress file format
bool couldBeCaress(const QFileInfo& info) {
    static QByteArray const header("\020\012DEFCMD DAT");
    return header == peek(0, header.size(), info);
}

// Yaml file format
bool couldBeYaml(const QFileInfo& info) {
    return info.suffix().toLower() == "yaml";
}

// Mar file format
bool couldBeMar(const QFileInfo& info) {
    static QByteArray const header("mar research");
    return header == peek(0x80, header.size(), info);
}

// Text .dat file with metadata for tiff files
bool couldBeTiffDat(const QFileInfo& info) {
    QFile file(info.filePath());
    if (!file.open(QFile::ReadOnly))
        return false;
    bool ret = false;
    QByteArray line;
    while (!(line = file.readLine()).isEmpty()) {
        QString s = line;
        const int commentPos = s.indexOf(';');
        if (commentPos >= 0)
            s = s.left(commentPos);
        if ((s = s.simplified()).isEmpty())
            continue;
        const int cnt = s.split(' ').count();
        if (cnt < 2 || cnt > 4)
            return false;
        ret = true;
    }
    return ret;
}

Rawfile load_low_level(const QString& filePath) {
    const QFileInfo info(filePath);
    if (!(info.exists()))
        THROW("File " % filePath % " does not exist");

    if (couldBeCaress(info))
        return load::loadCaress(filePath);
    else if (couldBeYaml(info))
        return load::loadYaml(filePath);
    else if (couldBeMar(info))
        return load::loadMar(filePath);
    else if (couldBeTiffDat(info))
        return load::loadTiffDat(filePath);
    else
        THROW("unknown file type: " % filePath);
}

} // local methods

namespace load {

Rawfile loadRawfile(const QString& filePath) {
    Rawfile ret {load_low_level(filePath)};
    if (!ret.numMeasurements())
        THROW("File " % filePath % " contains no cluster");
    return ret;
}

QString loadComment(const QFileInfo& info) {
    const QString& path = info.absoluteFilePath();
    if (couldBeCaress(info))
        return "[car] " + loadCaressComment(path);
    else if (couldBeMar(info))
        return "[mar] ";
    else if (couldBeTiffDat(info))
        return "[tif] ";
    else
        return "";
}

} // namespace load
