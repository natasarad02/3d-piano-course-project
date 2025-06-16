// Autor: Natasa Radmilovic, RA20/2021
// Opis: Klavir
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "SoundMaker.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <thread> // For std::this_thread::sleep_for
#include <chrono> // For std::chrono::duration

#define STB_EASY_FONT_IMPLEMENTATION
#include "stb_easy_font.h"
using namespace std;
using namespace SoundMaker;

unsigned int compileShader(GLenum type, const char* source);
unsigned int createShader(const char* vsSource, const char* fsSource);

void drawCube(glm::vec3 position, glm::vec3 scale, glm::vec3 color, GLuint shader, GLuint VAO, GLuint modelLoc, GLuint colorLoc);

glm::vec3 cameraPos(7.0f, 6.0f, 10.0f);
const glm::vec3 cameraTarget(7.0f, 0.0f, 0.0f);
const glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);

bool isScroll = false;

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    if (yoffset < 0) {
        VolumeDown();
    }
    else if (yoffset > 0) {
        VolumeUp();
    }

    isScroll = true;

}



glm::mat4 animateKeyPress(glm::vec3 position, glm::vec3 scale, bool pressed) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    if (pressed) {
 
        model = glm::translate(model, glm::vec3(0.0f, -0.05f, 0.0f));
        model = glm::rotate(model, glm::radians(2.0f), glm::vec3(1.0f, 0.0f, 0.0f));
      
    }
    model = glm::scale(model, scale);
    return model;
}
void drawCubeAnimated(glm::vec3 position, glm::vec3 scale, glm::vec3 color, bool pressed,
    GLuint shader, GLuint VAO, GLuint modelLoc, GLuint colorLoc) {
    glFrontFace(GL_CCW);

    glm::mat4 model = animateKeyPress(position, scale, pressed);
   
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniform3fv(colorLoc, 1, glm::value_ptr(color));

    glUniform1i(glGetUniformLocation(shader, "useTexture"), GL_FALSE);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
   
}







