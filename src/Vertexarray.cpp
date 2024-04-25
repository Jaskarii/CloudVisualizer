#include "Vertexarray.h"
#include "VertexBufferLayout.h"

Vertexarray::Vertexarray()
{
	glGenVertexArrays(1, &m_RendererID);
}

Vertexarray::~Vertexarray()
{
	glDeleteVertexArrays(1, &m_RendererID);
}

void Vertexarray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
{
	Bind();
	vb.Bind();
	const auto& elements = layout.GetElements();
	unsigned int offset = 0;

	for (unsigned int i = 0; i < elements.size(); i++)
	{
		const auto& element = elements[i];
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.GetStride(), (const void*)offset);
		offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
	}
	vb.UnBind();
	UnBind();
}

void Vertexarray::Bind() const
{
	glBindVertexArray(m_RendererID);
}

void Vertexarray::UnBind() const
{
	glBindVertexArray(0);
}
