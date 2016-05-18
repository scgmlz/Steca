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

typedef QVector<ReflectionInfo const*> info_vec;

// Calculates the difference of two angles. Parameters should be in [0, 360].
deg calculateDeltaBeta(deg beta1, deg beta2) {
  // Due to cyclicity of angles (360 is equivalent to 0), some magic is needed.
  qreal deltaBeta = beta1 - beta2;
  qreal tempDelta = deltaBeta - 360;

  if (qAbs(tempDelta) < qAbs(deltaBeta)) deltaBeta = tempDelta;

  tempDelta = deltaBeta + 360;

  if (qAbs(tempDelta) < qAbs(deltaBeta)) deltaBeta = tempDelta;

  ENSURE(-180 <= deltaBeta && deltaBeta <= 180)
  return (deg)deltaBeta;
}

// Calculates the angle between two points on a unit sphere.
deg angle(deg alpha1, deg alpha2, deg deltaBeta) {
  // Absolute value of deltaBeta is not needed because cos is an even function.
  auto a = rad(acos(cos(alpha1.toRad()) * cos(alpha2.toRad()) +
                    sin(alpha1.toRad()) * sin(alpha2.toRad()) *
                        cos(deltaBeta.toRad())))
               .toDeg();
  ENSURE(0 <= a && a <= 180)
  return a;
}

enum class eQuadrant {
  NORTHEAST,
  SOUTHEAST,
  SOUTHWEST,
  NORTHWEST,
};

static int NUM_QUADRANTS = 4;

typedef QVector<eQuadrant> Quadrants;

Quadrants allQuadrants() {
  return {eQuadrant::NORTHEAST, eQuadrant::SOUTHEAST, eQuadrant::SOUTHWEST,
          eQuadrant::NORTHWEST};
}

bool inQuadrant(eQuadrant quadrant, deg deltaAlpha, deg deltaBeta) {
  switch (quadrant) {
  case eQuadrant::NORTHEAST:
    return deltaAlpha >= 0 && deltaBeta >= 0;
  case eQuadrant::SOUTHEAST:
    return deltaAlpha >= 0 && deltaBeta < 0;
  case eQuadrant::SOUTHWEST:
    return deltaAlpha < 0 && deltaBeta < 0;
  case eQuadrant::NORTHWEST:
    return deltaAlpha < 0 && deltaBeta >= 0;
  default:
    NEVER return false;
  }
}

// Search quadrant remapping in case no point was found.
eQuadrant remapQuadrant(eQuadrant q) {
  switch (q) {
  case eQuadrant::NORTHEAST:
    return eQuadrant::NORTHWEST;
  case eQuadrant::SOUTHEAST:
    return eQuadrant::SOUTHWEST;
  case eQuadrant::SOUTHWEST:
    return eQuadrant::NORTHEAST;
  case eQuadrant::NORTHWEST:
    return eQuadrant::SOUTHEAST;
  default:
    NEVER return (eQuadrant)0;
  }
}

// Checks if (alpha,beta) is inside radius from (centerAlpha,centerBeta).
bool inRadius(deg alpha, deg beta, deg centerAlpha, deg centerBeta,
              deg radius) {
  qreal a = angle(alpha, centerAlpha, calculateDeltaBeta(beta, centerBeta));
  return qAbs(a) < (qreal)radius;
}

