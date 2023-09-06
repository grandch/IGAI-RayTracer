#ifndef MESHHALFEDGE
#define MESHHALFEDGE

#include <iostream>

#define GLM_ENABLE_EXPERIMENTAL

class MeshVertex;
class MeshEdge;
class MeshTriangle;
class Mesh;

using namespace std;

class MeshHalfEdge
{
    public:

        MeshHalfEdge(Mesh* mesh, MeshVertex* origin, MeshVertex* target, MeshTriangle* triangle);
        ~MeshHalfEdge();

        MeshVertex* getOrigin();
        MeshVertex* getTarget();

        MeshTriangle* getTriangle();

        MeshHalfEdge* getNext();
        void setNext(MeshHalfEdge* halfedge);
        
        MeshHalfEdge* getOpposite();

        MeshHalfEdge* getSibling();
        void setSibling(MeshHalfEdge* halfEdge);

    private:

        Mesh* m_mesh;

        MeshVertex* m_origin;
        MeshTriangle* m_triangle;
        MeshEdge* m_edge;

        MeshHalfEdge* m_next;
        MeshHalfEdge* m_opposite;
        MeshHalfEdge* m_sibling;
};
#endif