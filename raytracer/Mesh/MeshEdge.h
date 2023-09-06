#ifndef MESHEDGE
#define MESHEDGE

#define GLM_ENABLE_EXPERIMENTAL

class MeshVertex;
class Mesh;

class MeshEdge
{
    public:

    MeshEdge(Mesh* mesh, MeshVertex* v1, MeshVertex* v2);
    ~MeshEdge();

    MeshVertex* getVertex(int vertex);

    private:

        Mesh* m_mesh;

        MeshVertex* m_vertex1;
        MeshVertex* m_vertex2;
};
#endif