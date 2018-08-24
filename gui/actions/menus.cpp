//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/actions/menus.cpp
//! @brief     Implements class Menus
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "gui/actions/menus.h"
#include "gui/actions/image_trafo_actions.h"
#include "gui/view/toggles.h"
#include "gui/actions/triggers.h"
#include "gui/mainwin.h"

//! Initialize the menu bar.
Menus::Menus(QMenuBar* mbar)
    : mbar_(mbar)
{
#ifdef Q_OS_OSX
    mbar->setNativeMenuBar(false);
#else
    mbar->setNativeMenuBar(true);
#endif

    Triggers* triggers = gGui->triggers;
    Toggles* toggles = gGui->toggles;

    actionsToMenu(
        "&File",
        {   &triggers->addFiles,
                &triggers->removeFile,
                separator(),
                &triggers->corrFile,
                &toggles->enableCorr,
                separator(),
                &triggers->loadSession,
                &triggers->saveSession,
                &triggers->clearSession,
#ifndef Q_OS_OSX // Mac puts Quit into the Apple menu
                separator(),
#endif
                &triggers->quit });

    export_ = actionsToMenu(
        "&Export",
        {       &triggers->exportDfgram,
                &triggers->exportBigtable,
                &triggers->exportDiagram,
                &triggers->exportPolefig });

    image_ = actionsToMenu(
        "&Image",
        {   &gGui->imageTrafoActions->rotateImage,
                &gGui->imageTrafoActions->mirrorImage,
                &toggles->fixedIntenImage,
                &toggles->linkCuts,
                &toggles->crosshair,
                &toggles->showBins });

    dgram_ = actionsToMenu(
        "&Diffractogram",
        {   &toggles->showBackground,
                &triggers->baserangeAdd,
                &triggers->baserangeRemove,
                &triggers->baserangesClear,
                separator(),
                &triggers->peakAdd,
                &triggers->peakRemove,
                &triggers->peaksClear,
                separator(),
                &toggles->combinedDfgram,
                &toggles->fixedIntenDfgram });

    actionsToMenu(
        "&View",
        {   &toggles->viewFiles,
                &toggles->viewClusters,
                &toggles->viewMetadata,
                separator(),
#ifndef Q_OS_OSX
                &toggles->fullScreen,
#endif
                &toggles->viewStatusbar,
                separator(),
                &triggers->spawnTable,
                &triggers->spawnDiagram,
                &triggers->spawnPolefig,
                separator(),
                &triggers->viewsReset });

    actionsToMenu(
        "&Help",
        {   &triggers->about, // Mac puts About into the Apple menu
                &triggers->online,
                &triggers->checkUpdate });
}

QAction* Menus::separator() const
{
    QAction* ret = new QAction(mbar_);
    ret->setSeparator(true);
    return ret;
};

QMenu* Menus::actionsToMenu(const char* menuName, QList<QAction*> actions)
{
    QMenu* menu = new QMenu { menuName };
    mbar_->addMenu(menu);
    menu->addActions(actions);
    QString prefix = QString("%1: ").arg(menu->title().remove('&'));
    for (auto action : actions)
        action->setToolTip(prefix + action->toolTip());
    return menu;
};
