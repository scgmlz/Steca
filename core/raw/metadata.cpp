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

int noNumAttr = 0;

std::vector<MetaDefinition> metaDefs_ = {
    {"X", "X", averageMode::AVGE, valueType::DOUBLE}, {"Y", "Y", averageMode::AVGE, valueType::DOUBLE},
    {"Z", "Z", averageMode::AVGE, valueType::DOUBLE}, {"omega", "ω", averageMode::AVGE, valueType::DEG},
    {"mid2theta", "mid 2θ", averageMode::AVGE, valueType::DEG}, {"phi", "φ", averageMode::AVGE, valueType::DEG},
    {"chi", "χ", averageMode::AVGE, valueType::DEG}, {"PST", "PST", averageMode::AVGE, valueType::DOUBLE},
    {"SST", "SST", averageMode::AVGE, valueType::DOUBLE}, {"OmegaM", "ΩM", averageMode::AVGE, valueType::DEG},
    {"T", "T", averageMode::AVGE, valueType::DOUBLE}, {"teload", "teload", averageMode::AVGE, valueType::DOUBLE},
    {"tepos", "tepos", averageMode::AVGE, valueType::DOUBLE}, {"teext", "teext", averageMode::AVGE, valueType::DOUBLE},
    {"xe", "xe", averageMode::AVGE, valueType::DOUBLE}, {"ye", "ye", averageMode::AVGE, valueType::DOUBLE},
    {"ze", "ze", averageMode::AVGE, valueType::DOUBLE}, {"mon", "mon", averageMode::LAST, valueType::DOUBLE},
    {"delta_mon", "Δmon", averageMode::SUM, valueType::DOUBLE}, {"t", "t", averageMode::LAST, valueType::DOUBLE},
    {"delta_t", "Δt", averageMode::SUM, valueType::DOUBLE}, {"date", "date", averageMode::FIRST, valueType::STRING},
    {"comment", "comment", averageMode::FIRST, valueType::STRING},
};

} // namespace

MetaDefinition::MetaDefinition(const QString& name, const QString& niceName,
                               averageMode avgmode, valueType valtype)
    : niceName{niceName}
    , asciiName{name}
    , mode{avgmode}
    , type{valtype}
{
    if (valtype == valueType::STRING)
        noNumAttr++;
}

Metadata::Metadata()
    : Mapped{}
{}

QString Metadata::attributeStrValue(int i) const
{
    QVariant v = attributeValue(i);
    if (v.canConvert<deg>())
        return QString::number(double(v.value<deg>()));
    else
        return v.toString();
}

QVariant Metadata::attributeValue(int i) const
{
    if (i >= metaDefs_.size())
        qFatal("impossible case");
    else
        return at(metaDefs_.at(i).asciiName);
}

std::vector<QVariant> Metadata::attributeValues() const
{
    std::vector<QVariant> attrs;
    for (int i=0; i<metaDefs_.size(); ++i)
        attrs.push_back(attributeValue(i));
    return attrs;
}

namespace meta {

QStringList asciiTags;
QStringList niceTags;

int size()
{
    return attributeNaNs().size();
}

int numAttributes(bool onlyNum)
{
    return onlyNum ? metaDefs_.size()-noNumAttr : metaDefs_.size();
}

const QString& attributeTag(int i, bool nice)
{
    return attributeTags(nice).at(i);
}

const QStringList& attributeTags(bool nice)
{
    QStringList ret;
    if (nice) {
        for (MetaDefinition m : metaDefs_)
            ret.append(m.niceName);
        niceTags = ret;
        return niceTags;
    } else {
        for (MetaDefinition m : metaDefs_)
            ret.append(m.asciiName);
        asciiTags = ret;
        return asciiTags;
    }
}


std::vector<QVariant> attributeNaNs()
{
    return std::vector<QVariant>(metaDefs_.size(), Q_QNAN);
}

//! Return average over list of metadata.
Metadata computeAverage(const std::vector<const Metadata*>& vec)
{
    Metadata ret;
    double fac = 1.0/vec.size();
    for (MetaDefinition metaKey : metaDefs_) {
        QString key = metaKey.asciiName;
        switch (metaKey.mode) {
        case averageMode::FIRST: {
            const Metadata* firstMd = vec.front();
            ret.set(key, firstMd->get<QString>(key));
            break;
        }
        case averageMode::LAST: {
            const Metadata* lastMd = vec.back();
            ret.set(key, lastMd->get<double>(key));
            break;
        }
        case averageMode::SUM: {
            double sum = 0;
            for (const Metadata* d : vec)
                sum += d->get<double>(key);
            ret.set(key, sum);
            break;
        }
        case averageMode::AVGE: {
            double avg = 0;
            if (metaKey.type == valueType::DOUBLE) {
                for (const Metadata* d : vec)
                    avg += d->get<double>(key);
                avg *= fac;
                ret.set(key, avg);
            } else {
                for (const Metadata* d : vec)
                    avg += double(d->get<deg>(key));
                avg *= fac;
                ret.set(key, deg{avg});
            }
            break;
        }
        }
    }

    return ret;
}
} // namespace meta
