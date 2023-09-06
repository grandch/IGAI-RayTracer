#ifndef MESHVERTEX
#define MESHVERTEX

#include <string>
#include <vector>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;
using namespace std;

class MeshHalfEdge;
class MeshTriangle;
class Mesh;

class MeshVertex
{
public:
    MeshVertex(Mesh *mesh, string name);
    ~MeshVertex();

    void processTrianglesAround1();
    void processTrianglesAround2();

    int getNumber();
    void setNumber(int number);

    string getName();

    vec4 getAttribute(int index);
    MeshVertex *setCoord(vec3 coord);
    MeshVertex *setColor(vec3 color);
    MeshVertex *setNormal(vec3 normal);

    MeshHalfEdge *getHalfEdgeTo(MeshVertex *vertex);

    void linkSibling(MeshHalfEdge *halfEdge);
    void unlinkSibling(MeshHalfEdge *halfEdge);

private:
    string m_name;

    Mesh *m_mesh;

    vector<vec4> m_attributes; // 0 : coord 1 : color 2 : normal

    MeshHalfEdge *m_halfEdge;

    int m_number;
};
#endif