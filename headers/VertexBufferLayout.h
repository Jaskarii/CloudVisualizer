// VertexBufferLayout.h
#pragma once
#include <vector>
#include "glad/glad.h"

struct VertexBufferElement {
    unsigned int type;
    unsigned int count;
    unsigned char normalized;

    static unsigned int GetSizeOfType(unsigned int type) {
        switch (type) {
        case GL_FLOAT:
        case GL_UNSIGNED_INT:
            return 4;
        case GL_UNSIGNED_BYTE:
        case GL_BOOL:
            return 1; 
        default:
            return 0; // Or handle unsupported types accordingly
    }
    }
};

class VertexBufferLayout {
private:
    std::vector<VertexBufferElement> m_Elements;
    unsigned int m_Stride;

public:
    VertexBufferLayout() : m_Stride(0) {}

    // Overloaded Push functions for different types
    void PushFloat(unsigned int count) {
        m_Elements.push_back(VertexBufferElement{ GL_FLOAT, count, GL_FALSE });
        m_Stride += VertexBufferElement::GetSizeOfType(GL_FLOAT) * count;
    }

    void PushUInt(unsigned int count) {
        m_Elements.push_back(VertexBufferElement{ GL_UNSIGNED_INT, count, GL_FALSE });
        m_Stride += VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT) * count;
    }

    void PushUChar(unsigned int count) {
        m_Elements.push_back(VertexBufferElement{ GL_UNSIGNED_BYTE, count, GL_TRUE });
        m_Stride += VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE) * count;
    }

    void PushBool(unsigned int count) {
    m_Elements.push_back(VertexBufferElement{ GL_BOOL, count, GL_FALSE });
    m_Stride += VertexBufferElement::GetSizeOfType(GL_BOOL) * count;
    }

    inline const std::vector<VertexBufferElement>& GetElements() const {
        return m_Elements;
    }

    inline unsigned int GetStride() const {
        return m_Stride;
    }
};
