//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/data/export.cpp
//! @brief     Implements namespace data_export
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "export.h"
#include "qcr/base/debug.h"
#include "core/calc/info_sequence.h"
#include "core/data/cluster.h"
#include "core/typ/curve.h"
#include "core/typ/range.h"
#include "core/session.h"
#include <qmath.h>

// Covered by test011_export.

const QStringList data_export::defaultFormats = { "dat", "csv" };

QString data_export::separator(const QString& format)
{
    if (format=="csv")
        return ", ";
    return " ";
}

QString data_export::numberedFileName(const QString& templatedName, int num, int maxNum)
{
    if (!templatedName.contains("%d"))
        qFatal("templated name '%s' does not contain placeholder '%%d'",
               templatedName.toLatin1().constData());
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

//! Writes pole figure for one Bragg peak.

void data_export::writeInfoSequence(
    QTextStream& stream, const InfoSequence& peakInfos, const QString& separator)
{
    for (auto& info : peakInfos.peaks())
        stream << info.alpha() << separator
               << info.beta()  << separator
               << info.inten() << "\n";
}

void data_export::saveDiagram(QFile* file, const QString& format, QcrDialog*)
{
    ASSERT(file);
    QTextStream stream(file);

    // get data
    const int idxX = int(gSession->params.diagramX.val());
    const int idxY = int(gSession->params.diagramY.val());
    std::vector<double> xs, ys, ysLow, ysHig;
    const InfoSequence* peakInfos = gSession->allPeaks.currentInfoSequence();
    ASSERT(peakInfos);
    peakInfos->get4(idxX, idxY, xs, ys, ysLow, ysHig);
    ASSERT(xs.size());
    // write data table
    const QString separator = data_export::separator(format);
    for (int i=0; i<xs.size(); ++i) {
        stream << xs[i] << separator << ys[i];
        if (ysLow.size())
            stream << separator << ysLow[i] << separator << ysHig[i];
        stream << '\n';
    }
}
