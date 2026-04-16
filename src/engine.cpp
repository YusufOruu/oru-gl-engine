#include "engine.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <fstream>
#include <cmath>

int WIDTH = 512;
int HEIGHT = 512;

GLFWwindow* window;

// basit PPM loader
std::vector<unsigned char> loadPPM(const char* filename, int& w, int& h) {
    std::ifstream file(filename);
    std::string format;
    file >> format >> w >> h;
    int max;
    file >> max;

    std::vector<unsigned char> data(w * h * 3);
    for (int i = 0; i < w * h * 3; i++) {
        int val;
        file >> val;
        data[i] = (unsigned char)val;
    }
    return data;
}

void savePNG(const char* filename, std::vector<unsigned char>& buffer) {
    std::ofstream file(filename, std::ios::binary);
    file << "P6\n" << WIDTH << " " << HEIGHT << "\n255\n";
    file.write((char*)buffer.data(), WIDTH * HEIGHT * 3);
}

const char* vsSrc = R"(
#version 120
attribute vec2 aPos;
attribute vec2 aTex;
varying vec2 vTex;

void main() {
    vTex = aTex;
    gl_Position = vec4(aPos, 0.0, 1.0);
}
)";

const char* fsSrc = R"(
#version 120
varying vec2 vTex;
uniform sampler2D tex;

void main() {
    gl_FragColor = texture2D(tex, vTex);
}
)";

void oru::init() {
    glfwInit();
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    window = glfwCreateWindow(WIDTH, HEIGHT, "", NULL, NULL);
    glfwMakeContextCurrent(window);
    glewInit();
}

void oru::run() {

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vsSrc, NULL);
    glCompileShader(vs);

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fsSrc, NULL);
    glCompileShader(fs);

    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glUseProgram(program);

    float vertices[] = {
        // pos      tex
        -0.5f,-0.5f, 0,0,
         0.5f,-0.5f, 1,0,
         0.0f, 0.5f, 0.5,1
    };

    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLint posLoc = glGetAttribLocation(program, "aPos");
    GLint texLoc = glGetAttribLocation(program, "aTex");

    glEnableVertexAttribArray(posLoc);
    glVertexAttribPointer(posLoc, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);

    glEnableVertexAttribArray(texLoc);
    glVertexAttribPointer(texLoc, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));

    int tw, th;
    auto data = loadPPM("src/texture.ppm", tw, th);

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tw, th, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glViewport(0,0,WIDTH,HEIGHT);
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);

    glDrawArrays(GL_TRIANGLES, 0, 3);
    glFlush();

    std::vector<unsigned char> buffer(WIDTH*HEIGHT*3);
    glReadPixels(0,0,WIDTH,HEIGHT,GL_RGB,GL_UNSIGNED_BYTE,buffer.data());

    for(int y=0;y<HEIGHT/2;y++){
        for(int x=0;x<WIDTH*3;x++){
            std::swap(
                buffer[y*WIDTH*3+x],
                buffer[(HEIGHT-1-y)*WIDTH*3+x]
            );
        }
    }

    savePNG("output.png", buffer);
}