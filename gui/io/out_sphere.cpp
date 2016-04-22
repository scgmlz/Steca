// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      out_sphere.cpp
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

#include "out_sphere.h"
#include "thehub.h"
#include "core/core_reflection_info.h"
#include "types/core_type_matrix.h"
#include <GL/glu.h>

//#include "qv_gl.h"
#include <QMatrix4x4>
#include <QMouseEvent>

namespace gui { namespace io {
//------------------------------------------------------------------------------

GLWidget::GLWidget(QWidget* parent): super(parent) {
  init();
}

void GLWidget::reset() {
  init();
  updateGL();
}

void GLWidget::rotate(QQuaternion const& rot) {
  rot_ = rot;
  onRotate();
  updateGL();
  emit rotated();
}

int GLWidget::width() const {
  return super::width() * devicePixelRatio();
}

int GLWidget::height() const {
  return super::height() * devicePixelRatio();
}

void GLWidget::init() {
  zoom_       = 5;
  rot_        = QQuaternion();
  clearColor_ = Qt::black;
  flat_       = false;
}

void GLWidget::initializeGL() {
  glClearColor(clearColor_);
//  glDisable(GL_CULL_FACE);
//  glDisable(GL_DEPTH_TEST);

//  glShadeModel(GL_SMOOTH);
//  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
//  glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
}

void GLWidget::resizeGL(int,int) {
}

void GLWidget::paintGL() {
  glClear(GL_COLOR_BUFFER_BIT);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  int w = width(), h = height();

  if (flat_) {
    glViewport(0,0,w,h);
  } else {
    if (w > h) {
      glViewport(0, (h-w)/2, w, w);
    } else {
      glViewport((w-h)/2, 0, h, h);
    }
  }

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  if (flat_) {
    glOrtho(0,w,h,0,0,1000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
  } else {
    gluPerspective(zoom_,1,1,100);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0,0,-30);

    QMatrix4x4 matrix;
    matrix.translate(0,0,-10);
    matrix.rotate(rot_);
    glMultMatrixf(matrix.data());
  }

  onPaintGL();
}


void GLWidget::mousePressEvent(QMouseEvent* e) {
  lastMousePos = e->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent* e) {
  if (flat_) return;

  QPoint pos = e->pos();
  QVector2D d = QVector2D(pos - lastMousePos);
  lastMousePos = pos;

  int dx = d.x(), dy = d.y();

  QVector3D n = QVector3D(dy,dx,0).normalized();
  QQuaternion q = QQuaternion::fromAxisAndAngle(n,d.length()/2);

  rotate(q*rot_);
}

void GLWidget::mouseReleaseEvent(QMouseEvent*) {
}

void GLWidget::glClearColor(QColor const& c) {
  ::glClearColor(c.redF(),c.greenF(),c.blueF(),c.alphaF());
}

void GLWidget::glColor(QColor const& c) {
  ::glColor4f(c.redF(),c.greenF(),c.blueF(),c.alphaF());
}

//---------------------------------------------------------------------------

Sphere::vertex_t::vertex_t(): vertex_t(QVector3D()) {
}

Sphere::vertex_t::vertex_t(float x, float y, float z): vertex_t(QVector3D(x,y,z)) {
}

Sphere::vertex_t::vertex_t(QVector3D const& v): QVector3D(v), val(0), prominence(0) {
  for_i (8) nbr[i] = nullptr;
}

Sphere::vertex_t::vertex_t(core::vector3f const& v): vertex_t(QVector3D(v._0,v._1,v._2)) {
}

Sphere::vertex_t Sphere::vertex_t::fromPolar(qreal azi, qreal ele, qreal radius) {
  qreal	razi = deg2rad(azi), rele = deg2rad(ele);
  qreal cele = cosf(rele);
  qreal x = cosf(razi) * cele, y = sinf(razi) * cele, z = sinf(rele);
  return vertex_t(x*radius,y*radius,z*radius);
}

qreal Sphere::vertex_t::azi() const {
  return rad2deg(atan2f(y(),x()));
}

qreal Sphere::vertex_t::ele() const {
  return rad2deg(asinf(z()/length()));
}

//------------------------------------------------------------------------------

static float const t    = (1+sqrtf(5))/2;
static float const tau	= t/sqrtf(1+t*t);
static float const one	= 1/sqrtf(1+t*t);
static float const st3	= 1 / sqrtf(3);

typedef uint  const uint3[3];
typedef float const float3[3];

#if 0
// tetrahedron
static float3c tetra_vertices[] = {
	{ st3, st3, st3 }, {-st3,-st3, st3 }, {-st3, st3,-st3 }, { st3,-st3,-st3 },
	{ 99, 99, 99 }
};

uint3c tetra_faces[] = {
	{ 0, 2, 1 }, { 0, 1, 3 }, { 2, 3, 1 }, { 3, 2, 0 },
	{ 99, 99, 99 }
};
#endif

// octahedron
static float3 octa_vertices[] = {
	{ 0, 0,-1 }, { 1, 0, 0 }, { 0,-1, 0 }, {-1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 },
	{ 99, 99, 99 }
};

uint3 octa_faces[] = {
	{ 0, 1, 2 }, { 0, 2, 3 }, { 0, 3, 4 }, { 0, 4, 1 },
	{ 5, 2, 1 }, { 5, 3, 2 }, { 5, 4, 3 }, { 5, 1, 4 },
	{ 99, 99, 99 }
};

#if 0
// icosahedron
static float3c ico_vertices[] = {
	{ tau, one, 0 }, {-tau, one, 0 },  {-tau, -one, 0 }, { tau, -one, 0 },
	{ one, 0, tau }, { one, 0, -tau }, {-one, 0, -tau },  {-one, 0, tau },
	{ 0, tau, one }, { 0, -tau, one }, { 0, -tau, -one }, { 0, tau, -one },
	{ 99, 99, 99 }
};

uint3c ico_faces[] = {
	{  4,  8,  7 }, {  4,  7,  9 }, {  5,  6, 11 }, {  5,  6, 10 },
	{  0,  4,  3 }, {  0,  3,  5 }, {  2,  7,  1 }, {  2,  1,  6 },
	{  8,  0, 11 }, {  8, 11,  1 }, {  9, 10,  3 }, {  9,  2, 10 },
	{  8,  4,  0 }, { 11,  0,  5 }, {  4,  9,  3 }, {  5,  3, 10 },
	{  7,  8,  1 }, {  6,  1, 11 }, {  7,  2,  9 }, {  6, 10,  2 },
	{ 99, 99, 99 }
};
#endif

//---------------------------------------------------------------------------

class Sphere::Polyhedron {
public:
	struct face_t {
		face_t();
		face_t(uint,uint,uint);
		uint a,b,c; // vertex index

