#ifndef MESH
#define MESH

#include "../scene.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <string>

class MeshHalfEdge;
class MeshVertex;
class MeshTriangle;
class MeshEdge;

using namespace std;
using namespace glm;

class Mesh
{
public:
    Mesh(Material mat);
    ~Mesh();

    void addPolygon(vector<MeshVertex *> vertex, vec3 normal);

    MeshTriangle *addTriangle(MeshVertex *v1, MeshVertex *v2, MeshVertex *v3, vec3 normal);
    void pushTriangle(MeshTriangle *triangle);
    void popTriangle(MeshTriangle *triangle);

    void pushHalfEdge(MeshHalfEdge *halfEdge);
    void popHalfEdge(MeshHalfEdge *halfEdge);

    void pushEdge(MeshEdge *edge);
    void popEdge(MeshEdge *edge);

    MeshVertex *addVertex(string name);
    void pushVertex(MeshVertex *vertex);
    void popVertex(MeshVertex *vertex);

    vector<MeshTriangle *> getTriangles();

    Material *getMaterial();

private:
    vector<MeshTriangle *> m_triangleList;
    vector<MeshHalfEdge *> m_halfEdgeList;
    vector<MeshVertex *> m_vertexList;
    vector<MeshEdge *> m_edgeList;

    Material m_mat;
};
#endif