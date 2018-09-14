//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/loaders/load_tiff.cpp
//! @brief     Implements function loadTiff
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/misc/exception.h"
#include "core/raw/rawfile.h"
#include <QDataStream>
#include <QDir>

namespace {

//! Reads one TIFF file.
static void loadTiff(
    Rawfile* file, const QString& filePath, deg phi, double monitor, double expTime)
{
    Metadata md;
    md.motorPhi = phi;
    md.monitorCount = monitor;
    md.time = expTime;

    // see http://www.fileformat.info/format/tiff/egff.htm

    QFile f(filePath);
    if (!(f.open(QFile::ReadOnly))) THROW("cannot open file");

    QDataStream is(&f);
    is.setFloatingPointPrecision(QDataStream::SinglePrecision);

    auto check = [&is]() { if (!(QDataStream::Ok == is.status())) THROW("could not read data"); };

    // magic
    qint16 magic;
    is >> magic;

    if (0x4949 == magic) // II - intel
        is.setByteOrder(QDataStream::LittleEndian);
    else if (0x4d4d == magic) // MM - motorola
        is.setByteOrder(QDataStream::BigEndian);
    else
        THROW("bad magic");

    qint16 version;
    is >> version;
    if (42 != version)
        THROW("bad version");

    qint32 imageWidth = 0, imageHeight = 0, bitsPerSample = 0, sampleFormat = 0,
            rowsPerStrip = 0xffffffff, stripOffsets = 0, stripByteCounts = 0;

    qint16 tagId, dataType;
    qint32 dataCount, dataOffset;

    auto seek = [&f](qint64 offset) { if (!(f.seek(offset))) THROW("bad offset"); };

    auto asUint = [&]() -> int {
        if (!(1 == dataCount)) THROW("bad data count");
        switch (dataType) {
        case 1: // byte
            return dataOffset & 0x000000ff; // some tif files did have trash there
        case 3: // short
            return dataOffset & 0x0000ffff;
        case 4: return dataOffset;
        }

        THROW("not a simple number");
    };

    auto asStr = [&]()->QString {
        if (!(2 == dataType)) THROW("bad data type");
        auto lastPos = f.pos();

        seek(dataOffset);
        QByteArray data = f.readLine(dataCount);
        seek(lastPos);

        return QString(data);
    };

    qint32 dirOffset;
    is >> dirOffset;
    check();
    seek(dirOffset);

    qint16 numDirEntries;
    is >> numDirEntries;

    for (int i=0; i<numDirEntries; ++i) {
        is >> tagId >> dataType >> dataCount >> dataOffset;
        check();

        switch (tagId) {
        // numbers
        case 256: imageWidth = asUint(); break;
        case 257: imageHeight = asUint(); break;
        case 258: bitsPerSample = asUint(); break;
        case 259: // Compression
            if (!(1 == asUint())) THROW("compressed data");
            break;
        case 273: stripOffsets = asUint(); break;
        case 277: // SamplesPerPixel
            if (!(1 == asUint())) THROW("more than one sample per pixel");
            break;
        case 278: rowsPerStrip = asUint(); break;
        case 279: stripByteCounts = asUint(); break;
        case 284: // PlanarConfiguration
            if (!(1 == asUint())) THROW("not planar");
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

    if (imageWidth<=0)
        THROW("cannot read TIFF: unexpected imageWidth");
    if (imageHeight<=0)
        THROW("cannot read TIFF: unexpected imageHeight");
    if (stripOffsets<=0)
        THROW("cannot read TIFF: unexpected stripOffsets");
    if (stripByteCounts<=0)
        THROW("cannot read TIFF: unexpected stripByteCounts");
    if (imageHeight < rowsPerStrip)
        THROW("cannot read TIFF: imageHeight >= rowsPerStrip");

    if (!(1 == sampleFormat || 2 == sampleFormat || 3 == sampleFormat))
        THROW("cannot read TIFF: unexpected sampleFormat");
    if (bitsPerSample!=32)
        THROW("cannot read TIFF: bitsPerSample!=32");


    size2d size(imageWidth, imageHeight);

    int count = imageWidth * imageHeight;
    std::vector<float> intens(count);

    if (!((bitsPerSample / 8) * count == stripByteCounts))
        THROW("cannot read TIFF: unexpected bitsPerSample");

    seek(stripOffsets);

    for (int i=0; i<intens.size(); ++i)
        switch (sampleFormat) {
        case 1: {
            qint32 sample;
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

    file->addDataset(std::move(md), size, std::move(intens));
}

} // namespace


namespace load {

//! Reads a .dat file, which is a digest that contains a list of TIFF files plus a few parameters.

//! The dat file looks like so:
//!
//! ; comments
//!
//! ; file name, phi, monitor, Exposuretime  [the last two parameters are optional]
//!
//! Aus-Weimin-00001.tif -90
//! Aus-Weimin-00002.tif -85
//! Aus-Weimin-00003.tif -80
//! Aus-Weimin-00004.tif -75
//! Aus-Weimin-00005.tif -70
//! Aus-Weimin-00006.tif -65
//! Aus-Weimin-00007.tif -60
//! Aus-Weimin-00008.tif -55
//! Aus-Weimin-00009.tif -50

Rawfile loadTiffDat(const QString& filePath) {
    Rawfile ret(filePath);

    QFile f(filePath);
    if (!(f.open(QFile::ReadOnly)))
        THROW("cannot open file");

    QDir dir = QFileInfo(filePath).dir();

    QByteArray line;
    while (!(line = f.readLine()).isEmpty()) {
        QString s = line;

        // cut off comment
        int commentPos = s.indexOf(';');
        if (commentPos >= 0)
            s = s.left(commentPos);

        // split to parts
        if ((s = s.simplified()).isEmpty())
            continue;

        const QStringList lst = s.split(' ');
        const int cnt = lst.count();
        if (!(2 <= cnt && cnt <= 4)) THROW("bad metadata format");

        // file, phi, monitor, expTime
        bool ok;
        QString tiffFileName = lst.at(0);
        deg phi = lst.at(1).toDouble(&ok);
        if (!(ok)) THROW("bad phi value");

        double monitor = 0;
        if (cnt > 2) {
            monitor = lst.at(2).toDouble(&ok);
            if (!(ok)) THROW("bad monitor value");
        }

        double expTime = 0;
        if (cnt > 3) {
            expTime = lst.at(3).toDouble(&ok);
            if (!(ok)) THROW("bad expTime value");
        }

        try {
            // load one dataseq
            loadTiff(&ret, dir.filePath(tiffFileName), phi, monitor, expTime);
        } catch (Exception& ex) {
            THROW(tiffFileName + ": " + ex.msg());
        }
    }

    return ret;
}

} // namespace load
