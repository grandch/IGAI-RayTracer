#ifndef MESHTRIANGLE
#define MESHTRIANGLE

#include "../scene.h"

class MeshHalfEdge;
class Mesh;
class MeshVertex;

class MeshTriangle
{
public:
    MeshTriangle(Mesh *mesh, MeshVertex *v1, MeshVertex *v2, MeshVertex *v3, vec3 normal);
    ~MeshTriangle();

    MeshHalfEdge *getHalfEdge();

    MeshVertex *getVertex0();
    MeshVertex *getVertex1();
    MeshVertex *getVertex2();

    vec3 getU();
    vec3 getV();

    vec3 getA();

    Material *getMaterial();

    vec3 getNormal();

private:
    MeshHalfEdge *m_halfEdge;

    vec3 m_normal;

    Mesh *m_mesh;
};
#endif