
/*
    Copyright 2011 Aevum Software aevum @ aevumlab.com

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

    @author Victor Vicente de Carvalho victor.carvalho@aevumlab.com
    @author Ozires Bortolon de Faria ozires@aevumlab.com
*/

#include "VertexBufferObject.hpp"

#include "gdx-cpp/Gdx.hpp"
#include "gdx-cpp/graphics/GL20.hpp"
#include "gdx-cpp/graphics/GL11.hpp"
#include "gdx-cpp/graphics/GL10.hpp"
#include "gdx-cpp/graphics/glutils/ShaderProgram.hpp"

using namespace gdx_cpp::graphics::glutils;
using namespace gdx_cpp::graphics;
using namespace gdx_cpp;

int VertexBufferObject::createBufferObject () {
    if (Gdx::gl20 != NULL)
        Gdx::gl20->glGenBuffers(1, &tmpHandle);
    else
        Gdx::gl11->glGenBuffers(1, &tmpHandle);

    return tmpHandle;
}

int VertexBufferObject::getKind()
{
    return VertexData::Kind::VertexBufferObject;
}

gdx_cpp::graphics::VertexAttributes& VertexBufferObject::getAttributes () {
    return attributes;
}

int VertexBufferObject::getNumVertices () {
    return buffer.limit() * 4 / attributes.vertexSize;
}

int VertexBufferObject::getNumMaxVertices () {
    return byteBuffer.capacity() / attributes.vertexSize;
}

utils::float_buffer& VertexBufferObject::getBuffer () {
    isDirty = true;
    return buffer;
}

void VertexBufferObject::setVertices (const float* vertices, int offset, int count) {
    isDirty = true;
    if (isDirect) {
        byteBuffer.copy(vertices, count, offset);
        buffer.position(0);
        buffer.limit(count);
    } else {
        buffer.clear();
        buffer.put(vertices, count, offset, count);
        buffer.flip();
        byteBuffer.position(0);
        byteBuffer.limit(buffer.limit() << 2);
    }

    if (isBound) {
        if (Gdx::gl20 != NULL) {
            GL20& gl = *Gdx::gl20;
            gl.glBufferData(GL20::GL_ARRAY_BUFFER, byteBuffer.limit(), byteBuffer, usage);
        } else {
            GL11& gl = *Gdx::gl11;
            gl.glBufferData(GL11::GL_ARRAY_BUFFER, byteBuffer.limit(), byteBuffer, usage);
        }
        isDirty = false;
    }
}

void VertexBufferObject::bind () {
    GL11& gl = *Gdx::gl11;

    gl.glBindBuffer(GL11::GL_ARRAY_BUFFER, bufferHandle);
    if (isDirty) {
        byteBuffer.limit(buffer.limit() * 4);
        gl.glBufferData(GL11::GL_ARRAY_BUFFER, byteBuffer.limit(), byteBuffer, usage);
        isDirty = false;
    }

    int textureUnit = 0;
    int numAttributes = attributes.size();

    for (int i = 0; i < numAttributes; i++) {
        VertexAttribute& attribute = attributes.get(i);

        switch (attribute.usage) {
        case VertexAttributes::Usage::Position:
            gl.glEnableClientState(GL11::GL_VERTEX_ARRAY);
            gl.glVertexPointer(attribute.numComponents, GL10::GL_FLOAT, attributes.vertexSize, &attribute.offset);
            break;

        case VertexAttributes::Usage::Color:
        case VertexAttributes::Usage::ColorPacked:
        {
            int colorType = GL10::GL_FLOAT;
            if (attribute.usage == VertexAttributes::Usage::ColorPacked) colorType = GL11::GL_UNSIGNED_BYTE;

            gl.glEnableClientState(GL10::GL_COLOR_ARRAY);
            gl.glColorPointer(attribute.numComponents, colorType, attributes.vertexSize, &attribute.offset);
        }
            break;

        case VertexAttributes::Usage::Normal:
            gl.glEnableClientState(GL10::GL_NORMAL_ARRAY);
            gl.glNormalPointer(GL10::GL_FLOAT, attributes.vertexSize, &attribute.offset);
            break;

        case VertexAttributes::Usage::TextureCoordinates:
            gl.glClientActiveTexture(GL10::GL_TEXTURE0 + textureUnit);
            gl.glEnableClientState(GL10::GL_TEXTURE_COORD_ARRAY);
            gl.glTexCoordPointer(attribute.numComponents, GL10::GL_FLOAT, attributes.vertexSize, &attribute.offset);
            textureUnit++;
            break;

        default:
            break;
            // throw new GdxRuntimeException("unkown vertex attribute type: " + attribute.usage);
        }
    }

    isBound = true;
}

