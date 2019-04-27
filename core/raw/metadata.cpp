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
#include "core/session.h"
//#include "qcr/base/debug.h"

namespace {

static int noNumAttr = 2;

std::vector<MetaDefinition> metaDefs = {
    {"X", "X", averageMode::AVGE},
    {"Y", "Y", averageMode::AVGE},
    {"Z", "Z", averageMode::AVGE},
    {"omega", "ω", averageMode::AVGE},
    {"mid2theta", "mid 2θ", averageMode::AVGE},
    {"phi", "φ", averageMode::AVGE},
    {"chi", "χ", averageMode::AVGE},
    {"PST", "PST", averageMode::AVGE},
    {"SST", "SST", averageMode::AVGE},
    {"OmegaM", "ΩM", averageMode::AVGE},
    {"T", "T", averageMode::AVGE},
    {"teload", "teload", averageMode::AVGE},
    {"tepos", "tepos", averageMode::AVGE},
    {"teext", "teext", averageMode::AVGE},
    {"xe", "xe", averageMode::AVGE},
    {"ye", "ye", averageMode::AVGE},
    {"ze", "ze", averageMode::AVGE},
    {"mon", "mon", averageMode::LAST},
    {"delta_mon", "Δmon", averageMode::SUM},
    {"t", "t", averageMode::LAST},
    {"delta_t", "Δt", averageMode::SUM},
    {"measure_t", "measure_t", averageMode::FIRST},
    {"numMeasurement", "numMeasurement", averageMode::FIRST},
    {"date", "date", averageMode::FIRST},
    {"comment", "comment", averageMode::FIRST},
};

} // namespace

MetaDefinition::MetaDefinition(const QString& name, const QString& niceName, averageMode avgmode,
                               metaMode mM)
    : asciiName_{name}
    , niceName_{niceName}
    , mode_{avgmode}
    , metaMode_{mM}
{}

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
    if (i >= metaDefs.size())
        qFatal("impossible case");
    else
        return at(metaDefs.at(i).asciiName_);
}

std::vector<QVariant> Metadata::attributeValues() const
{
    std::vector<QVariant> attrs;
    for (int i=0; i<metaDefs.size(); ++i)
        attrs.push_back(attributeValue(i));
    return attrs;
}

namespace meta {

QStringList asciiNames;
QStringList niceNames;
std::vector<int> selectedMD;
std::vector<int> selectedFD;

int size()
{
    return attributeNaNs().size();
}

int numAttributes(bool onlyNum)
{
    return onlyNum ? metaDefs.size()-noNumAttr : metaDefs.size();
}

const QString& asciiTag(int i)
{
    return metaDefs.at(i).asciiName_;
}

const QString& niceTag(int i)
{
    return metaDefs.at(i).niceName_;
}

const QStringList& asciiTags()
{
    QStringList ret;
    for (int i=0; i<metaDefs.size(); i++)
        ret.append(asciiTag(i));
    asciiNames = ret;
    return asciiNames;
}

const QStringList& niceTags()
{
    QStringList ret;
    for (int i=0; i<metaDefs.size(); i++)
        ret.append(niceTag(i));
    niceNames = ret;
    return niceNames;
}


std::vector<QVariant> attributeNaNs()
{
    return std::vector<QVariant>(metaDefs.size(), Q_QNAN);
}

//! Return average over list of metadata.
Metadata computeAverage(const std::vector<const Metadata*>& vec)
{
    Metadata ret;
    double fac = 1.0/vec.size();
    for (MetaDefinition metaDef : metaDefs) {
        QString key = metaDef.asciiName_;
        switch (metaDef.mode_) {
        case averageMode::FIRST: {
            const Metadata* firstMd = vec.front();
            ret.set(key, firstMd->at(key));
            break;
        }
        case averageMode::LAST: {
            const Metadata* lastMd = vec.back();
            ret.set(key, lastMd->at(key));
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

std::vector<QVariant> metaValues(const Mapped map)
{
    std::vector<QVariant> attr;
    for (int i=0; i<metaDefs.size(); i++) {
        QString key = metaDefs.at(i).asciiName_;
        if (map.has(key))
            attr.push_back(map.at(key));
        else
            attr.push_back(Q_QNAN);
    }
    return attr;
}

void setMetaMode(int i, metaMode mM)
{
    metaDefs.at(i).metaMode_ = mM;
}

metaMode getMetaMode(int i)
{
    return metaDefs.at(i).metaMode_;
}

void clearMetaModes()
{
    for (int m=0; m<metaDefs.size(); m++) {
       setMetaMode(m, metaMode::CONSTANT);
    }
}

int numSelectedFileDependent()
{
    selectedFD.clear();
    for (int i=0; i<metaDefs.size(); i++) {
        if (getMetaMode(i) == metaMode::FILE_DEPENDENT &&
                gSession->params.smallMetaSelection.isSelected(i) ) {
            selectedFD.push_back(i);
        }
    }
    return selectedFD.size();
}

int numSelectedMeasurementDependent()
{
    selectedMD.clear();
    for (int i=0; i<metaDefs.size(); i++) {
        if (getMetaMode(i) == metaMode::MEASUREMENT_DEPENDENT &&
                gSession->params.smallMetaSelection.isSelected(i) ) {
            selectedMD.push_back(i);
        }
    }
    return selectedMD.size();
}

int selectedOfFileDependent(int i)
{
    return selectedFD.at(i);
}

int selectedOfMeasurementDependent(int i)
{
    return selectedMD.at(i);
}

} // namespace meta
