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

int Metadata::noNumAttr = 0;
std::vector<MetaDefinition> Metadata::metaKeys_ = Metadata::metaKeys();

MetaDefinition::MetaDefinition(QString niceName, averageMode avgmode, valueType valtype)
    : name{niceName}
    , mode{avgmode}
    , type{valtype}
{
    if (valtype == valueType::STRING)
        Metadata::noNumAttr++;
}

Metadata::Metadata()
    : Mapped{}
{}

int Metadata::numAttributes(bool onlyNum)
{
    return onlyNum ? metaKeys_.size()-noNumAttr : metaKeys_.size();
}

const QString& Metadata::attributeTag(int i, bool nice)
{
    return attributeTags(nice).at(i);
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
    if (i >= metaKeys_.size())
        qFatal("impossible case");
    else {
        MetaDefinition metaKey = metaKeys_.at(i);
        if (metaKey.type == valueType::DEG)
            return double(at<deg>(metaKey.name));
        else if (metaKey.type == valueType::STRING)
            return at<QString>(metaKey.name);
        else
            return at<double>(metaKey.name);
    }
}

std::vector<QVariant> Metadata::attributeValues() const
{
    std::vector<QVariant> attrs;
    for (int i=0; i<metaKeys_.size(); ++i)
        attrs.push_back(attributeValue(i));
    return attrs;
}

std::vector<QVariant> Metadata::attributeNaNs()
{
    return std::vector<QVariant>(metaKeys_.size(), Q_QNAN);
}

//! Return average over list of metadata.
Metadata Metadata::computeAverage(const std::vector<const Metadata*>& vec)
{
    Metadata ret;
    double fac = 1.0/vec.size();
    for (MetaDefinition metaKey : metaKeys_) {
        QString key = metaKey.name;
        switch (metaKey.mode) {
        case averageMode::FIRST: {
            const Metadata* firstMd = vec.front();
            ret.set(key, firstMd->at<QString>(key));
            break;
        }
        case averageMode::LAST: {
            const Metadata* lastMd = vec.back();
            ret.set(key, lastMd->at<double>(key));
            break;
        }
        case averageMode::SUM: {
            double sum = 0;
            for (const Metadata* d : vec)
                sum += d->at<double>(key);
            ret.set(key, sum);
            break;
        }
        case averageMode::AVGE: {
            double avg = 0;
            if (metaKey.type == valueType::DOUBLE) {
                for (const Metadata* d : vec)
                    avg += d->at<double>(key);
                avg *= fac;
                ret.set(key, avg);
            } else {
                for (const Metadata* d : vec)
                    avg += double(d->at<deg>(key));
                avg *= fac;
                ret.set(key, deg{avg});
            }
            break;
        }
        }
    }

    return ret;
}

namespace {
enum class iNums {
    MOTOR = 10,
    NM = 17,
    MONITOR = 17,
    D_MON,
    TIME,
    D_TIME,
    DATE,
    COMMENT
};
}

std::vector<MetaDefinition> Metadata::metaKeys() {
    std::vector<MetaDefinition> ret;
    for (int i=0; i<int(iNums::MOTOR); i++) {
        ret.push_back(MetaDefinition{asciiTags.at(i), averageMode::AVGE, valueType::DEG});
    }
    for (int i=10; i<int(iNums::NM); i++)
        ret.push_back(MetaDefinition{asciiTags.at(i), averageMode::AVGE, valueType::DOUBLE});
    ret.push_back(MetaDefinition{asciiTags.at(int(iNums::MONITOR)), averageMode::LAST, valueType::DOUBLE});
    ret.push_back(MetaDefinition{asciiTags.at(int(iNums::D_MON)), averageMode::SUM, valueType::DOUBLE});
    ret.push_back(MetaDefinition{asciiTags.at(int(iNums::TIME)), averageMode::LAST, valueType::DOUBLE});
    ret.push_back(MetaDefinition{asciiTags.at(int(iNums::D_TIME)), averageMode::SUM, valueType::DOUBLE});
    ret.push_back(MetaDefinition{asciiTags.at(int(iNums::DATE)), averageMode::FIRST, valueType::STRING});
    ret.push_back(MetaDefinition{asciiTags.at(int(iNums::COMMENT)), averageMode::FIRST, valueType::STRING});
    return ret;
}
