// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/io/io_tiff.cpp
//! @brief     Implements ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "io_io.h"
#include "def/idiomatic_for.h"
#include "data/metadata.h"
#include <QDataStream>
#include <QDir>

namespace io {

// implemented below
static void loadTiff(shp_Datafile&, rcstr, deg, qreal, qreal) THROWS;

// The dat file looks like so:
/*
; comments

; file name, phi, monitor, Exposuretime

Aus-Weimin-00001.tif -90
Aus-Weimin-00002.tif -85
Aus-Weimin-00003.tif -80
Aus-Weimin-00004.tif -75
Aus-Weimin-00005.tif -70
Aus-Weimin-00006.tif -65
Aus-Weimin-00007.tif -60
Aus-Weimin-00008.tif -55
Aus-Weimin-00009.tif -50
*/

shp_Datafile loadTiffDat(rcstr filePath) THROWS {
    shp_Datafile datafile(new Datafile(filePath));

    QFile f(filePath);
    RUNTIME_CHECK(f.open(QFile::ReadOnly), "cannot open file");

    QFileInfo info(filePath);
    QDir dir = info.dir();

    QByteArray line;
    while (!(line = f.readLine()).isEmpty()) {
        str s = line;

        // cut off comment
        int commentPos = s.indexOf(';');
        if (commentPos >= 0)
            s = s.left(commentPos);

        // split to parts
        if ((s = s.simplified()).isEmpty())
            continue;

        auto lst = s.split(' ');
        auto cnt = lst.count();
        RUNTIME_CHECK(2 <= cnt && cnt <= 4, "bad metadata format");

        // file, phi, monitor, expTime
        bool ok;
        str tiffFileName = lst.at(0);
        deg phi = lst.at(1).toDouble(&ok);
        RUNTIME_CHECK(ok, "bad phi value");

        qreal monitor = 0;
        if (cnt > 2) {
            monitor = lst.at(2).toDouble(&ok);
            RUNTIME_CHECK(ok, "bad monitor value");
        }

        qreal expTime = 0;
        if (cnt > 3) {
            expTime = lst.at(3).toDouble(&ok);
            RUNTIME_CHECK(ok, "bad expTime value");
        }

        try {
            // load one dataset
            loadTiff(datafile, dir.filePath(tiffFileName), phi, monitor, expTime);
        } catch (Exception& e) {
            // add file name to the message
            e.setMsg(tiffFileName + ": " + e.msg());
            throw e;
        }
    }

    return datafile;
}

#define IS_ASCII RUNTIME_CHECK(2 == dataType, BAD_FORMAT)

#define IS_NUMBER                                                                                  \
    RUNTIME_CHECK((1 == dataType || 3 == dataType || 4 == dataType) && 1 == dataCount, BAD_FORMAT)

#define CHECK_NUMBER(val)                                                                          \
    IS_NUMBER;                                                                                     \
    RUNTIME_CHECK(val == dataOffset, BAD_FORMAT)

static void
loadTiff(shp_Datafile& file, rcstr filePath, deg phi, qreal monitor, qreal expTime) THROWS {

    Metadata md;
    md.motorPhi = phi;
    md.monitorCount = monitor;
    md.time = expTime;

    // see http://www.fileformat.info/format/tiff/egff.htm

    QFile f(filePath);
    RUNTIME_CHECK(f.open(QFile::ReadOnly), "cannot open file");

    QDataStream is(&f);
    is.setFloatingPointPrecision(QDataStream::SinglePrecision);

    auto check = [&is]() { RUNTIME_CHECK(QDataStream::Ok == is.status(), "could not read data"); };

    // magic
    quint16 magic;
    is >> magic;

    if (0x4949 == magic) // II - intel
        is.setByteOrder(QDataStream::LittleEndian);
    else if (0x4d4d == magic) // MM - motorola
        is.setByteOrder(QDataStream::BigEndian);
    else
        THROW("bad magic");

    quint16 version;
    is >> version;
    if (42 != version)
        THROW("bad version");

    quint32 imageWidth = 0, imageHeight = 0, bitsPerSample = 0, sampleFormat = 0,
            rowsPerStrip = 0xffffffff, stripOffsets = 0, stripByteCounts = 0;

    quint16 tagId, dataType;
    quint32 dataCount, dataOffset;

    auto seek = [&f](qint64 offset) { RUNTIME_CHECK(f.seek(offset), "bad offset"); };

    auto asUint = [&]() -> uint {
        RUNTIME_CHECK(1 == dataCount, "bad data count");
        switch (dataType) {
        case 1: // byte
            return dataOffset & 0x000000ff; // some tif files did have trash there
        case 3: // short
            return dataOffset & 0x0000ffff;
        case 4: return dataOffset;
        }

        THROW("not a simple number");
    };

    auto asStr = [&]() {
        RUNTIME_CHECK(2 == dataType, "bad data type");
        auto lastPos = f.pos();

        seek(dataOffset);
        QByteArray data = f.readLine(dataCount);
        seek(lastPos);

        return str(data);
    };

    quint32 dirOffset;
    is >> dirOffset;
    check();
    seek(dirOffset);

    quint16 numDirEntries;
    is >> numDirEntries;

    for_i (numDirEntries) {
        is >> tagId >> dataType >> dataCount >> dataOffset;
        check();

        switch (tagId) {
        // numbers
        case 256: imageWidth = asUint(); break;
        case 257: imageHeight = asUint(); break;
        case 258: bitsPerSample = asUint(); break;
        case 259: // Compression
            RUNTIME_CHECK(1 == asUint(), "compressed data");
            break;
        case 273: stripOffsets = asUint(); break;
        case 277: // SamplesPerPixel
            RUNTIME_CHECK(1 == asUint(), "more than one sample per pixel");
            break;
        case 278: rowsPerStrip = asUint(); break;
        case 279: stripByteCounts = asUint(); break;
        case 284: // PlanarConfiguration
            RUNTIME_CHECK(1 == asUint(), "not planar");
            break;
        case 339:
            sampleFormat = asUint(); // 1 unsigned, 2 signed, 3 IEEE
            break;

        // text
        case 269: // DocumentName
            md.comment = asStr();
            break;
        case 306: // DateTime
            md.date = asStr();
            break;
            //    default:
            //      TR("* NEW TAG *" << tagId << dataType << dataCount << dataOffset)
            //      break;
        }
    }

    RUNTIME_CHECK(
        imageWidth > 0 && imageHeight > 0 && stripOffsets > 0 && stripByteCounts > 0
            && imageHeight <= rowsPerStrip,
        "bad format");

    RUNTIME_CHECK(
        (1 == sampleFormat || 2 == sampleFormat || 3 == sampleFormat) && 32 == bitsPerSample,
        "unhandled format");

    size2d size(imageWidth, imageHeight);

    uint count = imageWidth * imageHeight;
    inten_vec intens(count);

    RUNTIME_CHECK((bitsPerSample / 8) * count == stripByteCounts, "bad format");

    seek(stripOffsets);

    for_i (intens.count())
        switch (sampleFormat) {
        case 1: {
            quint32 sample;
            is >> sample;
            intens[i] = sample;
            break;
        }
        case 2: {
            qint32 sample;
            is >> sample;
            intens[i] = sample;
            break;
        }
        case 3: {
            float sample;
            is >> sample;
            intens[i] = sample;
            break;
        }
        }

    check();

    file->addDataset(md, size, intens);
}

} // namespace io
