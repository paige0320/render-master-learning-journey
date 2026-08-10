// ============================================================
//  Shader —— 包掉「編譯 + 連結 + 設定 uniform」的重複程式
//  用法：Shader s(vsSrc, fsSrc);  s.use();  s.set("uModel", mat);
// ============================================================
#pragma once
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <iostream>

class Shader {
public:
    unsigned int id = 0;

    Shader(const char* vsSrc, const char* fsSrc) {
        unsigned int vs = compile(GL_VERTEX_SHADER,   vsSrc, "vertex");
        unsigned int fs = compile(GL_FRAGMENT_SHADER, fsSrc, "fragment");
        id = glCreateProgram();
        glAttachShader(id, vs); glAttachShader(id, fs); glLinkProgram(id);
        int ok; glGetProgramiv(id, GL_LINK_STATUS, &ok);
        if (!ok) { char log[512]; glGetProgramInfoLog(id, 512, nullptr, log); std::cerr << "program 連結失敗:\n" << log << "\n"; }
        glDeleteShader(vs); glDeleteShader(fs);
    }

    void use() const { glUseProgram(id); }

    // C++ 的「函式多載」：同一個名字 set()，依參數型別自動選對的版本
    void set(const char* name, const glm::mat4& m) const { glUniformMatrix4fv(loc(name), 1, GL_FALSE, glm::value_ptr(m)); }
    void set(const char* name, const glm::vec3& v) const { glUniform3fv(loc(name), 1, glm::value_ptr(v)); }
    void set(const char* name, float f)            const { glUniform1f(loc(name), f); }
    void set(const char* name, int i)              const { glUniform1i(loc(name), i); }

private:
    int loc(const char* name) const { return glGetUniformLocation(id, name); }

    unsigned int compile(GLenum type, const char* src, const char* label) {
        unsigned int s = glCreateShader(type);
        glShaderSource(s, 1, &src, nullptr); glCompileShader(s);
        int ok; glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
        if (!ok) { char log[512]; glGetShaderInfoLog(s, 512, nullptr, log); std::cerr << label << " shader 編譯失敗:\n" << log << "\n"; }
        return s;
    }
};
