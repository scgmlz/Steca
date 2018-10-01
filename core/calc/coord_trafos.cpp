//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/calc/coord_trafos.cpp
//! @brief     Implements function calculateAlphaBeta
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/calc/coord_trafos.h"
#include "core/calc/matrix.h"
#include <qmath.h>

//! Calculates the polefigure coordinates alpha and beta with regards to
//! sample orientation and diffraction angles.

//! tth: Center of peak's 2theta interval.
//! gma: Center of gamma slice.
void algo::calculateAlphaBeta(deg& alpha, deg& beta, deg tth, deg gma, deg chi, deg omg, deg phi)
{
    // Rotate a unit vector initially parallel to the y axis with regards to the
    // angles. As a result, the vector is a point on a unit sphere
    // corresponding to the location of a polefigure point.
    // Note that the rotations here do not correspond to C. Randau's dissertation.
    // The rotations given in [J. Appl. Cryst. (2012) 44, 641-644] are incorrect.
    vec3r rotated = mat3r::rotationCWz(phi.toRad())
        * mat3r::rotationCWx(chi.toRad())
        * mat3r::rotationCWz(omg.toRad())
        * mat3r::rotationCWx(gma.toRad())
        * mat3r::rotationCCWz(tth.toRad() / 2) * vec3r(0, 1, 0);

    // Extract alpha (latitude) and beta (longitude).
    rad alphaRad = acos(rotated._2);
    rad betaRad  = atan2(rotated._0, rotated._1);

    // If alpha is in the wrong hemisphere, mirror both alpha and beta over the
    // center of a unit sphere.
    if (alphaRad > M_PI_2) { // REVIEW - seems like that happens only for a very narrow ring
        alphaRad = qAbs(alphaRad - M_PI);
        betaRad = betaRad + (betaRad < 0 ? M_PI : -M_PI);
    }

    // Keep beta between 0 and 2pi.
    if (betaRad < 0)
        betaRad = betaRad + 2 * M_PI;

    alpha = alphaRad.toDeg();
    beta = betaRad.toDeg();
}
