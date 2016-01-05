#include "core.h"
#include <memory>
#include "loadcaress.h"

namespace core {

void raiseError(rcstr msg) throw (Exception) {
  throw Exception(msg);
}

Session::Session() {
}

Session::~Session() {
  for (auto file: dataFiles) delete file;
}

Session::File::File(): File("") {
}

Session::File::File(rcstr fileName): info(fileName) {
}

void Session::File::load() throw (Exception) {
  if (!info.exists())
    raiseError("File " % info.filePath() % " does not exist");

  loadCaress(info.filePath());
}

void Session::addFile(rcstr fileName) throw (Exception) {
  if (hasFile(fileName)) return;

  std::unique_ptr<File> file(new File(fileName));
  file->load();

  dataFiles.append(file.release());
}

bool Session::hasFile(rcstr fileName) {
  QFileInfo fileInfo(fileName);
  for (auto& file: dataFiles)
    if (fileInfo == file->info) return true;
  return false;
}

void Session::remFile(uint i) {
  dataFiles.remove(i);
}

bool Session::hasCorrFile() const {
  return !corrFile.name().isEmpty();
}

void Session::setCorrFile(rcstr fileName) {
  corrFile = fileName;
}

void Setup::setPixFul(int pixWidth_, int pixHeight_)
{
  if ((pixWidth == pixWidth_) && (pixHeight == pixHeight_))
      return; // nothing to do

  // Delete the old angle correction array
  if (angleCorrArray != NULL) {
      for (int i = 0; i < pixWidth; i++) {
          delete[] angleCorrArray[i];
      }
      delete[] angleCorrArray;
      angleCorrArray = NULL;
  }

  // Delete the old intens correction array
  if (intensCorrArray != NULL) {
      for (int i = 0; i < pixWidth; i++)
          delete[] intensCorrArray[i];
      delete[] intensCorrArray;
      intensCorrArray = NULL;
  }

  if (intensCorrArrayNotStandardized != NULL) {
      for (int i = 0; i < pixWidth; i++)
          delete[] intensCorrArrayNotStandardized[i];
      delete[] intensCorrArrayNotStandardized;
      intensCorrArrayNotStandardized = NULL;
  }

  pixWidth = pixWidth_;
  pixHeight = pixHeight_;

  // Set border to 0
  setPixCut(0, 0, 0, 0);
}

void Setup::setPixFul(int pixWidth, int pixHeight, int pixMiddleWidthOffset, int pixMiddleHeightOffset)
{
  setPixFul(pixWidth, pixHeight);
  setPixXYOffset(pixMiddleWidthOffset, pixMiddleHeightOffset);
}

void Setup::setPixXYOffset(int pixWidthOffset, int pixHeightOffset)
{
  pixOffsetX = pixWidthOffset;
  pixOffsetY = pixHeightOffset;

}

void Setup::setPixCut(int borderPixLeft_, int borderPixRight_, int borderPixTop_, int borderPixBottom_)
{
  if (borderPixLeft_ + borderPixRight_ >= pixWidth)
      throw "setPixCut(): borderLeft + borderRight is to heigh";
  if (borderPixTop_ + borderPixBottom_ >= pixHeight)
      throw "setPixCut(): borderTop + borderBottom is to heigh";

  borderPixTop = borderPixTop_;
  borderPixBottom = borderPixBottom_;
  borderPixLeft = borderPixLeft_;
  borderPixRight = borderPixRight_;

  if (angleCorrArray == NULL)
      return; // nothing more to do

  // Calculate TTHMax, TTHMin, GammaMax and GammaMin after cut
  // Init start values
  int arrayMiddleX = (int)(getWidthCut() / 2) + borderPixLeft;
  int arrayMiddleY = (int)(getHeightCut() / 2) + borderPixTop;
  int arraySizeX = pixWidth;
  int arraySizeY = pixHeight;

  Pixpos middlePoint = angleCorrArray[arrayMiddleX][arrayMiddleY];
  cut.gamma.hig = middlePoint.gammaPix;
  cut.gamma.low = middlePoint.gammaPix;
  cut.tth_regular.hig = middlePoint.tthPix;
  cut.tth_regular.low = middlePoint.tthPix;
  cut.tth_gamma0.hig = angleCorrArray[arraySizeX - 1 - borderPixRight][getPixMiddleY()].tthPix;
  cut.tth_gamma0.low = angleCorrArray[borderPixLeft][getPixMiddleY()].tthPix;

  if (cut.tth_gamma0.low > cut.tth_gamma0.hig) {
      double temp = cut.tth_gamma0.hig;
      cut.tth_gamma0.hig = cut.tth_gamma0.low;
      cut.tth_gamma0.low = temp;
  }

  for (int i = borderPixLeft; i < arraySizeX - borderPixRight; i++) {
      for (int j = borderPixTop; j < arraySizeY - borderPixBottom; j++) {
          if (cut.gamma.hig < angleCorrArray[i][j].gammaPix)
              cut.gamma.hig = angleCorrArray[i][j].gammaPix;
          if (cut.gamma.low > angleCorrArray[i][j].gammaPix)
              cut.gamma.low = angleCorrArray[i][j].gammaPix;
          if (cut.tth_regular.hig < angleCorrArray[i][j].tthPix)
              cut.tth_regular.hig = angleCorrArray[i][j].tthPix;
          if (cut.tth_regular.low > angleCorrArray[i][j].tthPix)
              cut.tth_regular.low = angleCorrArray[i][j].tthPix;
      }
  }
}

int Setup::getWidthCut() const
{
  int widthCut = pixWidth - borderPixLeft - borderPixRight;
  if (widthCut < 1)
      throw "widthCut is lower than 1";
  return widthCut;
}

int Setup::getHeightCut() const
{
  int heightCut = pixHeight - borderPixTop - borderPixBottom;
  if (heightCut < 1)
      throw "heightCut is lower than 1";
  return heightCut;
}

int Setup::getPixMiddleX() const
{
  int tempPixMiddleX = pixWidth / 2 + pixOffsetX;
  if ((tempPixMiddleX <= 0) || (tempPixMiddleX >= pixWidth))
      throw "pixMiddleX is not set";
  return tempPixMiddleX;
}

int Setup::getPixMiddleY() const
{
  int tempPixMiddleY = pixHeight / 2 + pixOffsetY;
  if ((tempPixMiddleY <= 0) || (tempPixMiddleY >= pixHeight))
      throw "pixMiddleY is not set";
  return tempPixMiddleY;
}

Dataset::Dataset(Setup &setup_, std::__cxx11::string comment_, float motorXT_, float motorYT_, float motorZT_, float motorOmg_, float motorTth_, float motorPhi_, float motorChi_, double mon_, double deltaTime_, float *intensity_, int numberOfElements_, bool imageOverflow_, float motorPST_, float motorSST_, float motorOMGM_)
  : setup(setup_)
{
  motorXT = motorXT_;
  motorYT = motorYT_;
  motorZT = motorZT_;
  motorOmg = motorOmg_;
  motorTth = motorTth_;
  motorChi = motorChi_;
  motorPhi = motorPhi_;
  motorPST = motorPST_;
  motorSST = motorSST_;
  motorOMGM = motorOMGM_;
  overflow = imageOverflow_;

  if (mon >= 0)
      mon = mon_;
  else {
      mon = 1;
//      cout << "Warning Mon of image ... is negative!" << endl;
  }
  if (deltaTime >= 0)
      deltaTime = deltaTime_;
  else {
      deltaTime = 1;
//      cout << "Warning deltaTime of image ... is negative!" << endl;
  }

  comment = comment_;

  // Set detector Informations
  if ((setup.pixWidth * setup.pixHeight) != numberOfElements_) {
//      cout << "Achtung Fehler beim Pixelverhaeltnis" << endl;
      return;
  }

  // Create 2D detector image array
  float** intensArray2D = new float* [setup.pixWidth];
  for (int i = 0; i < setup.pixWidth; i++)
      intensArray2D[i] = new float[setup.pixHeight];

  // Fill the Array
  for (int i = 0; i < setup.pixWidth; i++) {
      for (int j = 0; j < setup.pixHeight; j++)
          intensArray2D[i][j] = intensity_[(j * setup.pixWidth) + i];
  }
  intensArray = intensArray2D;
}

}

// eof