void VertexBufferObject::bind (ShaderProgram& shader) {
    GL20& gl = *Gdx::gl20;

    gl.glBindBuffer(GL20::GL_ARRAY_BUFFER, bufferHandle);
    if (isDirty) {
        byteBuffer.limit(buffer.limit() * 4);
        gl.glBufferData(GL20::GL_ARRAY_BUFFER, byteBuffer.limit(), byteBuffer, usage);
        isDirty = false;
    }

    int numAttributes = attributes.size();
    for (int i = 0; i < numAttributes; i++) {
        VertexAttribute& attribute = attributes.get(i);
        shader.enableVertexAttribute(attribute.alias);
        int colorType = GL20::GL_FLOAT;
        bool normalize = false;
        if (attribute.usage == VertexAttributes::Usage::ColorPacked) {
            colorType = GL20::GL_UNSIGNED_BYTE;
            normalize = true;
        }
        shader.setVertexAttribute(attribute.alias, attribute.numComponents, colorType, normalize, attributes.vertexSize,
                                  attribute.offset);
    }
    isBound = true;
}

void VertexBufferObject::unbind () {
    GL11& gl = *Gdx::gl11;
    int textureUnit = 0;
    int numAttributes = attributes.size();

    for (int i = 0; i < numAttributes; i++) {

        VertexAttribute attribute = attributes.get(i);
        switch (attribute.usage) {
        case VertexAttributes::Usage::Position:
            break; // no-op, we also need a position bound in gles
        case VertexAttributes::Usage::Color:
        case VertexAttributes::Usage::ColorPacked:
            gl.glDisableClientState(GL11::GL_COLOR_ARRAY);
            break;
        case VertexAttributes::Usage::Normal:
            gl.glDisableClientState(GL11::GL_NORMAL_ARRAY);
            break;
        case VertexAttributes::Usage::TextureCoordinates:
            gl.glClientActiveTexture(GL11::GL_TEXTURE0 + textureUnit);
            gl.glDisableClientState(GL11::GL_TEXTURE_COORD_ARRAY);
            textureUnit++;
            break;
        default:
            break;
        }
    }

    gl.glBindBuffer(GL11::GL_ARRAY_BUFFER, 0);
    isBound = false;
}

void VertexBufferObject::unbind (ShaderProgram& shader) {
    GL20& gl = *Gdx::gl20;
    int numAttributes = attributes.size();
    for (int i = 0; i < numAttributes; i++) {
        VertexAttribute attribute = attributes.get(i);
        shader.disableVertexAttribute(attribute.alias);
    }
    gl.glBindBuffer(GL20::GL_ARRAY_BUFFER, 0);
    isBound = false;
}

void VertexBufferObject::invalidate () {
    bufferHandle = createBufferObject();
    isDirty = true;
}

void VertexBufferObject::dispose () {
    if (Gdx::gl20 != NULL) {
        tmpHandle = bufferHandle;
        GL20& gl = *Gdx::gl20;
        gl.glBindBuffer(GL20::GL_ARRAY_BUFFER, 0);
        gl.glDeleteBuffers(1, &tmpHandle);
        bufferHandle = 0;
    } else {
        tmpHandle = bufferHandle;
        GL11& gl = *Gdx::gl11;
        gl.glBindBuffer(GL11::GL_ARRAY_BUFFER, 0);
        gl.glDeleteBuffers(1, &tmpHandle);
        bufferHandle = 0;
    }

}

VertexBufferObject::VertexBufferObject(bool isStatic, int numVertices, const gdx_cpp::graphics::VertexAttributes& attributes)
:
  bufferHandle(0)
, tmpHandle(0)
, isDirect(true)
, usage(0)
, isDirty(false)
, attributes(attributes)
, isBound(false)
, isStatic(isStatic)
, byteBuffer(attributes.vertexSize * numVertices)
{
//TODO:     byteBuffer.order(ByteOrder.nativeOrder());
    byteBuffer.flip();
    buffer = byteBuffer.convert<float>();
    buffer.flip();
    
    bufferHandle = createBufferObject();
    usage = isStatic ? GL11::GL_STATIC_DRAW : GL11::GL_DYNAMIC_DRAW;
}


VertexBufferObject::VertexBufferObject(bool isStatic, int numVertices, const std::vector< VertexAttribute >& attributes)
:
bufferHandle(0)
, tmpHandle(0)
, isDirect(true)
, usage(0)
, isDirty(false)
, attributes(attributes)
, isBound(false)
, isStatic(isStatic)
, byteBuffer(this->attributes.vertexSize * numVertices)
{
    buffer = byteBuffer.convert<float>();
    
    byteBuffer.flip();
    buffer.flip();
    
    bufferHandle = createBufferObject();
    usage = isStatic ? GL11::GL_STATIC_DRAW : GL11::GL_DYNAMIC_DRAW;
}



