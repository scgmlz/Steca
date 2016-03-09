#include "core_polefigure.h"

#include <cmath>
#include <Eigen/Core>

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

qreal deltaBeta(const qreal beta1, const qreal beta2) noexcept {
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
  return   std::abs(angle(alpha, centerAlpha, deltaBeta(beta, centerBeta)))
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

//------------------------------------------------------------------------------
}

// eof
