#pragma once

#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <GL/glew.h>

class Texture
{
public:
    Texture(GLenum TextureTarget, const std::string& FileName);
    Texture();
    // Should be called once to load the texture
    bool Load();
    bool instance;
    // Must be called at least once for the specific texture unit
    void Bind(GLenum TextureUnit);
    int getNumberOfRows() {
        return numberOfRows;
    }
    int setNumberOfRows(int newNumberOfRows) {
        numberOfRows = newNumberOfRows;
    }
private:
    int numberOfRows = 3;
    std::string m_fileName;
    GLenum m_textureTarget;
    GLuint m_textureObj;

};
#endif  /* TEXTURE_H */