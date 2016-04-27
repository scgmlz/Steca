// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      gl_camera.h
//! @brief     OpenGl camera
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
#ifndef GL_CAMERA_H
#define GL_CAMERA_H

#include "core_defs.h"
#include <QMatrix4x4>
#include <QQuaternion>

namespace gui { namespace gl {
//------------------------------------------------------------------------------

class Canvas;

class Camera {
  friend class Canvas;
public:
  Camera();

  QMatrix4x4  const& proj() const { return proj_; }
  QMatrix4x4  const& eye()  const { return eye_;  }
  QQuaternion const& rot()  const { return rot_;  }

  void setAspectRatio(qreal);
  void setEye();

  void rotateTo(QQuaternion const&);
  void rotateBy(QQuaternion const&);

protected:
  Canvas     *canvas_;
  QMatrix4x4  proj_, eye_;
  QQuaternion rot_;
};

//------------------------------------------------------------------------------
}}
#endif // GL_CAMERA_H
#endif // STECA_LABS
