//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/loaders/loaders.cpp
//! @brief     Implements functions loadRawFile, loadComment in namespace load
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/base/exception.h"
#include "core/loaders/load_caress.h" // provides load::loadCaress(..), load::loadCaressComment(..)
#include "core/loaders/load_mar.h"    // provides load::loadMar(..)
#include "core/loaders/load_tiff.h"   // provides load::loadTiff(..)
#include "core/loaders/load_yaml.h"   // provides load::loadYaml(..)
#include "core/raw/rawfile.h"
#include <QStringBuilder> // for ".." % ..

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
        THROW("File '" % filePath % "' does not exist");

    if (couldBeCaress(info))
        return load::loadCaress(filePath);
    else if (couldBeYaml(info))
        return load::loadYaml(filePath);
    else if (couldBeMar(info))
        return load::loadMar(filePath);
    else if (couldBeTiffDat(info))
        return load::loadTiffDat(filePath);
    else
        THROW("File '" % filePath % "' has unknown type");
}

} // local methods

namespace load {

//! Loads a raw file of any supported type.

//! The file type (format) will be determined automatically,
//! and the corresponding loader will be called.

Rawfile loadRawfile(const QString& filePath) {
    Rawfile ret {load_low_level(filePath)};
    if (!ret.numMeasurements())
        THROW("File '" % filePath % "' contains no cluster");
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