		GLdouble depth;
		friend bool operator< (face_t const&, face_t const&);	// by depth
	};

	typedef QVector<face_t> faces_t;

public:
	Polyhedron();

private:
	void init();
	void subdivide();
	uint addVertex(vertex_t const&);

public:
	vertex_vec	vertices;
	faces_t     faces;
};

Sphere::Polyhedron::face_t::face_t()
  :	a(0), b(0), c(0), depth(0) {
}

Sphere::Polyhedron::face_t::face_t(uint a_, uint b_, uint c_)
  :	a(a_), b(b_), c(c_), depth(0) {
}

bool operator< (Sphere::Polyhedron::face_t const& f1, Sphere::Polyhedron::face_t const& f2) {
	return f1.depth < f2.depth;
}

Sphere::Polyhedron::Polyhedron(): vertices(), faces() {
	init();
}

void Sphere::Polyhedron::init() {
	for (float3 *p = octa_vertices; 99 > (*p)[0]; ++p) {
		vertices.append(vertex_t(QVector3D((*p)[0],(*p)[1],(*p)[2])));
	}

	for (uint3 *p = octa_faces; 99 > (*p)[0]; ++p) {
		faces.append(face_t((*p)[0],(*p)[1],(*p)[2]));
	}

	subdivide();
	subdivide();
	subdivide();
	subdivide();
}

void Sphere::Polyhedron::subdivide() {
	uint const fsize = faces.size();

	for (uint i=0; i<fsize; ++i) {
		face_t &face = faces[i];
		uint ai = face.a, bi = face.b, ci = face.c;
		QVector3D &a = vertices[ai], &b = vertices[bi], &c = vertices[ci];
		QVector3D ab = (a + (b - a)/2).normalized();
		QVector3D bc = (b + (c - b)/2).normalized();
		QVector3D ca = (c + (a - c)/2).normalized();
		uint abi = addVertex(vertex_t(ab));
		uint bci = addVertex(vertex_t(bc));
		uint cai = addVertex(vertex_t(ca));

		face.a = abi; face.b = bci; face.c = cai;
		faces.append(face_t(ai,abi,cai));
		faces.append(face_t(bi,bci,abi));
		faces.append(face_t(ci,cai,bci));
	}
}

uint Sphere::Polyhedron::addVertex(vertex_t const& v) {
	for (int i=0; i<vertices.size(); ++i) {
		if (((QVector3D const&)vertices[i])==((QVector3D const&)v)) return i;
	}

	vertices.append(v); return vertices.size()-1;
}

//---------------------------------------------------------------------------

class Sphere::Geography {
public:
	Geography();

public:
	static int const STEP = 15;
	static int const GF = 1;	// grid factor

private:
	void init();

public:
	vertex_vec vertices;
};

Sphere::Geography::Geography(): vertices() {
	init();
}

int wrap(int i, int mx) {
	if (i < 0)
		i = mx - 1;
	else if (i >= mx) i = 0;
	return i;
}

#define AZI(i) wrap(i,aziSize)
#define ELI(i) wrap(i,eliSize)

void Sphere::Geography::init() {
	for (int azi=-180; azi<180; azi+=STEP) {
		for (int ele=-90; ele<=90; ele+=STEP) {
			vertices.append(vertex_t::fromPolar(azi, ele));
		}
	}

	// neighbours
	vertex_vec& vs = vertices;
	uint const aziSize = 360/STEP;
	uint const eliSize = 180/STEP + 1;
	for (uint azi = 0; azi < aziSize; ++azi) {
		for (uint eli = 0; eli < eliSize; ++eli) {
			vertex_t &v = vs[azi*eliSize+eli];
			v.sn(0,&vs[AZI(azi+1)*eliSize+ELI(eli+0)]); // e
			v.sn(1,&vs[AZI(azi+1)*eliSize+ELI(eli+1)]); // ne
			v.sn(2,&vs[AZI(azi+0)*eliSize+ELI(eli+1)]); // n
			v.sn(3,&vs[AZI(azi-1)*eliSize+ELI(eli+1)]); // nw
			v.sn(4,&vs[AZI(azi-1)*eliSize+ELI(eli+0)]); // w
			v.sn(5,&vs[AZI(azi-1)*eliSize+ELI(eli-1)]); // sw
			v.sn(6,&vs[AZI(azi+0)*eliSize+ELI(eli-1)]); // s
			v.sn(7,&vs[AZI(azi+1)*eliSize+ELI(eli-1)]); // se
		}
	}
}

//------------------------------------------------------------------------------

Sphere::Sphere(QWidget *parent)
  : super(parent), poly_(NULL), geo_(NULL),
	geoMesh_(false), polyMesh_(false), faces_(false),
	blackWhite_(false) {

	poly_ = new Polyhedron(); geo_ = new Geography();
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	if (blackWhite_) {
		clearColor_     = QColor(Qt::white);
		polyMeshColor_  = QColor(Qt::darkGray);
		geoMeshColor_   = QColor(Qt::darkGray);
		geoCrossColor_  = QColor(Qt::black);
	} else {
		clearColor_     = QColor(Qt::gray);
		polyMeshColor_  = QColor(Qt::gray).darker(300);
		geoMeshColor_   = QColor(Qt::gray).darker(160);
		geoCrossColor_  = QColor(Qt::darkYellow);
	}
}

void Sphere::update() {
	updateGL();
}

void Sphere::showGeo(bool show) {
	geoMesh_ = show;
	updateGL();
}

void Sphere::showSphere(bool show) {
	flat_ = !show;
	updateGL();
}

void Sphere::showPoly(bool show) {
	polyMesh_ = show;
	updateGL();
}

void Sphere::showFaces(bool show) {
	faces_ = show;
	updateGL();
}

void Sphere::showBW(bool bw) {
	blackWhite_ = bw;
	updateGL();
}

void Sphere::resetRot() {
	rotate(QQuaternion(.5,-.5,-.5,-.5));
}

void Sphere::init() {
	super::init();
	resetRot();
}

void Sphere::onPaintGL() {
  glLineWidth(1);

  if (polyMesh_ && !flat_) paintPolyhedronMesh();
	if (geoMesh_)	paintGeographicMesh();
	if (faces_)		paintFaces();
	if (flat_)		paintTops();
}

void Sphere::onRotate() {
	sortFaces();
}

void Sphere::sortFaces() {
	GLdouble model[16];	glGetDoublev(GL_MODELVIEW_MATRIX, model);
	GLdouble proj[16];	glGetDoublev(GL_PROJECTION_MATRIX, proj);
	GLint    view[4];	  glGetIntegerv(GL_VIEWPORT, view);

	// compute the depth of projected vertices
	vertex_vec const	  &vs = poly_->vertices;
	Polyhedron::faces_t &fs = poly_->faces;

	/* Sorting faces based on the first vertex. Works well for a sphere,
	 * since faces do not overlap.
	 */
	for (Polyhedron::faces_t::iterator fi = fs.begin(); fi!=fs.end(); ++fi) {
		vertex_t const &v = vs[fi->a];
		GLdouble prjX, prjY, prjZ;
		gluProject(v.x(), v.y(), v.z(), model, proj, view, &prjX, &prjY, &prjZ);
		fi->depth = prjZ;
	}

	std::sort(fs.begin(), fs.end());
}

void Sphere::paintPolyhedronMesh() const {
	glColor(polyMeshColor_);

	Polyhedron::faces_t const &fs = poly_->faces;
	for (Polyhedron::faces_t::const_iterator fi = fs.begin(); fi != fs.end(); ++fi) {
		Polyhedron::face_t const &face = *fi;
		vertex_vec const &vs = poly_->vertices;
		vertex_t const &a = vs[face.a], &b = vs[face.b], &c = vs[face.c];

		glBegin(GL_LINE_LOOP);
		glMappedVertex(a); glMappedVertex(b); glMappedVertex(c);
		glEnd();
	}
}

void Sphere::paintGeographicMesh() const {
	glColor(geoMeshColor_);

	vertex_vec const& vs = geo_->vertices;
	uint const aziSize = 360/Geography::STEP;
	uint const eliSize = 180/Geography::STEP + 1;
	if (flat_) {
		int w = width(), h = height();
		// paint lines of longitude
		for (uint azi = 0; azi < aziSize; azi+=Geography::GF) {
			float x = (float)azi/(float)aziSize;
			glBegin(GL_LINES);
			glVertex2f(x*w,0);
			glVertex2f(x*w,h);
			glEnd();
		}

		// paint lines of latitude
		for (uint eli = 0; eli < eliSize; eli+=Geography::GF) {
			float y = (float)eli/(float)(eliSize-1);
			glBegin(GL_LINES);
			glVertex2f(0,y*h);
			glVertex2f(w,y*h);
			glEnd();
		}

		if (blackWhite_) glLineWidth(1);

		// cross
		glColor(geoCrossColor_);
		{
			float x = 0.25f;
			glBegin(GL_LINES);
			glVertex2f(x*w,0);
			glVertex2f(x*w,h);
			glEnd();
		}
		{
			float x = 0.5;
			glBegin(GL_LINES);
			glVertex2f(x*w,0);
			glVertex2f(x*w,h);
			glEnd();
		}
		{
			float x = 0.75f;
			glBegin(GL_LINES);
			glVertex2f(x*w,0);
			glVertex2f(x*w,h);
			glEnd();
		}
		{
			float y = 0.5;
			glBegin(GL_LINES);
			glVertex2f(0,y*h);
			glVertex2f(w,y*h);
			glEnd();
		}
	} else {
		// paint lines of longitude
		for (uint azi = 0; azi < aziSize; ++azi) {
			glBegin(GL_LINE_STRIP);
			for (uint eli = 0; eli < eliSize; ++eli) {
				vertex_t const &v = vs[azi*eliSize+eli];
				glMappedVertex(v);
			}
			glEnd();
		}

		// paint lines of latitude
		for (uint eli = 0; eli < eliSize; ++eli) {
			glBegin(GL_LINE_LOOP);
			for (uint azi = 0; azi < aziSize; ++azi) {
				vertex_t const &v = vs[azi*eliSize+eli];
				glMappedVertex(v);
			}
			glEnd();
		}

		// cross
		glColor(geoCrossColor_);
		{
			glBegin(GL_LINE_STRIP);
			for (uint eli = 0; eli < eliSize; ++eli) {
				vertex_t const &v = vs[aziSize/2*eliSize+eli];
				glMappedVertex(v);
			}
			glEnd();
		}
	}
}

static float const GR_SCALE = 1;
static float clamp(float v) {
	return qBound(0.f,v,1.f);
}

static void black_blue_green(QColor &qc, float v) {
	float const mid = 0.5f;
	float cr, cg, cb = v * 1.6f;
	if (v >= mid) {
		v = (v - mid) * 2;
		cr = v * qMax(0.f, 2 * (v - mid));
		cg = v * GR_SCALE;
		//			c.b = f * .2f;
		float b = 1 - (cr + cg) / 1;
		cr *= 1.1;
		cg *= 1.1;
		cb = qMin(cb, b);
	} else {
		//f = (mid - f) * 2;
		cr = 0;//f * max(0.f, 2 * (f - mid));
		cg = 0;
	}

	qc.setRgbF(clamp(cr), clamp(cg), clamp(cb), clamp((cr + cg + cb) / 3));
}

static void white_black(QColor &qc, float v) {
	v = v*v*v;
	qc.setRgbF(clamp(1-v), clamp(1-v), clamp(1-v), clamp(v));
}

static void colorScale(QColor &qc, float v, bool blackWhite) {
	if (blackWhite)
		white_black(qc, v);
	else
		black_blue_green(qc, v);
}

void Sphere::paintFaces() const {
	if (flat_) {
		vertex_vec const& vs = geo_->vertices;
		uint const aziSize = 360/Geography::STEP;
		uint const eliSize = 180/Geography::STEP + 1;
		int w = width(), h = height();
		for (uint azi = 0; azi < aziSize; ++azi) {
			for (uint eli = 0; eli < eliSize-1; ++eli) {
				uint az1 = (azi==aziSize-1) ? 0 : (azi+1);
				vertex_t const &a = vs[azi*eliSize+eli];
				vertex_t const &b = vs[azi*eliSize+eli+1];
				vertex_t const &c = vs[az1*eliSize+eli+1];
				vertex_t const &d = vs[az1*eliSize+eli];
				QColor ca, cb, cc, cd;
				colorScale(ca, a.val, blackWhite_);
				colorScale(cb, b.val, blackWhite_);
				colorScale(cc, c.val, blackWhite_);
				colorScale(cd, d.val, blackWhite_);

				// "1-" to turn upside down and left-right >>>
				float x1 = w*(1-(float)azi/(float)aziSize);
				float x2 = w*(1-(float)(azi+1)/(float)aziSize);
				float y1 = h*(1-(float)eli/(float)(eliSize-1));
				float y2 = h*(1-(float)(eli+1)/(float)(eliSize-1));

				glBegin(GL_QUADS);
				glColor(ca); glVertex2f(x1,y1);
				glColor(cb); glVertex2f(x1,y2);
				glColor(cc); glVertex2f(x2,y2);
				glColor(cd); glVertex2f(x2,y1);
				glEnd();
			}
		}
	} else {
		Polyhedron::faces_t const &fs = poly_->faces;
		for (Polyhedron::faces_t::const_iterator fi = fs.begin(); fi != fs.end(); ++fi) {
			Polyhedron::face_t const &face = *fi;
			vertex_vec const &vs = poly_->vertices;
			vertex_t const &a = vs[face.a], &b = vs[face.b], &c = vs[face.c];

			QColor ca, cb, cc;
			colorScale(ca, a.val, blackWhite_);
			colorScale(cb, b.val, blackWhite_);
			colorScale(cc, c.val, blackWhite_);

			glBegin(GL_TRIANGLES);
			glColor(ca);	glMappedVertex(a);
			glColor(cb);	glMappedVertex(b);
			glColor(cc);	glMappedVertex(c);
			glEnd();
		}
	}
}

// Bezier quad control point
static QPointF bezP1(QPointF const& p0, QPointF const& f, QPointF const& p2) {
	float u = 0.5;
	return (f - (1-u)*(1-u)*p0 - u*u*p2)/(2*(1-u)*u);
}

// Bezier point
static QPointF bezP(QPointF const& p0, QPointF const& p1, QPointF const& p2, float t) {
	float t1 = (1-t);
	return (t1*t1)*p0 + (2*t1*t)*p1 + (t*t)*p2;
}

void Sphere::paintTops() const {
	int w = width(), h = height();
#if 0 	//
	uint numTops = 0;
	for (uint azi = 1; azi < aziSize-1; ++azi) {
		for (uint eli = 1; eli < eliSize-1; ++eli) {
			Vertex const &v = vs[azi][eli];
			if (v.prominence_ > 0) {
				numTops++;
				// "1-" to turn upside down and left-right >>>
				float x = w*(1-(float)azi/(float)aziSize);
				float y = h*(1-(float)eli/(float)(eliSize-1));
				glColor(QColor(Qt::red));

				uint const S = v.prominence_/6;
				glBegin(GL_QUADS);
				glVertex2f(x+S,y);
				glVertex2f(x,y+S);
				glVertex2f(x-S,y);
				glVertex2f(x,y-S);
				glEnd();
			}
		}
	}
#endif

	// jagged
//	if (tops.size()>=2) {
//		float thd = 14.f/tops.size();
//		for (int i=tops.size()-2; i>=0; --i) {
//			QPointF const& top0 = tops[i];
//			QPointF const& top1 = tops[i+1];
//			// "1-" to turn upside down and left-right >>>
//			float x0 = w*(1-top0.x());
//			float y0 = h*(1-top0.y());
//			float x1 = w*(1-top1.x());
//			float y1 = h*(1-top1.y());
//
//			glColor(QColor(Qt::darkRed));
//			DrawLine(x0, y0, x1, y1, thd*(i+1),thd*i);
//		}
//	}

	// Bezier
	if (tops_.size()>=3) {
		glLineWidth(6);
		glColor(QColor(Qt::darkGray));

		glBegin(GL_LINE_STRIP);
		for (int i=tops_.size()-1; i>2; --i) {
			QPointF p0 = tops_[i];
			QPointF f = tops_[i-1];
			QPointF p2 = tops_[i-2];
			QPointF p1 = bezP1(p0,f,p2);
			uint const SEGS = 4;
			for (uint s=0; s<SEGS; ++s) {
				float t = 0.5*s/SEGS;
				QPointF p = bezP(p0,p1,p2,t);
				glMappedVertex(p, w, h);
			}
		}
		glEnd();
	}
}

QVector3D Sphere::mapSpherePoint(QVector3D const& v) const {
	return v;
}

void Sphere::glMappedVertex(QVector3D const& v) const {
	QVector3D m = mapSpherePoint(v);
	glVertex3d(m.x(), m.y(), m.z());
}

void Sphere::glMappedVertex(QPointF p, int w, int h) const {
	float x = w*(1-p.x());
	float y = h*(1-p.y());
	glVertex2f(x,y);
}

Sphere::vertex_vec& Sphere::vertices() {
  return flat_ ? geo_->vertices : poly_->vertices;
}

//------------------------------------------------------------------------------

class OutPoleSphereParams: public panel::BoxPanel {
  SUPER(OutPoleSphereParams,panel::BoxPanel)
public:
  OutPoleSphereParams(TheHub&);
};

OutPoleSphereParams::OutPoleSphereParams(TheHub& hub): super("", hub, Qt::Horizontal) {
  box_->addWidget(label("Param"));
  box_->addWidget(editCell(16));
  box_->addStretch();
}

//------------------------------------------------------------------------------

OutSphere::OutSphere() {
  showGeo(true);
}

void OutSphere::set(core::ReflectionInfos infos) {
  infos_ = infos;
  update();
}

void OutSphere::onPaintGL() {
  super::onPaintGL();

  for (auto &info: infos_)
    point(info.alpha(),info.beta(),info.inten());
}

void OutSphere::point(float alpha, float beta, float inten) {
  alpha = 90 - alpha;
  vertex_t v1 = vertex_t::fromPolar(beta,alpha,1);
  vertex_t v2 = vertex_t::fromPolar(beta,alpha,1 + inten/300);

  glLineWidth(2);
  QColor clr(Qt::yellow);
  glColor(clr);
  glBegin(GL_LINES);
  glMappedVertex(v1);
  glMappedVertex(v2);
  glEnd();
}

//------------------------------------------------------------------------------

PoleSphere::PoleSphere(TheHub& hub,rcstr title,QWidget* parent): super(hub,title,parent) {
  setWidgets(new OutPoleSphereParams(hub), (sphere_ = new OutSphere()));
  calculate();
}

void PoleSphere::calculate() {
  sphere_->set(hub_.reflectionInfos(1));
}

//------------------------------------------------------------------------------
}}
#endif // STECA_LABS
// eof
