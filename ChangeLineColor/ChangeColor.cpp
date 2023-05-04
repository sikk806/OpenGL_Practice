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

GLfloat VBO[2][7] = {
    { 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0 },
    { 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 }
};

GLfloat sizes[2] = { 10.0, 20.0 };
GLuint pgID;
GLint vtxColor;
GLint vtxPosition;
GLuint VertexArrayID;
int flag = 0;
int color_click = 1;
int color_s = 1;
int color_e = 1;

void myMouse(int button, int state, int x, int y) {
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        if (color_click == 1) {
            VBO[0][3] = 1.0;
            VBO[0][4] = 0.0;
            VBO[0][5] = 0.0;
            VBO[1][3] = 1.0;
            VBO[1][4] = 0.0;
            VBO[1][5] = 0.0;
            color_click = 2;
        }
        else if (color_click == 2) {
            VBO[0][3] = 0.0;
            VBO[0][4] = 1.0;
            VBO[0][5] = 0.0;
            VBO[1][3] = 0.0;
            VBO[1][4] = 1.0;
            VBO[1][5] = 0.0;
            color_click = 3;
        }
        else if (color_click == 3) {
            VBO[0][3] = 0.0;
            VBO[0][4] = 0.0;
            VBO[0][5] = 1.0;
            VBO[1][3] = 0.0;
            VBO[1][4] = 0.0;
            VBO[1][5] = 1.0;
            color_click = 1;
        }
    }
}


void myKeyBoard(unsigned char key, int x, int y) {
    switch (key) {
    case 115:
        if (color_s == 1) {
            VBO[0][3] = 1.0;
            VBO[0][4] = 0.0;
            VBO[0][5] = 0.0;
            color_s = 2;
        }
        else if (color_s == 2) {
            VBO[0][3] = 0.0;
            VBO[0][4] = 1.0;
            VBO[0][5] = 0.0;
            color_s = 3;
        }
        else if (color_s == 3) {
            VBO[0][3] = 0.0;
            VBO[0][4] = 0.0;
            VBO[0][5] = 1.0;
            color_s = 1;
        }
        break;
    case 101:
        if (color_e == 1) {
            VBO[1][3] = 1.0;
            VBO[1][4] = 0.0;
            VBO[1][5] = 0.0;
            color_e = 2;
        }
        else if (color_e == 2) {
            VBO[1][3] = 0.0;
            VBO[1][4] = 1.0;
            VBO[1][5] = 0.0;
            color_e = 3;
        }
        else if (color_e == 3) {
            VBO[1][3] = 0.0;
            VBO[1][4] = 0.0;
            VBO[1][5] = 1.0;
            color_e = 1;
        }
        break;
    }
    glutPostRedisplay();
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*14, VBO, GL_STATIC_DRAW);  
}

void renderScene(void)
{
    //Clear all pixels
    glClear(GL_COLOR_BUFFER_BIT);
    //Let's draw something here


    glBindBuffer(GL_ARRAY_BUFFER, Buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 14, VBO, GL_DYNAMIC_DRAW);
    GLuint posAttribLoc = glGetAttribLocation(pgID, "vtxPosition");
    glVertexAttribPointer(posAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*7, (GLvoid*)(0));
    glEnableVertexAttribArray(posAttribLoc);

    GLuint psizeAttribLoc = glGetAttribLocation(pgID, "uPcolor");
    glVertexAttribPointer(psizeAttribLoc, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*7, (GLvoid*)(sizeof(GLfloat)*3));
    glEnableVertexAttribArray(psizeAttribLoc);

    glDrawArrays(GL_LINES, 0, 2);

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
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
    //These two functions are used to define the position and size of the window.
    glutInitWindowPosition(200, 200);
    glutInitWindowSize(500, 500);
    //This is used to define the name of the window.
    glutCreateWindow("Simple OpenGL Window");

    //call initization function
    init();

    //this for click and draw a line.
    pgID = LoadShaders("VertexShader.txt", "FragmentShader.txt");
    glUseProgram(pgID);

    //register callbacks
    glutDisplayFunc(renderScene);
    glutKeyboardFunc(myKeyBoard);
    glutMouseFunc(myMouse);

    //enter GLUT event processing cycle
    glutMainLoop();

    glDeleteVertexArrays(1, &VertexArrayID);

    return 1;
}