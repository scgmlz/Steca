//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/calc/export.cpp
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
#include "core/base/async.h"
#include "core/calc/onepeak_allinfos.h"
#include "core/data/cluster.h"
#include "core/typ/curve.h"
#include "core/typ/range.h"
#include "core/session.h"
#include <qmath.h>

// Covered by test011_export.

namespace {

//! Writes pole figure for one Bragg peak as alpha-beta-inten format list.

void writeFullInfoSequence(
    QTextStream& stream, const OnePeakAllInfos& peakInfos, const QString& separator)
{
    for (auto& info : peakInfos.peakInfos())
        stream << info.alpha() << separator
               << info.beta()  << separator
               << info.inten() << "\n";
}

//! Writes intensities (only!) for pole figure for one Bragg peak.

//! Makes sense for interpolated data only; assumes alpha-beta grid to be known.

void writeCompactInfoSequence(QTextStream& stream, const OnePeakAllInfos& peakInfos)
{
    int count = 0;
    for (auto& info : peakInfos.peakInfos()) {
        stream << info.inten();
        count = (count+1)%10;
        if (count == 0)
            stream << "\n";
        else
            stream << " ";
    }
    stream << "\n";
}

//! Writes pole figure for one Bragg peak.

void writeInfoSequence(QTextStream& stream, const OnePeakAllInfos& peakInfos, const QString& format)
{
    if (format=="pol")
        writeCompactInfoSequence(stream, peakInfos);
    else
        writeFullInfoSequence(stream, peakInfos, data_export::separator(format));
}

} // namespace

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
               CSTRI(templatedName));
    QString ret = templatedName;
    int nDigits = (int)log10((double)maxNum)+1;
    ret.replace("%d", QString("%1").arg(num, nDigits, 10, QLatin1Char('0')));
    return ret;
}

//! Writes one diffractogram.

void data_export::writeCurve(
    QTextStream& stream, const Curve& curve, const Cluster* cluster,
    const Range& rgeGma, const QString& separator)
{
    if (curve.isEmpty())
        qFatal("curve is empty");
    ASSERT(rgeGma.isValid());
    const Metadata& md = cluster->avgMetadata();
    stream << "#Comment: " << md.comment << '\n';
    stream << "#Date: " << md.date << '\n';
    stream << "#Gamma range min: " << rgeGma.min << '\n';
    stream << "#Gamma range max: " << rgeGma.max << '\n';

    for (int i=0; i<Metadata::numAttributes(false); ++i)
        stream << "#" << Metadata::attributeTag(i, false) << ": "
               << md.attributeValue(i).toDouble() << '\n';

    stream << "#Tth" << separator << "Intensity" << '\n';
    for (int i=0; i<curve.xs().size(); ++i)
        stream << curve.x(i) << separator << curve.y(i) << '\n';

    stream.flush(); // not sure whether we need this
}

//! Writes diffractogram at given index.

void data_export::writeDfgram(QTextStream& stream, const int idx, const QString& format)
{
    const int nSlices = gSession->gammaSelection.numSlices.val();
    const int iSlice = idx%nSlices;
    const int iCluster = idx/nSlices;
    const Cluster* cluster = gSession->activeClusters.clusters.yield().at(iCluster);
    const Range gmaStripe = gSession->gammaSelection.slice2range(cluster->rangeGma(), iSlice);
    const Curve& curve = cluster->dfgrams.yield_at(iSlice,cluster).curve;
    data_export::writeCurve(stream, curve, cluster, gmaStripe, separator(format));
}

//! Writes current diffractogram.

void data_export::writeCurrentDfgram(QTextStream& stream, const QString& format)
{
    const Cluster* cluster = gSession->currentCluster();
    ASSERT(cluster);
    const Curve& curve = cluster->currentDfgram().curve;
    data_export::writeCurve(stream, curve, cluster, cluster->rangeGma(), separator(format));
}

//! Writes all diffractograms, with appropriate subheaders.

void data_export::writeAllDfgrams(
    QTextStream& stream, TakesLongTime& progress, const QString& format)
{
    const int nSlice = qMax(1, gSession->gammaSelection.numSlices.val());
    const int nCluster = gSession->activeClusters.size();
    const QString separator = data_export::separator(format);
    ASSERT(progress.total()==nCluster*nSlice);
    for (int iCluster=0; iCluster<nCluster; ++iCluster) {
        const Cluster* cluster = gSession->activeClusters.clusters.yield().at(iCluster);
        for (int iSlice=0; iSlice<nSlice; ++iSlice) {
            const Range gmaStripe = gSession->gammaSelection.slice2range(
                cluster->rangeGma(), iSlice);
            const Curve& curve = cluster->dfgrams.yield_at(iSlice,cluster).curve;
            stream << "Picture Nr: " << iCluster+1 << '\n';
            if (nSlice > 1)
                stream << "Gamma slice Nr: " << iSlice+1 << '\n';
            writeCurve(stream, curve, cluster, gmaStripe, separator);
            progress.step();
        }
    }
}

//! Writes x-y data from current diagram.

void data_export::writeDiagram(QTextStream& stream, const QString& separator)
{
    // get data
    const int idxX = int(gSession->params.diagramX.val());
    const int idxY = int(gSession->params.diagramY.val());
    std::vector<double> xs, ys, ysSigma;
    gSession->peaksOutcome.currentInfoSequence()->getValuesAndSigma(idxX, idxY, xs, ys, ysSigma);

    // write header
    QStringList tags = gSession->numericAsciiKeys();
    stream << "# " << tags[idxX] << separator << tags[idxY];
    if (ysSigma.size() > 0)
        stream << separator << "sigma_"  << tags[idxY];
    stream << '\n';

    // write data
    for (size_t i = 0; i < xs.size(); ++i) {
        stream << xs[i] << separator << ys[i];
        if (ysSigma.size() > 0)
            stream << separator << ysSigma[i];
        stream << '\n';
    }
}

//! Writes pole figure for current Bragg peak.

void data_export::writeCurrentInfoSequence(QTextStream& stream, const QString& format)
{
    const auto* infos = gSession->peaksOutcome.currentInfoSequence();
    ASSERT(infos);
    writeInfoSequence(stream, *infos, format);
}

//! Writes pole figure for Bragg peak at given index.

void data_export::writeOneInfoSequence(QTextStream& stream, int idx, const QString& format)
{
    const auto* infos = gSession->peaksOutcome.allInfoSequences().at(idx);
    ASSERT(infos);
    writeInfoSequence(stream, *infos, format);
}

//! Writes pole figures for all Bragg peaks, with appropriate subheaders.

void data_export::writeAllInfoSequence(
    QTextStream& stream, TakesLongTime& progress, const QString& format)
{
    int iPeak = 0;
    for (const auto* peak : gSession->peaksOutcome.allInfoSequences()) {
        stream << "Peak Nr: " << ++iPeak << '\n';
        writeInfoSequence(stream, *peak, format);
        progress.step();
    }
}
