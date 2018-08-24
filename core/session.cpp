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
#include "core/aux/exception.h"
#include "qcr/base/debug.h" // ASSERT
#include <QJsonDocument>

Session* gSession; //!< global, for data handling

Session::Session()
{
    gSession = this;
}

void Session::onDetector() const
{
    angleMap.invalidate();
    activeClusters.avgDfgram.invalidate();
    for (auto const& cluster: dataset.allClusters)
        cluster->dfgrams.invalidate();
}

void Session::onCut() const
{
    onDetector();
    corrset.invalidateNormalizer();
}

void Session::onBaseline() const
{
    for (auto const& cluster: dataset.allClusters)
        cluster->dfgrams.forAllValids(
            cluster.get(), [](const Dfgram& d){d.invalidateBg();});
}

void Session::onPeaks() const
{
    for (auto const& cluster: dataset.allClusters)
        cluster->dfgrams.forAllValids(
            cluster.get(), [](const Dfgram& d){d.invalidatePeaks();});
}

void Session::onPeakAt(int jP) const
{
    for (auto const& cluster: dataset.allClusters)
        cluster->dfgrams.forAllValids(
            cluster.get(), [jP](const Dfgram& d){d.invalidatePeakAt(jP);});
}

void Session::onInterpol() const
{
    allPeaks.invalidateInterpolated();
}

//! Removes all data, sets all parameters to their defaults. No need to invalidate caches?
void Session::clear()
{
    dataset.clear();
    corrset.clear();
    // params.clear(); TODO
    baseline.clear();
    peaks.clear();
}

void Session::sessionFromJson(const QByteArray& json)
{
    QJsonParseError parseError;
    QJsonDocument doc(QJsonDocument::fromJson(json, &parseError));
    if (!(QJsonParseError::NoError == parseError.error))
        THROW("Error parsing session file");

    clear();
    //qDebug() << "sessionFromJson: cleared old session";

    JsonObj top(doc.object());

    dataset.fromJson(top.loadObj("dataset"));
    corrset.fromJson(top.loadObj("corrset"));
    peaks.fromJson(top.loadArr("peaks"));
    baseline.fromJson(top.loadObj("baseline"));

    params.intenScaledAvg.setVal(top.loadBool("average intensity?", true));
    params.intenScale.setVal(top.loadPreal("intensity scale", 1));

    params.detector.fromJson(top.loadObj("params.detector"));
    params.imageCut.fromJson(top.loadObj("cut"));
    gammaSelection.fromJson(top.loadObj("gamma selection"));
    thetaSelection.fromJson(top.loadObj("theta selection"));

    //qDebug() << "installed session from file";
}

QByteArray Session::serializeSession() const
{
    QJsonObject top;

    top.insert("dataset", dataset.toJson());
    top.insert("corrset", corrset.toJson());
    top.insert("peaks", peaks.toJson());
    top.insert("baseline", baseline.toJson());

    // TODO serialize metaSelection_

    top.insert("average intensity?", params.intenScaledAvg.val());
    top.insert("intensity scale", double_to_json((double)params.intenScale.val()));
    // TODO serialize image rotation and mirror
    top.insert("params.detector", params.detector.toJson());
    top.insert("cut", params.imageCut.toJson());
    top.insert("gamma selection", gammaSelection.toJson());
    top.insert("theta selection", thetaSelection.toJson());

    return QJsonDocument(top).toJson();
}

void Session::updateImageSize()
{
    if (!dataset.countFiles() && !corrset.hasFile())
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
    return params.imageTransform.isTransposed() ? imageSize_.transposed() : imageSize_;
}

const Dfgram* Session::currentOrAvgeDfgram() const
{
    if (gSession->params.showAvgeDfgram.val())
        return &gSession->activeClusters.avgDfgram.get();
    const Cluster* cluster = gSession->currentCluster();
    ASSERT(cluster);
    return &cluster->currentDfgram();
}
