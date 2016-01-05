#ifndef CORE_H
#define CORE_H

#ifdef BUILDING_LIBRARY
#define QSHARED_EXPORT Q_DECL_EXPORT
#else
#define QSHARED_EXPORT Q_DECL_IMPORT
#endif

#include <QVector>
#include <QFileInfo>
#include <QException>

namespace core {

typedef QString     str;
typedef str const&  rcstr;

class QSHARED_EXPORT Exception: public QException {
public:
  Exception(rcstr msg_): msg(msg_) {}

  const char* what() const throw () {
    return msg.toLocal8Bit().data();
  }

  str msg;
};

void raiseError(rcstr msg) throw (Exception);

class QSHARED_EXPORT Session final {
public:

  class QSHARED_EXPORT File final {
  public:
    File();
    File(rcstr fileName);

    str name() const { return info.fileName(); }

    void load() throw (Exception);

  private:
    friend class Session;
    QFileInfo info;
  };

  class QSHARED_EXPORT Files final: public QVector<File*> {
  public:
  };

public:
  Session();
 ~Session();

  void addFile(rcstr fileName) throw (Exception);
  bool hasFile(rcstr fileName);
  void remFile(uint i);

  bool hasCorrFile()   const;
  void setCorrFile(rcstr fileName); // fileName may be empty -> unsets

  Files const& getDataFiles() const { return dataFiles; }
  File  const& getCorrFile()  const { return corrFile; }

private:
  Files dataFiles;
  File  corrFile;
};

// TODO temp

class Interval {
public:
    double low;
    double hig;

    //! Constructs an invalid interval.
    Interval() { clear(); };
    //! Constructs interval with given limits.
    Interval( double low_, double hig_ ) : low(low_), hig(hig_) { /*assert(hig>=low);*/ };

    //! Empties an interval.
    void clear() {
        low = +std::numeric_limits<double>::infinity();
        hig = -std::numeric_limits<double>::infinity();
    }
};

class Borders {
  public:
    Borders() : gamma(0, 0), tth_regular(0, 0), tth_gamma0(0, 0){};
    Interval gamma;
    Interval tth_regular;
    Interval tth_gamma0; // at gamma=0
};

typedef struct {
    double gammaPix;
    double tthPix;
} Pixpos;

class Setup {
  public:
    // Detector image original pix informations
    int pixWidth = 0;
    int pixHeight = 0;

    // Detector image original middle point
    int pixOffsetX = 0;
    int pixOffsetY = 0;

    // Detector image cut informations
    int borderPixLeft = 0;
    int borderPixRight = 0;
    int borderPixTop = 0;
    int borderPixBottom = 0;

    // Correction informations
    Pixpos** angleCorrArray = NULL;
    float** intensCorrArray = NULL;
    float** intensCorrArrayNotStandardized = NULL;
    bool doNormalize = false;

    // Calculated detector informations
    Borders ful; //!< @todo Seems to stay at default value: functionality lost while refactoring?
    Borders cut;

    // Detector informations
    double pixSpan = 0.1;
    double detectorSampleSpan = 0.1;

  void setPixFul(int pixWidth_, int pixHeight_);
  void setPixFul(int pixWidth, int pixHeight, int pixMiddleWidthOffset,
                 int pixMiddleHeightOffset);
  void setPixXYOffset(int pixWidthOffset, int pixHeightOffset);
  void setPixCut(int borderPixLeft, int borderPixRight, int borderPixTop, int borderPixBottom);
  int getWidthCut() const;
  int getHeightCut() const;
  int getPixMiddleX() const;
  int getPixMiddleY() const;
};

class Dataset {

  public:
    Dataset(Setup& setup_, std::string comment_, float motorXT_, float motorYT_, float motorZT_,
            float motorOmg_, float motorTth_, float motorPhi_, float motorChi_, double mon_,
            double deltaTime_, float* intensity_, int numberOfElements_,
            bool imageOverflow_ = false, float motorPST_ = 0.0, float motorSST_ = 0.0,
            float motorOMGM_ = 0.0);

  private:
    Setup& setup;

    float motorXT;
    float motorYT;
    float motorZT;
    float motorOmg;
    float motorTth;
    float motorPhi;
    float motorChi;
    float motorPST;
    float motorSST;
    float motorOMGM;

    std::string comment;

    float** intensArray; //!< Detector image original

    bool overflow; //!< Detector info overflow

    double mon;
    double deltaTime;
};

}

#endif
