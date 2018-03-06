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
#include "gui/base/displays.h"
#include "gui/base/layout.h"
#include "gui/mainwin.h"
#include "gui/actions/triggers.h"


// ************************************************************************** //
//  class ExperimentControls
// ************************************************************************** //

ExperimentControls::ExperimentControls() {

    auto layout = newQ::HBoxLayout();
    setLayout(layout);

    // 'combine' control
    layout->addWidget(new QLabel("combine:"));
    layout->addWidget(&combineMeasurements_);
    connect(&combineMeasurements_, _SLOT_(QSpinBox, valueChanged, int),
            [this](int num) { gSession->dataset().setBinning(num); });

    // 'if incomplete' control
    layout->addWidget(&dropIncompleteButton_);
    connect(&dropIncompleteAction_, &QAction::toggled,
            [this](bool on) { gSession->dataset().setDropIncomplete(on); });
    layout->addStretch(1);

    // back connection, to change controls from saved session
    connect(gSession, &Session::sigClusters, [=]() {
            combineMeasurements_.setValue(gSession->dataset().binning());
            dropIncompleteAction_.setEnabled(gSession->dataset().hasIncomplete()); });
    dropIncompleteAction_.setEnabled(false);
}


// ************************************************************************** //
//  class ControlsDetector
// ************************************************************************** //

ControlsDetector::ControlsDetector()
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
        if (gGui->toggles->linkCuts.isChecked())
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

    _add({ new QLabel("det. distance"), &detDistance_, new QLabel("mm") });
    _add({ new QLabel("pixel size"), &detPixelSize_, new QLabel("mm") });
    _add({ new QLabel("beam offset X"), &beamOffsetI_, new QLabel("pix") });
    _add({ new QLabel("Y"), &beamOffsetJ_, new QLabel("pix") });
    _add({ new QLabel("image rotate"),
                new XIconButton(&gGui->triggers->rotateImage),
                new QLabel("mirror"),
                new XIconButton(&gGui->toggles->mirrorImage) });
    _add({ new XIconButton(&gGui->toggles->linkCuts),
                new QLabel("cut"),
                new XIcon(":/icon/cutLeft"),
                &cutLeft_,
                new XIcon(":/icon/cutRight"),
                &cutRight_ }, 3);
    _add({ new XIcon(":/icon/cutTop"),
                &cutTop_,
                new XIcon(":/icon/cutBottom"),
                &cutBottom_ });

    grid->setColumnStretch(grid->columnCount(), 1);

    box->addLayout(grid);
    box->addWidget(&experimentControls); // controls row
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
