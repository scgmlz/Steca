// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/actions/toggles.h
//! @brief     Defines class Toggles
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef TOGGLES_H
#define TOGGLES_H

#include "gui/base/controls.h"

//! Collection of toggle actions, for use as member of MainWin.

class Toggles : private QObject {
public:
    Toggles();
    CToggle combinedDgram {"dfg.all", "All measurements", false, ":/icon/all"};
    CToggle crosshair {"crosshair", "Show crosshair", false, ":/icon/crop"};
    CToggle enableCorr {"enableCorr", "Enable correction file", false, ":/icon/useCorrection"};
    CToggle fixedIntenDgram {"dfg.fixInt", "Fixed intensity scale", false, ":/icon/scale"};
    CToggle fixedIntenImage {"img.fixInt", "Global intensity scale", false, ":/icon/scale"};
    CToggle linkCuts {"linkCuts", "Link cuts", false, ":/icon/link"};
    CToggle showBackground {"showBackground", "Show fitted background", false,
            ":/icon/showBackground"};
    CToggle showBins {"showBins", "Show bins", false, ":/icon/angle"};
    CToggle viewClusters {"viewMeasurements", "Measurements", true, "", Qt::Key_F9};
    CToggle viewFiles {"viewFiles", "Files", true, "", Qt::Key_F8};
    CToggle viewMetadata {"viewMetadata", "Metadata", true, "", Qt::Key_F10};
    CToggle viewStatusbar {"viewStatusbar", "Statusbar", true, "", Qt::Key_F12};
#ifndef Q_OS_OSX
    CToggle fullScreen {"toggle_fullScreen", "FullScreen", false, "", Qt::Key_F11};
#endif
};

#endif // TOGGLES_H
