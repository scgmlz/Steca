#include "core_session.h"

namespace core {

Session::Session(): upDown(false), leftRight(false), turnClock(false), turnCounter(false) {
}

Session::~Session() {
}

uint Session::numFiles(bool withCorr) {
  return dataFiles.count() + (withCorr && hasCorrFile() ? 1 : 0);
}

void Session::addFile(rcstr fileName) THROWS {
  if (hasFile(fileName)) return;

  QSharedPointer<File> file(new File(fileName));
  file->load();

  setImageSize(file->getDatasets().getImageSize());

  dataFiles.append(file);
}

bool Session::hasFile(rcstr fileName) {
  QFileInfo fileInfo(fileName);
  for (auto& file: dataFiles)
    if (fileInfo == file->getInfo()) return true;
  return false;
}

File const& Session::getFile(uint i) {
  if ((uint)dataFiles.count() == i) {
    ASSERT(!corrFile.isNull())
    return *corrFile;
  } else {
    return *dataFiles.at(i);
  }
}

void Session::remFile(uint i) {
  dataFiles.remove(i);
  updateImageSize();
}

bool Session::hasCorrFile() const {
  return !corrFile.isNull();
}

void Session::setCorrFile(rcstr fileName) {
  if (!fileName.isEmpty()) {
    QSharedPointer<File> file(new File(fileName));
    file->load();
    corrFile = file;
    setImageSize(file->getDatasets().getImageSize());
  } else {
    corrFile.clear();
    updateImageSize();
  }
}

void Session::setImageSize(QSize const& size) THROWS {
  if (size.isEmpty())
    THROW(" inconsistent image size");
  if (imageSize.isEmpty())
    imageSize = size;
  else if (imageSize != size)
    THROW(" session-inconsistent image size");
}

void Session::updateImageSize() {
  if (dataFiles.isEmpty() && !hasCorrFile())
    imageSize = QSize();
}

void Session::setUpDown(bool on) {
  upDown = on;
}

void Session::setLeftRight(bool on) {
  leftRight = on;
}

void Session::setTurnClock(bool on) {
  turnClock = on; turnCounter = false;
}

void Session::setTurnCounter(bool on) {
  turnClock = false; turnCounter = on;
}

QPoint Session::getPixMiddle(Image const& image) const {
  auto size = image.getSize();
  QPoint middle(
    size.width()  / 2 + middlePixXOffset,
    size.height() / 2 + middlePixYOffset);
  // TODO was: if ((tempPixMiddleX *[<=]* 0) || (tempPixMiddleX >= getWidth()))
  RUNTIME_CHECK(interval_t(0,size.width()).contains(middle.x()), "bad pixMiddle");
  RUNTIME_CHECK(interval_t(0,size.height()).contains(middle.y()), "bad pixMiddle");
  return middle;
}

// TODO this is a slightly modified original code; be careful; eventually refactor
void Session::createAngleCorrArray(Image const& image, qreal tthMitte) {
  auto size = image.getSize();
  ASSERT(!size.isEmpty())

  int sizeX = size.width(), sizeY = size.height();

  angleCorrArray.resize(image.pixCount());

  auto angleCorr = [this,&image](int x, int y) {
    return angleCorrArray[image.index(*this,x,y)];
  };

  ful.tth_regular.set(tthMitte);
  ful.tth_gamma0.set(tthMitte);
  ful.gamma.set(0);

  QPoint pixMiddle = getPixMiddle(image);

  auto length = [](qreal x, qreal y) { return sqrt(x*x + y*y); };

  // Fill the Array
  for (int i = 0; i < sizeY; i++) {
    int abstandInPixVertical = pixMiddle.y() - i;
    qreal y = abstandInPixVertical * pixSpan;
    for (int j = 0; j < sizeX; j++) {
      // TTH des Pixels berechnen
      int abstandInPixHorizontal = - pixMiddle.x() + j;
      qreal x = abstandInPixHorizontal * pixSpan;
      qreal z = length(x,y);
      qreal detektorAbstandPixel = length(z,sampleDetectorSpan);
      qreal tthHorAktuell = tthMitte + atan(x / sampleDetectorSpan);
      qreal detektorAbstandHorPixel = length(x,sampleDetectorSpan);
      qreal h = cos(tthHorAktuell) * detektorAbstandHorPixel;
      qreal tthNeu = acos(h / detektorAbstandPixel);

      // Gamma des Pixels berechnen
      qreal r = sqrt((detektorAbstandPixel * detektorAbstandPixel) - (h * h));
      qreal gamma = asin(y / r);

      // Check: if tthNeu negativ
      if (tthHorAktuell < 0) {
        tthNeu *= -1;
        gamma *= -1;
      }

      // Maxima und minima setzen
      ful.gamma.include(gamma);
      ful.tth_regular.include(tthNeu);
      ful.tth_gamma0.include(tthHorAktuell);

      // Write angle in array
      angleCorr(j,i) = Pixpos(gamma,tthNeu);
    }
  }

  // Calculate Gamma and TTH after cut
  // TODO the original code called setPixCut - is used elsewhere?
  // TODO refactor
  int arrayMiddleX = imageCut.left + (sizeX - imageCut.left - imageCut.right)  / 2;
  int arrayMiddleY = imageCut.top  + (sizeY - imageCut.top  - imageCut.bottom) / 2;

  Pixpos middlePoint = angleCorr(arrayMiddleX,arrayMiddleY);
  cut.gamma.set(middlePoint.gammaPix);
  cut.tth_regular.set(middlePoint.tthPix);
  cut.tth_gamma0.safeSet(
    angleCorr(size.width() - 1 - imageCut.right,pixMiddle.y()).tthPix,
    angleCorr(imageCut.left,pixMiddle.y()).tthPix);

  for (int i = imageCut.left; i < sizeX - imageCut.right; i++) {
    for (int j = imageCut.top; j < sizeY - imageCut.bottom; j++) {
      auto ac = angleCorr(i,j);
      cut.gamma.include(ac.gammaPix);
      cut.tth_regular.include(ac.tthPix);
    }
  }
}

Session::imagecut_t::imagecut_t(int top_, int bottom_, int left_, int right_)
: top(top_), bottom(bottom_), left(left_), right(right_) {
}

void Session::setImageCut(bool topLeft, bool linked, imagecut_t const& imageCut_) {
  if (imageSize.isEmpty())
    imageCut = imagecut_t();
  else {
    auto limit = [linked](int &thisOne, int &thatOne, int maxTogether) {
      if (linked && thisOne+thatOne>=maxTogether) {
        thisOne = thatOne = qMax(0, (maxTogether-1) / 2);
      } else {
        thisOne = qMax(qMin(thisOne, maxTogether - thatOne - 1), 0);
        thatOne = qMax(qMin(thatOne, maxTogether - thisOne - 1), 0);
      }
    };

    imageCut = imageCut_;
    // make sure that cut values are valid; in the right order
    if (topLeft) {
      limit(imageCut.top,   imageCut.bottom,  imageSize.height());
      limit(imageCut.left,  imageCut.right,   imageSize.width());
    } else {
      limit(imageCut.bottom,imageCut.top,     imageSize.height());
      limit(imageCut.right, imageCut.left,    imageSize.width());
    }
  }

}

}

// eof
