// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/io/io_io.cpp
//! @brief     Implements loader functions.
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "data/datafile.h"
#include "typ/exception.h"
#include <QStringBuilder> // for ".." % ..
#include <QFileInfo>

namespace io {
Datafile loadCaress(rcstr filePath) THROWS;
Datafile loadMar(rcstr filePath) THROWS;
Datafile loadTiffDat(rcstr filePath) THROWS;
str loadCaressComment(rcstr filePath);
}

namespace {

// peek at up to maxLen bytes (to establish the file type)
static QByteArray peek(uint pos, uint maxLen, QFileInfo const& info) {
    QFile file(info.filePath());

    if (file.open(QFile::ReadOnly) && file.seek(pos))
        return file.read(maxLen);

    return QByteArray();
}

// Caress file format
bool couldBeCaress(QFileInfo const& info) {
    static QByteArray const header("\020\012DEFCMD DAT");
    return header == peek(0, to_u(header.size()), info);
}

// Mar file format
bool couldBeMar(QFileInfo const& info) {
    static QByteArray const header("mar research");
    return header == peek(0x80, to_u(header.size()), info);
}

// Text .dat file with metadata for tiff files
bool couldBeTiffDat(QFileInfo const& info) {
    QFile file(info.filePath());

    if (!file.open(QFile::ReadOnly))
        return false;

    bool couldBe = false;

    QByteArray line;

    while (!(line = file.readLine()).isEmpty()) {
        str s = line;

        int commentPos = s.indexOf(';');
        if (commentPos >= 0)
            s = s.left(commentPos);

        if ((s = s.simplified()).isEmpty())
            continue;

        auto lst = s.split(' ');
        int cnt = lst.count();
        if (cnt < 2 || cnt > 4)
            return false;

        couldBe = true;
    }

    return couldBe;
}

Datafile load_low_level(rcstr filePath) THROWS {
    QFileInfo info(filePath);
    RUNTIME_CHECK(info.exists(), "File " % filePath % " does not exist");

    if (couldBeCaress(info))
        return io::loadCaress(filePath);
    else if (couldBeMar(info))
        return io::loadMar(filePath);
    else if (couldBeTiffDat(info))
        return io::loadTiffDat(filePath);
    else
        THROW("unknown file type: " % filePath);
}

} // anonymous namespace

namespace io {

QSharedPointer<Datafile const> loadDatafile(rcstr filePath) THROWS {
    auto ret = QSharedPointer<Datafile const>(new Datafile(load_low_level(filePath)));
    RUNTIME_CHECK(ret->suite().count() > 0, "File " % filePath % " contains no suite");
    return ret;
}

str loadComment(QFileInfo const& info) {
    auto path = info.absoluteFilePath();
    if (couldBeCaress(info))
        return "[car] " + io::loadCaressComment(path);
    else if (couldBeMar(info))
        return "[mar] ";
    else if (couldBeTiffDat(info))
        return "[tif] ";
    else
        return "";
}

} // namespace io
