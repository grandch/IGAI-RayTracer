#include "Importer.h"
#include <fstream>

#include "../Mesh/Mesh.h"

using namespace std;

Importer::Importer(Mesh *mesh) : m_mesh(mesh)
{
}

Importer::~Importer()
{
}

void Importer::loadObjFile(string fileName)
{
    map<int, list<MeshVertex *>> vertexList;

    vector<vec3> coordList;
    vector<vec2> texCoordList;
    vector<vec3> normalList;

    ifstream inputStream;
    inputStream.open(fileName.c_str(), ifstream::in);

    char line[180];
    while (inputStream.getline(line, sizeof(line)))
    {
        char *word = NULL;
        word = strtok(line, " ");
        if (word == NULL)
            continue;

        if (strcmp(word, "v") == 0)
        {
            //vertex coord
            float x = atof(strtok(NULL, " "));
            float y = atof(strtok(NULL, " "));
            float z = atof(strtok(NULL, " "));

            coordList.push_back(vec3(x, y, z));
        }
        else if (strcmp(word, "vt") == 0)
        {
            //texture coord
            float u = atof(strtok(NULL, " "));
            float v = atof(strtok(NULL, " "));

            texCoordList.push_back(vec2(u, v));
        }
        else if (strcmp(word, "vn") == 0)
        {
            //normal
            float nx = atof(strtok(NULL, " "));
            float ny = atof(strtok(NULL, " "));
            float nz = atof(strtok(NULL, " "));

            normalList.push_back(vec3(nx, ny, nz));
        }
        if (strcmp(word, "f") == 0)
        {
            vec3 faceNormal;
            vector<MeshVertex *> vertex;

            //first vertex index
            word = strtok(NULL, " ");
            MeshVertex *v1 = findOrCreateVertex(word, vertexList, coordList, texCoordList, normalList, &faceNormal);
            vertex.push_back(v1);

            //second vertex index
            word = strtok(NULL, " ");
            MeshVertex *v2 = findOrCreateVertex(word, vertexList, coordList, texCoordList, normalList, &faceNormal);
            vertex.push_back(v2);

            //other vertex index
            while ((word = strtok(NULL, " ")))
            {
                MeshVertex *v3 = findOrCreateVertex(word, vertexList, coordList, texCoordList, normalList, &faceNormal);
                vertex.push_back(v3);
                v2 = v3;
            }
            m_mesh->addPolygon(vertex, faceNormal);
        }
    }
    inputStream.close();
}

MeshVertex *Importer::findOrCreateVertex(char *nvntnn,
                                         map<int, list<MeshVertex *>> &vertexList,
                                         vector<vec3> &coordList,
                                         vector<vec2> &texCoordList,
                                         vector<vec3> &normalList,
                                         vec3 *faceNormal)
{
    //index extracting
    int nv, nt, nn;
    sscanf(nvntnn, "%d/%d/%d", &nv, &nt, &nn);
    nv--;
    nn--;

    nt = 0;

    //vertex name
    stringstream ss;
    ss << "v(" << nv << ", " << nt << ", " << nn << ")";
    string name = ss.str();

    *faceNormal = normalList[nn];

    //search vertex in vertexList
    for (MeshVertex *s : vertexList[nv])
    {
        if (name == s->getName())
        {
            return s;
        }
    }

    //create vertex
    MeshVertex *vertex = m_mesh->addVertex(name);
    vertexList[nv].push_front(vertex);

    //vertex settings
    vertex->setCoord(coordList[nv]);
    //vertex->setTexCoord(texCoordList[nt]); //need to implement texture support before activating this line
    vertex->setNormal(normalList[nn]);
    vertex->setColor(vec3(1.0, 1.0, 1.0)); //white color for an AWESOME render

    return vertex; //to construct polygon
}