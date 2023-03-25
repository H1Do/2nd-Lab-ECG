#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "math_3d.h"

GLuint VBO; // Указатель на буфер вершины
GLuint gWorldLocation; // Указатель для доступа к всемирной матрице

static const char* pVS = "                                                          \n\
#version 330                                                                        \n\
                                                                                    \n\
layout (location = 0) in vec3 Position;                                             \n\
                                                                                    \n\
uniform mat4 gWorld;                                                                \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
    gl_Position = gWorld * vec4(Position, 1.0);                                     \n\
}";

static const char* pFS = "                                                          \n\
#version 330                                                                        \n\
                                                                                    \n\
out vec4 FragColor;                                                                 \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
    FragColor = vec4(1.0, 0.0, 0.0, 1.0);                                           \n\
}";

static void RenderSceneCB()
{
    glClear(GL_COLOR_BUFFER_BIT); // Очищение буфера кадра 

    static float Scale = 0.0f;

    Scale += 0.001f;

    Matrix4f World;

    // Настраиваем матрицу изменения масштаба 
    World.m[0][0] = sinf(Scale); World.m[0][1] = 0.0f;        World.m[0][2] = 0.0f;        World.m[0][3] = 0.0f;
    World.m[1][0] = 0.0f;        World.m[1][1] = cosf(Scale); World.m[1][2] = 0.0f;        World.m[1][3] = 0.0f;
    World.m[2][0] = 0.0f;        World.m[2][1] = 0.0f;        World.m[2][2] = sinf(Scale); World.m[2][3] = 0.0f;
    World.m[3][0] = 0.0f;        World.m[3][1] = 0.0f;        World.m[3][2] = 0.0f;        World.m[3][3] = 1.0f;



    glUniformMatrix4fv(gWorldLocation, 1, GL_TRUE, &World.m[0][0]); // Загружаем матрицу в шейдер
    // Передаём адрес uniform-переменной. Кол-во матриц, к. мы обрабатываем. Строковый порядок. Сама матрица

    glEnableVertexAttribArray(0); // Включаем атрибуты вершины
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); // Указываем конвейеру, как воспринимать данные внутри буфера

    glDrawArrays(GL_TRIANGLES, 0, 3); // Запускаем функцию для отрисовки

    glDisableVertexAttribArray(0); // Выключаем атрибуты вершины

    glutSwapBuffers(); // Меняем местами фоновый буфер и буфер кадра
}


static void InitializeGlutCallbacks()
{
    glutDisplayFunc(RenderSceneCB);
    glutIdleFunc(RenderSceneCB);
}

static void CreateVertexBuffer()
{
    Vector3f Vertices[3];
    Vertices[0] = Vector3f(-1.0f, -1.0f, 0.0f);
    Vertices[1] = Vector3f(1.0f, -1.0f, 0.0f);
    Vertices[2] = Vector3f(0.0f, 1.0f, 0.0f);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
}

static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
    GLuint ShaderObj = glCreateShader(ShaderType); // Создаём шейдер

    if (ShaderObj == 0) { // Проверяем на ошибку
        fprintf(stderr, "Error creating shader type %d\n", ShaderType);
        exit(0);
    }

    const GLchar* p[1];
    p[0] = pShaderText;
    GLint Lengths[1];
    Lengths[0] = strlen(pShaderText);
    glShaderSource(ShaderObj, 1, p, Lengths); // указываем исходных код шейдера

    glCompileShader(ShaderObj); // Компилируем шейдер
    GLint success;
    glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
    if (!success) { // Проверяем на ошибки
        GLchar InfoLog[1024];
        glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
        fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
        exit(1);
    }

    glAttachShader(ShaderProgram, ShaderObj); // Присоединяем скомп. объект шейдера к объекту программы
}

static void CompileShaders()
{
    GLuint ShaderProgram = glCreateProgram(); // Создаем программный объект

    if (ShaderProgram == 0) { // Проверяем на ошибку
        fprintf(stderr, "Error creating shader program\n");
        exit(1);
    }

    AddShader(ShaderProgram, pVS, GL_VERTEX_SHADER);
    AddShader(ShaderProgram, pFS, GL_FRAGMENT_SHADER);

    GLint Success = 0;
    GLchar ErrorLog[1024] = { 0 };

    glLinkProgram(ShaderProgram); // Линкуем скомпилированные и присоед. к программе шейдеры 
    glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);
    if (Success == 0) { // Проверяем на ошибку
        glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
        exit(1);
    }

    glValidateProgram(ShaderProgram); // Проверяем на успешность линковки 
    glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);
    if (!Success) {
        glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
        exit(1);
    }

    glUseProgram(ShaderProgram); // Назначаем отлинкованной программы шейдеров для конвейера

    gWorldLocation = glGetUniformLocation(ShaderProgram, "gWorld");
    assert(gWorldLocation != 0xFFFFFFFF);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv); // Иницианализируем GLUT
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // Включаем двойную буферизацию и буфер цвета 
    glutInitWindowSize(1024, 768); // Задаём размер окна
    glutInitWindowPosition(100, 100); // Позицию окна
    glutCreateWindow("Something"); // Создаём окно и задаём ему заголовок

    InitializeGlutCallbacks();

    GLenum res = glewInit(); // Инициализируем GLEW
    if (res != GLEW_OK) { // Проверяем на ошибку
        fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
        return 1;
    }

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Задаем цвет, к. будет использован во время очистки буфера кадра

    CreateVertexBuffer();

    CompileShaders(); // Компилируем шейдеры

    glutMainLoop(); // Передаём контроль GLUT

    return 0;
}