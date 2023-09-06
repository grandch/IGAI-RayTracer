#include "MeshVertex.h"

#include "MeshHalfEdge.h"
#include "MeshTriangle.h"
#include "Mesh.h"

MeshVertex::MeshVertex(Mesh* mesh, string name): m_name(name), m_mesh(mesh), m_halfEdge(nullptr) //p150
{
    if(mesh != nullptr)
    {
        mesh->pushVertex(this);
    }
            
    m_attributes.push_back(vec4(0.0)); //coords
    m_attributes.push_back(vec4(0.0)); //color
    m_attributes.push_back(vec4(0.0)); //normal
}

MeshVertex::~MeshVertex()
{
    if(m_mesh != nullptr)
    {
        m_mesh->popVertex(this);
        m_mesh = nullptr;
    }

    //remove triangles around the vertex
    MeshHalfEdge* halfEdge = m_halfEdge;
    while(halfEdge != nullptr)
    {
        MeshHalfEdge* next = halfEdge->getSibling();
        delete halfEdge->getTriangle();
        halfEdge = next;
    }
}


void MeshVertex::processTrianglesAround1()
{
    MeshHalfEdge* halfEdge = m_halfEdge; //starting half edge

    while(halfEdge != nullptr)
    {
        //MeshTriangle* triangle = halfEdge->getTriangle();

        //do something with the triangle

        halfEdge = halfEdge->getSibling();

        if(halfEdge->getOpposite() != nullptr)
        {
            halfEdge = halfEdge->getOpposite()->getNext();
        }
        else
        {
            halfEdge = nullptr;
        }
    }
}

void MeshVertex::processTrianglesAround2()
{
    MeshHalfEdge* halfEdge = m_halfEdge; //starting half edge

    while(halfEdge != nullptr)
    {
        //MeshTriangle* triangle = halfEdge->getTriangle();

        //do something with the triangle

        halfEdge = halfEdge->getSibling();
    }
}

int MeshVertex::getNumber()
{
    return m_number;
}

void MeshVertex::setNumber(int number)
{
    m_number = number;
}

string MeshVertex::getName()
{
    return m_name;
}

vec4 MeshVertex::getAttribute(int index)
{
    return m_attributes[index];
}

MeshVertex* MeshVertex::setCoord(vec3 coord) //p151
{
    m_attributes[0] = vec4(coord.x, coord.y, coord.z, 1.0f);
    return this;
}

MeshVertex* MeshVertex::setColor(vec3 color)
{
    m_attributes[1] = vec4(color.x, color.y, color.z, 1.0f);
    return this;
}

MeshVertex* MeshVertex::setNormal(vec3 normal)
{
    m_attributes[2] = vec4(normal.x, normal.y, normal.z, 1.0f);
    return this;
}

MeshHalfEdge* MeshVertex::getHalfEdgeTo(MeshVertex* vertex)
{
    MeshHalfEdge* halfEdge = m_halfEdge;

    while(halfEdge != nullptr)
    {
        if(halfEdge->getTarget() == vertex)
        {
            return halfEdge;
        }

        halfEdge = halfEdge->getSibling();
    }
    return nullptr;
}

void MeshVertex::linkSibling(MeshHalfEdge* halfEdge)
{
    halfEdge->setSibling(m_halfEdge);
    m_halfEdge = halfEdge;
}

void MeshVertex::unlinkSibling(MeshHalfEdge* halfEdge)
{
    if(m_halfEdge == halfEdge)
    {
        m_halfEdge = m_halfEdge->getSibling();
    }
    else
    {
        MeshHalfEdge* other = m_halfEdge;
        while(other != nullptr)
        {
            if(other->getSibling() == halfEdge)
            {
                other->setSibling(halfEdge->getSibling());
                break;
            }
            other = other->getSibling();
        }
    }
}