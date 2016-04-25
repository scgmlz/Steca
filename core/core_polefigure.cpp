// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      core_polefigure.cpp
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#include "core_polefigure.h"
#include <qmath.h>

namespace core { namespace pole {
//------------------------------------------------------------------------------

// Calculates the difference of two angles. Parameters should be in [0, 360].
qreal calculateDeltaBeta(qreal beta1, qreal beta2) {
  // Due to cyclicity of angles (360 is equivalent to 0), some magic is needed.
  qreal deltaBeta = beta1 - beta2;
  qreal tempDelta = deltaBeta - 360;
  if (qAbs(tempDelta) < qAbs(deltaBeta)) deltaBeta = tempDelta;
  tempDelta = deltaBeta + 360;
  if (qAbs(tempDelta) < qAbs(deltaBeta)) deltaBeta = tempDelta;
  ASSERT(-180 <= deltaBeta && deltaBeta <= 180)
  return deltaBeta;
}

// Calculates the angle between two points on a unit sphere.
qreal angle(qreal alpha1, qreal alpha2,
            qreal deltaBeta) {
  alpha1 = deg2rad(alpha1);
  alpha2 = deg2rad(alpha2);
  deltaBeta = deg2rad(deltaBeta);
  // Absolute value of deltaBeta is not needed because cos is an even function.
  auto a = rad2deg(acos(cos(alpha1) * cos(alpha2) + sin(alpha1) * sin(alpha2) * cos(deltaBeta)));
  ASSERT(0<=a && a<=180)
  return a;
}

namespace Quadrant {
  enum Quadrant {
    NORTHEAST,
    SOUTHEAST,
    SOUTHWEST,
    NORTHWEST,
    NUM_QUADRANTS
  };
}

uint_vec allQuadrants() {
  return uint_vec(Quadrant::NUM_QUADRANTS, 0);
}

bool inQuadrant(int quadrant,
                qreal deltaAlpha, qreal deltaBeta) noexcept {
  switch (quadrant) {
  case Quadrant::NORTHEAST:
    return deltaAlpha >= 0 && deltaBeta >= 0;
  case Quadrant::SOUTHEAST:
    return deltaAlpha >= 0 && deltaBeta < 0;
  case Quadrant::SOUTHWEST:
    return deltaAlpha < 0 && deltaBeta < 0;
  case Quadrant::NORTHWEST:
    return deltaAlpha < 0 && deltaBeta >= 0;
  default:
    NEVER_HERE return false;
  }
}

// Search quadrant remapping in case no point was found.
Quadrant::Quadrant remapQuadrant(Quadrant::Quadrant const Q) {
  switch(Q) {
  case Quadrant::NORTHEAST: return Quadrant::NORTHWEST;
  case Quadrant::SOUTHEAST: return Quadrant::SOUTHWEST;
  case Quadrant::SOUTHWEST: return Quadrant::NORTHEAST;
  case Quadrant::NORTHWEST: return Quadrant::SOUTHEAST;
  default: NEVER_HERE return Quadrant::NUM_QUADRANTS;
  }
}

// Checks if (alpha,beta) is inside radius from (centerAlpha,centerBeta).
bool inRadius(qreal alpha, qreal beta,
              qreal centerAlpha, qreal centerBeta,
              qreal radius) {
  auto a = angle(alpha, centerAlpha, calculateDeltaBeta(beta, centerBeta));
  return qAbs(a) < radius;
}

// Adds data from reflection infos within radius from alpha and beta
// to the peak parameter lists.
void searchPoints(qreal alpha, qreal beta, qreal radius,
                  ReflectionInfos const& infos,
                  qreal_vec &peakOffsets,
                  qreal_vec &peakHeights,
                  qreal_vec &FWHMs) {
  // REVIEW Use value trees to improve performance.
  for (auto const& info : infos) {
    if (inRadius(info.alpha(), info.beta(), alpha, beta, radius)) {
      peakOffsets.append(info.tth());
      peakHeights.append(info.inten());
      FWHMs.append(info.fwhm());
    }
  }
}

// Searches closest ReflectionInfos to given alpha and beta in quadrants.
void searchInQuadrants(
    uint_vec const& quadrants,
    qreal alpha, qreal beta,
    qreal searchRadius,
    ReflectionInfos const& infos,
    QVector<ReflectionInfo const*> & foundInfos,
    qreal_vec & distances) {
  ASSERT(quadrants.size() <= Quadrant::NUM_QUADRANTS);
  // Take only reflection infos with beta within +/- BETA_LIMIT degrees into
  // account. Original STeCa used something like +/- 1.5*36 degrees.
  qreal const BETA_LIMIT = 30;
  distances.fill(std::numeric_limits<qreal>::max(), quadrants.size());
  foundInfos.fill(nullptr, quadrants.size());
  // Find infos closest to given alpha and beta in each quadrant.
  for (auto const& info : infos) {
    // REVIEW We could do better with value trees than looping over all infos.
    auto deltaBeta = calculateDeltaBeta(info.beta(), beta);
    if (fabs(deltaBeta) > BETA_LIMIT) continue;
    auto deltaAlpha = info.alpha() - alpha;
    // "Distance" between grid point and current info.
    auto d = angle(alpha, info.alpha(), deltaBeta);
    for (int i = 0; i < quadrants.size(); ++i) {
      if (inQuadrant(quadrants[i], deltaAlpha, deltaBeta)) {
        if (d >= distances[i]) continue;
        distances[i] = d;
        if (qIsNaN(searchRadius) || d < searchRadius) {
          foundInfos[i] = &info;
        }
      }
    }
  }
}

void inverseDistanceWeighing(qreal_vec const& distances,
                             QVector<ReflectionInfo const*> const& infos,
                             ReflectionInfo& out) {
  // Generally, only distances.size() == values.size() > 0 is needed for this
  // algorithm. However, in this context we expect exactly the following:
  RUNTIME_CHECK(distances.size() == Quadrant::NUM_QUADRANTS,
                "distances size should be 4");
  RUNTIME_CHECK(infos.size() == Quadrant::NUM_QUADRANTS,
                "infos size should be 4");
  QVector<qreal> inverseDistances(Quadrant::NUM_QUADRANTS);
  qreal inverseDistanceSum = 0;
  for_i (Quadrant::NUM_QUADRANTS) {
    if (distances[i] == 0) {
      // Points coincide; no need to interpolate.
      auto &in = infos.at(i);
      out.setInten(in->inten());
      out.setTth(in->tth());
      out.setFwhm(in->fwhm());
      return;
    }
    inverseDistances[i] = 1 / distances[i];
    inverseDistanceSum += inverseDistances[i];
  }
  // REVIEW The RAW peak may need different handling.
  qreal offset = 0;
  qreal height = 0;
  qreal fwhm = 0;
  for_i (Quadrant::NUM_QUADRANTS) {
    auto &in = infos.at(i);
    offset += in->tth()   * inverseDistances[i];
    height += in->inten() * inverseDistances[i];
    fwhm   += in->fwhm()  * inverseDistances[i];
  }

  out.setInten(height / inverseDistanceSum);
  out.setTth(offset / inverseDistanceSum);
  out.setFwhm(fwhm / inverseDistanceSum);
}

// Interpolates reflection infos to a single point using idw.
void interpolateValues(qreal searchRadius,
                       ReflectionInfos const& infos,
                       ReflectionInfo& out) {
  QVector<ReflectionInfo const*> interpolationInfos;
  qreal_vec distances;
  searchInQuadrants(allQuadrants(),
                    out.alpha(), out.beta(),
                    searchRadius,
                    infos,
                    interpolationInfos,
                    distances);
  // Check that infos were found in all quadrants.
  uint numQuadrantsOk = 0;
  for (int i = 0; i < Quadrant::NUM_QUADRANTS; ++i) {
    if (interpolationInfos[i]) {
      ++numQuadrantsOk;
      continue;
    }
    // No info found in quadrant? Try another quadrant. See
    // [J.Appl.Cryst.(2011),44,641] for the angle mapping.
    int   const newQ = remapQuadrant(static_cast<Quadrant::Quadrant>(i));
    qreal const newAlpha
      = i == Quadrant::NORTHEAST || i == Quadrant::SOUTHEAST ?
        180 - out.alpha() : -out.alpha();
    qreal newBeta = out.beta() < 180 ?   out.beta() + 180
                                     : out.beta() - 180;
    QVector<ReflectionInfo const*> renewedSearch;
    QVector<qreal> newDistance;
    searchInQuadrants(QVector<uint>(1, newQ),
                      newAlpha, newBeta, searchRadius,
                      infos, renewedSearch, newDistance);
    ASSERT(renewedSearch.size() == 1);
    ASSERT(newDistance.size() == 1);
    if (renewedSearch.front()) {
      interpolationInfos[i] = renewedSearch.front();
      distances[i] = newDistance.front();
      ++numQuadrantsOk;
    }
  }
  // Use inverse distance weighing if everything is alright.
  if (numQuadrantsOk == Quadrant::NUM_QUADRANTS) {
    inverseDistanceWeighing(distances, interpolationInfos, out);
  }
}

// Interpolates infos to equidistant grid in alpha and beta.
ReflectionInfos interpolate(ReflectionInfos const& infos,
                            qreal alphaStep, qreal betaStep,
                            qreal averagingAlphaMax, qreal averagingRadius,
                            qreal idwRadius, qreal inclusionTreshold)
{
  // Two interpolation methods are used here:
  // If grid point alpha <= averagingAlphaMax, points within averagingRadius
  // will be averaged.
  // If averaging fails, or alpha > averagingAlphaMax, inverse distance weighing
  // will be used.

  ASSERT(!infos.isEmpty());
  ASSERT(alphaStep > 0 && alphaStep <= 90);
  ASSERT(betaStep > 0 && betaStep <= 360);
  ASSERT(averagingAlphaMax >= 0 && averagingAlphaMax <= 90);
  ASSERT(averagingRadius >= 0);
  // Setting idwRadius = NaN disables idw radius checks and falling back to
  // idw when averaging fails.
  ASSERT(qIsNaN(idwRadius) || idwRadius >= 0);
  ASSERT(inclusionTreshold >= 0 && inclusionTreshold <= 1);

  // NOTE We expect all infos to have the same gamma range.

  int const numAlphas = qCeil(90 / alphaStep);
  int const numBetas  = qCeil(360 / betaStep);

  QVector<ReflectionInfo> interpolatedInfos; // Output data.
  interpolatedInfos.reserve(numAlphas * numBetas);

  for_int (i,numAlphas) {
    auto const alpha = i * alphaStep;
    for_int (j,numBetas) {
      auto const beta = j * betaStep;
      if (alpha <= averagingAlphaMax) {
        // Use averaging.
        QVector<qreal> tempPeakOffsets;
        QVector<qreal> tempPeakHeights;
        QVector<qreal> tempPeakFWHMs;
        searchPoints(alpha,
                     beta,
                     averagingRadius,
                     infos,
                     tempPeakOffsets,
                     tempPeakHeights,
                     tempPeakFWHMs);
        if (!tempPeakOffsets.isEmpty()) {
          // If inclusionTreshold < 1, we'll only use a fraction of largest
          // reflection parameter values.
          // REVIEW Does the sorting make sense? Should the offsets and FWHMs
          // be sorted according to heights?
          std::sort(tempPeakOffsets.begin(), tempPeakOffsets.end());
          std::sort(tempPeakHeights.begin(), tempPeakHeights.end());
          std::sort(tempPeakFWHMs.begin(), tempPeakFWHMs.end());
          qreal peakOffset = 0;
          qreal peakHeight = 0;
          qreal peakFWHM = 0;
          int kTreshold = qRound(  tempPeakOffsets.size()
                                 - tempPeakOffsets.size() * inclusionTreshold);
          if (kTreshold >= tempPeakOffsets.size())
            kTreshold = tempPeakOffsets.size() - 1;
          auto iterPeakOffsets = tempPeakOffsets.begin() + kTreshold;
          auto iterPeakHeights = tempPeakHeights.begin() + kTreshold;
          auto iterPeakFWHMs   = tempPeakFWHMs.begin()   + kTreshold;
          for (int k = kTreshold; k < tempPeakOffsets.size(); ++k) {
            peakOffset += *iterPeakOffsets;
            peakHeight += *iterPeakHeights;
            peakFWHM += *iterPeakFWHMs;
            ++iterPeakOffsets;
            ++iterPeakHeights;
            ++iterPeakFWHMs;
          }
          peakOffset /= tempPeakOffsets.size() - kTreshold;
          peakHeight /= tempPeakHeights.size() - kTreshold;
          peakFWHM   /= tempPeakFWHMs.size()   - kTreshold;

          interpolatedInfos.append(
            ReflectionInfo(alpha,
                           beta,
                           infos.first().rgeGamma(),
                           peakHeight,peakOffset,
                           peakFWHM));
          continue;
        }

        if (!qIsNaN(idwRadius)) {
          // Don't fall back to idw, just add an unmeasured info.
          ReflectionInfo invalidInfo(alpha,beta,infos.first().rgeGamma()); // TODO insert?
          interpolatedInfos.append(invalidInfo);
          continue;
        }
      }
      // Use idw, if alpha > averagingAlphaMax OR averaging failed (too small
      // averagingRadius?).
      ReflectionInfo interpolatedInfo(alpha,beta,infos.first().rgeGamma());
      interpolateValues(idwRadius, infos, interpolatedInfo);
      interpolatedInfos.append(interpolatedInfo);
    }
  }

  return interpolatedInfos;
}

//------------------------------------------------------------------------------
}}
// eof
