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
#include "qcr/engine/debug.h"
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
    connect(this, &Session::sigActivated,     [this]() {EMITS("sigActivated", sigDfgram());} );
    connect(this, &Session::sigBaseline,      [this]() {EMITS("sigBaseline", sigDfgram());} );
    connect(this, &Session::sigCorr,          [this]() {EMITS("sigCorr", sigDfgram());} );
    connect(this, &Session::sigDetector,      [this]() {EMITS("sigDetector", sigDfgram());} );
    connect(this, &Session::sigGamma,         [this]() {EMITS("sigGamma", sigDfgram());} );
    connect(this, &Session::sigNorm,          [this]() {EMITS("sigNorm", sigDfgram());} );
    connect(this, &Session::sigPeaks,         [this]() {EMITS("sigPeaks", sigDfgram());} );

    connect(this, &Session::sigCorr,          [this]() {EMITS("sigCorr", sigImage());} );
    connect(this, &Session::sigDataHighlight, [this]() {EMITS("sigDataHighlight", sigImage());} );
    connect(this, &Session::sigDetector,      [this]() {EMITS("sigDetector", sigImage());} );
    connect(this, &Session::sigNorm,          [this]() {EMITS("sigNorm", sigImage());} );

    connect(this, &Session::sigDfgram,        [this]() {EMITS("sigDfgram", sigDoFits());} );

    connect(this, &Session::sigDataHighlight, &gammaSelection_, &GammaSelection::onData);
    connect(this, &Session::sigDataHighlight, &thetaSelection_, &ThetaSelection::onData);
}

Session::~Session()
{
    geometry().toSettings();
    interpol().toSettings();
}

const PeakInfos& Session::peakInfos() const
{
    if (interpol().enabled())
        return interpolatedPeakInfos_;
    else
        return directPeakInfos_;
}

void Session::clear()
{
    dataset_.clear();
    corrset_.clear();
    baseline_.clear();
    peaks_.clear();

    normMode_ = eNorm::NONE;

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
    EMITS("Session::setMetaSelected", gSession->sigMetaSelection());
}

void Session::updateImageSize()
{
    if (0 == dataset().countFiles() && !corrset().hasFile())
        imageSize_ = size2d(0, 0);
}

void Session::setImageSize(const size2d& size)
{
    if (size.isEmpty())
        THROW("image is empty or has ill defined size");
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

// TODO: split into two functions (see usage in panel_diff..)
void Session::setIntenScaleAvg(bool avg, double scale)
{
    intenScaledAvg_ = avg;
    intenScale_ = scale;
    EMITS("Session::setIntenScaleAvg", gSession->sigNorm());
}

void Session::setNormMode(eNorm normMode)
{
    normMode_ = normMode;
    EMITS("Session::setNormMode", gSession->sigNorm());
}
