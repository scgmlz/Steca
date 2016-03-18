#include "core_polefigure.h"

#include "core_curve.h"
#include "core_fit_fitting.h"
#include "core_session.h"

#include <algorithm>
#include <cmath>
#include <Eigen/Core>
#include <QLinkedList>
#include <QtMath>

namespace core {
//------------------------------------------------------------------------------

using matrix3d = Eigen::Matrix<qreal,3,3>;
using vector3d = Eigen::Matrix<qreal,3,1>;

// Returns a clockwise rotation matrix around the x axis.
matrix3d rotationCWx(const qreal angle) {
  matrix3d m;
  m <<  1,       0,                0
       ,0, std::cos(angle), -std::sin(angle)
       ,0, std::sin(angle),  std::cos(angle);
  return m;
}

// Returns a counterclockwise rotation matrix around the x axis.
matrix3d rotationCCWx(const qreal angle) {
  return rotationCWx(angle).transpose();
}

// Returns a clockwise rotation matrix around the y axis.
matrix3d rotationCWy(const qreal angle) {
  matrix3d m;
  m <<   std::cos(angle), 0, std::sin(angle)
       ,       0        , 1,       0
       ,-std::sin(angle), 0, std::cos(angle);
  return m;
}

// Returns a counterclockwise rotation matrix around the y axis.
matrix3d rotationCCWy(const qreal angle) {
  return rotationCWy(angle).transpose();
}

// Returns a clockwise rotation matrix around the z axis.
matrix3d rotationCWz(const qreal angle) {
  matrix3d m;
  m <<   std::cos(angle), -std::sin(angle), 0
        ,std::sin(angle),  std::cos(angle), 0
        ,      0        ,        0        , 1;
  return m;
}

// Returns a counterclockwise rotation matrix around the z axis.
matrix3d rotationCCWz(const qreal angle) {
    return rotationCWz(angle).transpose();
}

// Calculates the polefigure coordinates alpha and beta with regards to
// sample orientation and diffraction angles.
void calculateAlphaBeta(const qreal omgDet, const qreal phiDet,
                        const qreal chiDet,
                        const qreal tthRef, const qreal gammaRef,
                        qreal& alpha, qreal& beta) {
  // Note that the rotations here do not correspond to C. Randau's dissertation.
  // The rotations given in [J. Appl. Cryst. (2012) 44, 641-644] are incorrect.
  const vector3d rotated =   rotationCWz (phiDet)
                           * rotationCWx (chiDet)
                           * rotationCWz (omgDet)
                           * rotationCWx (gammaRef)
                           * rotationCCWz(tthRef / 2)
                           * vector3d(0,1,0);
  alpha = std::acos(rotated(2,0));
  beta  = std::atan2(rotated(0,0), rotated(1,0));
  // Mirror angles.
  if (alpha > M_PI / 2) {
    alpha = std::abs(alpha - M_PI);
    beta += beta < 0 ? M_PI : -M_PI;
  }
  // Keep beta between 0 and 2pi.
  if (beta < 0)
    beta += 2 * M_PI;
}

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

Range gammaRange(shp_LensSystem lenses, qreal const tth) {
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
                            Polefigure::Point const& point,
                            qreal &peakOffset,
                            qreal &peakHeight,
                            qreal &peakFWHM) {
  if (inQuadrant(quadrant, deltaAlpha, deltaBeta)) {
    peakOffset = point.peakPosition.x;
    peakHeight = point.peakPosition.y;
    peakFWHM = point.peakFWHM;
    return true;
  }
  return false;
}

//------------------------------------------------------------------------------

Polefigure::Point::Point(qreal const alpha_,
                         qreal const beta_,
                         XY const& position,
                         qreal const FWHM)
  :  alpha(alpha_)
    ,beta(beta_)
    ,peakPosition(position)
    ,peakFWHM(FWHM)
{
}

Polefigure::Point Polefigure::makePoint(Session const& session,
                                        Dataset const& dataset,
                                        shp_LensSystem lenses,
                                        Reflection const& reflection,
                                        Range const& gammaStripe) {
  auto gammaCutCurve = makeCurve(lenses,
                                 gammaStripe,
                                 session.getCut().tth_regular);
  const fit::Polynomial stripeBg
    = fit::fitBackground(gammaCutCurve, session.getBgRanges(),
                         session.getBgPolynomial().getDegree());
  gammaCutCurve.subtractFunction(stripeBg);
  auto peakFunction = reflection.makePeakFunction();
  fit::fitPeak(*peakFunction,
               gammaCutCurve,
               reflection.getRange());
  qreal alpha;
  qreal beta;
  calculateAlphaBeta(
    dataset.getNumericalAttributeValue(Dataset::MOTOR_OMG),
    dataset.getNumericalAttributeValue(Dataset::MOTOR_PHI),
    dataset.getNumericalAttributeValue(Dataset::MOTOR_CHI),
    reflection.getRange().center(),
    gammaStripe.center(),
    alpha,
    beta
  );
  return Point(alpha,
               beta,
               peakFunction->getFitPeak(),
               peakFunction->getFitFWHM());
}


Polefigure::Polefigure(Session &session,
                       shp_File file,
                       Reflection const& reflection_,
                       qreal const alphaStep_,
                       qreal const betaStep_,
                       Range gammaRge)
: alphaStep(alphaStep_)
 ,betaStep(betaStep_)
 ,FWHMs()
 ,peakPositions()
 ,points(NUM_BETAS)
 ,reflection(&reflection_)
{
  // Fill the points lists.
  for (uint i = 0; i < file->numDatasets(); ++i) {
    auto dataset = file->getDataset(i);
    auto lenses = session.allLenses(*dataset, false);
    if (!gammaRge.isValid()) {
      const auto& reflectionRange = reflection->getRange();
      gammaRge = gammaRange(lenses, reflectionRange.center());
    }

    const auto numGammaRows = qCeil(gammaRge.width() / betaStep);
    const auto gammaStep = gammaRge.width() / numGammaRows;

    for (int j = 0; j < numGammaRows; ++j) {
      Range gammaStripe;
      gammaStripe.min = gammaRge.min + j * gammaStep;
      gammaStripe.max = gammaStripe.min + gammaStep;
      const auto p = makePoint(session,
                               *dataset,
                               lenses,
                               *reflection,
                               gammaStripe);
      const uint iBeta = qFloor(deg_rad(p.beta) / NUM_BETAS);
      points[iBeta].push_back(p);
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
      // No points were found for the polefigure center.
      // If no points are found at all, the output values will be negative
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
    for (int j=0; j < points[i].size(); j++) {
      if (inRadius(points[i][j].alpha, points[i][j].beta, alpha, beta, radius)) {
        peakOffsets.push_back(points[i][j].peakPosition.x);
        peakHeights.push_back(points[i][j].peakPosition.y);
        FWHMs.push_back(points[i][j].peakFWHM);
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
    
    for (int k = 0; k < points[j].size(); ++k) {
      const auto deltaAlpha = points[j][k].alpha - alpha;
      const auto deltaBeta = calculateDeltaBeta(points[j][k].beta, beta);
      const auto deltaZ = angle(alpha, points[j][k].alpha, deltaBeta);
      QVector<qreal> tempDeltaZs(Quadrant::MAX_QUADRANTS);
      QVector<qreal> tempPeakOffsets(Quadrant::MAX_QUADRANTS);
      QVector<qreal> tempPeakHeights(Quadrant::MAX_QUADRANTS);
      QVector<qreal> tempPeakFWHMs(Quadrant::MAX_QUADRANTS);
      for (int iQ = 0; iQ < Quadrant::MAX_QUADRANTS; ++iQ) {
        tempDeltaZs[iQ] = deltaZ;
        if (!searchPointsInQuadrant(iQ,
                                    deltaAlpha,
                                    deltaBeta,
                                    points[j][k],
                                    tempPeakOffsets[iQ],
                                    tempPeakHeights[iQ],
                                    tempPeakFWHMs[iQ])) {
          // Try another quadrant. See [J.Appl.Cryst.(2011),44,641] for the
          // angle mapping and quadrant mirroring.
          const int newQ = remapQuadrant(static_cast<Quadrant::Quadrant>(iQ));
          const double newAlpha = M_PI - alpha;
          const qreal newBeta = beta < M_PI ? beta + M_PI : beta - M_PI;
          const auto newDeltaAlpha = points[j][k].alpha - newAlpha;
          const auto newDeltaBeta = calculateDeltaBeta(points[j][k].beta, newBeta);
          tempDeltaZs[iQ] = angle(newAlpha, points[j][k].alpha, newDeltaBeta);
          searchPointsInQuadrant(newQ,
                                 newDeltaAlpha,
                                 newDeltaBeta,
                                 points[j][k],
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
