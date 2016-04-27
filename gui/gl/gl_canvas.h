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

#ifdef STECA_LABS
#ifndef GL_CANVAS_H
#define GL_CANVAS_H

#include "core_defs.h"
#include "gl_camera.h"

#include <QOpenGLWidget>
#include <QOpenGLFunctions>

namespace gui { namespace gl {
//------------------------------------------------------------------------------

class Canvas: public QOpenGLWidget, protected QOpenGLFunctions {
  SUPER(Canvas,QOpenGLWidget)
  Q_OBJECT

public:
  Canvas();

  QSize minimumSizeHint() const { return QSize(50,50);    }
  QSize sizeHint()        const { return QSize(400,400);  }

  void setCamera(Camera*);  // takes ownership

protected:
  void initializeGL();
  void paintGL();
  void resizeGL(int w, int h);

  void mousePressEvent(QMouseEvent*);
  void mouseMoveEvent(QMouseEvent*);
  void mouseReleaseEvent(QMouseEvent*);

private:
  QScopedPointer<Camera> camera_;
  QColor clearColor_;
  QPoint mousePressPos_, mouseLastPos_;
};

//------------------------------------------------------------------------------
}}
#endif // GL_CANVAS_H
#endif // STECA_LABS
