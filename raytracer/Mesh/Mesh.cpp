#include "Mesh.h"

#include "MeshHalfEdge.h"
#include "MeshVertex.h"
#include "MeshTriangle.h"
#include "MeshEdge.h"

Mesh::Mesh(Material mat)
{
    m_mat = mat;
}

Mesh::~Mesh()
{
}

void Mesh::addPolygon(vector<MeshVertex *> vertex, vec3 normal)
{
    MeshVertex *pivot = vertex[0]; //center of the triangle fan
    int count = vertex.size();

    for (int i = 0; i < count - 2; i++) //faces
    {
        this->addTriangle(pivot, vertex[i + 1], vertex[i + 2], normal);
    }

    for (int i = 0; i < count - 1; i++) //edges
    {
        new MeshEdge(this, vertex[i], vertex[i + 1]);
    }
}

MeshTriangle *Mesh::addTriangle(MeshVertex *v1, MeshVertex *v2, MeshVertex *v3, vec3 normal)
{
    MeshTriangle *triangle = new MeshTriangle(this, v1, v2, v3, normal);
    return triangle;
}

void Mesh::pushTriangle(MeshTriangle *triangle)
{
    m_triangleList.push_back(triangle);
}

void Mesh::popTriangle(MeshTriangle *triangle)
{
    int i = 0;

    while (m_triangleList[i] != triangle)
    {
        i++;
    }

    m_triangleList.erase(m_triangleList.begin() + i);
}

void Mesh::pushHalfEdge(MeshHalfEdge *halfEdge)
{
    m_halfEdgeList.push_back(halfEdge);
}

void Mesh::popHalfEdge(MeshHalfEdge *halfEdge)
{
    int i = 0;

    while (m_halfEdgeList[i] != halfEdge)
    {
        i++;
    }

    m_halfEdgeList.erase(m_halfEdgeList.begin() + i);
}

void Mesh::pushEdge(MeshEdge *edge)
{
    bool exist = false;

    for (MeshEdge *line : m_edgeList)
    {
        if ((line->getVertex(0) == edge->getVertex(0) && line->getVertex(1) == edge->getVertex(1)) || (line->getVertex(1) == edge->getVertex(0) && line->getVertex(0) == edge->getVertex(1)))
        {
            exist = true;
        }
    }

    if (!exist)
    {
        m_edgeList.push_back(edge);
    }
}

void Mesh::popEdge(MeshEdge *edge)
{
    int i = 0;

    while (m_edgeList[i] != edge)
    {
        i++;
    }

    m_edgeList.erase(m_edgeList.begin() + i);
}

MeshVertex *Mesh::addVertex(string name)
{
    for (MeshVertex *vertex : m_vertexList)
    {
        if (vertex->getName() == name)
        {
            return vertex;
        }
    }

    MeshVertex *vertex = new MeshVertex(this, name);
    return vertex;
}

void Mesh::pushVertex(MeshVertex *vertex)
{
    m_vertexList.push_back(vertex);
}

void Mesh::popVertex(MeshVertex *vertex)
{
    int i = 0;

    while (m_vertexList[i] != vertex)
    {
        i++;
    }

    m_vertexList.erase(m_vertexList.begin() + i);
}

vector<MeshTriangle *> Mesh::getTriangles()
{
    return m_triangleList;
}

Material *Mesh::getMaterial()
{
    return &m_mat;
}