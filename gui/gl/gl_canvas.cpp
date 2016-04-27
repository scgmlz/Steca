// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      gl_canvas.h
//! @brief     OpenGl canvas
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

#include "gl_canvas.h"
#include "gl_camera.h"
#include <QMouseEvent>

namespace gui { namespace gl {
//------------------------------------------------------------------------------

Canvas::Canvas(): clearColor_(Qt::gray) {
}

void Canvas::setCamera(Camera* camera) {
  camera_.reset(camera);
  if (camera_) {
    camera_->canvas_ = this;
    camera_->setEye();
  }
}

QVector<GLfloat> glData;

void Canvas::initializeGL() {
  initializeOpenGLFunctions();
}

void Canvas::paintGL() {
  glClearColor(clearColor_.redF(),clearColor_.greenF(),clearColor_.blueF(),1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  if (camera_) {
    auto m = camera_->proj()*camera_->eye();
    QVector3D v1(0,0,0), v2(10,0,0), v3(0,10,0), v4(0,10,10);

    v1 = m * v1;
    v2 = m * v2;
    v3 = m * v3;
    v4 = m * v4;

    glBegin(GL_LINE_LOOP);
    glVertex3d(v1.x(),v1.y(),v1.z());
    glVertex3d(v2.x(),v2.y(),v2.z());
    glVertex3d(v3.x(),v3.y(),v3.z());
    glVertex3d(v4.x(),v4.y(),v4.z());
    glEnd();
  }
}

void Canvas::resizeGL(int w, int h) {
  if (camera_)
    camera_->setAspectRatio(h>0 ? qreal(w)/h : 1);
}

void Canvas::mousePressEvent(QMouseEvent* e) {
  mouseLastPos_ = mousePressPos_ = e->pos();
}

void Canvas::mouseMoveEvent(QMouseEvent* e) {
  QPoint pos = e->pos();
  auto d = QVector2D(pos - mouseLastPos_);
  mouseLastPos_ = pos;

  if (camera_) {
    auto n = QVector3D(d.y(),d.x(),0).normalized();
    auto q = QQuaternion::fromAxisAndAngle(n, d.length()/2);
    camera_->rotateBy(q);
  }
}

void Canvas::mouseReleaseEvent(QMouseEvent*) {
  // nothing here
}

//------------------------------------------------------------------------------
}}
// eof
