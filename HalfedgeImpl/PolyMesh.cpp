#include "PolyMesh.h"

void MVertex::adjustOutgoingHalfedge() {
    if (isIsolated())
        return;

    MHalfedge* begin = halfEdge();
    MHalfedge* he = begin;

    do {
        if (he->isBoundary()) {
            setHalfedge(he);
            return;
        }
        he = he->twin()->next();
    } while (he != begin);
}

MPoint3 MFace::getFaceCenter() {
    assert(he_ != nullptr);

    MHalfedge* he = he_;

    MPoint3 getCenter(0, 0, 0);
    do {
        MVertex* fv = he->toVertex();
        getCenter += fv->position();
        he = he->next();
    } while (he != he_);

    getCenter = MPoint3(getCenter.x() / 3, getCenter.y() / 3, getCenter.z() / 3);
    return getCenter;
}

std::vector<MVertex*> PolyMesh::faceVertices(MFace* face) const {
    std::vector<MVertex*> f_vert;
    MHalfedge* he_begin = face->halfEdge();
    MHalfedge* he = face->halfEdge();

    do {
        f_vert.push_back(he->fromVertex());
        he = he->next();
    } while (he != he_begin);
    return f_vert;
}

MVertex* PolyMesh::addVertex(double x, double y, double z) {
    auto w_ptr = new MVertex(x, y, z);
    w_ptr->set_index(vertices_.size());
    vertices_.push_back(w_ptr);
    return w_ptr;
}


MFace* PolyMesh::addFace(std::vector<size_t>& v_loop_id) {
    size_t v_size = v_loop_id.size();

    std::vector<MVertex*> v_loop;
    v_loop.reserve(v_size);

    for (size_t i = 0; i < v_size; i++) {
        v_loop.push_back(vertices_[v_loop_id[i]]);
    }
    return addFace(v_loop);
}

MFace * PolyMesh::addFace(std::vector<MVertex*>&v) {
    size_t v_size = v.size();

    MFace* face = newFace();

    std::vector<bool> is_edge_new(v_size, false);
    std::vector<bool> is_needc(v_size, false);

    MHalfedge* he1 = nullptr;
    MHalfedge* he2 = nullptr;
    std::vector<MHalfedge*> edges;
    for (size_t i = 0; i < v_size; i++) {
        size_t j = (i + 1) % v_size;

        MEdge* e = edgeBetween(v[i], v[j]);
        if (e == nullptr) {
            e = addEdge(v[i], v[j]);
            is_edge_new[i] = true;
        }

        if (e->halfEdge()->fromVertex() == v[i]) {
            he1 = e->halfEdge();
            he2 = e->halfEdge()->twin();
        }
        else {
            he1 = e->halfEdge()->twin();
            he2 = e->halfEdge();
        }

        if (face->halfEdge() == nullptr) {
            face->setHalfedge(he1);
        }
        he1->setFace(face);
        edges.push_back(he1);
    }

    MHalfedge* he_ip, * he_in;
    MHalfedge* he_bp, * he_bn;
    for (size_t i = 0; i < v_size; i++) {
        size_t j = (i + 1) % v_size;

        MVertex* vh = v[j];
        he_ip = edges[i];
        he_in = edges[j];

        assert(he_ip != nullptr);
        assert(he_in != nullptr);

        size_t id = 0;
        if (is_edge_new[i]) id |= 1;
        if (is_edge_new[j]) id |= 2;

        MHalfedge* he_op = he_in->twin();
        MHalfedge* he_on = he_ip->twin();

        switch (id) {
        case 1: // prev is new, next is old
            he_bp = he_in->prev();
            he_bp->setNext(he_on);
            he_on->setPrev(he_bp);
            vh->setHalfedge(he_on);
            break;

        case 2: // next is new, prev is old
            he_bn = he_ip->next();
            he_op->setNext(he_bn);
            he_bn->setPrev(he_op);
            vh->setHalfedge(he_bn);
            break;

        case 3: // both are new
            if (vh->halfEdge() == nullptr)
            {
                vh->setHalfedge(he_on);
                he_op->setNext(he_on);
                he_on->setPrev(he_op);
            }
            else
            {
                he_bn = vh->halfEdge();
                he_bp = he_bn->prev();

                he_bp->setNext(he_on);
                he_on->setPrev(he_bp);
                he_op->setNext(he_bn);
                he_bn->setPrev(he_op);
            }
            break;
        case 0:
            is_needc[j] = true;
        }

        he_ip->setNext(he_in);
        he_in->setPrev(he_ip);
    }

    for (size_t i = 0; i < v_size; i++) {
        if (is_needc[i]) { // 都是旧边调整出去的第一条边，尽量为边界边
            v[i]->adjustOutgoingHalfedge();
        }
    }

    return face;
}

MEdge* PolyMesh::edgeBetween(MVertex* v0, MVertex* v1) {
    //MHalfedge* he = halfedgeBetween(v0, v1);
    //return he->edge();

    if (v0 == nullptr || v1 == nullptr) return nullptr;
    if (v0->halfEdge() == nullptr) return nullptr;

    MHalfedge* he_begin = v0->halfEdge();
    MHalfedge* he = v0->halfEdge();

    do {
        if (he->toVertex() == v1) {
            return he->edge();
        }
        he = he->rotateNext();
    } while (he != he_begin);

    return nullptr;
}

