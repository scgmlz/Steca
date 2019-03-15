//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/peakfit/raw_outcome.cpp
//! @brief     Implements class RawOutcome
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/peakfit/raw_outcome.h"
#include "core/typ/curve.h"
#include "qcr/base/debug.h"
#include <qmath.h>

//! Computes raw peak characteristics.

//! Given curve should be restricted to peak range, and corrected for background fit.

Mapped analyseRawPeak(const Curve& curve)
{
    int n = curve.size();
    if (n <= 0)
        return {};
    double intensity = 0;
    double center = 0;
    double stdv = 0; // TODO compute stdv in one pass
    for (int i=0; i<n; ++i) {
        double x = curve.x(i);
        double y = curve.y(i);
        intensity += y;
        center += x*y;
        stdv += x*x*y;
    }
    center /= intensity;
    stdv = sqrt( stdv/intensity - center*center );
    intensity *= curve.rgeX().width() / curve.size();
    Mapped ret;
    ret.setDouble("center", center);
    ret.setDouble("intensity", intensity);
    ret.setDouble("fwhm", sqrt(8*log(2))*stdv);
    return ret;
}
