// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/actions.cpp
//! @brief     Implements functions newTrigger, newToggle, and class Actions
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "thehub.h"
#include <QApplication>

namespace gui {

// ************************************************************************** //
//  class Action
// ************************************************************************** //

QAction* newTrigger(rcstr text, rcstr iconFile) {
    QAction* ret = new QAction(text, qApp);
    ret->setToolTip(text.toLower());
    if (iconFile!="")
        ret->setIcon(QIcon(iconFile));
    return ret;
};

QAction* newToggle(rcstr text, rcstr iconFile) {
    QAction* ret = new QAction(text, qApp);
    ret->setToolTip(text.toLower());
    if (iconFile!="")
        ret->setIcon(QIcon(iconFile));
    ret->setCheckable(true);
    return ret;
};

// ************************************************************************** //
//  class Actions
// ************************************************************************** //

Actions::Actions(TheHub& hub) : hub_(hub) {
    using QKey = QKeySequence;

    // create actions

    about = newTrigger("About && Configuration...");
    online = newTrigger("Online documentation...");
    checkUpdate = newTrigger("Check for update...");
    quit = newTrigger("Quit");

    viewStatusbar = newToggle("Statusbar");
    viewFiles = newToggle("Files");
    viewDatasets = newToggle("Datasets");
    viewDatasetInfo = newToggle("Metadata");
    viewReset = newTrigger("Reset");
#ifndef Q_OS_OSX
    fullScreen = newToggle("FullScreen");
#endif

    loadSession = newTrigger("Load session...");
    saveSession = newTrigger("Save session...");
    clearSession = newTrigger("Clear session (to defaults)");

    addFiles = newTrigger("Add files...", ":/icon/add");
    remFile = newTrigger("Remove selected file(s)", ":/icon/rem");
    enableCorr = newToggle("Enable correction file...", ":/icon/useCorrection");
    remCorr = newTrigger("Remove correction file", ":/icon/clear");

    rotateImage = newTrigger("Rotate", ":/icon/rotate0");
    mirrorImage = newToggle("Mirror", ":/icon/mirrorHorz");
    linkCuts = newToggle("Link cuts", ":/icon/link");
    showOverlay = newToggle("Show overlay", ":/icon/crop");
    stepScale = newToggle("Scale in steps", ":/icon/steps");
    showBins = newToggle("Show bins", ":/icon/angle");

    fixedIntenImage = newToggle("Global intensity scale", ":/icon/scale");
    fixedIntenDgram = newToggle("Fixed intensity scale");

    combinedDgram = newToggle("All datasets");

    selRegions = newToggle("Select regions", ":/icon/selRegion");
    showBackground = newToggle("Show fitted background", ":/icon/showBackground");
    clearBackground = newTrigger("Clear background regions", ":/icon/clear");
    clearReflections = newTrigger("Clear reflections", ":/icon/clear");

    addReflection = newTrigger("Add reflection", ":/icon/add");
    remReflection = newTrigger("Remove reflection", ":/icon/rem");

    outputPolefigures = newTrigger("Pole figures...");
    outputDiagrams = newTrigger("Diagrams...");
    outputDiffractograms = newTrigger("Diffractograms...");

    // key shortcuts

    quit->setShortcut(QKey::Quit);

    viewStatusbar->setShortcut(Qt::Key_F12);
    viewFiles->setShortcut(Qt::Key_F8);
    viewDatasets->setShortcut(Qt::Key_F9);
    viewDatasetInfo->setShortcut(Qt::Key_F10);

#ifndef Q_OS_OSX
    fullScreen->setShortcut(Qt::Key_F11);
#endif

    addFiles->setShortcut(Qt::CTRL | Qt::Key_O);
    remFile->setShortcut(QKey::Delete);
    enableCorr->setShortcut(Qt::SHIFT | Qt::CTRL | Qt::Key_C);

    rotateImage->setShortcut(Qt::CTRL | Qt::Key_R);

    // handle signals

    QObject::connect(&hub_, &TheHubSignallingBase::sigFilesSelected,
            [this]() { remFile->setEnabled(!hub_.collectedFromFiles().isEmpty()); });
    QObject::connect(&hub_, &TheHubSignallingBase::sigCorrFile,
            [this](data::shp_File file) { remCorr->setEnabled(!file.isNull()); });
    QObject::connect(&hub_, &TheHubSignallingBase::sigCorrEnabled,
            [this](bool on) { enableCorr->setChecked(on); });

    auto deselect = [this]() {
        fixedIntenImage->setChecked(false);
        fixedIntenDgram->setChecked(false);
        combinedDgram->setChecked(false);
    };

    QObject::connect(&hub_, &TheHubSignallingBase::sigGeometryChanged,
                     [deselect]() { deselect(); });
    QObject::connect(&hub_, &TheHubSignallingBase::sigDatasetsChanged,
                     [deselect]() { deselect(); });
    QObject::connect(&hub_, &TheHubSignallingBase::sigCorrEnabled,
                     [deselect]() { deselect(); });
}

} // namespace gui