int main() {
    if (!glfwInit()) {
        std::cout << "GLFW init failed!" << std::endl;
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwInit();
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "Fullscreen", monitor, NULL);
    glfwMakeContextCurrent(window);

   // GLFWwindow* window = glfwCreateWindow(1920, 1080, "Klavijatura", NULL, NULL);
    if (!window) {
        std::cout << "GLFW window creation failed!" << std::endl;
        glfwTerminate();
        return 2;
    }

    glfwMakeContextCurrent(window);
    glewExperimental = true;

    if (glewInit() != GLEW_OK) {
        std::cout << "GLEW init failed!" << std::endl;
        return 3;
    }

    // Shader
    GLuint shader = createShader("basic.vert", "basic.frag");
    glUseProgram(shader);

    GLuint modelLoc = glGetUniformLocation(shader, "uM");
    GLuint viewLoc = glGetUniformLocation(shader, "uV");
    GLuint projLoc = glGetUniformLocation(shader, "uP");
    GLuint colorLoc = glGetUniformLocation(shader, "uColor");

    

    // VAO za kocku
    float cubeVertices[] = {
        // X Y Z
        -0.5, -0.5, -0.5,  // 0
         0.5, -0.5, -0.5,  // 1
         0.5,  0.5, -0.5,  // 2
        -0.5,  0.5, -0.5,  // 3
        -0.5, -0.5,  0.5,  // 4
         0.5, -0.5,  0.5,  // 5
         0.5,  0.5,  0.5,  // 6
        -0.5,  0.5,  0.5   // 7
    };
    unsigned int indices[] = {
        // donja strana
        0, 1, 2,  2, 3, 0,
        // gornja strana
        4, 5, 6,  6, 7, 4,
        // prednja strana
        4, 5, 1,  1, 0, 4,
        // zadnja strana
        7, 6, 2,  2, 3, 7,
        // leva strana
        0, 4, 7,  7, 3, 0,
        // desna strana
        5, 1, 2,  2, 6, 5,

    };



   

   

    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

  
    glEnable(GL_DEPTH_TEST);

    // Kamera
 

    /*glm::mat4 view = glm::lookAt(glm::vec3(7.0f, 6.0f, 10.0f),
        glm::vec3(7.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));*/
    glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, cameraUp);
    glm::mat4 projection = glm::perspective(glm::radians(60.0f), 1920.0f / 1080.0f, 0.1f, 100.0f);

    //glm::mat4 projP = glm::perspective(glm::radians(90.0f), (float)1920 / (float)1080, 0.1f, 100.0f); //Matrica perspektivne projekcije (FOV, Aspect Ratio, prednja ravan, zadnja ravan)
    glm::mat4 projO = glm::ortho(-10.0f, 10.0f, -4.0f, 4.0f, 0.1f, 90.0f);




    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Bele i crne dirke
    const int numWhite = 14;
    const int numBlack = (numWhite / 7) * 5;
    float whiteWidth = 1.0f;
    float whiteDepth = 4.0f;
    float blackWidth = 0.6f;
    float blackDepth = 2.5f;

    std::vector<int> blackKeyOffsets = { 1, 2, 4, 5, 6 }; 

    bool keysPressed[numWhite] = { false };
    bool blackKeysPressed[numBlack] = { false };

    glfwSetScrollCallback(window, scroll_callback);

    const double targetFrameTime = 1.0 / 60.0; // 60 FPS (16.67 ms per frame)
    double previousTime = glfwGetTime();
    double currentTime;
    double deltaTime;
    

    while (!glfwWindowShouldClose(window)) {


        


        currentTime = glfwGetTime();
        deltaTime = currentTime - previousTime;

        glClearColor(0.2f, 0.0f, 0.0f, 1.0f);


        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

       
      

        float gap = 0.07f;
        // Bele dirke
        for (int i = 0; i < numWhite; ++i) {
            glm::vec3 pos(i * (whiteWidth + gap), 0.0f, 0.0f);
            glm::vec3 scale(whiteWidth, 1.0f, whiteDepth);
            glm::vec3 color(1.0f, 1.0f, 1.0f);
            drawCubeAnimated(pos, scale, color, keysPressed[i], shader, VAO, modelLoc, colorLoc);


        }

        // Crne dirke
        int blackIndex = 0; 
        for (int i = 0; i < numWhite; ++i) {
            int octPos = i % 7;
            float spacing = whiteWidth + gap;

            if (std::find(blackKeyOffsets.begin(), blackKeyOffsets.end(), octPos) != blackKeyOffsets.end()) {
                glm::vec3 pos(i * spacing - blackWidth / 2.0f - 0.25, 0.3f, -0.75f);
                glm::vec3 scale(blackWidth, 1.0f, blackDepth);
                glm::vec3 color(0.0f, 0.0f, 0.0f);

                drawCubeAnimated(pos, scale, color, blackKeysPressed[blackIndex], shader, VAO, modelLoc, colorLoc);
                blackIndex++; 
            }
        }

        // Podloga
        glm::vec3 basePos = glm::vec3(((numWhite - 1) * (whiteWidth + gap)) / 2.0f, -0.6f, 0.0f);
        glm::vec3 baseScale = glm::vec3(numWhite * (whiteWidth + gap) + 1.0, 0.5f, whiteDepth + 1.0f); // šire i malo dublje
        glm::vec3 baseColor = glm::vec3(0.0f, 0.0f, 0.0f); // crna boja

        drawCubeAnimated(basePos, baseScale, baseColor, false, shader, VAO, modelLoc, colorLoc);

        float baseWidth = numWhite * (whiteWidth + gap) + 1.0f;
        float baseHeight = 0.5f;
        float baseDepth = whiteDepth + 1.0f;

        float pillarWidth = 0.3f;
        float pillarHeight = 2.0f;
        float pillarDepth = baseDepth;

        // Levi stub
        glm::vec3 leftPillarPos = glm::vec3(basePos.x - baseWidth / 2.0f - pillarWidth / 2.0f,
            pillarHeight / 2.0f - 0.85f, 0.0f);
        glm::vec3 leftPillarScale = glm::vec3(pillarWidth, pillarHeight, pillarDepth);

        drawCubeAnimated(leftPillarPos, leftPillarScale, baseColor, false, shader, VAO, modelLoc, colorLoc);

        // Desni stub
        glm::vec3 rightPillarPos = glm::vec3(basePos.x + baseWidth / 2.0f + pillarWidth / 2.0f,
            pillarHeight / 2.0f - 0.85f, 0.0f);
        glm::vec3 rightPillarScale = glm::vec3(pillarWidth, pillarHeight, pillarDepth);

        drawCubeAnimated(rightPillarPos, rightPillarScale, baseColor, false, shader, VAO, modelLoc, colorLoc);

        // Okvir
        float backWidth = numWhite * (whiteWidth + gap) + 5.0f;
        float backHeight = 7.0f;
        float backDepth = 0.5f;

     
        glm::vec3 backPos = glm::vec3((backWidth - 6.0f) / 2.0f, 0.2f, -3.0);
        glm::vec3 backScale = glm::vec3(backWidth, backHeight, backDepth);
        glm::vec3 backColor = glm::vec3(0.0f, 0.0f, 0.0f);

       
        drawCubeAnimated(backPos, backScale, backColor, false, shader, VAO, modelLoc, colorLoc);



        
        //Testiranje dubine
        if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS)
        {
            glEnable(GL_DEPTH_TEST); //Ukljucivanje testiranja Z bafera
        }
        if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS)
        {
            glDisable(GL_DEPTH_TEST);
        }

        //Menjanje projekcija
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        {
            glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projO));
        }

       // glEnable(GL_CULL_FACE);
        //Odstranjivanje lica (Prethodno smo podesili koje lice uklanjamo sa glCullFace)
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        {
            glEnable(GL_CULL_FACE);
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
        {
            glDisable(GL_CULL_FACE);
        }
      

       
            if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
                keysPressed[7] = true;
                SoundMaker::playSound("res/C4.mp3", true);
            }
            else {
                keysPressed[7] = false;
            }

            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
                keysPressed[8] = true;
                SoundMaker::playSound("res/D4.mp3", true);
            }
            else {
                keysPressed[8] = false;
            }

            if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
                keysPressed[9] = true;
                SoundMaker::playSound("res/E4.mp3", true);
            }
            else {
                keysPressed[9] = false;
            }

            if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
                keysPressed[10] = true;
                SoundMaker::playSound("res/F4.mp3", true);
            }
            else {
                keysPressed[10] = false;
            }
            if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
                keysPressed[11] = true;
                SoundMaker::playSound("res/G4.mp3", true);
            }
            else {
                keysPressed[11] = false;
            }
            if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
                keysPressed[12] = true;
                SoundMaker::playSound("res/A4.mp3", true);
            }
            else {
                keysPressed[12] = false;
            }
            if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
                keysPressed[13] = true;
                SoundMaker::playSound("res/B4.mp3", true);
            }
            else {
                keysPressed[13] = false;
            }

            if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
                SoundMaker::playSound("res/Db4.mp3", true);
                blackKeysPressed[5] = true;
            }
            else {
                blackKeysPressed[5] = false;
            }

            if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
                SoundMaker::playSound("res/Eb4.mp3", true);
                blackKeysPressed[6] = true;
            }
            else {
                blackKeysPressed[6] = false;
            }

            if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) {
                SoundMaker::playSound("res/Gb4.mp3", true);
                blackKeysPressed[7] = true;
            }
            else {
                blackKeysPressed[7] = false;
            }

            if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) {
                SoundMaker::playSound("res/Ab4.mp3", true);
                blackKeysPressed[8] = true;
            }
            else {
                blackKeysPressed[8] = false;
            }

            if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS) {
                SoundMaker::playSound("res/Bb4.mp3", true);
                blackKeysPressed[9] = true;
            }
            else {
                blackKeysPressed[9] = false;
            }
       
        
            if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
                keysPressed[0] = true;
                SoundMaker::playSound("res/C3.mp3", true);

            }
            else {
                keysPressed[0] = false;
            }

            if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
                keysPressed[1] = true;
                SoundMaker::playSound("res/D3.mp3", true);
            }
            else {
                keysPressed[1] = false;
                
            }

            if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
                keysPressed[2] = true;
                SoundMaker::playSound("res/E3.mp3", true);
            }
            else {
                keysPressed[2] = false;
            }

            if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
                keysPressed[3] = true;
                SoundMaker::playSound("res/F3.mp3", true);
            }
            else {
                keysPressed[3] = false;
            }
            if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
                keysPressed[4] = true;
                SoundMaker::playSound("res/G3.mp3", true);
            }
            else {
                keysPressed[4] = false;
            }
            if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
                keysPressed[5] = true;
                SoundMaker::playSound("res/A3.mp3", true);
            }
            else {
                keysPressed[5] = false;
            }
            if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
                keysPressed[6] = true;
                SoundMaker::playSound("res/B3.mp3", true);
            }
            else {
                keysPressed[6] = false;
            }

            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
                SoundMaker::playSound("res/Db3.mp3", true);
                blackKeysPressed[0] = true;
            }
            else {
                blackKeysPressed[0] = false;
            }

            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
                SoundMaker::playSound("res/Eb3.mp3", true);
                blackKeysPressed[1] = true;
            }
            else {
                blackKeysPressed[1] = false;
            }

            if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
                SoundMaker::playSound("res/Gb3.mp3", true);
                blackKeysPressed[2] = true;
            }
            else {
                blackKeysPressed[2] = false;
            }

            if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
                SoundMaker::playSound("res/Ab3.mp3", true);
                blackKeysPressed[3] = true;
            }
            else {
                blackKeysPressed[3] = false;
            }

            if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
                SoundMaker::playSound("res/Bb3.mp3", true);
                blackKeysPressed[4] = true;
            }
            else {
                blackKeysPressed[4] = false;
            }

            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            {
                glfwSetWindowShouldClose(window, GL_TRUE);
            }
      

        const float cameraSpeed = 0.1f; 

        if (cameraPos.z <= 17.0f && glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_W) == GLFW_REPEAT) {
           

                cameraPos.z += cameraSpeed;  
              //  std::cout << "Donja granica: " << cameraPos.z << std::endl;
             
               
          
        }

        if (cameraPos.z >= 8.49f && glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_W) == GLFW_REPEAT) {


            cameraPos.z -= cameraSpeed;
          //  std::cout << "Gornja granica: " << cameraPos.z << std::endl;
          
           

        }
        glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, cameraUp);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));


        glfwSwapBuffers(window);
        glfwPollEvents();

        if (deltaTime < targetFrameTime) {
            std::this_thread::sleep_for(std::chrono::duration<double>(targetFrameTime - deltaTime));
        }

        // Update previous time for the next frame
        previousTime = glfwGetTime();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shader);
    glfwTerminate();
    return 0;

  

}

