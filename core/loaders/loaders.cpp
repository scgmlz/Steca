// ************************************************************************** //
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
// ************************************************************************** //

#include "core/data/datafile.h"
#include "core/typ/exception.h"
#include <QStringBuilder> // for ".." % ..
#include <QFileInfo>

namespace load {
Datafile loadCaress(rcstr filePath) THROWS;
Datafile loadMar(rcstr filePath) THROWS;
Datafile loadTiffDat(rcstr filePath) THROWS;
str loadCaressComment(rcstr filePath);
}

namespace {

// peek at up to maxLen bytes (to establish the file type)
static QByteArray peek(int pos, int maxLen, QFileInfo const& info) {
    QFile file(info.filePath());
    if (file.open(QFile::ReadOnly) && file.seek(pos))
        return file.read(maxLen);
    return QByteArray();
}

// Caress file format
bool couldBeCaress(QFileInfo const& info) {
    static QByteArray const header("\020\012DEFCMD DAT");
    return header == peek(0, header.size(), info);
}

// Mar file format
bool couldBeMar(QFileInfo const& info) {
    static QByteArray const header("mar research");
    return header == peek(0x80, header.size(), info);
}

// Text .dat file with metadata for tiff files
bool couldBeTiffDat(QFileInfo const& info) {
    QFile file(info.filePath());
    if (!file.open(QFile::ReadOnly))
        return false;
    bool ret = false;
    QByteArray line;
    while (!(line = file.readLine()).isEmpty()) {
        str s = line;
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

Datafile load_low_level(rcstr filePath) THROWS {
    const QFileInfo info(filePath);
    RUNTIME_CHECK(info.exists(), "File " % filePath % " does not exist");

    if (couldBeCaress(info))
        return load::loadCaress(filePath);
    else if (couldBeMar(info))
        return load::loadMar(filePath);
    else if (couldBeTiffDat(info))
        return load::loadTiffDat(filePath);
    else
        THROW("unknown file type: " % filePath);
}

} // local methods

namespace load {

QSharedPointer<Datafile> loadDatafile(rcstr filePath) THROWS {
    const QSharedPointer<Datafile> ret(new Datafile(load_low_level(filePath)));
    RUNTIME_CHECK(ret->cluster().count() > 0, "File " % filePath % " contains no cluster");
    return ret;
}

str loadComment(QFileInfo const& info) {
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
