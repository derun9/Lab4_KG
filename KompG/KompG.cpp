#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "shader_loader.h"
#include "glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <cmath>

// Массив с вершинами для шестиугольника
float vertices[] = {
    0.0f,  1.0f, 0.0f,
    0.866f,  0.5f, 0.0f,
    0.866f, -0.5f, 0.0f,
    0.0f, -1.0f, 0.0f,
   -0.866f, -0.5f, 0.0f,
   -0.866f,  0.5f, 0.0f
};

// Индексы для рисования шестиугольника
unsigned int indices[] = { 0, 1, 2, 3, 4, 5 };

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float yaw = -90.0f;   // начальный угол по Y, -90 чтобы камера смотрела вперёд
float pitch = 0.0f;   // начальный угол по X
float sensitivity = 0.1f;

bool firstMouse = true;

void settingMat4(int ID, const std::string& name, const glm::mat4& mat) {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 768;

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

glm::mat4 view = glm::lookAt(cameraPos, cameraFront + cameraPos, cameraUp);

const float cameraSpeed = 0.05f;

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    static float xoffset, yoffset;

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    xoffset = xpos - lastX;
    yoffset = lastY - ypos; // инверсия, чтобы двигалась естественно

    lastX = xpos;
    lastY = ypos;

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // Ограничиваем угол тангажа
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    // Обновляем фронт камеры
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

int main() {
    // Инициализация GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Устанавливаем параметры для контекста OpenGL 4.6
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Создаем окно
    GLFWwindow* window = glfwCreateWindow(512, 512, "OpenGL", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // скрыть курсор
    glfwSetCursorPosCallback(window, mouse_callback);            // обработчик мыши
    glewExperimental = GL_TRUE;

    // Инициализация GLEW
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // Генерация объектов VAO, VBO и EBO
    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Настройка указателя атрибута
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Создание шейдерной программы
    GLuint shaderProgram = createShader("vertex.glsl", "fragment.glsl");
    glUseProgram(shaderProgram);
    glClearColor(1.0f, 1.0f, 0.5f, 1.0f);

    // Главный цикл рендеринга
    while (!glfwWindowShouldClose(window)) {
        // Выход через ESC
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // Обработка ввода
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            cameraPos += cameraSpeed * cameraFront;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            cameraPos -= cameraSpeed * cameraFront;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

        // Обновление матрицы вида
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        
        glClear(GL_COLOR_BUFFER_BIT);

        settingMat4(shaderProgram, "projection", projection);
        settingMat4(shaderProgram, "view", view);

        // Получаем текущее время
        float timeValue = glfwGetTime();

        // Устанавливаем значение uniform-переменной timeValue
        GLint timeLoc = glGetUniformLocation(shaderProgram, "timeValue");
        if (timeLoc != -1) {
            glUniform1f(timeLoc, timeValue);
        }
        else {
            std::cerr << "ERROR: Uniform timeValue not found!" << std::endl;
        }

        // Рисуем шестиугольник
        glDrawElements(GL_TRIANGLE_FAN, 6, GL_UNSIGNED_INT, 0);

        // Обновляем экран
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Удаление объектов
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    // Завершаем работу с GLFW
    glfwTerminate();
    return 0;
}
