//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/calc/onepeak_allinfos.cpp
//! @brief     Implements class InfoSequence
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/calc/onepeak_allinfos.h"
#include "core/session.h"
#include "qcr/base/debug.h" // ASSERT

namespace {

// sorts xs and ys the same way, by (x,y)
static void sortColumns(std::vector<double>& xs, std::vector<double>& ys, std::vector<int>& is)
{
    ASSERT(xs.size() == ys.size());

    int count = xs.size();

    is.resize(count);
    for (int i=0; i<count; ++i)
        is[i] = i;

    std::sort(is.begin(), is.end(), [&xs, &ys](int i1, int i2) {
        double x1 = xs.at(i1), x2 = xs.at(i2);
        if (x1 < x2)
            return true;
        if (x1 > x2)
            return false;
        return ys.at(i1) < ys.at(i2); });

    std::vector<double> r(count);

    for (int i=0; i<count; ++i)
        r[i] = xs.at(is.at(i));
    xs = r;

    for (int i=0; i<count; ++i)
        r[i] = ys.at(is.at(i));
    ys = r;
}

} // namespace


void OnePeakAllInfos::appendPeak(PeakInfo&& info)
{
    peakInfos_.push_back(std::move(info));
}

//! Returns entries indexX and indexY, as sorted vectors X and Ylow,Y,Yhig, for use in diagrams.

void OnePeakAllInfos::get4(const int indexX, const int indexY,
                     std::vector<double>& xs, std::vector<double>& ys,
                     std::vector<double>& ysLow, std::vector<double>& ysHig) const
{
    int n = peakInfos_.size();
    xs.resize(n);
    ys.resize(n);

    for (int i=0; i<n; ++i) {
        const PeakInfo& peakInfo = peakInfos_.at(i);
        const std::vector<QVariant> row = peakData(peakInfo);
        xs[i] = row.at(indexX).toDouble();
        ys[i] = row.at(indexY).toDouble();
    }

    std::vector<int> is;
    sortColumns(xs, ys, is);

    const OnePeakSettings* peak = gSession->peaksSettings.selectedPeak();
    if (peak && !peak->isRaw() && gSession->hasSigma(indexY)) {
        ysLow.resize(n);
        ysHig.resize(n);
        for (int i=0; i<n; ++i) {
            const PeakInfo& peakInfo = peakInfos_.at(is.at(i));
            const std::vector<QVariant> row = peakData(peakInfo);
            double sigma = row.at(indexY+1).toDouble(); // SIGMA_X has tag position of X plus 1
            double y = ys.at(i);
            ysLow[i] = y - sigma;
            ysHig[i] = y + sigma;
        }
    } else {
        ysLow.resize(0);
        ysHig.resize(0);
    }
}

//! Returns entries indexX and indexY, as sorted vectors X and Ylow,Y,Yhig, for use in diagrams.

void OnePeakAllInfos::getValuesAndSigma(const size_t indexX, const size_t indexY,
                                     std::vector<double>& xs, std::vector<double>& ys,
                                     std::vector<double>& ysSigma) const
{
    size_t n = peakInfos_.size();
    xs.resize(n);
    ys.resize(n);

    for (size_t i=0; i<n; ++i) {
        const PeakInfo& peakInfo = peakInfos_.at(i);
        const std::vector<QVariant> row = peakData(peakInfo);
        xs[i] = row.at(indexX).toDouble();
        ys[i] = row.at(indexY).toDouble();
    }

    std::vector<int> is;
    sortColumns(xs, ys, is);

    const OnePeakSettings* peak = gSession->peaksSettings.selectedPeak();
    if (peak && !peak->isRaw() && gSession->hasSigma(indexY)) {

        ysSigma.resize(n);
        for (auto i : is) {
            const PeakInfo& peakInfo = peakInfos_.at(is.at(i));
            const std::vector<QVariant> row = peakData(peakInfo);
            ysSigma[i] = row.at(indexY+1).toDouble(); // SIGMA_X has tag position of X plus 1
        }
    } else {
        ysSigma.resize(0);
    }
}
