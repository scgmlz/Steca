//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/data/export.cpp
//! @brief     Implements
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "export.h"
#include "qcr/base/debug.h"
#include "core/data/cluster.h"
#include "core/typ/curve.h"
#include "core/typ/range.h"
#include <qmath.h>

// Covered by test011_export.

QString data_export::numberedFileName(const QString& templatedName, int num, int maxNum) {
    if (!templatedName.contains("%d"))
        qFatal("path does not contain placeholder %%d");
    QString ret = templatedName;
    int nDigits = (int)log10((double)maxNum)+1;
    ret.replace("%d", QString("%1").arg(num, nDigits, 10, QLatin1Char('0')));
    return ret;
}

void data_export::writeCurve(
    QTextStream& stream, const Curve& curve, const Cluster* cluster,
    const Range& rgeGma, const QString& separator)
{
    if (curve.isEmpty())
        qFatal("curve is empty");
    ASSERT(rgeGma.isValid());
    const Metadata& md = cluster->avgMetadata();
    stream << "Comment: " << md.comment << '\n';
    stream << "Date: " << md.date << '\n';
    stream << "Gamma range min: " << rgeGma.min << '\n';
    stream << "Gamma range max: " << rgeGma.max << '\n';

    for (int i=0; i<Metadata::numAttributes(true); ++i)
        stream << Metadata::attributeTag(i, true) << ": "
               << md.attributeValue(i).toDouble() << '\n';

    stream << "Tth" << separator << "Intensity" << '\n';
    for (int i=0; i<curve.xs().size(); ++i)
        stream << curve.x(i) << separator << curve.y(i) << '\n';

    stream.flush(); // not sure whether we need this
}
