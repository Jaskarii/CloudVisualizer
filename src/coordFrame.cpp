#include "coordFrame.h"
#include "VertexBufferLayout.h"

CoordFrame::CoordFrame() : _shader(Shader("./shaders/coord.glsl"))
{
    ColoredPoint3D points[6];
    points[0] = ColoredPoint3D(0,0,0,1,0,0);
    points[1] = ColoredPoint3D(gridSize,0,0,1,0,0);
    points[2] = ColoredPoint3D(0,0,0,0,1,0);
    points[3] = ColoredPoint3D(0,gridSize,0,0,1,0);
    points[4] = ColoredPoint3D(0,0,0,0,0,1);
    points[5] = ColoredPoint3D(0,0,gridSize,0,0,1);

    _vbo = new VertexBuffer(points, 6 * sizeof(ColoredPoint3D));
    VertexBufferLayout layout;
    layout.PushFloat(3);
    layout.PushFloat(3);
    _vao = new Vertexarray();
    _vao->AddBuffer(*_vbo, layout);
}

CoordFrame::~CoordFrame()
{
}

void CoordFrame::Render(glm::mat4 mvp)
{
    _shader.Bind();
    _shader.SetUniformMat4f("MVP", mvp);
    _vao->Bind();
    glDrawArrays(GL_LINES, 0, 6);
}
