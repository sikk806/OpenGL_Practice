#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>

#include <GL/glew.h>
#include <GL/glut.h>

using namespace std;

GLuint Buffers[1];
GLfloat vertices[2][3] = {
    {0.0, 0.0, 0.0},
    {0.0, 0.0, 0.0}
};
GLuint pgID;
GLint vtxPosition;
GLuint VertexArrayID;
int flag = 0;


void myMouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        float a = (x - 250) * 0.004;
        float b = (y - 250) * 0.004;
        // glutInitWindowSize = 500, 500 이므로 1/500 = 0.002이다.
        // 따라서 0.004를 곱해주어야 한다. (좌표를 중앙으로 옮기면 반만 쓰기 때문.)
        // y축은 Windows랑 openGL과 반대. 따라서 - 를 붙임.1
        if (flag == 1) {
            vertices[0][0] = a;
            vertices[0][1] = -b;
        }
        else if (flag == 0) {
            vertices[1][0] = a;
            vertices[1][1] = -b;
        }
    }
}

GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path)
{
    //create the shaders 
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    GLint Result = GL_FALSE;
    int InfoLogLength;
    //Read the vertex shader code from the file 
    string VertexShaderCode;
    ifstream VertexShaderStream(vertex_file_path, ios::in);
    if (VertexShaderStream.is_open())
    {
        string Line = "";
        while (getline(VertexShaderStream, Line))
            VertexShaderCode += "\n" + Line;
        VertexShaderStream.close();
    }
    //Compile Vertex Shader 
    printf("Compiling shader : %s\n", vertex_file_path);

    char const* VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
    glCompileShader(VertexShaderID);
    //Check Vertex Shader 
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength != 0) {
        vector<char> VertexShaderErrorMessage(InfoLogLength);
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
        fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);
    }
    //Read the fragment shader code from the file 
    string FragmentShaderCode;
    ifstream FragmentShaderStream(fragment_file_path, ios::in);
    if (FragmentShaderStream.is_open())
    {
        string Line = "";
        while (getline(FragmentShaderStream, Line))
            FragmentShaderCode += "\n" + Line;
        FragmentShaderStream.close();
    }
    //Compile Fragment Shader 
    printf("Compiling shader : %s\n", fragment_file_path);
    char const* FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
    glCompileShader(FragmentShaderID);
    //Check Fragment Shader 
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength != 0) {
        vector<char> FragmentShaderErrorMessage(InfoLogLength);
        glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
        fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);
    }
    //Link the program 
    fprintf(stdout, "Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    // Check the program 
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength != 0) {
        vector<char> ProgramErrorMessage(max(InfoLogLength, int(1)));
        glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);
    }

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    return ProgramID;
}

void init()
{
    GLenum res = glewInit();
    if (res != GLEW_OK)
    {
        fprintf(stderr, "Error: '%s' \n", glewGetErrorString(res));
    }
    //select the background color
    glClearColor(1.0, 1.0, 1.0, 1.0);

    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

    // 점을 찍고 선을 만들고 다시 찍고 ... = DYNAMIC
    glGenBuffers(1, Buffers);
    glBindBuffer(GL_ARRAY_BUFFER, Buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*6, vertices, GL_DYNAMIC_DRAW);
}

void renderScene(void)
{
    //Clear all pixels
    glClear(GL_COLOR_BUFFER_BIT);
    //Let's draw something here
    if (flag == 0) {
        // 점을 배열로..
        glBindBuffer(GL_ARRAY_BUFFER, Buffers[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6, vertices, GL_STATIC_DRAW);

        GLuint vtxPosition = glGetAttribLocation(pgID, "vtxPosition");
        glVertexAttribPointer(vtxPosition, 3, GL_FLOAT, GL_FALSE, 0, ((void*)(0)));
        glEnableVertexAttribArray(vtxPosition);
        glDrawArrays(GL_LINES, 0, 2);
        flag = 1;

        // 점을 다시 찍어야 하므로 끊어버림
        glDisableVertexAttribArray(vtxPosition);
    }
    else {
        GLint vtxPosition = glGetAttribLocation(pgID, "vtxPosition");
        glVertexAttrib3f(vtxPosition, vertices[0][0], vertices[0][1], 0.0);
        glDrawArrays(GL_POINTS, 0, 1);
        flag = 0;
    }


    //Double buffer
    glutSwapBuffers();

    //If using SingleBuffer
    //glFlush();
}

int main(int argc, char** argv)
{
    //init GLUT and create Window
    //initialize the GLUT
    glutInit(&argc, argv);
    //GLUT_DOUBLE enables double buffering (drawing to a background buffer while the other buffer is displayed)
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    //These two functions are used to define the position and size of the window.
    glutInitWindowPosition(200, 200);
    glutInitWindowSize(500, 500);
    //This is used to define the name of the window.
    glutCreateWindow("Simple OpenGL Window");

    //call initization function
    init();

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    pgID = LoadShaders("VertexShader.txt", "FragmentShader.txt");
    glUseProgram(pgID);

    //register callbacks
    glutDisplayFunc(renderScene);
    glutMouseFunc(myMouse);  
    //enter GLUT event processing cycle
    glutMainLoop();

    glDeleteVertexArrays(1, &VertexArrayID);

    return 1;
}