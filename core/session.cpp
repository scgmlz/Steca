//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/session.cpp
//! @brief     Implements class Session
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

// This needs to be placed once, somewhere, in our sources:
#ifdef QT_NO_EXCEPTIONS
#error needs exception handling
#endif

#include "core/session.h"
#include "core/fit/peak_functions.h"
#include <QJsonDocument>

Session* gSession; //!< global, for data handling

Session::Session()
    : metaSelection_( std::vector<bool>(Metadata::size(), false) )
{
    gSession = this;

    register_peak_functions();

    geometry().fromSettings();
    interpol().fromSettings();

    // Some signals imply other signals:
    connect(this, &Session::sigActivated,     [this]() {EMIT(sigDfgram());} );
    connect(this, &Session::sigBaseline,      [this]() {EMIT(sigDfgram());} );
    connect(this, &Session::sigCorr,          [this]() {EMIT(sigDfgram());} );
    connect(this, &Session::sigDetector,      [this]() {EMIT(sigDfgram());} );
    connect(this, &Session::sigGamma,         [this]() {EMIT(sigDfgram());} );
    connect(this, &Session::sigNorm,          [this]() {EMIT(sigDfgram());} );
    connect(this, &Session::sigPeaks,         [this]() {EMIT(sigDfgram());} );

    connect(this, &Session::sigCorr,          [this]() {EMIT(sigImage());} );
    connect(this, &Session::sigDataHighlight, [this]() {EMIT(sigImage());} );
    connect(this, &Session::sigDetector,      [this]() {EMIT(sigImage());} );
    connect(this, &Session::sigNorm,          [this]() {EMIT(sigImage());} );

    connect(this, &Session::sigDfgram,        [this]() {EMIT(sigDoFits());} );

    connect(this, &Session::sigDataHighlight, &gammaSelection_, &GammaSelection::onData);
    connect(this, &Session::sigDataHighlight, &thetaSelection_, &ThetaSelection::onData);
}

Session::~Session()
{
    geometry().toSettings();
    interpol().toSettings();
}

void Session::clear()
{
    dataset_.clear();
    corrset_.clear();
    baseline_.clear();
    peaks_.clear();

    normMode_ = eNorm::NONE;

    angleMapCache_.clear();

    intenScaledAvg_ = true;
    intenScale_ = 1;
}

void Session::sessionFromJson(const QByteArray& json)
{
    QJsonParseError parseError;
    QJsonDocument doc(QJsonDocument::fromJson(json, &parseError));
    if (!(QJsonParseError::NoError == parseError.error))
        THROW("Error parsing session file");

    clear();
    TR("sessionFromJson: cleared old session");

    JsonObj top(doc.object());

    dataset().fromJson(top.loadObj("dataset"));
    corrset().fromJson(top.loadObj("corrset"));
    peaks().fromJson(top.loadArr("peaks"));
    baseline().fromJson(top.loadObj("baseline"));

    bool arg1 = top.loadBool("average intensity?", true);
    double arg2 = top.loadPreal("intensity scale", 1);
    setIntenScaleAvg(arg1, arg2);

    geometry().fromJson(top.loadObj("detector"));
    imageCut().fromJson(top.loadObj("cut"));
    gammaSelection().fromJson(top.loadObj("gamma selection"));
    thetaSelection().fromJson(top.loadObj("theta selection"));

    TR("installed session from file");
}

QByteArray Session::serializeSession() const
{
    QJsonObject top;

    top.insert("dataset", dataset().toJson());
    top.insert("corrset", corrset().toJson());
    top.insert("peaks", peaks().toJson());
    top.insert("baseline", baseline().toJson());

    // TODO serialize metaSelection_

    top.insert("average intensity?", intenScaledAvg());
    top.insert("intensity scale", double_to_json((double)intenScale()));
    // TODO serialize image rotation and mirror
    top.insert("detector", geometry().toJson());
    top.insert("cut", imageCut().toJson());
    top.insert("gamma selection", gammaSelection().toJson());
    top.insert("theta selection", thetaSelection().toJson());

    return QJsonDocument(top).toJson();
}

void Session::setMetaSelected(int i, bool on)
{
    metaSelection_[i] = on;
    EMIT(gSession->sigMetaSelection());
}

void Session::updateImageSize()
{
    if (0 == dataset().countFiles() && !corrset().hasFile())
        imageSize_ = size2d(0, 0);
}

void Session::setImageSize(const size2d& size)
{
    if (!(!size.isEmpty())) THROW("image is empty or has ill defined size");
    if (imageSize_.isEmpty())
        imageSize_ = size; // the first one
    else if (imageSize_ != size)
        THROW("image size differs from previously loaded images");
}

size2d Session::imageSize() const
{
    return imageTransform_.isTransposed() ? imageSize_.transposed() : imageSize_;
}

void Session::setImageTransformMirror(bool on)
{
    imageTransform_ = imageTransform_.mirror(on);
}

void Session::setImageTransformRotate(const ImageTransform& rot)
{
    imageTransform_ = imageTransform_.rotateTo(rot);
}

IJ Session::midPix() const
{
    size2d sz = imageSize();
    IJ mid(sz.w / 2, sz.h / 2);

    const IJ& off = geometry_.midPixOffset();
    mid.i += off.i;
    mid.j += off.j;

    return mid;
}

const AngleMap& Session::angleMap(const Measurement& one) const
{
    ImageKey key(geometry_, imageSize_, imageCut_, midPix(), one.midTth());
    AngleMap* map = angleMapCache_.value(key);
    if (!map)
        map = angleMapCache_.insert(key, shp_AngleMap(new AngleMap(key)));
    return *map;
}

// TODO: split into two functions (see usage in panel_diff..)
void Session::setIntenScaleAvg(bool avg, double scale)
{
    intenScaledAvg_ = avg;
    intenScale_ = scale;
    EMIT(gSession->sigNorm());
}

void Session::setNormMode(eNorm normMode)
{
    normMode_ = normMode;
    EMIT(gSession->sigNorm());
}

// only used in background normalization
// TODO move out from here
double Session::calcAvgBackground(const Sequence& seq) const
{
    Curve gmaCurve = seq.toCurve(1.);
    Polynom bgPolynom = Polynom::fromFit(baseline().polynomDegree(), gmaCurve, baseline().ranges());
    return bgPolynom.avgY(seq.rgeTth());
}

// TODO move out from here
double Session::calcAvgBackground() const
{
    TakesLongTime __;
    double bg = 0;
    for (const Cluster* cluster : activeClusters().clusters())
        bg += calcAvgBackground(*cluster);
    return bg / activeClusters().size();
}
