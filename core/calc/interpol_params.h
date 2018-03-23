// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/calc/interpol_params.h
//! @brief     Defines InterpolParams
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef INTERPOL_PARAMS_H
#define INTERPOL_PARAMS_H


//! Supports different ways of setting a gamma range.

class InterpolParams : public QObject {
public:
    InterpolParams() {}

    void fromJson(const JsonObj& obj);
    void fromSettings();

    void setEnabled(bool);
    void setStepAlpha(double);
    void setStepBeta(double);
    void setIdwRadius(double);
    void setAvgAlphaMax(double);
    void setAvgRadius(double);
    void setThreshold(int);

    bool   enabled()     const { return enabled_; }
    double stepAlpha()   const { return stepAlpha_; }
    double stepBeta()    const { return stepBeta_; }
    double idwRadius()   const { return idwRadius_; }
    double avgAlphaMax() const { return avgAlphaMax_; }
    double avgRadius()   const { return avgRadius_; }
    int    threshold()   const { return threshold_; }

    QJsonObject toJson() const;
    void toSettings() const;

private:
    bool   enabled_     {false};
    double stepAlpha_   {5.};
    double stepBeta_    {5.};
    double idwRadius_   {10.};
    double avgAlphaMax_ {5.};
    double avgRadius_   {5.};
    int    threshold_   {100};
};

#endif // INTERPOL_PARAMS_H
