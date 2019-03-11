//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/raw/metadata.cpp
//! @brief     Implements class Metadata
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/raw/metadata.h"
//#include "qcr/base/debug.h"

namespace {
enum class eAttr {
    MOTOR_XT,
    MOTOR_YT,
    MOTOR_ZT,
    MOTOR_OMG,
    MOTOR_TTH,
    MOTOR_PHI,
    MOTOR_CHI,
    MOTOR_PST,
    MOTOR_SST,
    MOTOR_OMGM,
    T,
    TELOAD,
    TEPOS,
    TEEXT,
    XE,
    YE,
    ZE,
    MONITOR_COUNT,
    DELTA_MONITOR_COUNT,
    TIME,
    DELTA_TIME,

    NUM_NUMERICAL_ATTRIBUTES,

    // non-numbers must come last
    DATE = NUM_NUMERICAL_ATTRIBUTES,
    COMMENT,

    NUM_ALL_ATTRIBUTES
};
} // namespace


Metadata::Metadata()
    : date{}
    , comment {}
    , motorXT {0}
    , motorYT {0}
    , motorZT {0}
    , motorOmg {0}
    , motorTth {0}
    , motorPhi {0}
    , motorChi {0}
    , motorPST {0}
    , motorSST {0}
    , motorOMGM {0}
    , nmT {0}
    , nmTeload {0}
    , nmTepos {0}
    , nmTeext {0}
    , nmXe {0}
    , nmYe {0}
    , nmZe {0}
    , monitorCount {0}
    , deltaMonitorCount {0}
    , time {0}
    , deltaTime {0}
{}

int Metadata::numAttributes(bool onlyNum)
{
    return int(onlyNum ? eAttr::NUM_NUMERICAL_ATTRIBUTES : eAttr::NUM_ALL_ATTRIBUTES);
}

const QString& Metadata::attributeTag(int i, bool nice)
{
    return attributeTags(nice).at(i);
}

namespace {

    const QStringList niceTags = {
        "X",   "Y",   "Z",    "ω",      "mid 2θ", "φ",     "χ",       "PST",
        "SST", "ΩM",  "T",    "teload", "tepos",  "teext", "xe",      "ye",
        "ze",  "mon", "Δmon", "t",      "Δt",     "date",  "comment",
    };

    const QStringList asciiTags = {
        "X",   "Y",      "Z",         "omega",  "mid2theta", "phi",   "chi",     "PST",
        "SST", "OmegaM", "T",         "teload", "tepos",     "teext", "xe",      "ye",
        "ze",  "mon",    "delta_mon", "t",      "delta_t",   "date",  "comment",
    };

}

const QStringList& Metadata::attributeTags(bool nice)
{
    return nice ? niceTags : asciiTags;
}

QString Metadata::attributeStrValue(int i) const
{
    return attributeValue(i).toString();
}

QVariant Metadata::attributeValue(int i) const
{
    switch (eAttr(i)) {
    case eAttr::DATE: return date;
    case eAttr::COMMENT: return comment;
    case eAttr::MOTOR_XT: return double(motorXT);
    case eAttr::MOTOR_YT: return double(motorYT);
    case eAttr::MOTOR_ZT: return double(motorZT);
    case eAttr::MOTOR_OMG: return double(motorOmg);
    case eAttr::MOTOR_TTH: return double(motorTth);
    case eAttr::MOTOR_PHI: return double(motorPhi);
    case eAttr::MOTOR_CHI: return double(motorChi);
    case eAttr::MOTOR_PST: return double(motorPST);
    case eAttr::MOTOR_SST: return double(motorSST);
    case eAttr::MOTOR_OMGM: return double(motorOMGM);
    case eAttr::T: return nmT;
    case eAttr::TELOAD: return nmTeload;
    case eAttr::TEPOS: return nmTepos;
    case eAttr::TEEXT: return nmTeext;
    case eAttr::XE: return nmXe;
    case eAttr::YE: return nmYe;
    case eAttr::ZE: return nmZe;
    case eAttr::MONITOR_COUNT: return monitorCount;
    case eAttr::DELTA_MONITOR_COUNT: return deltaMonitorCount;
    case eAttr::TIME: return time;
    case eAttr::DELTA_TIME: return deltaTime;
    default: qFatal("impossible case");
    }
}

std::vector<QVariant> Metadata::attributeValues() const
{
    std::vector<QVariant> attrs;
    for (int i=0; i<int(eAttr::NUM_ALL_ATTRIBUTES); ++i)
        attrs.push_back(attributeValue(i));
    return attrs;
}

std::vector<QVariant> Metadata::attributeNaNs()
{
    return std::vector<QVariant>(int(eAttr::NUM_ALL_ATTRIBUTES), Q_QNAN);
}

//! Return average over list of metadata.
Metadata Metadata::computeAverage(const std::vector<const Metadata*>& vec)
{
    Metadata ret;
    const Metadata* firstMd = vec.front();
    ret.date = firstMd->date;
    ret.comment = firstMd->comment;

    // sums: delta mon. count and time,
    // takes the last ones (presumed the maximum) of mon. count and time,
    // averages the rest
    for (const Metadata* d : vec) {

        ret.motorXT += d->motorXT;
        ret.motorYT += d->motorYT;
        ret.motorZT += d->motorZT;

        ret.motorOmg += d->motorOmg;
        ret.motorTth += d->motorTth;
        ret.motorPhi += d->motorPhi;
        ret.motorChi += d->motorChi;

        ret.motorPST += d->motorPST;
        ret.motorSST += d->motorSST;
        ret.motorOMGM += d->motorOMGM;

        ret.nmT += d->nmT;
        ret.nmTeload += d->nmTeload;
        ret.nmTepos += d->nmTepos;
        ret.nmTeext += d->nmTeext;
        ret.nmXe += d->nmXe;
        ret.nmYe += d->nmYe;
        ret.nmZe += d->nmZe;

        ret.deltaMonitorCount += d->deltaMonitorCount;
        ret.deltaTime += d->deltaTime;

        ret.monitorCount = d->monitorCount;
        ret.time = d->time;
    }

    double fac = 1.0 / vec.size();

    ret.motorXT *= fac;
    ret.motorYT *= fac;
    ret.motorZT *= fac;

    ret.motorOmg *= fac;
    ret.motorTth *= fac;
    ret.motorPhi *= fac;
    ret.motorChi *= fac;

    ret.motorPST *= fac;
    ret.motorSST *= fac;
    ret.motorOMGM *= fac;

    ret.nmT *= fac;
    ret.nmTeload *= fac;
    ret.nmTepos *= fac;
    ret.nmTeext *= fac;
    ret.nmXe *= fac;
    ret.nmYe *= fac;
    ret.nmZe *= fac;

    return ret;
}
