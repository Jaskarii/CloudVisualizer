#include "Vertexarray.h"
#include "Shader.h"

class CoordFrame
{
    const float gridSize = 4.0f;
    struct ColoredPoint3D
    {
        float x, y, z, r,g,b;

        ColoredPoint3D() : x(0), y(0), z(0), r(0),g(0),b(0) {}
        ColoredPoint3D(float x_, float y_, float z_,float r_, float g_, float b_) : x(x_), y(y_), z(z_),r(r_),g(g_),b(b_) {}
    };

    public:
    CoordFrame();
    ~CoordFrame();
    void Render(glm::mat4 mvp);

    private:
    Vertexarray* _vao;
    VertexBuffer* _vbo;
    Shader _shader;
};