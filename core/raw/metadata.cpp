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

static int noNumAttr = 0;

std::vector<MetaDefinition> metaDefs_ = {
    {"X", "X", averageMode::AVGE},              {"Y", "Y", averageMode::AVGE},
    {"Z", "Z", averageMode::AVGE},              {"omega", "ω", averageMode::AVGE},
    {"mid2theta", "mid 2θ", averageMode::AVGE}, {"phi", "φ", averageMode::AVGE},
    {"chi", "χ", averageMode::AVGE},            {"PST", "PST", averageMode::AVGE},
    {"SST", "SST", averageMode::AVGE},          {"OmegaM", "ΩM", averageMode::AVGE},
    {"T", "T", averageMode::AVGE},              {"teload", "teload", averageMode::AVGE},
    {"tepos", "tepos", averageMode::AVGE},      {"teext", "teext", averageMode::AVGE},
    {"xe", "xe", averageMode::AVGE},            {"ye", "ye", averageMode::AVGE},
    {"ze", "ze", averageMode::AVGE},            {"mon", "mon", averageMode::LAST},
    {"delta_mon", "Δmon", averageMode::SUM},    {"t", "t", averageMode::LAST},
    {"delta_t", "Δt", averageMode::SUM},        {"date", "date", averageMode::FIRST},
    {"comment", "comment", averageMode::FIRST},
};

} // namespace

MetaDefinition::MetaDefinition(const QString& name, const QString& niceName, averageMode avgmode)
    : niceName{niceName}
    , asciiName{name}
    , mode{avgmode}
{
    if (mode == averageMode::FIRST)
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
    for (MetaDefinition metaDef : metaDefs_) {
        QString key = metaDef.asciiName;
        switch (metaDef.mode) {
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
            if (vec.front()->at(key).canConvert<double>()) {
                double avg = 0;
                for (const Metadata* d : vec)
                    avg += d->get<double>(key);
                avg *= fac;
                ret.set(key, avg);
            } else {
                deg avg = 0;
                for (const Metadata* d : vec)
                    avg += d->get<deg>(key);
                avg *= fac;
                ret.set(key, avg);
            }
            break;
        }
        }
    }

    return ret;
}
} // namespace meta
