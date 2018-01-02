// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/signalling.h
//! @brief     Defines class TheHubSignallingBase
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef SIGNALLING_H
#define SIGNALLING_H

#include "calc/calc_reflection.h"
#include "data/datafile.h"
#include "typ/range.h"
#include "typ/realpair.h"

// make connects shorter

#define slot(Type, method, parType) static_cast<void (Type::*)(parType)>(&Type::method)

namespace gui {

class TheHub;

enum class eFittingTab {
    NONE,
    BACKGROUND,
    REFLECTIONS,
};

class TheHubSignallingBase : public QObject {
private:
    Q_OBJECT

    TheHub& asHub();

public: // emit signals
    void tellSessionCleared();
    void tellDatasetSelected(shp_Dataset);
    void tellSelectedReflection(calc::shp_Reflection);
    void tellReflectionData(calc::shp_Reflection);
    void tellReflectionValues(Range const&, qpair const&, fwhm_t, bool);

signals:
    void sigSessionCleared();

    void sigFilesChanged(); // the set of loaded files has changed
    void sigFilesSelected(); // the selection of loaded files has changed

    void sigDatasetsChanged(); // the set of datasets collected from selected
    // files has changed
    void sigDatasetSelected(shp_Dataset);

    void sigCorrFile(shp_Datafile);
    void sigCorrEnabled(bool);

    void sigReflectionsChanged();
    void sigReflectionSelected(calc::shp_Reflection);
    void sigReflectionData(calc::shp_Reflection);
    void sigReflectionValues(Range const&, qpair const&, fwhm_t, bool);

    void sigDisplayChanged();
    void sigGeometryChanged();

    void sigGammaRange();

    void sigBgChanged(); // ranges and poly: refit
    void sigNormChanged();

    void sigFittingTab(eFittingTab);

protected:
    uint sigLevel_ = 0;

    //! to prevent some otherwise recursive calls // TODO get rid of this
    class level_guard {
    public:
        level_guard(uint&);
        ~level_guard();

    private:
        uint& level_;
    };
};

} //namespace gui

#endif // SIGNALLING_H
