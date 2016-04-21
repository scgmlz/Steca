// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      out_sphere.h
//! @brief     OpenGl sphere
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

// Original sphere code Copyright 2013-6 Ian G Burleigh http://phi53.xyz
// MIT-licence

#ifdef STECA_LABS
#ifndef OUT_SPHERE_H
#define OUT_SPHERE_H

#include "core_defs.h"
#include "types/core_types_fwd.h"
#include "types/core_coords.h"

#include <QGLWidget>
#include <QQuaternion>
#include <QColor>

namespace gui { namespace io {
//------------------------------------------------------------------------------

class GLWidget: public QGLWidget {
  SUPER(GLWidget,QGLWidget)
  Q_OBJECT
public:
  GLWidget(QWidget* = nullptr);

  virtual void reset();

  void rotate(QQuaternion const&);

  int width()  const;
  int height() const;

signals:
  void rotated();

protected:
  virtual void init();

  float zoom_; QQuaternion rot_;
  QColor clearColor_;
  bool flat_;

  void glInit();
  void initializeGL();
  void resizeGL(int,int);
  void paintGL();

  virtual void onPaintGL() = 0;
  virtual void onRotate() {}

  QPoint lastMousePos;

  void mousePressEvent(QMouseEvent*);
  void mouseMoveEvent(QMouseEvent*);
  void mouseReleaseEvent(QMouseEvent*);
};

//------------------------------------------------------------------------------

class Sphere: public GLWidget {
	SUPER(Sphere,GLWidget)
  Q_OBJECT
public:
  struct vertex_t: QVector3D {
    vertex_t();
    vertex_t(QVector3D const&);
    vertex_t(core::vector3f const&);
    vertex_t(float azi, float ele);

    core::NormalizedAngle<float> azi, ele;
    float val;
    int  prominence;

    vertex_t const *nbr[8];	// e,ne,n,nw,w,sw,s,se

    void sn(uint i, vertex_t const* v) {
      nbr[i] = v;
    }

    float nv(uint i) { // nbr value
      return nbr[i]->val;
    }

  private:
    static void      toPolar(QVector3D const&,float& azi, float& ele);
    static QVector3D toCart(float azi, float ele);
  };

  typedef QVector<vertex_t> vertex_vec;

public:
  Sphere(QWidget* = NULL);

	void update();
	void showGeo(bool);
	void showSphere(bool);
	void showPoly(bool);
	void showFaces(bool);
	void showBW(bool);
	void resetRot();

protected:
	void init();
	void onPaintGL();
	void onRotate();

private:
	bool compare(int,int);
	void sortFaces();

	void paintPolyhedronMesh()	const;
	void paintGeographicMesh()	const;
	void paintFaces()			const;
	void paintTops()			const;

	QVector3D mapSpherePoint(QVector3D const&)  const;	// distort
	void glMappedVertex(QVector3D const&) const;
	void glMappedVertex(QPointF p, int w, int h) const;

public:
	class Polyhedron; class Geography;

	vertex_vec& vertices();

private:
	Polyhedron *poly_; Geography *geo_;
	bool  geoMesh_, polyMesh_, faces_;
	bool  blackWhite_;

	QColor geoMeshColor_, polyMeshColor_, geoCrossColor_;
	QVector<QPointF> tops_;
};

//------------------------------------------------------------------------------
}}
#endif // OUT_SPHERE_H
#endif // STECA_LABS
