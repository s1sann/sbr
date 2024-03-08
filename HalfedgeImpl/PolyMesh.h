#include "MPoint3.h"
#include "MVector3.h"

#ifndef SBR_POLYMESH_H
#define SBR_POLYMESH_H

class MVertex;
class MEdge;
class MHalfedge;
class MFace;

class MVertex {
private:
	int index_;

	MPoint3 point_;

	MHalfedge* he_;
public:
	MVertex() : index_(-1), point_(0, 0, 0), he_(nullptr) {}
	MVertex(double x, double y, double z) : index_(-1), point_(x, y, z), he_(nullptr) {}
	~MVertex() { index_ = -1; }

public:
	MHalfedge* const halfEdge() { return he_; }
	const MHalfedge* const halfEdge() const { return he_; }

	void setHalfedge(MHalfedge* he) { he_ = he; }

	void setPosition(MPoint3 p) { point_ = p; }
	void setPosition(double x, double y, double z) { point_ = MPoint3(x, y, z); }
	MPoint3 position() { return point_; }
	const MPoint3& position() const { return point_; }

	double x() const { return point_.x(); }
	double y() const { return point_.y(); }
	double z() const { return point_.z(); }

	int index() const { return index_; }
	void set_index(int index) { index_ = index; }

	bool isIsolated() const { return he_ == nullptr; }

	void adjustOutgoingHalfedge();
};

class MHalfedge {
private:
	int index_;

	MVertex* v_;
	MEdge* e_;
	MFace* f_;

	MHalfedge* next_, * prev_;
	MHalfedge* twin_;

public:
	MHalfedge() : index_(-1), next_(nullptr), prev_(nullptr), twin_(nullptr),
		v_(nullptr), e_(nullptr), f_(nullptr) {}
	MHalfedge(MHalfedge* next, MHalfedge* prev, MHalfedge* twin, MVertex* v, MEdge* e, MFace* f)
		: index_(-1), next_(next), prev_(prev), twin_(twin), v_(v), e_(e), f_(f) {}

	~MHalfedge() { index_ = -1; }

public:
	MHalfedge* const next() { return next_; }
	MHalfedge* const prev() { return prev_; }
	MHalfedge* const twin() { return twin_; }
	MVertex* const fromVertex() { return v_; }
	MVertex* const toVertex() { return next()->fromVertex(); }
	MEdge* const edge() { return e_; }
	MFace* const face() { return f_; }

	MHalfedge* const rotateNext() { return twin()->next(); }
	MHalfedge* const rotatePrev() { return prev()->twin(); }

	const MHalfedge* const next() const { return next_; }
	const MHalfedge* const prev() const { return prev_; }
	const MHalfedge* const twin() const { return twin_; }
	const MVertex* const fromVertex() const { return v_; }
	const MVertex* const toVertex() const { return next()->fromVertex(); }
	const MEdge* const edge() const { return e_; }
	const MFace* const face() const { return f_; }

	void setNext(MHalfedge* next) { next_ = next; }
	void setPrev(MHalfedge* prev) { prev_ = prev; }
	void setTwin(MHalfedge* twin) { twin_ = twin; }
	void setVertex(MVertex* vertex) { v_ = vertex; }
	void setEdge(MEdge* edge) { e_ = edge; }
	void setFace(MFace* face) { f_ = face; }

	bool isBoundary() const { return f_ == nullptr; }

	int index() { return index_; }

	void set_index(int index) { index_ = index; }
};

class MEdge {
private:
	int index_;

	MVertex* v1_, * v2_;

	MHalfedge* he_;

public:
	MEdge() : index_(-1), v1_(nullptr), v2_(nullptr), he_(nullptr) {}
	MEdge(MVertex* v1, MVertex* v2) : index_(-1), v1_(v1), v2_(v2), he_(nullptr) {}
	MEdge(MVertex* v1, MVertex* v2, MHalfedge* he) : index_(-1), v1_(v1), v2_(v2), he_(he) {}

	~MEdge() { index_ = -1; };

public:
	MHalfedge* const halfEdge() { return he_; }
	const MHalfedge* const halfEdge() const { return const_cast<MEdge*>(this)->halfEdge(); }

	void setHalfedge(MHalfedge* he) { he_ = he; }
	void setVertex(MVertex* v1, MVertex* v2) { v1_ = v1; v2_ = v2; }
	void updateVertex() { v1_ = he_->fromVertex(), v2_ = he_->toVertex(); }

	int index() const { return index_; }
	void set_index(int index) { index_ = index; }

	MVertex* getVertex(int edge_v)
	{
		updateVertex();
		if (edge_v == 0) return v1_;
		else if (edge_v == 1) return v2_;
		else return nullptr;
	}

	const MVertex* getVertex(int edge_v) const {
		return const_cast<MEdge*>(this)->getVertex(edge_v);
	}

	double length()
	{
		updateVertex();
		MVector3 t = v1_->position() - v2_->position();
		return t.norm();
	}

	MPoint3 getCenter()
	{
		updateVertex();
		return v1_->position() * 0.5 + v2_->position() * 0.5;
	}
};

class MFace {
private:
	int index_;

	MHalfedge* he_;

public:
	MFace() : index_(-1), he_(nullptr) {}
	MFace(MHalfedge* he) : index_(-1), he_(he) {}
	~MFace() { index_ = -1; }

public:
	MHalfedge* const halfEdge() { return he_; }
	const MHalfedge* const halfEdge() const { return const_cast<MFace*>(this)->halfEdge(); }

	void setHalfedge(MHalfedge* he) { he_ = he; }

	int index() const { return index_; }
	void set_index(int index) { index_ = index; }

	MPoint3 getFaceCenter();
};

class PolyMesh {
private:
	std::vector<MHalfedge*> half_edges_;
	std::vector<MVertex*> vertices_;
	std::vector<MEdge*> edges_;
	std::vector<MFace*> faces_;

public:
	PolyMesh() {};

	~PolyMesh() {};

public:
	std::vector<MVertex*> faceVertices(MFace* face) const;

	std::vector<MFace*> vertexAdjacentFace(MVertex* vertex) const;

	std::vector<MHalfedge*> vertexAdjacentHalfEdge(MVertex* vertex) const;

	std::vector<MHalfedge*> vertexAdjacentReverseHalfEdge(MVertex* vertex) const;

	std::vector<MVertex*> vertexAdjacentVertices(MVertex* vertex) const;

	std::vector<MFace*> edgeAdjacentFace(MEdge* edge) const;

	MFace* edgeAdjacentAnotherFace(MEdge* edge, MFace* face) const;

	bool isBoundary(MVertex* vertex) const;

	bool isBoundary(const MEdge* edge) const;

	bool isBoundary(const MHalfedge* halfedge) const;

	MVertex* addVertex(double x, double y, double z);

	MFace* addFace(std::vector<size_t>& v_id);

	MFace* addFace(std::vector<MVertex*>& v);

	MEdge* edgeBetween(MVertex* v0, MVertex* v1);

	MEdge* addEdge(MVertex* v_begin, MVertex* v_end);

	MEdge* newEdge(MVertex* v1, MVertex* v2);

	MHalfedge* newHalfEdge();

	MFace* newFace();

	size_t nVertices() const { return vertices_.size(); }

	size_t nFaces() const { return faces_.size(); }

	MVertex* vertex(const size_t id) { return (id < nVertices() ? vertices_[id] : nullptr); }

	MFace* face(const size_t id) { return (id < nFaces() ? faces_[id] : nullptr); }
};

#endif // SBR_POLYMESH_H