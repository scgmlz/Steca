#include "detector.h"
#include "mainwin.h"
#include "settings.h"
#include "gui_helpers.h"

namespace panel {

static str GROUP_DETECTOR("Detector");
static str KEY_DISTANCE("distance");
static str KEY_PIXEL_SIZE("pixel_size");
static str KEY_IS_OFFSET("is_offset");
static str KEY_OFFSET_X("offset_x");
static str KEY_OFFSET_Y("offset_y");

Detector::Detector(MainWin& mainWin_,Session& session_): super("Detector",mainWin_,session_)
, spinDistance(nullptr), spinPixelSize(nullptr)
, checkIsBeamOffset(nullptr), spinOffsetX(nullptr), spinOffsetY(nullptr)
, isSetting(false) {

  int row = 0;
  grid->addWidget(label("Distance"),row,0);
  grid->addWidget((spinDistance = spinCell(8,0.0)),row,1);
  grid->addWidget(label("mm"),row,2);

  ++row;
  grid->addWidget(label("Pixel size"),row,0);
  grid->addWidget((spinPixelSize = spinCell(8,0.01)),row,1);
  grid->addWidget(label("mm"),row,2);

  ++row;
  grid->addWidget((checkIsBeamOffset = check("Beamcenter offset")),row,0,1,3);

  ++row;
  grid->addWidget(label("X"),row,0);
  grid->addWidget((spinOffsetX = spinCell(8,0)),row,1);
  grid->addWidget(label("pix"),row,2);

  ++row;
  grid->addWidget(label("Y"),row,0);
  grid->addWidget((spinOffsetY = spinCell(8,0)),row,1);
  grid->addWidget(label("pix"),row,2);

  grid->setColumnStretch(3,1);

  auto setEnabled = [this]() {
    bool on = checkIsBeamOffset->isChecked();
    spinOffsetX->setEnabled(on);
    spinOffsetY->setEnabled(on);
  };

  connect(checkIsBeamOffset, &QCheckBox::toggled, [setEnabled]() {
    setEnabled();
  });

  setEnabled();
  readSettings(session);

  connect(&session, &Session::sessionLoaded, [this]() {
    setFrom(session);
  });

  connect(spinDistance, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this]() {
    setTo(session);
  });

  connect(spinPixelSize, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this]() {
    setTo(session);
  });

  connect(checkIsBeamOffset, &QCheckBox::toggled, [this]() {
    setTo(session);
  });

  connect(spinOffsetX, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this]() {
    setTo(session);
  });

  connect(spinOffsetY, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this]() {
    setTo(session);
  });
}

Detector::~Detector() {
  saveSettings();
}

void Detector::setTo(Session& session) {
  if (isSetting) return;

  session.sampleDetectorSpan = spinDistance->value();
  session.pixSpan            = spinPixelSize->value();
  session.middlePixXOffset   = spinOffsetX->value();
  session.middlePixYOffset   = spinOffsetY->value();
  session.hasBeamOffset      = checkIsBeamOffset->isChecked();
}

void Detector::setFrom(Session& session) {
  BoolGuard _(isSetting);

  spinDistance->setValue(session.sampleDetectorSpan);
  spinPixelSize->setValue(session.pixSpan);
  spinOffsetX->setValue(session.middlePixXOffset);
  spinOffsetY->setValue(session.middlePixYOffset);
  checkIsBeamOffset->setChecked(session.hasBeamOffset);
}

void Detector::readSettings(Session& session) {
  Settings s(GROUP_DETECTOR);
  s.read(KEY_DISTANCE,    spinDistance,     session.sampleDetectorSpan);
  s.read(KEY_PIXEL_SIZE,  spinPixelSize,    session.pixSpan);
  s.read(KEY_IS_OFFSET,   checkIsBeamOffset,session.hasBeamOffset);
  s.read(KEY_OFFSET_X,    spinOffsetX,      session.middlePixXOffset);
  s.read(KEY_OFFSET_Y,    spinOffsetY,      session.middlePixYOffset);
}

void Detector::saveSettings() {
  Settings s(GROUP_DETECTOR);
  s.save(KEY_DISTANCE,    spinDistance);
  s.save(KEY_PIXEL_SIZE,  spinPixelSize);
  s.save(KEY_IS_OFFSET,   checkIsBeamOffset);
  s.save(KEY_OFFSET_X,    spinOffsetX);
  s.save(KEY_OFFSET_Y,    spinOffsetY);
}

}

// eof
