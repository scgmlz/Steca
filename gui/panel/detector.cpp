#include "detector.h"
#include "mainwin.h"
#include "session.h"
#include "settings.h"

namespace panel {

Detector::Detector(MainWin& mainWin_,Session& session_): super("Detector",mainWin_,session_) {

  grid->addWidget(label("Distance"),                  0,0);
  grid->addWidget((spinDistance = spinCell(8,0.0)),   0,1);
  grid->addWidget(label("mm"),                        0,2);
  grid->addWidget(label("Pixel size"),                1,0);
  grid->addWidget((spinPixelSize = spinCell(8,0.01)), 1,1);
  grid->addWidget(label("mm"),                        1,2);
  grid->setColumnStretch(3,1);

  readSettings(session);

  connect(&session, &Session::geometryChanged, [this]() {
    setFrom(session);
  });

  connect(spinDistance, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this]() {
    setTo(session);
  });

  connect(spinPixelSize, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this]() {
    setTo(session);
  });
}

Detector::~Detector() {
  saveSettings();
}

void Detector::setTo(Session& session) {
  session.setDetectorGeometry(
    spinDistance->value(),
    spinPixelSize->value());
}

void Detector::setFrom(Session& session) {
  auto const& g = session.getGeometry();

  spinDistance->setValue(g.sampleDetectorSpan);
  spinPixelSize->setValue(g.pixSpan);
}

static str GROUP_DETECTOR("Detector");
static str KEY_DISTANCE("distance");
static str KEY_PIXEL_SIZE("pixel_size");

void Detector::readSettings(Session& session) {
  Settings s(GROUP_DETECTOR);
  auto const& g = session.getGeometry();

  s.read(KEY_DISTANCE,    spinDistance,     g.sampleDetectorSpan);
  s.read(KEY_PIXEL_SIZE,  spinPixelSize,    g.pixSpan);
}

void Detector::saveSettings() {
  Settings s(GROUP_DETECTOR);
  s.save(KEY_DISTANCE,    spinDistance);
  s.save(KEY_PIXEL_SIZE,  spinPixelSize);
}

}

// eof