// Adds data from reflection infos within radius from alpha and beta
// to the peak parameter lists.
void searchPoints(deg alpha, deg beta, deg radius, ReflectionInfos const& infos,
                  qreal_vec& peakOffsets, qreal_vec& peakHeights,
                  qreal_vec& FWHMs) {
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
void searchInQuadrants(Quadrants const& quadrants, deg alpha, deg beta,
                       deg searchRadius, ReflectionInfos const& infos,
                       info_vec& foundInfos,
                       qreal_vec&                      distances) {
  ENSURE(quadrants.size() <= NUM_QUADRANTS);
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

struct itf_t {
  inten_t inten;
  deg     tth;
  qreal   fwhm;
};

void inverseDistanceWeighing(qreal_vec const& distances, info_vec const& infos,
                             itf_t& itf) {
  // Generally, only distances.size() == values.size() > 0 is needed for this
  // algorithm. However, in this context we expect exactly the following:
  RUNTIME_CHECK(distances.size() == NUM_QUADRANTS,
                "distances size should be 4");
  RUNTIME_CHECK(infos.size() == NUM_QUADRANTS, "infos size should be 4");
  qreal_vec inverseDistances(NUM_QUADRANTS);
  qreal     inverseDistanceSum = 0;
  for_i (NUM_QUADRANTS) {
    if (distances[i] == 0) {
      // Points coincide; no need to interpolate.
      auto& in = infos.at(i);
      itf.inten = in->inten();
      itf.tth   = in->tth();
      itf.fwhm  = in->fwhm();
      return;
    }
    inverseDistances[i] = 1 / distances[i];
    inverseDistanceSum += inverseDistances[i];
  }
  // REVIEW The RAW peak may need different handling.
  qreal offset = 0;
  qreal height = 0;
  qreal fwhm   = 0;
  for_i (NUM_QUADRANTS) {
    auto& in = infos.at(i);
    offset += in->tth() * inverseDistances[i];
    height += in->inten() * inverseDistances[i];
    fwhm += in->fwhm() * inverseDistances[i];
  }

  itf.inten = height / inverseDistanceSum;
  itf.tth   = offset / inverseDistanceSum;
  itf.fwhm  = fwhm / inverseDistanceSum;
}

// Interpolates reflection infos to a single point using idw.
void interpolateValues(deg searchRadius, ReflectionInfos const& infos,
                       deg alpha, deg beta, itf_t& out) {
  info_vec interpolationInfos;
  qreal_vec                      distances;
  searchInQuadrants(allQuadrants(), alpha, beta, searchRadius,
                    infos, interpolationInfos, distances);
  // Check that infos were found in all quadrants.
  int numQuadrantsOk = 0;
  for_i (NUM_QUADRANTS) {
    if (interpolationInfos[i]) {
      ++numQuadrantsOk;
      continue;
    }
    // No info found in quadrant? Try another quadrant. See
    // [J.Appl.Cryst.(2011),44,641] for the angle mapping.
    eQuadrant    newQ = remapQuadrant((eQuadrant)i);
    qreal const newAlpha =
        i == (int)eQuadrant::NORTHEAST || i == (int)eQuadrant::SOUTHEAST
            ? 180 - alpha
            : -alpha;
    qreal newBeta = beta < 180 ? beta + 180 : beta - 180;
    info_vec renewedSearch;
    qreal_vec                      newDistance;
    searchInQuadrants({newQ}, newAlpha, newBeta, searchRadius, infos,
                      renewedSearch, newDistance);
    ENSURE(renewedSearch.size() == 1);
    ENSURE(newDistance.size() == 1);
    if (renewedSearch.front()) {
      interpolationInfos[i] = renewedSearch.front();
      distances[i]          = newDistance.front();
      ++numQuadrantsOk;
    }
  }
  // Use inverse distance weighing if everything is alright.
  if (numQuadrantsOk == NUM_QUADRANTS) {
    inverseDistanceWeighing(distances, interpolationInfos, out);
  }
}

// Interpolates infos to equidistant grid in alpha and beta.
ReflectionInfos interpolate(ReflectionInfos const& infos, deg alphaStep,
                            deg betaStep, deg averagingAlphaMax,
                            deg averagingRadius, deg idwRadius,
                            qreal inclusionTreshold) {
  // Two interpolation methods are used here:
  // If grid point alpha <= averagingAlphaMax, points within averagingRadius
  // will be averaged.
  // If averaging fails, or alpha > averagingAlphaMax, inverse distance weighing
  // will be used.

  EXPECT(0 <  alphaStep && alphaStep <= 90);
  EXPECT(0 <  betaStep  && betaStep  <= 360);
  EXPECT(0 <= averagingAlphaMax && averagingAlphaMax <= 90);
  EXPECT(0 <= averagingRadius);
  // Setting idwRadius = NaN disables idw radius checks and falling back to
  // idw when averaging fails.
  EXPECT(qIsNaN(idwRadius) || 0 <= idwRadius);
  EXPECT(0 <= inclusionTreshold && inclusionTreshold <= 1);

  // NOTE We expect all infos to have the same gamma range.

  int const numAlphas = qCeil(90  / alphaStep);
  int const numBetas  = qCeil(360 / betaStep);

  ReflectionInfos interpolatedInfos;  // Output data.
  interpolatedInfos.reserve(numAlphas * numBetas);

  for_int (i, numAlphas) {
    deg const alpha = i * alphaStep;
    for_int (j, numBetas) {
      deg const beta = j * betaStep;
      if (alpha <= averagingAlphaMax) {
        // Use averaging.
        qreal_vec tempPeakOffsets, tempPeakHeights, tempPeakFWHMs;
        searchPoints(alpha, beta, averagingRadius, infos, tempPeakOffsets,
                     tempPeakHeights, tempPeakFWHMs);

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
          qreal peakFWHM   = 0;
          int   kTreshold  = qRound(tempPeakOffsets.size() -
                                 tempPeakOffsets.size() * inclusionTreshold);
          if (kTreshold >= tempPeakOffsets.size())
            kTreshold          = tempPeakOffsets.size() - 1;
          auto iterPeakOffsets = tempPeakOffsets.begin() + kTreshold;
          auto iterPeakHeights = tempPeakHeights.begin() + kTreshold;
          auto iterPeakFWHMs   = tempPeakFWHMs.begin() + kTreshold;
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
          peakFWHM /= tempPeakFWHMs.size() - kTreshold;

          interpolatedInfos.append(ReflectionInfo(
              alpha, beta, infos.first().rgeGamma(), peakHeight,
              peakOffset, peakFWHM));
          continue;
        }

        if (!qIsNaN(idwRadius)) {
          // Don't fall back to idw, just add an unmeasured info.
          ReflectionInfo invalidInfo(alpha, beta,
                                     infos.first().rgeGamma());
          interpolatedInfos.append(invalidInfo);
          continue;
        }
      }
      // Use idw, if alpha > averagingAlphaMax OR averaging failed (too small
      // averagingRadius?).
      itf_t itf;
      interpolateValues(idwRadius, infos, alpha, beta, itf);
      interpolatedInfos.append(
        ReflectionInfo(alpha, beta, infos.first().rgeGamma(),
                       itf.inten, itf.tth, itf.fwhm));
    }
  }

  return interpolatedInfos;
}

//------------------------------------------------------------------------------
}}
// eof
