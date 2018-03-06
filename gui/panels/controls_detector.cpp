// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/controls_detector.cpp
//! @brief     Implements classes (Cut|Experiment|Geometry)Controls, ControlsDetector
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
#include "gui/actions/triggers.h"
#include <QSpacerItem>

// ************************************************************************** //
//  class CutControls
// ************************************************************************** //

CutControls::CutControls()
{
    // inbound connection
    connect(gSession, &Session::sigDetector, this, &CutControls::fromCore);

    // outbound connections
    connect(&cutLeft_, _SLOT_(QSpinBox, valueChanged, int), [](int value) {
            gSession->imageCut().setLeft(value); });
    connect(&cutRight_,  _SLOT_(QSpinBox, valueChanged, int), [](int value) {
            gSession->imageCut().setRight(value); });
    connect(&cutTop_,    _SLOT_(QSpinBox, valueChanged, int), [](int value) {
            gSession->imageCut().setTop(value); });
    connect(&cutBottom_, _SLOT_(QSpinBox, valueChanged, int), [](int value) {
            gSession->imageCut().setBottom(value); });
    connect(&gGui->toggles->linkCuts, &QAction::toggled, [](bool value) {
            gSession->imageCut().setLinked(value); });

    // layout
    layout_.setSpacing(2);
    layout_.setContentsMargins(1,3,3,3);
    layout_.addWidget(new QLabel("cut"), 1, 0);
    layout_.setColumnStretch(1, 1);
    layout_.addWidget(&cutLeft_, 1, 2);
    layout_.addWidget(&link_, 1, 3, Qt::AlignHCenter);
    layout_.addWidget(&cutTop_, 0, 3);
    layout_.addWidget(&cutBottom_, 2, 3);
    layout_.addWidget(&cutRight_, 1, 4);
    layout_.setColumnStretch(5, 1);
    setLayout(&layout_);
}

void CutControls::fromCore()
{
    const ImageCut& cut = gSession->imageCut();
    gGui->toggles->linkCuts.setChecked(cut.linked());
    cutLeft_.setValue(cut.left());
    cutTop_.setValue(cut.top());
    cutRight_.setValue(cut.right());
    cutBottom_.setValue(cut.bottom());
}


// ************************************************************************** //
//  class ExperimentControls
// ************************************************************************** //

ExperimentControls::ExperimentControls()
{
    // inbound connection
    connect(gSession, &Session::sigClusters, [=]() {
            combineMeasurements_.setValue(gSession->dataset().binning());
            dropIncompleteAction_.setEnabled(gSession->dataset().hasIncomplete()); });

    // outbound connections
    connect(&combineMeasurements_, _SLOT_(QSpinBox, valueChanged, int),
            [](int num) { gSession->dataset().setBinning(num); });
    connect(&dropIncompleteAction_, &QAction::toggled,
            [](bool on) { gSession->dataset().setDropIncomplete(on); });

    //initialization
    dropIncompleteAction_.setEnabled(false);

    // layout
    layout_.addWidget(new QLabel("combine"));
    layout_.addWidget(&combineMeasurements_);
    layout_.addWidget(new QLabel("measurements"));
    layout_.addWidget(&dropIncompleteButton_);
    layout_.addStretch(1);
    setLayout(&layout_);
}


// ************************************************************************** //
//  class GeometryControls
// ************************************************************************** //

GeometryControls::GeometryControls()
{
    // inbound connection
    connect(gSession, &Session::sigDetector, this, &GeometryControls::fromCore);

    // outbound connections and control widget setup
    connect(&detDistance_, _SLOT_(QDoubleSpinBox, valueChanged, double), [](double val) {
            gSession->geometry().setDetectorDistance(val); });

    detPixelSize_.setDecimals(3);
    connect(&detPixelSize_, _SLOT_(QDoubleSpinBox, valueChanged, double), [](double val) {
            gSession->geometry().setPixSize(val); });

    connect(&beamOffsetI_, _SLOT_(QSpinBox, valueChanged, int), [](int val) {
            gSession->geometry().midPixOffset().i = val; emit gSession->sigDetector(); });
    connect(&beamOffsetJ_, _SLOT_(QSpinBox, valueChanged, int), [](int val) {
            gSession->geometry().midPixOffset().j = val; emit gSession->sigDetector(); });

    // layout
    mmGrid_.addWidget(new QLabel("det. distance"), 0, 0);
    mmGrid_.addWidget(&detDistance_, 0, 1);
    mmGrid_.addWidget(new QLabel("mm"), 0, 2);
    mmGrid_.addWidget(new QLabel("pixel size"), 1, 0);
    mmGrid_.addWidget(&detPixelSize_, 1, 1);
    mmGrid_.addWidget(new QLabel("mm"), 1, 2);

    trafoLayout_.addWidget(new QLabel("image rotate"));
    trafoLayout_.addWidget(new XIconButton(&gGui->triggers->rotateImage));
    trafoLayout_.addWidget(new QLabel("mirror"));
    trafoLayout_.addWidget(new XIconButton(&gGui->toggles->mirrorImage));
    trafoLayout_.addStretch(1);

    offsetLayout_.addWidget(new QLabel("offset X"));
    offsetLayout_.addWidget(&beamOffsetI_);
    offsetLayout_.addWidget(new QLabel(" Y"));
    offsetLayout_.addWidget(&beamOffsetJ_);
    offsetLayout_.addWidget(new QLabel("pix"));
    offsetLayout_.addStretch(1);

    vbox_.addLayout(&mmGrid_);
    vbox_.addLayout(&trafoLayout_);
    vbox_.addLayout(&offsetLayout_);
    setLayout(&vbox_);

    // initialization
    fromCore();
}

void GeometryControls::fromCore() {
    const Geometry& g = gSession->geometry();
    detDistance_.setValue(g.detectorDistance());
    detPixelSize_.setValue(g.pixSize());
    beamOffsetI_.setValue(g.midPixOffset().i);
    beamOffsetJ_.setValue(g.midPixOffset().j);
}


// ************************************************************************** //
//  class ControlsDetector
// ************************************************************************** //

ControlsDetector::ControlsDetector()
{
    vbox_.addWidget(&geometryControls_);
    vbox_.addWidget(&cutControls_);
    vbox_.addWidget(&experimentControls_);
    vbox_.addStretch();
    setLayout(&vbox_);
}
