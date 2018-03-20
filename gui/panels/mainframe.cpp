// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/mainframe.cpp
//! @brief     Implements class SubframeImage
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "mainframe.h"
#include "core/session.h"
#include "gui/panels/tab_image.h"

// ************************************************************************** //
//  class Mainframe
// ************************************************************************** //

Mainframe::Mainframe()
    : CTabWidget {"images"}
{
    setTabPosition(QTabWidget::North);
    addTab((dataImageTab_ = new DataImageTab), "Data image");
    addTab((corrImageTab_ = new CorrImageTab), "Corr image");
    connect(gSession, &Session::sigCorr, [this]() {
            setTabEnabled(1, gSession->corrset().hasFile()); });
    setTabEnabled(1, false);

    // local connection
    connect(this, &Mainframe::currentChanged, [this](int) { render(); });
    // inbound connection
    connect(gSession, &Session::sigImage, [this]() { render(); });
}

void Mainframe::render()
{
    auto currentTab = dynamic_cast<ImageTab*>(currentWidget());
    if (!currentTab)
        return;
    currentTab->render();
}

Mainframe::~Mainframe()
{
    clear();
    delete dataImageTab_;
    delete corrImageTab_;
}
