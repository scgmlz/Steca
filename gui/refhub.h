// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/refhub.h
//! @brief     Defines classes TheHubSignallingBase and RefHub
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef REFHUB_H
#define REFHUB_H

#include "calc/calc_reflection.h"
#include "data/datafile.h"

namespace gui {

//! The base class of the hub that signals.

class TheHub;

enum class eFittingTab {
    NONE,
    BACKGROUND,
    REFLECTIONS,
};

class TheHubSignallingBase : public QObject {
private:
    Q_OBJECT
    friend class RefHub;

    TheHub& asHub();

protected:
    // emit signals - only TheHub can call these
    void tellSessionCleared();
    void tellDatasetSelected(data::shp_Dataset);
    void tellSelectedReflection(calc::shp_Reflection);
    void tellReflectionData(calc::shp_Reflection);
    void tellReflectionValues(typ::Range const&, qpair const&, fwhm_t, bool);

signals:
    void sigSessionCleared();

    void sigFilesChanged(); // the set of loaded files has changed
    void sigFilesSelected(); // the selection of loaded files has changed

    void sigDatasetsChanged(); // the set of datasets collected from selected
    // files has changed
    void sigDatasetSelected(data::shp_Dataset);

    void sigCorrFile(data::shp_File);
    void sigCorrEnabled(bool);

    void sigReflectionsChanged();
    void sigReflectionSelected(calc::shp_Reflection);
    void sigReflectionData(calc::shp_Reflection);
    void sigReflectionValues(typ::Range const&, qpair const&, fwhm_t, bool);

    void sigDisplayChanged();
    void sigGeometryChanged();

    void sigGammaRange();

    void sigBgChanged(); // ranges and poly: refit
    void sigNormChanged();

    void sigFittingTab(eFittingTab);

protected:
    // to prevent some otherwise recursive calls
    typedef uint level_t;
    level_t sigLevel_ = 0;

    class level_guard {
    public:
        level_guard(level_t&);
        ~level_guard();

    private:
        level_t& level_;
    };
};


//! The base class that refers to the hub + support methods.

class RefHub {
public:
    RefHub(TheHub&);
protected:
    TheHub& hub_;
protected:
    void tellDatasetSelected(data::shp_Dataset);
    void tellSelectedReflection(calc::shp_Reflection);
    void tellReflectionData(calc::shp_Reflection);
    void tellReflectionValues(typ::Range const&, qpair const&, fwhm_t, bool);
};

} //namespace gui

#endif // REFHUB_H
