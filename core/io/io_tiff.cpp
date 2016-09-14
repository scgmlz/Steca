// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      io_tiff.cpp
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#include "io_io.h"
#include <QDir>
#include <QDataStream>

namespace io {
//------------------------------------------------------------------------------

static void loadTiff(data::shp_File&, rcstr, typ::deg, qreal, qreal) THROWS;

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

data::shp_File loadTiffDat(rcstr filePath) THROWS {
  data::shp_File file(new data::File(filePath));

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
    typ::deg phi     = lst.at(1).toDouble(&ok);
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

    // load one dataset
    loadTiff(file, dir.filePath(tiffFileName), phi, monitor, expTime);
  }

  return file;
}


#define READ(type, name) \
  type name; is >> name; \
  RUNTIME_CHECK(QDataStream::Ok == is.status(), BAD_FORMAT)

#define IS_ASCII \
  RUNTIME_CHECK(2==dataType, BAD_FORMAT)

#define IS_NUMBER \
  RUNTIME_CHECK((1==dataType || 3==dataType || 4==dataType) && 1==dataCount, BAD_FORMAT)

#define CHECK_NUMBER(val) \
  IS_NUMBER; RUNTIME_CHECK(val==dataOffset, BAD_FORMAT)

#define SEEK(offset) \
  RUNTIME_CHECK(f.seek(offset), BAD_FORMAT)

static void loadTiff(data::shp_File& file, rcstr filePath, typ::deg phi, qreal monitor, qreal expTime) THROWS {

  QFile f(filePath);
  RUNTIME_CHECK(f.open(QFile::ReadOnly), "cannot open file");

  str const BAD_FORMAT = "cannot handle file format";
  QDataStream is(&f);
  is.setFloatingPointPrecision(QDataStream::SinglePrecision);

  READ(quint16, magic);

  if (0x4949 == magic) {
    // intel
    is.setByteOrder(QDataStream::LittleEndian);
  } else if (0x4d4d == magic) {
    // motorola
    is.setByteOrder(QDataStream::BigEndian);
  } else {
    THROW(BAD_FORMAT);
  }

  READ(quint16, version);
  RUNTIME_CHECK(42 == version, BAD_FORMAT);

  READ(quint32, offset);
  SEEK(offset);

  quint32 imageWidth = 0, imageHeight = 0,
          bitsPerSample = 0, sampleFormat = 0,
          rowsPerStrip = 0xffffffff,
          stripOffsets = 0, stripByteCounts = 0;

  quint16 numDirEntries; is >> numDirEntries;
  for_i(numDirEntries) {
    READ(quint16, tagId);
    READ(quint16, dataType);  // 1 byte, 2 ascii, 3 short, 4 long
    READ(quint32, dataCount);
    READ(quint32, dataOffset);

    switch (dataType) {
    case 1:
      dataOffset &= 0x000000ff;
      break;
    case 3:
      dataOffset &= 0x0000ffff;
      break;
    }

    WT(tagId << dataType << dataCount << dataOffset)

    switch (tagId) {
      case 256: // ImageWidth
        IS_NUMBER; imageWidth = dataOffset;     // 2048
        break;
      case 257: // ImageHeight (TIFF calls: ImageLength)
        IS_NUMBER; imageHeight = dataOffset;    // 2048
        break;
      case 258: // BitsPerSample
        IS_NUMBER; bitsPerSample = dataOffset;  // 32
        break;
      case 259: // Compression
        CHECK_NUMBER(1);                        // 1 - uncompressed
        break;
      case 269: // DocumentName
        IS_ASCII; // ignore
        break;
      case 273: // StripOffsets
        IS_NUMBER; stripOffsets = dataOffset;
        break;
      case 277: // SamplesPerPixel
        CHECK_NUMBER(1);
        break;
      case 278: // RowsPerStrip
        IS_NUMBER; rowsPerStrip = dataOffset;
      case 279: // StripByteCounts
        IS_NUMBER; stripByteCounts = dataOffset;
        break;
      case 284: // PlanarConfiguration
        CHECK_NUMBER(1);
        break;
      case 306: // DateTime
        IS_ASCII; // ignore
        break;
      case 339: // SampleFormat
        IS_NUMBER; sampleFormat = dataOffset; // 1 unsigned, 2 signed, 3 IEEE
        break;
      default:
        TR("* NEW TAG *")
        break;
    }
  }

  RUNTIME_CHECK(imageWidth>0 && imageHeight>0 && stripOffsets>0 && stripByteCounts>0, BAD_FORMAT);
  RUNTIME_CHECK(1==sampleFormat || 2==sampleFormat || 3==sampleFormat, BAD_FORMAT);
  RUNTIME_CHECK(32==bitsPerSample, BAD_FORMAT);
  RUNTIME_CHECK(imageHeight <= rowsPerStrip, BAD_FORMAT);

  typ::size2d size(imageWidth, imageHeight);
  inten_vec intens(imageWidth * imageHeight);

  RUNTIME_CHECK((bitsPerSample/8) * imageWidth * imageHeight == stripByteCounts, BAD_FORMAT);

  SEEK(stripOffsets);

  for_i (intens.count()) {
    switch (sampleFormat) {
    case 1: {
      READ(quint32, sample);
      intens[i] = sample;
      break;
    }
    case 2: {
      READ(qint32, sample); TR(sample)
      intens[i] = sample;
      break;
    }
    case 3: {
      READ(float, sample);
      intens[i] = sample;
      break;
    }
    }
  }

  data::Metadata md;
  // md.date    = str::fromStdString(s_date);
  md.motorPhi     = phi;
  md.monitorCount = monitor;
  md.time         = expTime;

  file->datasets().append(
    data::shp_OneDataset(new data::OneDataset(md, size,
        not_null<inten_t const*>::from(intens.constData()))
    )
  );
}

//------------------------------------------------------------------------------
}
// eof
