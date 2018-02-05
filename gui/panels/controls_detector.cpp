// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/controls_detector.cpp
//! @brief     Implements class SubframeSetup, and local classes
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "controls_detector.h"
#include "core/session.h"
#include "gui/mainwin.h"

ControlsDetector::ControlsDetector()
    : detDistance_("detDistance_", 6, Geometry::MIN_DETECTOR_DISTANCE)
    , detPixelSize_("detPixelSize_", 6, Geometry::MIN_DETECTOR_PIXEL_SIZE)
    , beamOffsetI_("beamOffsetI_", 6, true)
    , beamOffsetJ_("beamOffsetJ_", 6, true)
    , cutLeft_("cutLeft_", 4, false, 0)
    , cutTop_("cutTop_", 4, false, 0)
    , cutRight_("cutRight_", 4, false, 0)
    , cutBottom_("cutBottom_", 4, false, 0)
{

    auto* box = newQ::VBoxLayout();
    setLayout(box);

    connect(gSession, &Session::sigDetector, this, &ControlsDetector::fromSession);

    // widgets

    detDistance_.setValue(Geometry::DEF_DETECTOR_DISTANCE);
    connect(&detDistance_, _SLOT_(QDoubleSpinBox, valueChanged, double), [this]() { toSession(); });

    detPixelSize_.setDecimals(3);
    detPixelSize_.setValue(Geometry::DEF_DETECTOR_PIXEL_SIZE);
    connect(&detPixelSize_, _SLOT_(QDoubleSpinBox, valueChanged, double), [this]() {
            toSession(); });

    auto _setImageCut = [this](bool isTopOrLeft, int value) {
        ASSERT(value >= 0);
        if (gHub->toggle_linkCuts->isChecked())
            gSession->setImageCut(isTopOrLeft, true, ImageCut(value, value, value, value));
        else
            gSession->setImageCut(isTopOrLeft, false,
                                  ImageCut(cutLeft_.value(), cutTop_.value(),
                                           cutRight_.value(), cutBottom_.value()));
    };

    connect(&cutLeft_, _SLOT_(QSpinBox, valueChanged, int), [_setImageCut](int value) {
            _setImageCut(true, value); });

    connect(&cutTop_, _SLOT_(QSpinBox, valueChanged, int), [_setImageCut](int value) {
            _setImageCut(true, value); });

    connect(&cutRight_, _SLOT_(QSpinBox, valueChanged, int), [_setImageCut](int value) {
            _setImageCut(false, value); });

    connect(&cutBottom_, _SLOT_(QSpinBox, valueChanged, int), [_setImageCut](int value) {
            _setImageCut(false, value); });

    // layout

    QGridLayout* grid = newQ::GridLayout();
    int row = 0;

    auto _add = [&grid, &row](const QVector<QWidget*>& ws, int left = 1) {
        int i = 0, cnt = ws.count();
        QBoxLayout* box = newQ::HBoxLayout();
        box->addStretch(1);
        while (i < left)
            box->addWidget(ws.at(i++));
        grid->addLayout(box, row, 0);
        box = newQ::HBoxLayout();
        while (i < cnt)
            box->addWidget(ws.at(i++));
        grid->addLayout(box, row, 1);
        box->addStretch(1);
        row++;
    };

    _add({ newQ::Label("det. distance"), &detDistance_, newQ::Label("mm") });
    _add({ newQ::Label("pixel size"), &detPixelSize_, newQ::Label("mm") });
    _add({ newQ::Label("beam offset X"), &beamOffsetI_, newQ::Label("pix") });
    _add({ newQ::Label("Y"), &beamOffsetJ_, newQ::Label("pix") });
    _add({ newQ::Label("image rotate"),
                newQ::IconButton(gHub->trigger_rotateImage),
                newQ::Label("mirror"),
                newQ::IconButton(gHub->toggle_mirrorImage) });
    _add({ newQ::IconButton(gHub->toggle_linkCuts),
                newQ::Label("cut"),
                newQ::Icon(":/icon/cutLeft"),
                &cutLeft_,
                newQ::Icon(":/icon/cutRight"),
                &cutRight_ }, 3);
    _add({ newQ::Icon(":/icon/cutTop"),
                &cutTop_,
                newQ::Icon(":/icon/cutBottom"),
                &cutBottom_ });

    grid->setColumnStretch(grid->columnCount(), 1);

    box->addLayout(grid);
    box->addStretch();
}

void ControlsDetector::toSession() {
    gSession->setGeometry(
        qMax(qreal(Geometry::MIN_DETECTOR_DISTANCE), detDistance_.value()),
        qMax(qreal(Geometry::MIN_DETECTOR_PIXEL_SIZE), detPixelSize_.value()),
        IJ(beamOffsetI_.value(), beamOffsetJ_.value()));
}

void ControlsDetector::fromSession() {
    const Geometry& g = gSession->geometry();

    detDistance_.setValue(g.detectorDistance);
    detPixelSize_.setValue(g.pixSize);

    beamOffsetI_.setValue(g.midPixOffset.i);
    beamOffsetJ_.setValue(g.midPixOffset.j);

    const ImageCut& cut = gSession->imageCut();

    cutLeft_.setValue(cut.left);
    cutTop_.setValue(cut.top);
    cutRight_.setValue(cut.right);
    cutBottom_.setValue(cut.bottom);
}
