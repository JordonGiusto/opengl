
#include <cmath>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

using namespace std;


void cursor_position_callback(GLFWwindow* window, double new_xpos, double new_ypos){
    cout << "xpos: " << new_xpos << " ypos: " << new_ypos << endl;
}


static unsigned int CompileShader(unsigned int type, string& source){
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if(result == GL_FALSE){
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        cout << "Failed to compile " <<
        (type == GL_VERTEX_SHADER ? "vertex" : "fragment") <<" shader" << endl;
        cout << source << endl;
        glDeleteShader(id);
        return 0;
    }

    return id;

}

static unsigned int CreateShader(const string& vertFilename, const string& fragFilename){
    
    string vertSource;
    ifstream vertFile(vertFilename);
    if(vertFile.is_open()){
        string line;
        while (vertFile)
        {
            getline(vertFile, line);
            if(!vertFile){
                break;
            }
            vertSource += line + '\n';
        }
    }
    string fragSource;
    ifstream fragFile(fragFilename);

    if(fragFile.is_open()){
        string line;
        while (fragFile)
        {
            getline(fragFile, line);
            if(!fragFile){
                break;
            }
            fragSource += line + '\n';
        }
    }

    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertSource);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragSource);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;

}

glm::mat4 get_MVP(float t, float yaw, float pitch){
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 3, 0));
    view = glm::rotate(view, glm::radians(yaw), glm::vec3(0, 1, 0));

    glm::vec4 view_x = view * glm::vec4(1, 0, 0, 1);

    view = glm::rotate(view, glm::radians(pitch), glm::vec3(1, 0, 0));

    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
    model = glm::scale(model, glm::vec3(10));
    
    view = glm::inverse(view);
    return proj * view * model;
}


int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(600, 400, "Window", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    
    if(glewInit() != GLEW_OK)
        cout << "Error!" << endl;

    cout << glGetString(GL_VERSION) << endl;
    float t = 0.0f;

    float verts[24] = {
        -1.0f, 0.0f,  1.0,  0.7f, 0.0f, 0.0f,
        -1.0f, 0.0f, -1.0,  0.0f, 0.7f, 0.0f,
         1.0f, 0.0f,  1.0,  0.0f, 0.0f, 0.7f,
         1.0f, 0.0f, -1.0,  0.0f, 0.0f, 0.7f,
    };
    unsigned int indicies[] = {
        0, 1, 2,
        1, 2, 3
    };
    

    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), verts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (const void*)(sizeof(float) * 3));
    
    

    unsigned int ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indicies, GL_STATIC_DRAW);

    unsigned int shader = CreateShader("vert.glsl", "frag.glsl");
    glUseProgram(shader);

    int UTimelocation = glGetUniformLocation(shader, "u_time");
    glUniform1f(UTimelocation, t);

    int UOffsets = glGetUniformLocation(shader, "u_offsets");

    glm::vec3 offsets[5][5];
    for(int i = 0; i < 5; i++){
        for(int j = 0; j < 5; j++){
            offsets[i][j] = glm::vec3(i - 2, 0, j - 2) * 3.0f;
        }
    }
    
    glUniform3fv(UOffsets, 25, (float*)offsets);

    
    glm::mat4 mvp = get_MVP(t, 0, 0);

    int UProjLocation = glGetUniformLocation(shader, "u_mvp");
    
    glUniformMatrix4fv(UProjLocation, 1, GL_FALSE, &mvp[0][0]);


    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
     

    double xpos, ypos;
    double new_xpos, new_ypos;

    float yaw = 0, pitch = 0;
    glfwGetCursorPos(window, &xpos, &ypos);
    new_xpos = xpos;
    new_ypos = ypos;
    
    
    glfwSetCursorPosCallback(window, cursor_position_callback);
    
    
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        t+=0.02f;
        
        
        cout << "Time: " << t;

        glm::mat4 mvp = get_MVP(t, yaw, pitch);
        glUniformMatrix4fv(UProjLocation, 1, GL_FALSE, &mvp[0][0]);


        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr, 25);
        

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwWaitEvents();


        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* return_struct = glfwGetVideoMode(monitor);

        int height = return_struct->height;
        glUniform1f(UTimelocation, t);

        //cout << "width: " << return_struct->width << " height: " << return_struct->height << endl;
    }

    glfwTerminate();
    return 0;
}