MEdge* PolyMesh::addEdge(MVertex* v_begin, MVertex* v_end) {
    if (v_begin == nullptr || v_end == nullptr) {
        return nullptr;
    }

    MEdge* edge = edgeBetween(v_begin, v_end);
    if (edge != nullptr) {
        return edge;
    }

    edge = newEdge(v_begin, v_end);
    MHalfedge* he0 = newHalfEdge();
    MHalfedge* he1 = newHalfEdge();

    edge->setHalfedge(he0);
    //edge->set_index(edges_.size()); edges_.push_back(edge);

    he0->setNext(he1);
    he0->setPrev(he1);
    he0->setTwin(he1);
    he0->setVertex(v_begin);
    he0->setEdge(edge);
    he0->setFace(nullptr);

    he1->setNext(he0);
    he1->setPrev(he0);
    he1->setTwin(he0);
    he1->setVertex(v_end);
    he1->setEdge(edge);
    he1->setFace(nullptr);

    return edge;
}

MEdge* PolyMesh::newEdge(MVertex* v1, MVertex* v2) {
    MEdge* w_ptr = new MEdge(v1, v2);
    w_ptr->set_index(edges_.size());
    edges_.push_back(w_ptr);
    return w_ptr;
}

MHalfedge* PolyMesh::newHalfEdge() {
    MHalfedge* w_ptr = new MHalfedge();
    w_ptr->set_index(half_edges_.size());
    half_edges_.push_back(w_ptr);
    return w_ptr;
}

MFace* PolyMesh::newFace() {
    MFace* w_ptr = new MFace();
    w_ptr->set_index(faces_.size());
    faces_.push_back(w_ptr);
    return w_ptr;
}


std::vector<MFace*> PolyMesh::vertexAdjacentFace(MVertex* vert) const {
    std::vector<MFace*> f_list;
    if (vert == nullptr || vert->halfEdge() == nullptr)return f_list;

    auto he_begin = vert->halfEdge();
    auto he = he_begin;

    do {
        if (he->face() != nullptr)f_list.push_back(he->face());
        he = he->rotateNext();
    } while (he != he_begin);

    return f_list;
}

std::vector<MHalfedge*> PolyMesh::vertexAdjacentHalfEdge(MVertex* vert) const { // 出发的边
    std::vector<MHalfedge*> v_adj;
    if (vert == nullptr || vert->halfEdge() == nullptr)return v_adj;

    auto he_begin = vert->halfEdge();
    auto he = he_begin;

    do {
        v_adj.push_back(he);
        he = he->rotateNext();
    } while (he != he_begin);

    return v_adj;
}

std::vector<MHalfedge*> PolyMesh::vertexAdjacentReverseHalfEdge(MVertex* vert) const { // 进入的边
    std::vector<MHalfedge*> v_adj;
    if (vert == nullptr || vert->halfEdge() == nullptr)return v_adj;

    auto he_begin = vert->halfEdge();
    auto he = he_begin;

    do {
        v_adj.push_back(he->twin());
        he = he->rotateNext();
    } while (he != he_begin);

    return v_adj;
}

std::vector<MVertex*> PolyMesh::vertexAdjacentVertices(MVertex* vert) const {
    std::vector<MVertex*> v_adj;
    if (vert == nullptr || vert->halfEdge() == nullptr)return v_adj;

    auto he_begin = vert->halfEdge();
    auto he = he_begin;

    do {
        v_adj.push_back(he->toVertex());
        he = he->rotateNext();
    } while (he != he_begin);

    return v_adj;

}

std::vector<MFace*> PolyMesh::edgeAdjacentFace(MEdge* edge) const {
    std::vector<MFace*> e_poly; e_poly.clear();

    MFace* poly = edge->halfEdge()->face();
    if (poly != nullptr) e_poly.push_back(poly);

    poly = edge->halfEdge()->twin()->face();
    if (poly != nullptr) e_poly.push_back(poly);

    return e_poly;
}

MFace* PolyMesh::edgeAdjacentAnotherFace(MEdge* edge, MFace* face) const {
    MFace* p1, * p2;

    p1 = edge->halfEdge()->face();
    p2 = edge->halfEdge()->twin()->face();
    if (p1 == face) {
        return p2;
    }
    return p1;
}

bool PolyMesh::isBoundary(MVertex* vert) const {
    if (vert->halfEdge() == nullptr)
    {
        return true;
    }

    MHalfedge* he_begin = vert->halfEdge();
    MHalfedge* he = vert->halfEdge();
    do {
        if (he->isBoundary())
        {
            return true;
        }
        he = he->rotateNext();

    } while (he != he_begin);

    return false;
}

bool PolyMesh::isBoundary(const MEdge* edge) const {
    return (edge->halfEdge()->isBoundary() || edge->halfEdge()->twin()->isBoundary());
}

bool PolyMesh::isBoundary(const MHalfedge* halfedge) const {
    return halfedge->isBoundary();
}