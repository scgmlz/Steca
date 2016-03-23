#include "core_polefigure.h"

#include "core_curve.h"
#include "core_fit_fitting.h"
#include "core_session.h"

#include <algorithm>
#include <cmath>
#include <QLinkedList>
#include <QtMath>

namespace core {
//------------------------------------------------------------------------------

qreal calculateDeltaBeta(const qreal beta1, const qreal beta2) noexcept {
  qreal deltaBeta = beta1 - beta2;
  qreal tempDelta = deltaBeta - 2 * M_PI;
  if (std::abs(tempDelta) < std::abs(deltaBeta)) deltaBeta = tempDelta;
  tempDelta = deltaBeta + 2 * M_PI;
  if (std::abs(tempDelta) < std::abs(deltaBeta)) deltaBeta = tempDelta;
  return deltaBeta;
}

// Calculates the angle between two points on a unit sphere.
qreal angle(const qreal alpha1, const qreal alpha2,
            const qreal deltaBeta) noexcept {
  return std::acos(
      std::cos(alpha1) * std::cos(alpha2)
    + std::sin(alpha1) * std::sin(alpha2) * std::cos(deltaBeta)
  );
}

// Checks if (alpha,beta) is inside radius from (centerAlpha,centerBeta).
bool inRadius(const qreal alpha, const qreal beta,
              const qreal centerAlpha, const qreal centerBeta,
              const qreal radius) noexcept {
  return   std::abs(angle(alpha, centerAlpha, calculateDeltaBeta(beta, centerBeta)))
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

bool inQuadrant(const int quadrant,
                const qreal deltaAlpha, const qreal deltaBeta) noexcept {
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

Range gammaRangeAt(shp_LensSystem lenses, qreal const tth) {
  const auto s = lenses->getSize();
  RUNTIME_CHECK(s.width() > 0 && s.height() > 0, "invalid image size");
  Range r;
  auto angles = lenses->getAngles(0, 0);
  auto nextAngles = lenses->getAngles(1, 0);
  for (int iy = 0; iy < s.height(); ++iy) {
    for (int ix = 0; ix < s.width() - 1; ++ix) {
      if (tth >= angles.tth && tth < nextAngles.tth) {
        r.extend(angles.gamma);
      }
      angles = nextAngles;
      nextAngles = lenses->getAngles(ix + 1, iy);
    }
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
  const qreal inverseDistanceSum
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
    peakOffset = info.getPeakPosition().x;
    peakHeight = info.getPeakPosition().y;
    peakFWHM = info.getPeakFWHM();
    return true;
  }
  return false;
}

//------------------------------------------------------------------------------

Polefigure::Polefigure(Session &session,
                       shp_File file,
                       Reflection const& reflection_,
                       qreal const alphaStep_,
                       qreal const betaStep_,
                       Range gammaRange)
: alphaStep(alphaStep_)
 ,betaStep(betaStep_)
 ,FWHMs()
 ,peakPositions()
 ,reflectionInfos(NUM_BETAS)
 ,reflection(&reflection_)
{
  // Fill the reflection info lists.
  for (uint i = 0; i < file->numDatasets(); ++i) {
    auto dataset = file->getDataset(i);
    auto lenses = session.allLenses(*dataset, false);
    if (!gammaRange.isValid()) {
      const auto& reflectionRange = reflection->getRange();
      gammaRange = gammaRangeAt(lenses, reflectionRange.center());
    }

    const auto numGammaRows = qCeil(gammaRange.width() / betaStep);
    const auto gammaStep = gammaRange.width() / numGammaRows;

    for (int j = 0; j < numGammaRows; ++j) {
      Range gammaStripe;
      gammaStripe.min = gammaRange.min + j * gammaStep;
      gammaStripe.max = gammaStripe.min + gammaStep;
      const auto info = dataset->makeReflectionInfo(session,
                                                    *reflection,
                                                    gammaStripe);
      const uint iBeta = qFloor(deg_rad(info.getBeta()) / NUM_BETAS);
      reflectionInfos[iBeta].push_back(info);
    }
  }
}

void Polefigure::generate(qreal const centerRadius,
                          qreal const centerSearchRadius,
                          qreal const intensityTreshold,
                          qreal const searchRadius) {
  FWHMs.clear();
  peakPositions.clear();

  for (int i = 0; i < qCeil(M_PI / 2 / alphaStep); ++i) {
    const auto alpha = i * alphaStep;
    for (int j = 0; j < qCeil(2 * M_PI / betaStep); ++j) {
      const auto beta = j * betaStep;
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
        if (!tempPeakOffsets.empty()) {
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
          FWHMs.push_back(peakFWHM);
          peakPositions.push_back(XY(peakOffset, peakHeight));

          continue;
        }
        if (!qIsNaN(searchRadius)) {
          FWHMs.push_back(-1);
          peakPositions.push_back(XY(-1, -1));

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
      peakPositions.push_back(XY(peakOffset, peakHeight));
      FWHMs.push_back(peakFWHM);
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
    for (int j=0; j < reflectionInfos[i].size(); j++) {
      if (inRadius(reflectionInfos[i][j].getAlpha(),
                   reflectionInfos[i][j].getBeta(),
                   alpha,
                   beta,
                   radius)) {
        peakOffsets.push_back(reflectionInfos[i][j].getPeakPosition().x);
        peakHeights.push_back(reflectionInfos[i][j].getPeakPosition().y);
        FWHMs.push_back(reflectionInfos[i][j].getPeakFWHM());
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
  const int iBegin = qFloor(deg_rad(beta) / Polefigure::NUM_BETAS - 1);
  const int iEnd = iBegin + 3; // A magic number.

  for (int i = iBegin; i < iEnd; ++i) {
    // REVIEW Index magick. Is the number 10 linked to searchPoints()?
    int j;
    if (i >= 10) j = i - 10;
    else if (i < 0) j = i + 10;
    else j = i;
    
    for (int k = 0; k < reflectionInfos[j].size(); ++k) {
      const auto deltaAlpha = reflectionInfos[j][k].getAlpha() - alpha;
      const auto deltaBeta
        = calculateDeltaBeta(reflectionInfos[j][k].getBeta(), beta);
      const auto deltaZ
        = angle(alpha, reflectionInfos[j][k].getAlpha(), deltaBeta);
      QVector<qreal> tempDeltaZs(Quadrant::MAX_QUADRANTS);
      QVector<qreal> tempPeakOffsets(Quadrant::MAX_QUADRANTS);
      QVector<qreal> tempPeakHeights(Quadrant::MAX_QUADRANTS);
      QVector<qreal> tempPeakFWHMs(Quadrant::MAX_QUADRANTS);
      for (int iQ = 0; iQ < Quadrant::MAX_QUADRANTS; ++iQ) {
        tempDeltaZs[iQ] = deltaZ;
        if (!searchPointsInQuadrant(iQ,
                                    deltaAlpha,
                                    deltaBeta,
                                    reflectionInfos[j][k],
                                    tempPeakOffsets[iQ],
                                    tempPeakHeights[iQ],
                                    tempPeakFWHMs[iQ])) {
          // Try another quadrant. See [J.Appl.Cryst.(2011),44,641] for the
          // angle mapping and quadrant mirroring.
          const int newQ = remapQuadrant(static_cast<Quadrant::Quadrant>(iQ));
          const double newAlpha = M_PI - alpha;
          const qreal newBeta = beta < M_PI ? beta + M_PI : beta - M_PI;
          const auto newDeltaAlpha
            = reflectionInfos[j][k].getAlpha() - newAlpha;
          const auto newDeltaBeta
            = calculateDeltaBeta(reflectionInfos[j][k].getBeta(), newBeta);
          tempDeltaZs[iQ]
            = angle(newAlpha, reflectionInfos[j][k].getAlpha(), newDeltaBeta);
          searchPointsInQuadrant(newQ,
                                 newDeltaAlpha,
                                 newDeltaBeta,
                                 reflectionInfos[j][k],
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
