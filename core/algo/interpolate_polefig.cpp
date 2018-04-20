//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/algo/interpolate_polefig.cpp
//! @brief     Implements function interpolateInfos
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "interpolate_polefig.h"
#include "core/session.h"
#include "core/def/idiomatic_for.h"
#include "core/data/peak_info.h"
#include "core/typ/async.h"
#include <qmath.h>

//  ***********************************************************************************************
//  local methods
//  ***********************************************************************************************

namespace {

struct itf_t {
    itf_t() : itf_t(float(Q_QNAN), deg(Q_QNAN), float(Q_QNAN)) {}
    itf_t(float _inten, deg _tth, float _fwhm) : inten(_inten), tth(_tth), fwhm(_fwhm) {}

    void operator+=(const itf_t&); // used once to compute average

    float inten;
    deg tth;
    float fwhm;
};

void itf_t::operator+=(const itf_t& that)
{
    inten += that.inten;
    tth += that.tth;
    fwhm += that.fwhm;
}

// Calculates the difference of two angles. Parameters should be in [0, 360].
deg calculateDeltaBeta(deg beta1, deg beta2)
{
    // Due to cyclicity of angles (360 is equivalent to 0), some magic is needed.
    double deltaBeta = beta1 - beta2;
    double tempDelta = deltaBeta - 360;
    if (qAbs(tempDelta) < qAbs(deltaBeta))
        deltaBeta = tempDelta;
    tempDelta = deltaBeta + 360;
    if (qAbs(tempDelta) < qAbs(deltaBeta))
        deltaBeta = tempDelta;
    ASSERT(-180 <= deltaBeta && deltaBeta <= 180);
    return deg(deltaBeta);
}

// Calculates the angle between two points on a unit sphere.
deg angle(deg alpha1, deg alpha2, deg deltaBeta)
{
    // Absolute value of deltaBeta is not needed because cos is an even function.
    deg ret = rad(acos( cos(alpha1.toRad()) * cos(alpha2.toRad())
                       + sin(alpha1.toRad()) * sin(alpha2.toRad()) * cos(deltaBeta.toRad())))
                 .toDeg();
    ASSERT(0 <= ret && ret <= 180);
    return ret;
}

enum class eQuadrant {
    NORTHEAST,
    SOUTHEAST,
    SOUTHWEST,
    NORTHWEST,
};

static int NUM_QUADRANTS = 4;

typedef std::vector<eQuadrant> Quadrants;

Quadrants allQuadrants()
{
    return { eQuadrant::NORTHEAST, eQuadrant::SOUTHEAST,
            eQuadrant::SOUTHWEST, eQuadrant::NORTHWEST };
}

bool inQuadrant(eQuadrant quadrant, deg deltaAlpha, deg deltaBeta)
{
    switch (quadrant) {
    case eQuadrant::NORTHEAST: return deltaAlpha >= 0 && deltaBeta >= 0;
    case eQuadrant::SOUTHEAST: return deltaAlpha >= 0 && deltaBeta < 0;
    case eQuadrant::SOUTHWEST: return deltaAlpha < 0 && deltaBeta < 0;
    case eQuadrant::NORTHWEST: return deltaAlpha < 0 && deltaBeta >= 0;
    }
    qFatal("impossible case");
}

// Search quadrant remapping in case no point was found.
eQuadrant remapQuadrant(eQuadrant q)
{
    switch (q) {
    case eQuadrant::NORTHEAST: return eQuadrant::NORTHWEST;
    case eQuadrant::SOUTHEAST: return eQuadrant::SOUTHWEST;
    case eQuadrant::SOUTHWEST: return eQuadrant::NORTHEAST;
    case eQuadrant::NORTHWEST: return eQuadrant::SOUTHEAST;
    }
    qFatal("impossible case");
}

// Checks if (alpha,beta) is inside radius from (centerAlpha,centerBeta).
bool inRadius(deg alpha, deg beta, deg centerAlpha, deg centerBeta, deg radius)
{
    double a = angle(alpha, centerAlpha, calculateDeltaBeta(beta, centerBeta));
    return qAbs(a) < radius;
}

// Adds data from peak infos within radius from alpha and beta
// to the peak parameter lists.
void searchPoints(deg alpha, deg beta, deg radius, const PeakInfos& infos,
                  std::vector<itf_t>& itfs)
{
    // REVIEW Use value trees to improve performance.
    for (const PeakInfo& info : infos.peaks()) {
        if (inRadius(info.alpha(), info.beta(), alpha, beta, radius))
            itfs.push_back(itf_t(info.inten(), info.tth(), info.fwhm()));
    }
}

// Searches closest PeakInfos to given alpha and beta in quadrants.
void searchInQuadrants(
    const Quadrants& quadrants, deg alpha, deg beta, deg searchRadius, const PeakInfos& infos,
    std::vector<PeakInfo const*>& foundInfos, std::vector<double>& distances)
{
    ASSERT(quadrants.size() <= NUM_QUADRANTS);
    // Take only peak infos with beta within +/- BETA_LIMIT degrees into
    // account. Original STeCa used something like +/- 1.5*36 degrees.
    double const BETA_LIMIT = 30;

    // Resize and fill distances and foundInfos vectors with default values:
    distances.resize(quadrants.size());
    std::fill(distances.begin(), distances.end(), std::numeric_limits<double>::max());
    foundInfos.resize(quadrants.size());
    std::fill(foundInfos.begin(), foundInfos.end(), nullptr);

    // Find infos closest to given alpha and beta in each quadrant.
    for (const PeakInfo& info : infos.peaks()) {
        // REVIEW We could do better with value trees than looping over all infos.
        deg deltaBeta = calculateDeltaBeta(info.beta(), beta);
        if (fabs(deltaBeta) > BETA_LIMIT)
            continue;
        deg deltaAlpha = info.alpha() - alpha;
        // "Distance" between grid point and current info.
        deg d = angle(alpha, info.alpha(), deltaBeta);
        for_i (quadrants.size()) {
            if (inQuadrant(quadrants.at(i), deltaAlpha, deltaBeta)) {
                if (d >= distances.at(i))
                    continue;
                distances[i] = d;
                if (qIsNaN(searchRadius) || d < searchRadius)
                    foundInfos[i] = &info;
            }
        }
    }
}

itf_t inverseDistanceWeighing(const std::vector<double>& distances, const std::vector<PeakInfo const*>& infos)
{
    int N = NUM_QUADRANTS;
    // Generally, only distances.count() == values.count() > 0 is needed for this
    // algorithm. However, in this context we expect exactly the following:
    if (!(distances.size() == N)) qFatal("distances size should be 4");
    if (!(infos.size() == N)) qFatal("infos size should be 4");
    std::vector<double> inverseDistances(N);
    double inverseDistanceSum = 0;
    for_i (NUM_QUADRANTS) {
        if (distances.at(i) == .0) {
            // Points coincide; no need to interpolate.
            const PeakInfo* in = infos.at(i);
            return { in->inten(), in->tth(), in->fwhm() };
        }
        inverseDistances[i] = 1 / distances.at(i);
        inverseDistanceSum += inverseDistances.at(i);
    }
    // REVIEW The RAW peak may need different handling.
    double offset = 0;
    double height = 0;
    double fwhm = 0;
    for_i (N) {
        const PeakInfo* in = infos.at(i);
        double d = inverseDistances.at(i);
        offset += in->tth() * d;
        height += in->inten() * d;
        fwhm += in->fwhm() * d;
    }

    return { float(height/inverseDistanceSum),
            deg(offset/inverseDistanceSum),
            float(fwhm/inverseDistanceSum) };
}

// Interpolates peak infos to a single point using idw.
itf_t interpolateValues(deg searchRadius, const PeakInfos& infos, deg alpha, deg beta)
{
    std::vector<PeakInfo const*> interpolationInfos;
    std::vector<double> distances;
    searchInQuadrants(
        allQuadrants(), alpha, beta, searchRadius, infos, interpolationInfos, distances);
    // Check that infos were found in all quadrants.
    int numQuadrantsOk = 0;
    for_i (NUM_QUADRANTS) {
        if (interpolationInfos.at(i)) {
            ++numQuadrantsOk;
            continue;
        }
        // No info found in quadrant? Try another quadrant.
        // See J.Appl.Cryst.(2011),44,641 for the angle mapping.
        eQuadrant newQ = remapQuadrant(eQuadrant(i));
        double const newAlpha = i == int(eQuadrant::NORTHEAST) || i == int(eQuadrant::SOUTHEAST)
            ? 180 - alpha
            : -alpha;
        double newBeta = beta < 180 ? beta + 180 : beta - 180;
        std::vector<PeakInfo const*> renewedSearch;
        std::vector<double> newDistance;
        searchInQuadrants(
            { newQ }, newAlpha, newBeta, searchRadius, infos, renewedSearch, newDistance);
        ASSERT(renewedSearch.size() == 1);
        ASSERT(newDistance.size() == 1);
        if (renewedSearch.front()) {
            interpolationInfos[i] = renewedSearch.front();
            distances[i] = newDistance.front();
            ++numQuadrantsOk;
        }
    }
    // Use inverse distance weighing if everything is alright.
    if (numQuadrantsOk == NUM_QUADRANTS)
        return inverseDistanceWeighing(distances, interpolationInfos);
    else
        return {};
}

} // namespace