void drawCube(glm::vec3 position, glm::vec3 scale, glm::vec3 color, GLuint shader, GLuint VAO, GLuint modelLoc, GLuint colorLoc) {
    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
    model = glm::scale(model, scale);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniform3fv(colorLoc, 1, glm::value_ptr(color));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

unsigned int compileShader(GLenum type, const char* source)
{
    std::string content = "";
    std::ifstream file(source);
    std::stringstream ss;
    if (file.is_open())
    {
        ss << file.rdbuf();
        file.close();
        std::cout << "Uspjesno procitao fajl sa putanje \"" << source << "\"!" << std::endl;
    }
    else {
        ss << "";
        std::cout << "Greska pri citanju fajla sa putanje \"" << source << "\"!" << std::endl;
    }
     std::string temp = ss.str();
     const char* sourceCode = temp.c_str();

    int shader = glCreateShader(type);
    
    int success;
    char infoLog[512];
    glShaderSource(shader, 1, &sourceCode, NULL);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        if (type == GL_VERTEX_SHADER)
            printf("VERTEX");
        else if (type == GL_FRAGMENT_SHADER)
            printf("FRAGMENT");
        printf(" sejder ima gresku! Greska: \n");
        printf(infoLog);
    }
    return shader;
}
unsigned int createShader(const char* vsSource, const char* fsSource)
{
    unsigned int program;
    unsigned int vertexShader;
    unsigned int fragmentShader;

    program = glCreateProgram();

    vertexShader = compileShader(GL_VERTEX_SHADER, vsSource);
    fragmentShader = compileShader(GL_FRAGMENT_SHADER, fsSource);

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);
    glValidateProgram(program);

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_VALIDATE_STATUS, &success);
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(program, 512, NULL, infoLog);
        std::cout << "Objedinjeni sejder ima gresku! Greska: \n";
        std::cout << infoLog << std::endl;
    }

    glDetachShader(program, vertexShader);
    glDeleteShader(vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(fragmentShader);

    return program;
}


