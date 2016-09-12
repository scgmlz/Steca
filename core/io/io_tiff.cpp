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
#include <tiffio.h>
#include <QDir>
#include <QDataStream>

namespace io {
//------------------------------------------------------------------------------
// Code taken from the original STeCa, modified.

static void loadTiff(data::shp_File&, rcstr, typ::deg, qreal, qreal) THROWS;

data::shp_File loadTiffDat(rcstr filePath) THROWS {
  data::shp_File file(new data::File(filePath));

  QFile f(filePath);
  RUNTIME_CHECK(f.open(QFile::ReadOnly), "cannot open file");

  QFileInfo info(filePath);
  QDir dir = info.dir();

  QByteArray line;
  while (!(line = f.readLine()).isEmpty()) {
    str s = line;

    int commentPos = s.indexOf(';');
    if (commentPos >= 0)
      s = s.left(commentPos);

    if ((s = s.simplified()).isEmpty())
      continue;

    auto lst = s.split(' ');
    auto cnt = lst.count();
    RUNTIME_CHECK(2 <= cnt && cnt <= 4, "bad metadata format");

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

    loadTiff(file, dir.filePath(tiffFileName), phi, monitor, expTime);
  }

  return file;
}


#define READ(type, name) \
  type name; is >> name; \
  RUNTIME_CHECK(QDataStream::Ok == is.status(), BAD_FORMAT)

#define IS_NUMBER \
  RUNTIME_CHECK((1==dataType || 2==dataType || 4==dataType) && 1==dataCount, BAD_FORMAT)

static void loadTiff(data::shp_File& file, rcstr filePath, typ::deg phi, qreal monitor, qreal expTime) THROWS {
  TIFF* tif = TIFFOpen("foo.tif", "r");
  TIFFClose(tif);

//  QFile f(filePath);
//  RUNTIME_CHECK(f.open(QFile::ReadOnly), "cannot open file");

//  str const BAD_FORMAT = "bad file format";
//  QDataStream is(&f);

//  READ(quint16, magic);

//  if (0x4949 == magic) {
//    // intel
//    is.setByteOrder(QDataStream::LittleEndian);
//  } else if (0x4d4d == magic) {
//    // motorola
//    is.setByteOrder(QDataStream::BigEndian);
//  } else {
//    THROW(BAD_FORMAT);
//  }

//  READ(quint16, version);
//  RUNTIME_CHECK(42 == version, BAD_FORMAT);

//  READ(quint32, offset);
//  RUNTIME_CHECK(f.seek(offset), BAD_FORMAT);

//  uint imageWidth = 0, imageHeight = 0,
//       bitsPerSample = 0;

//  quint16 numDirEntries; is >> numDirEntries;
//  for_i(numDirEntries) {
//    READ(quint16, tagId);
//    READ(quint16, dataType);  // 1 byte, 2 ascii, 3 short, 4 long, 5 rational
//    READ(quint32, dataCount);
//    READ(quint32, dataOffset);
//    TR(tagId << dataType << dataCount << dataOffset)
//    switch (tagId) {
//      case 256: // ImageWidth
//        IS_NUMBER; imageWidth = dataOffset;
//        break;
//      case 257: // ImageHeight
//        IS_NUMBER; imageHeight = dataOffset;
//        break;
//      case 258: // BitsPerSample
//        IS_NUMBER; bitsPerSample = dataOffset;
//        break;
//      case 259: // Compression
//        break;
//      case 269: // DocumentName
//        break;
//      case 273: // StripOffsets
//        break;
//      case 277: // SamplesPerPixel
//        break;
//      case 279: // StripByteCounts
//        break;
//      case 284: // PlanarConfiguration
//        break;
//      case 306: // DateTime
//        break;
//      case 339: // SampleFormat
//        break;
//    }

//        259 3 1 1
//        269 2 47 16777362
//        273 4 1 8
//        277 3 1 1
//        279 4 1 16777216
//        284 3 1 1
//        306 2 20 16777410
//        339 3 1 3
//  }

//      WT(numDirEntries)

//  QImageReader reader(&f, "tiff");
//  RUNTIME_CHECK(reader.canRead(), str("cannot read ") + filePath);

//  QImage image = reader.read();
//  RUNTIME_CHECK(!image.isNull(), reader.errorString() + ": " + filePath);

  //Read Image
//  QFileInfo fileInfo(QString(path.c_str()));
//  QString dirPath = fileInfo.dir().absolutePath();
//  fileInfo.setFile(dirPath, QString(pictureFileName.c_str()));
//  QString picturePath = fileInfo.absoluteFilePath();

//  float *image_f4 = NULL;
//  TIFF *file = TIFFOpen(picturePath.toStdString().c_str(), "r");
//  quint32 imageWidth = 0;
//  quint32 imageHeight = 0;
//  quint16 sampleFormat = 0;
//  quint16 samplesPerPixel = 0;
//  quint16 bitsPerSample = 0;
//  quint16 fillOrder = 0;
//  unsigned int numberOfPixel = 0;
//  if (file)
//  {
//      if ((TIFFGetFieldDefaulted(file, TIFFTAG_IMAGEWIDTH, &imageWidth)==1) &&
//          (TIFFGetFieldDefaulted(file, TIFFTAG_IMAGELENGTH, &imageHeight)==1) &&
//          (TIFFGetFieldDefaulted(file, TIFFTAG_SAMPLESPERPIXEL, &samplesPerPixel)==1) &&
//          (TIFFGetFieldDefaulted(file, TIFFTAG_BITSPERSAMPLE, &bitsPerSample)==1) &&
//          (TIFFGetFieldDefaulted(file, TIFFTAG_SAMPLEFORMAT, &sampleFormat)==1) &&
//          (TIFFGetFieldDefaulted(file, TIFFTAG_FILLORDER, &fillOrder)==1))
//      {
//          numberOfPixel = imageWidth * imageHeight;
//          image_f4 = new float[numberOfPixel];

//          size_t scanlineSize = TIFFScanlineSize(file);
//          quint8* buffer = (quint8*) malloc(scanlineSize);

//          for (quint32 y=0; y<imageHeight; y++)
//          {
//              if (TIFFReadScanline(file, buffer, y)==1)
//              {
//                  for (quint32 x=0; x<imageWidth; x++)
//                  {
//                      switch (sampleFormat)
//                      {
//                      case SAMPLEFORMAT_INT:
//                          switch (bitsPerSample)
//                          {
//                          case 1:
//                          case 2:
//                          case 4:
//                          case 8:
//                              image_f4[y*imageWidth + x] = ((qint8*)buffer)[x];
//                              break;
//                          case 16:
//                              image_f4[y*imageWidth + x] = ((qint16*)buffer)[x];
//                              break;
//                          case 32:
//                              image_f4[y*imageWidth + x] = ((qint32*)buffer)[x];
//                              break;
//                          }
//                          break;
//                      case SAMPLEFORMAT_UINT:
//                          switch (bitsPerSample)
//                          {
//                          case 1:
//                          case 2:
//                          case 4:
//                          case 8:
//                              image_f4[y*imageWidth + x] = ((quint8*)buffer)[x];
//                              break;
//                          case 16:
//                              image_f4[y*imageWidth + x] = ((quint16*)buffer)[x];
//                              break;
//                          case 32:
//                              image_f4[y*imageWidth + x] = ((quint32*)buffer)[x];
//                              break;
//                          }
//                          break;
//                      case SAMPLEFORMAT_IEEEFP:
//                          switch (bitsPerSample)
//                          {
//                          case 32:
//                              image_f4[y*imageWidth + x] = ((float*)buffer)[x];
//                              break;
//                          case 64:
//                              image_f4[y*imageWidth + x] = ((double*)buffer)[x];
//                              break;
//                          }
//                          break;
//                      }
//                  }
//              }
//          }
//          free(buffer);
//      }
//      //img -> getTiffMetaData(file);
//      TIFFClose(file);
//  }
//  //End Read Image

//  if (image_f4 == NULL) break;

//  //Check Pixel Level um Daten mit defekten Pixeln lesen zu können
//  if (MeasurementData::isPixelLevelUsed())
//  {
//      for (unsigned int i=0; i<numberOfPixel; i++)
//      {
//          if (image_f4[i] > MeasurementData::pixelLevel)
//              image_f4[i] = -1;
//      }
//  }

//  totalTime += expTime;

//  //Objekt inizialisieren
//  DetectorDataset::setPixOrg(imageWidth, imageHeight);
//  PoleFigureCalculator::DetectorDataset *detectorDataset;
//  detectorDataset = new PoleFigureCalculator::DetectorDataset("no comment", 0, 0, 0,
//                                                              0/*omga*/, 0/*theta*/, phi*RAD/*phi*/, 0/*chi*/,
//                                                              monitor/*mon*/, expTime/*time*/, totalTime/*sumTime*/,
//                                                              image_f4/*array*/, imageWidth*imageHeight,
//                                                              pictureOverflow);
//  vectorDataset.push_back(detectorDataset);

//  //Delete Data
//  delete image_f4;
//}

//// Close input file
//datFile.close();
//// End of Close

//return true;

}

//------------------------------------------------------------------------------
/*
shp_File loadTiffs(rcstr filePath) THROWS {
  shp_File file(new File(filePath));

  QFileInfo info(filePath);

  QFile diskFile(filePath);
  RUNTIME_CHECK(diskFile.open(QIODevice::ReadOnly), "cannot open file");

  QTextStream in(&diskFile);

  vec<Dataset> datasets;
  while (!in.atEnd()) {
    str line = in.readLine();

    str_lst dataFromFile;
    for_i (line.size()) { dataFromFile.append(line.section(',', i, i)); }
  }

  return shp_File(new File(filePath));
}

void saveTiffs(rcFile file, rcstr fileName) THROWS {
  QFileInfo info(fileName);
  QDir().mkpath(info.absoluteDir().absolutePath());

  QFile diskFile(info.filePath() + ".csv");
  RUNTIME_CHECK(diskFile.open(QIODevice::WriteOnly), "cannot open file");

  QTextStream out(&diskFile);
  // generate .csv File for referencing to tiff image files
  for_i (file.numDatasets()) {
    Dataset dataset = *file.getDataset(i);
    str     fileName =
        info.absoluteFilePath() + str(".%1.tiff").arg(i, 3, 10, QChar('0'));

    QImage tiffImage(dataset.parentFile().getImageSize(), QImage::Format_RGB32);
    Image  image = dataset.getImage();
    auto   size  = image.size();

    for_i (size.height()) {
      auto y = i;
      for_i (size.width()) {
        auto x      = i;
        auto intens = image.intensity(x + y * size.width());
        tiffImage.setPixel(x, y, qRgb(0, intens, 0));
      }
    }

    tiffImage.save(fileName, "tiff");

    out << fileName;
    for_i (Dataset::numAttributes()) {
      out << ", " << dataset.getAttributeStrValue(i);
    }
    out << "\n";
    out.flush();
  }
}
*/
//------------------------------------------------------------------------------
}
// eof
