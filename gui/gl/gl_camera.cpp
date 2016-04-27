// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      gl_camera.cpp
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

// Original code Copyright 2015-6 Ian G Burleigh
// MIT-licence

#ifdef STECA_LABS

#include "gl_camera.h"
#include "gl_canvas.h"

namespace gui { namespace gl {
//------------------------------------------------------------------------------

Camera::Camera(): canvas_(nullptr) {
}

void Camera::setEye() {
  eye_.setToIdentity();
  // assumes that the model is scaled to -1 .. +1 // TODO
  eye_.lookAt(QVector3D(0,0,2),QVector3D(0,0,0),QVector3D(0,1,0));
  eye_.rotate(rot_);

  if (canvas_) canvas_->update();
}

void Camera::setAspectRatio(qreal ratio) {
  proj_.setToIdentity();
  // assumes that the model is scaled to -1 .. +1 // TODO
  proj_.perspective(40, ratio, .01, 100);

  if (canvas_) canvas_->update();
}

void Camera::rotateTo(QQuaternion const& rot) {
  rot_ = rot;
  setEye();
}

void Camera::rotateBy(QQuaternion const& rot) {
  rotateTo(rot * rot_);
}

//------------------------------------------------------------------------------
}}
#endif // STECA_LABS
// eof
