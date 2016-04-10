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

#include "types/core_type_geometry.h"
#include "core_fit_fitting.h"
#include "core_session.h"
#include "core_reflection_info.h"

#include <algorithm>
#include <qmath.h>

namespace core {
//------------------------------------------------------------------------------

qreal calculateDeltaBeta(qreal beta1, qreal beta2) noexcept {
  qreal deltaBeta = beta1 - beta2;
  qreal tempDelta = deltaBeta - 2 * M_PI;
  if (qAbs(tempDelta) < qAbs(deltaBeta)) deltaBeta = tempDelta;
  tempDelta = deltaBeta + 2 * M_PI;
  if (qAbs(tempDelta) < qAbs(deltaBeta)) deltaBeta = tempDelta;
  return deltaBeta;
}

// Calculates the angle between two points on a unit sphere.
qreal angle(qreal alpha1, qreal alpha2,
            qreal deltaBeta) noexcept {
  return acos(
      cos(alpha1) * cos(alpha2)
    + sin(alpha1) * sin(alpha2) * cos(deltaBeta)
  );
}

// Checks if (alpha,beta) is inside radius from (centerAlpha,centerBeta).
bool inRadius(qreal alpha, qreal beta,
              qreal centerAlpha, qreal centerBeta,
              qreal radius) noexcept {
  return   qAbs(angle(alpha, centerAlpha, calculateDeltaBeta(beta, centerBeta)))
         < radius;
}

namespace Quadrant {
  enum Quadrant {
    NORTHEAST,
    SOUTHEAST,
    SOUTHWEST,
    NORTHWEST,
    MAX_QUADRANTS
  };
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

Quadrant::Quadrant remapQuadrant(Quadrant::Quadrant const Q) {
  switch(Q) {
  case Quadrant::NORTHEAST: return Quadrant::NORTHWEST;
  case Quadrant::SOUTHEAST: return Quadrant::SOUTHWEST;
  case Quadrant::SOUTHWEST: return Quadrant::NORTHEAST;
  case Quadrant::NORTHWEST: return Quadrant::SOUTHEAST;
  default: NEVER_HERE return Quadrant::MAX_QUADRANTS;
  }
}

static Range gammaRangeAt(shp_Lens lens, qreal const tth) {
  auto s = lens->size();
  RUNTIME_CHECK(s.width() > 0 && s.height() > 0, "invalid image size");
  Range r;
  auto angles = lens->angles(0, 0);
  auto nextAngles = lens->angles(1, 0);
  for_ij (s.width(), s.height()) {
    if (tth >= angles.tth && tth < nextAngles.tth) {
      r.extendBy(angles.gamma);
    }
    angles = nextAngles;
    nextAngles = lens->angles(i + 1, j);
  }
  return r;
}

template<typename Container>
qreal inverseDistanceWeighing(Container const& distances,
                              Container const& values) {
  // Generally, only distances.size() == values.size() > 0 is needed for this
  // algorithm. However, in this context we expect exactly the following:
  RUNTIME_CHECK(distances.size() == Quadrant::MAX_QUADRANTS,
                "distances size should be 4");
  RUNTIME_CHECK(values.size() == Quadrant::MAX_QUADRANTS,
                "values size should be 4");
  Container inverseDistances;
  for (const auto& d : distances) inverseDistances.push_back(1 / d);
  qreal const inverseDistanceSum
    = std::accumulate(inverseDistances.cbegin(), inverseDistances.cend(), 0.0);
  return std::inner_product(values.begin(),
                            values.end(),
                            inverseDistances.begin(),
                            0.0)
            / inverseDistanceSum;
}

template<typename Container>
bool inSearchRadius(qreal const radius, Container const& deltaZs) {
  RUNTIME_CHECK(deltaZs.size() == Quadrant::MAX_QUADRANTS,
                "incorrect container size");
  return std::all_of(deltaZs.cbegin(), deltaZs.cend(), [radius](qreal deltaZ) {
    return deltaZ <= radius;
  });
}

bool searchPointsInQuadrant(int const quadrant,
                            qreal const deltaAlpha,
                            qreal const deltaBeta,
                            ReflectionInfo const& info,
                            qreal &peakOffset,
                            qreal &peakHeight,
                            qreal &peakFWHM) {
  if (inQuadrant(quadrant, deltaAlpha, deltaBeta)) {
    peakOffset = info.peakPosition().x;
    peakHeight = info.peakPosition().y;
    peakFWHM = info.peakFWHM();
    return true;
  }

  return false;
}

//------------------------------------------------------------------------------

Polefigure::Polefigure(rcSession session,
                       rcDatasets datasets,
                       Reflection const& reflection,
                       qreal const alphaStep,
                       qreal const betaStep,
                       rcRange rgeTth,
                       Range gammaRange)
: _alphaStep(alphaStep)
, _betaStep(betaStep)
, _FWHMs()
, _peakPositions()
, _reflectionInfos(NUM_BETAS)
, _reflection(reflection)
{
  // Fill the reflection info lists.
  for (auto const& dataset: datasets) {
    auto lenses = session.lens(true, true, session.norm(), *dataset);
    if (!gammaRange.isValid()) {
      const auto& reflectionRange = _reflection.range();
      gammaRange = gammaRangeAt(lenses, reflectionRange.center());
    }

    const auto numGammaRows = qCeil(gammaRange.width() / _betaStep);
    const auto gammaStep = gammaRange.width() / numGammaRows;

    for (int j = 0; j < numGammaRows; ++j) {
      Range gammaStripe;
      gammaStripe.min = gammaRange.min + j * gammaStep;
      gammaStripe.max = gammaStripe.min + gammaStep;
      const auto info = ReflectionInfo::make(session,
                                             datasets, *dataset,
                                                    _reflection,
                                                    rgeTth,
                                                    gammaStripe);
      uint iBeta = qFloor(rad2Deg(info.beta()) / NUM_BETAS);
      _reflectionInfos[iBeta].push_back(info);
    }
  }
}

void Polefigure::generate(qreal centerRadius,
                          qreal centerSearchRadius,
                          qreal intensityTreshold,
                          qreal searchRadius) {
  _FWHMs.clear();
  _peakPositions.clear();

  for (int i = 0; i < qCeil(M_PI / 2 / _alphaStep); ++i) {
    const auto alpha = i * _alphaStep;
    for (int j = 0; j < qCeil(2 * M_PI / _betaStep); ++j) {
      const auto beta = j * _betaStep;
      if (alpha <= centerRadius) {
        QList<qreal> tempPeakOffsets;
        QList<qreal> tempPeakHeights;
        QList<qreal> tempPeakFWHMs;
        searchPoints(alpha,
                     beta,
                     centerSearchRadius,
                     tempPeakOffsets,
                     tempPeakHeights,
                     tempPeakFWHMs);
        if (!tempPeakOffsets.isEmpty()) {
          // REVIEW Does the sorting make sense? Should the offsets and FWHMs
          // be sorted according to heights?
          std::sort(tempPeakOffsets.begin(), tempPeakOffsets.end());
          std::sort(tempPeakHeights.begin(), tempPeakHeights.end());
          std::sort(tempPeakFWHMs.begin(), tempPeakFWHMs.end());
          qreal peakOffset = 0;
          qreal peakHeight = 0;
          qreal peakFWHM = 0;
          auto iterPeakOffsets = tempPeakOffsets.begin();
          auto iterPeakHeights = tempPeakHeights.begin();
          auto iterPeakFWHMs = tempPeakFWHMs.begin();
          int kTreshold = qRound(  tempPeakOffsets.size()
                                 - tempPeakOffsets.size() * intensityTreshold);
          if (kTreshold >= tempPeakOffsets.size())
            kTreshold = tempPeakOffsets.size() - 1;
          for (int k = 0; k < tempPeakOffsets.size(); ++k) {
            if (k >= kTreshold) {
              peakOffset += *iterPeakOffsets;
              peakHeight += *iterPeakHeights;
              peakFWHM += *iterPeakFWHMs;
            }
            ++iterPeakOffsets;
            ++iterPeakHeights;
            ++iterPeakFWHMs;
          }
          peakOffset /= tempPeakOffsets.size() - kTreshold;
          peakHeight /= tempPeakHeights.size() - kTreshold;
          peakFWHM /= tempPeakFWHMs.size() - kTreshold;
          _FWHMs.push_back(peakFWHM);
          _peakPositions.push_back(XY(peakOffset, peakHeight));

          continue;
        }
        if (!qIsNaN(searchRadius)) {
          _FWHMs.push_back(-1);
          _peakPositions.push_back(XY(-1, -1));

          continue;
        }
      }
      // No reflection infos were found for the polefigure center.
      // If no infos are found at all, the output values will be negative
      // as some analysis software treat -1 as an unmeasured area.
      qreal peakOffset = -1;
      qreal peakHeight = -1;
      qreal peakFWHM   = -1;
      searchPointsInAllQuadrants(alpha,
                                 beta,
                                 searchRadius,
                                 peakOffset,
                                 peakHeight,
                                 peakFWHM);
      _peakPositions.push_back(XY(peakOffset, peakHeight));
      _FWHMs.push_back(peakFWHM);
    }
  }
}

void Polefigure::searchPoints(qreal const alpha,
                              qreal const beta,
                              qreal const radius,
                              QList<qreal> &peakOffsets,
                              QList<qreal> &peakHeights,
                              QList<qreal> &FWHMs) const {
  for (int i = 0; i < 10; i++) { // Why 10 and not NUM_BETAS? Noone knows.
    for (int j=0; j < _reflectionInfos[i].count(); j++) {
      if (inRadius(_reflectionInfos[i][j].alpha(),
                   _reflectionInfos[i][j].beta(),
                   alpha,
                   beta,
                   radius)) {
        peakOffsets.push_back(_reflectionInfos[i][j].peakPosition().x);
        peakHeights.push_back(_reflectionInfos[i][j].peakPosition().y);
        FWHMs.push_back(_reflectionInfos[i][j].peakFWHM());
      }
    }
  }
}


void Polefigure::searchPointsInAllQuadrants(qreal const alpha,
                                            qreal const beta,
                                            qreal const searchRadius,
                                            qreal &peakOffset,
                                            qreal &peakHeight,
                                            qreal &peakFWHM) const {
  int const iBegin = qFloor(rad2Deg(beta) / Polefigure::NUM_BETAS - 1);
  int const iEnd = iBegin + 3; // A magic number.

  for (int i = iBegin; i < iEnd; ++i) {
    // REVIEW Index magick. Is the number 10 linked to searchPoints()?
    int j;
    if (i >= 10) j = i - 10;
    else if (i < 0) j = i + 10;
    else j = i;

    for (int k = 0; k < _reflectionInfos[j].count(); ++k) {
      const auto deltaAlpha = _reflectionInfos[j][k].alpha() - alpha;
      const auto deltaBeta
        = calculateDeltaBeta(_reflectionInfos[j][k].beta(), beta);
      const auto deltaZ
        = angle(alpha, _reflectionInfos[j][k].alpha(), deltaBeta);
      QVector<qreal> tempDeltaZs(Quadrant::MAX_QUADRANTS);
      QVector<qreal> tempPeakOffsets(Quadrant::MAX_QUADRANTS);
      QVector<qreal> tempPeakHeights(Quadrant::MAX_QUADRANTS);
      QVector<qreal> tempPeakFWHMs(Quadrant::MAX_QUADRANTS);
      for (int iQ = 0; iQ < Quadrant::MAX_QUADRANTS; ++iQ) {
        tempDeltaZs[iQ] = deltaZ;
        if (!searchPointsInQuadrant(iQ,
                                    deltaAlpha,
                                    deltaBeta,
                                    _reflectionInfos[j][k],
                                    tempPeakOffsets[iQ],
                                    tempPeakHeights[iQ],
                                    tempPeakFWHMs[iQ])) {
          // Try another quadrant. See [J.Appl.Cryst.(2011),44,641] for the
          // angle mapping and quadrant mirroring.
          int   const newQ = remapQuadrant(static_cast<Quadrant::Quadrant>(iQ));
          qreal const newAlpha = M_PI - alpha;
          qreal const newBeta = beta < M_PI ? beta + M_PI : beta - M_PI;
          const auto newDeltaAlpha
            = _reflectionInfos[j][k].alpha() - newAlpha;
          const auto newDeltaBeta
            = calculateDeltaBeta(_reflectionInfos[j][k].beta(), newBeta);
          tempDeltaZs[iQ]
            = angle(newAlpha, _reflectionInfos[j][k].alpha(), newDeltaBeta);
          searchPointsInQuadrant(newQ,
                                 newDeltaAlpha,
                                 newDeltaBeta,
                                 _reflectionInfos[j][k],
                                 tempPeakOffsets[iQ],
                                 tempPeakHeights[iQ],
                                 tempPeakFWHMs[iQ]);
        }
      }
      if (qIsNaN(searchRadius) || inSearchRadius(searchRadius, tempDeltaZs)) {
        peakOffset = inverseDistanceWeighing(tempDeltaZs, tempPeakOffsets);
        peakHeight = inverseDistanceWeighing(tempDeltaZs, tempPeakHeights);
        peakFWHM   = inverseDistanceWeighing(tempDeltaZs, tempPeakFWHMs);
      }
    }
  }
}

//------------------------------------------------------------------------------
}
// eof
