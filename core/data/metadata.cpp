// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/data/metadata.cpp
//! @brief     Implements class Metadata
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "metadata.h"
#include "core/def/idiomatic_for.h"

// metadata attributes

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

Metadata::Metadata()
    : date()
    , comment()
    , motorXT(0)
    , motorYT(0)
    , motorZT(0)
    , motorOmg(0)
    , motorTth(0)
    , motorPhi(0)
    , motorChi(0)
    , motorPST(0)
    , motorSST(0)
    , motorOMGM(0)
    , nmT(0)
    , nmTeload(0)
    , nmTepos(0)
    , nmTeext(0)
    , nmXe(0)
    , nmYe(0)
    , nmZe(0)
    , monitorCount(0)
    , deltaMonitorCount(0)
    , time(0)
    , deltaTime(0) {}

uint Metadata::numAttributes(bool onlyNum) {
    return uint(onlyNum ? eAttr::NUM_NUMERICAL_ATTRIBUTES : eAttr::NUM_ALL_ATTRIBUTES);
}

rcstr Metadata::attributeTag(uint i, bool out) {
    return attributeTags(out).at(i);
}

QStringList Metadata::attributeTags(bool out) {
    static QStringList const tags = {
        "X",   "Y",   "Z",    "ω",      "mid 2θ", "φ",     "χ",       "PST",
        "SST", "ΩM",  "T",    "teload", "tepos",  "teext", "xe",      "ye",
        "ze",  "mon", "Δmon", "t",      "Δt",     "date",  "comment",
    };

    static QStringList const outTags = {
        "X",   "Y",      "Z",         "omega",  "mid2theta", "phi",   "chi",     "PST",
        "SST", "OmegaM", "T",         "teload", "tepos",     "teext", "xe",      "ye",
        "ze",  "mon",    "delta_mon", "t",      "delta_t",   "date",  "comment",
    };

    return out ? outTags : tags;
}

cmp_vec Metadata::attributeCmps() {
    static cmp_vec const cmps = {
        cmp_real, cmp_real, cmp_real, cmp_real, cmp_real, cmp_real, cmp_real, cmp_real,
        cmp_real, cmp_real, cmp_real, cmp_real, cmp_real, cmp_real, cmp_real, cmp_real,
        cmp_real, cmp_real, cmp_real, cmp_real, cmp_real, cmp_date, cmp_str,
    };
    return cmps;
}

str Metadata::attributeStrValue(uint i) const {
    qreal value = 0;

    switch (eAttr(i)) {
    case eAttr::MOTOR_XT: value = motorXT; break;
    case eAttr::MOTOR_YT: value = motorYT; break;
    case eAttr::MOTOR_ZT: value = motorZT; break;
    case eAttr::MOTOR_OMG: value = motorOmg; break;
    case eAttr::MOTOR_TTH: value = motorTth; break;
    case eAttr::MOTOR_PHI: value = motorPhi; break;
    case eAttr::MOTOR_CHI: value = motorChi; break;
    case eAttr::MOTOR_PST: value = motorPST; break;
    case eAttr::MOTOR_SST: value = motorSST; break;
    case eAttr::MOTOR_OMGM: value = motorOMGM; break;

    case eAttr::T: value = nmT; break;
    case eAttr::TELOAD: value = nmTeload; break;
    case eAttr::TEPOS: value = nmTepos; break;
    case eAttr::TEEXT: value = nmTeext; break;
    case eAttr::XE: value = nmXe; break;
    case eAttr::YE: value = nmYe; break;
    case eAttr::ZE: value = nmZe; break;

    case eAttr::MONITOR_COUNT: value = monitorCount; break;
    case eAttr::DELTA_MONITOR_COUNT: value = deltaMonitorCount; break;

    case eAttr::TIME: value = time; break;
    case eAttr::DELTA_TIME: value = deltaTime; break;

    case eAttr::DATE: return date;
    case eAttr::COMMENT: return comment;

    default: NEVER;
    }

    return str::number(value);
}

QVariant Metadata::attributeValue(uint i) const {
    switch (eAttr(i)) {
    case eAttr::DATE: return date;
    case eAttr::COMMENT: return comment;
    case eAttr::MOTOR_XT: return qreal(motorXT);
    case eAttr::MOTOR_YT: return qreal(motorYT);
    case eAttr::MOTOR_ZT: return qreal(motorZT);
    case eAttr::MOTOR_OMG: return qreal(motorOmg);
    case eAttr::MOTOR_TTH: return qreal(motorTth);
    case eAttr::MOTOR_PHI: return qreal(motorPhi);
    case eAttr::MOTOR_CHI: return qreal(motorChi);
    case eAttr::MOTOR_PST: return qreal(motorPST);
    case eAttr::MOTOR_SST: return qreal(motorSST);
    case eAttr::MOTOR_OMGM: return qreal(motorOMGM);
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
    default: NEVER return 0;
    }
}

row_t Metadata::attributeValues() const {
    row_t attrs;
    for_i (uint(eAttr::NUM_ALL_ATTRIBUTES))
        attrs.append(attributeValue(i));
    return attrs;
}

row_t Metadata::attributeNaNs() {
    static row_t row;
    if (row.isEmpty())
        for_i (uint(eAttr::NUM_ALL_ATTRIBUTES))
            row.append(NAN);
    return row;
}
