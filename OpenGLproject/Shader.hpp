//
//  Shader.hpp
//  Lab3
//
//  Created by CGIS on 05/10/2016.
//  Copyright © 2016 CGIS. All rights reserved.
//

#ifndef Shader_hpp
#define Shader_hpp

#if defined (__APPLE__)
    #include <OpenGL/gl3.h>
#else
    #define GLEW_STATIC
    #include <GLEW/glew.h>
#endif

#include <fstream>
#include <sstream>
#include <iostream>


namespace gps {
    
    class Shader {

    public:
        GLuint shaderProgram;
        void loadShader(std::string vertexShaderFileName, std::string fragmentShaderFileName);
        void useShaderProgram();
    
    private:
        std::string readShaderFile(std::string fileName);
        void shaderCompileLog(GLuint shaderId);
        void shaderLinkLog(GLuint shaderProgramId);
    };
    
}

#endif /* Shader_hpp */
