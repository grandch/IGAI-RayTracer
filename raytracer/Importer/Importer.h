#ifndef IMPORTER
#define IMPORTER

#include <GLES3/gl3.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../Mesh/MeshTriangle.h"
#include "../Mesh/MeshHalfEdge.h"
#include "../Mesh/MeshEdge.h"
#include "../Mesh/MeshVertex.h"

#include <string>
#include <sstream>

#include <map>
#include <list>

#include <vector>

class Mesh;

using namespace std;

class Importer
{
public:
    Importer(Mesh *mesh);
    ~Importer();

    void loadObjFile(string fileName);

    MeshVertex *findOrCreateVertex(char *nvntnn,
                                   map<int, list<MeshVertex *>> &vertexList,
                                   vector<vec3> &coordList,
                                   vector<vec2> &texCoordList,
                                   vector<vec3> &normalList,
                                   vec3 *faceNormal);

    string findPath();

private:
    Mesh *m_mesh;
};
#endif