//  ***********************************************************************************************
//  exported function
//  ***********************************************************************************************

//! Interpolates infos to equidistant grid in alpha and beta.
void algo::interpolateInfos(QProgressBar* progressBar)
{
    if (!gSession->interpol().enabled()) {
        gSession->setInterpolatedPeakInfos({});
        return;
    }
    qDebug() << "interpolation began";

    double stepAlpha   = gSession->interpol().stepAlpha();
    double stepBeta    = gSession->interpol().stepBeta();
    double idwRadius   = gSession->interpol().idwRadius();
    double avgAlphaMax = gSession->interpol().avgAlphaMax();
    double avgRadius   = gSession->interpol().avgRadius();
    int    threshold   = gSession->interpol().threshold();

    // Two interpolation methods are used here:
    // If grid point alpha <= averagingAlphaMax, points within averagingRadius
    // will be averaged.
    // If averaging fails, or alpha > averagingAlphaMax, inverse distance weighing
    // will be used.

    // NOTE We expect all infos to have the same gamma range.

    // REVIEW qRound oder qCeil?
    int numAlphas = qRound(90. / stepAlpha);
    int numBetas = qRound(360. / stepBeta);

    PeakInfos tmp; // Output data.
    const PeakInfos& infos = gSession->directPeakInfos();

    Progress progress(progressBar, "interpolation", numAlphas * numBetas); // TODO check number + 1?

    for_int (i, numAlphas + 1) { // TODO why + 1 ?
        deg const alpha = i * stepAlpha;
        for_int (j, numBetas) {
            deg const beta = j * stepBeta;

            progress.step();

            if (infos.peaks().empty()) {
                tmp.append(PeakInfo(alpha, beta));
                continue;
            }

            if (alpha <= avgAlphaMax) {
                // Use averaging.

                std::vector<itf_t> itfs;
                searchPoints(alpha, beta, avgRadius, infos, itfs);

                if (!itfs.empty()) {
                    // If treshold < 1, we'll only use a fraction of largest
                    // peak parameter values.
                    std::sort(itfs.begin(), itfs.end(), [](const itf_t& i1, const itf_t& i2) {
                        return i1.inten < i2.inten;
                    });

                    itf_t avg(0, 0, 0);

                    int iEnd = itfs.size();
                    int iBegin = qMax(0, qMin(qRound(itfs.size() * (1. - threshold)), iEnd - 1));
                    ASSERT(iBegin < iEnd);
                    int n = iEnd - iBegin;

                    for (int i = iBegin; i < iEnd; ++i)
                        avg += itfs.at(i);

                    tmp.append(PeakInfo(alpha, beta, infos.peaks().front().rgeGma(),
                                        avg.inten / n, float(Q_QNAN),
                        avg.tth / n, deg(Q_QNAN), avg.fwhm / n, float(Q_QNAN)));
                    continue;
                }

                if (qIsNaN(idwRadius)) {
                    // Don't fall back to idw, just add an unmeasured info.
                    tmp.append(PeakInfo(alpha, beta));
                    continue;
                }
            }

            // Use idw, if alpha > avgAlphaMax OR averaging failed (too small avgRadius?).
            itf_t itf = interpolateValues(idwRadius, infos, alpha, beta);
            tmp.append(PeakInfo(
                           alpha, beta, infos.peaks().front().rgeGma(), itf.inten,
                           float(Q_QNAN), itf.tth, deg(Q_QNAN), itf.fwhm, float(Q_QNAN)));
        }
    }

    gSession->setInterpolatedPeakInfos(std::move(tmp));
    qDebug() << "interpolation ended";
}
