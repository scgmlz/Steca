//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/calc/info_sequence.cpp
//! @brief     Implements class InfoSequence
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/calc/info_sequence.h"
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


void InfoSequence::appendPeak(PeakInfo&& info)
{
    peaks_.push_back(std::move(info));
}

//! Returns entries idxX and idxY, as sorted vectors X and Ylow,Y,Yhig, for use in diagrams.

void InfoSequence::get4(const int idxX, const int idxY,
                     std::vector<double>& xs, std::vector<double>& ys,
                     std::vector<double>& ysLow, std::vector<double>& ysHig) const
{
    int n = peaks_.size();
    xs.resize(n);
    ys.resize(n);

    for (int i=0; i<n; ++i) {
        const std::vector<QVariant> row = peaks_.at(i).data();
        xs[i] = row.at(idxX).toDouble();
        ys[i] = row.at(idxY).toDouble();
    }

    std::vector<int> is;
    sortColumns(xs, ys, is);

    using eReflAttr = PeakInfo::eReflAttr;
    eReflAttr ye = (eReflAttr) idxY;
    const Peak* peak = gSession->peaks.selectedPeak();
    if (peak
        && !peak->isRaw()
        && (ye==eReflAttr::INTEN || ye==eReflAttr::TTH || ye==eReflAttr::FWHM)) {
        ysLow.resize(n);
        ysHig.resize(n);
        for (int i=0; i<n; ++i) {
            const std::vector<QVariant> row = peaks_.at(is.at(i)).data();
            double sigma = row.at(idxY+1).toDouble(); // SIGMA_X has tag position of X plus 1
            double y = ys.at(i);
            ysLow[i] = y - sigma;
            ysHig[i] = y + sigma;
        }
    } else {
        ysLow.resize(0);
        ysHig.resize(0);
    }
}


//! Returns entries idxX and idxY, as sorted vectors X and Ylow,Y,Yhig, for use in diagrams.

void InfoSequence::getValuesAndSigma(const size_t idxX, const size_t idxY,
                                     std::vector<double>& xs, std::vector<double>& ys,
                                     std::vector<double>& ysSigma) const
{
    size_t n = peaks_.size();
    xs.resize(n);
    ys.resize(n);

    for (size_t i=0; i<n; ++i) {
        const std::vector<QVariant> row = peaks_.at(i).data();
        xs[i] = row.at(idxX).toDouble();
        ys[i] = row.at(idxY).toDouble();
    }

    std::vector<int> is;
    sortColumns(xs, ys, is);

    using eReflAttr = PeakInfo::eReflAttr;
    eReflAttr ye = (eReflAttr) idxY;
    const Peak* peak = gSession->peaks.selectedPeak();
    if (peak
        && !peak->isRaw()
        && (ye==eReflAttr::INTEN || ye==eReflAttr::TTH || ye==eReflAttr::FWHM)) {

        ysSigma.resize(n);
        for (auto i : is) {
            const std::vector<QVariant> row = peaks_.at(i).data();
            ysSigma[i] = row.at(idxY+1).toDouble(); // SIGMA_X has tag position of X plus 1
        }
    } else {
        ysSigma.resize(0);
    }
}

//! For debugging only.

void InfoSequence::inspect(const QString& header) const
{
    qDebug() << header << "#peaks=" << peaks_.size();
    if (!peaks_.size())
        return;
    const PeakInfo& p = peaks_.front();
    qDebug() << " .. first entry: alpha=" << p.alpha() << "beta=" << p.beta()
             << "inten=" << p.inten();
    const PeakInfo& q = peaks_.back();
    qDebug() << " ..  last entry: alpha=" << q.alpha() << "beta=" << q.beta()
             << "inten=" << q.inten();
}
