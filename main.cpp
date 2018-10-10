#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "gltext.h"
#include "generateshaders.hpp"

#include <iostream>
#include <math.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
GLFWwindow* initializeGLFWandGLAD();
GLFWwindow* windowcreateGLFWWINDOW(unsigned int screen_height,unsigned int screen_width);
char *textFileRead(char *fn);//To read shader file to a char*


int* createVertexandFragmentShaders(char vs[],char fs[]);//To create shader files usable in main

int createShaderProgram(int shaderfile0, int shaderfile1);//Create shader program



float xr = 0.0;
float yr = 0.0;
float CPU_x = 0.0;
float CPU_y = 0.0;

float dx = xr - CPU_x;
float dy = yr - CPU_y;
float distance = 0.0;

int main()
{
    //Initialize GLFW/GLAD
    GLFWwindow* window = initializeGLFWandGLAD();

    
    //Create our vertex and fragment shaders
    int *user_shaders  = createVertexandFragmentShaders("/Users/gregsmith/Desktop/opengldotcom_v0/opengldotcom_v0/shaders/vs_0.vertex","/Users/gregsmith/Desktop/opengldotcom_v0/opengldotcom_v0/shaders/fs_0.fragment");
    
    int *CPU_shaders = createVertexandFragmentShaders("/Users/gregsmith/Desktop/opengldotcom_v0/opengldotcom_v0/shaders/vs_1.vertex","/Users/gregsmith/Desktop/opengldotcom_v0/opengldotcom_v0/shaders/fs_1.fragment");

    //Create User shader program
    int shaderProgram = createShaderProgram(user_shaders[0], user_shaders[1]);
    
    //Create CPU shader program
    int CPU_shaderProgram = createShaderProgram(CPU_shaders[0], CPU_shaders[1]);
    
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        -0.95f, -0.05f, 0.0f, // left
        -0.90f, 0.05f, 0.0f, // right
        -0.85f,  -0.05f, 0.0f,  // top
        
    };
    
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);//Generate VBO for user triangle
    

    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    GLuint v_loc = glGetAttribLocation(shaderProgram,"aPos");
    glVertexAttribPointer(v_loc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(v_loc);
    

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    

    
    
    //Computer triangle
    float cpu_vertices[] = {
        0.80f,0.80f,0.0f, //left
        0.85f,0.90f,0.0f, //left
        0.90f,0.80f,0.0f, //left
        
    };
    
    
    unsigned int CPU_VAO;
    glGenVertexArrays(1, &CPU_VAO);
    glBindVertexArray(CPU_VAO);
    unsigned int CPU_VBO;
    glGenBuffers(1, &CPU_VBO);//Generate VBO for cpu triangle
    glBindBuffer(GL_ARRAY_BUFFER, CPU_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cpu_vertices), cpu_vertices, GL_STATIC_DRAW);
    
    GLuint CPU_v_loc = glGetAttribLocation(CPU_shaderProgram,"CPU_aPos");
    glVertexAttribPointer(CPU_v_loc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(CPU_v_loc);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    
    
    
    
    
    
    
    //Initialize glText
    if (!gltInit())
    {
        printf("Failed to initialize glText\n");
        glfwTerminate();
        return EXIT_FAILURE;
    }
    
    //Create and set text
    GLTtext *text1 = gltCreateText();
    double time;
    char str[30];
    int viewportWidth, viewportHeight;
    
    
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        
//        time = glfwGetTime();//seed time
        glfwGetFramebufferSize(window, &viewportWidth, &viewportHeight);//get size of screen
        glViewport(0, 0, viewportWidth, viewportHeight);
        
        // input
        // -----
        processInput(window);
        
        // render
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        // draw our first triangle
        glUseProgram(shaderProgram);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glUniform2f(glGetUniformLocation(shaderProgram,"displacement"),xr,yr);
        glBindBuffer(GL_ARRAY_BUFFER, 0); // no need to unbind it every time
        glBindVertexArray(0);

        
        //Draw CPU Traingle
        glBindVertexArray(CPU_VAO);
        glUseProgram(CPU_shaderProgram);
        glBindBuffer(GL_ARRAY_BUFFER, CPU_VBO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glUniform2f(glGetUniformLocation(CPU_shaderProgram,"CPU_displacement"),CPU_x,CPU_y);
        glBindBuffer(GL_ARRAY_BUFFER, 0); // no need to unbind it every time
        glBindVertexArray(0);
        
        time = glfwGetTime();//seed time
        //Draw text
        gltColor(1.0f, 1.0f, 1.0f, 1.0f);//set text color
        sprintf(str, "Time: %.4f", time);//put time variable into str variable
//        std::cout << "Time:" << time << "\n";
        gltSetText(text1, str);
        
        if(distance >= 1.90){
            std::cout << "Collision\n" << "Your time was:" << time << "\n\n\n";
            exit(-1);
        }
        gltDrawText2DAligned(text1, 0.0f, (GLfloat)viewportHeight, 1.0f, GLT_LEFT, GLT_BOTTOM);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    gltDeleteText(text1);
    glfwTerminate();
    return 0;
}




//    |-----                |
//    |-----              _____ *
//    |         |---   ---  |   |  ---  |---    |--
//    |   |___| |   | |___  |   | |___| |   | __|

//FUNCTION IMPLEMENTAITIONS

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }

    if(glfwGetKey(window, GLFW_KEY_UP ) == GLFW_PRESS ){
//        std::cout << "Up Key Pressed!\n";
        //Move Character Up Here
        if(!(fabs(yr + 0.01) > 0.90)){
            yr = yr + 0.01;
        }
    }
    if(glfwGetKey(window, GLFW_KEY_DOWN ) == GLFW_PRESS ){
//        std::cout << "Down Key Pressed!\n";
        //Move Character Down Here
        if(!(fabs(yr - 0.01) > 0.90)){
            yr = yr - 0.01;
        }
    }
//    if(glfwGetKey(window, GLFW_KEY_LEFT ) == GLFW_PRESS ){
//        std::cout << "Left Key Pressed!\n";
//        //Move Character Left Here
//        xr = xr - 0.01;
//    }
//    if(glfwGetKey(window, GLFW_KEY_RIGHT ) == GLFW_PRESS ){
//        std::cout << "Right Key Pressed!\n";
//        //Move Character Right Here
//        xr = xr + 0.01;
//    }
//    yr = yr - 0.005;
    float dx = xr - CPU_x;
    float dy = yr - CPU_y;
//    float distance = 0.0;
    distance = sqrt(dx*dx + dy*dy);
    //Check for a collision
    if(distance >= 1.90){
        std::cout << "Collision:" << distance <<"\n";
    }
    
    if((CPU_x > - 1.75)){
        CPU_x = CPU_x - 0.005;
    }
    else{
        CPU_x = 1.0;
    }
    
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}



GLFWwindow* windowcreateGLFWWINDOW(unsigned int screen_height,unsigned int screen_width){
    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(screen_width, screen_height, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    return window;
}

GLFWwindow* initializeGLFWandGLAD(){
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif
    
    //    // glfw window creation
    //    // --------------------
    unsigned int screen_height = 600;
    unsigned int screen_width = 800;
    GLFWwindow* window = windowcreateGLFWWINDOW(screen_height, screen_width);
    
    
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(-1);
    }
    
    return window;
}



int* createVertexandFragmentShaders(char vs[],char fs[]){
    int success;
    char infoLog[512];
    
    
    //Vertex shader
    char *vs_array = vs;
    
    
    const char* vertexShaderSource = textFileRead(vs_array);
    
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    
    
    
    //Fragment shader
    char *fs_array = fs;
    
    
    const char* fragmentShaderSource = textFileRead(fs_array);
    
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    int* shaderArray = new int[2];
    shaderArray[0] = vertexShader;
    shaderArray[1] = fragmentShader;
    
    
    return shaderArray;
}



char *textFileRead(char *fn) {
    
    FILE *fp;
    char *content = NULL;
    
    int count=0;
    
    if (fn != NULL) {
        fp = fopen(fn,"rt");
        
        if (fp != NULL) {
            
            fseek(fp, 0, SEEK_END);
            count = ftell(fp);
            rewind(fp);
            
            if (count > 0) {
                content = (char *)malloc(sizeof(char) * (count+1));
                count = fread(content,sizeof(char),count,fp);
                content[count] = '\0';
            }
            fclose(fp);
        }
    }
    return content;
}

int createShaderProgram(int shaderfile0, int shaderfile1){
    
    int success;
    char infoLog[512];
    // link shaders
    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, shaderfile0);
    glAttachShader(shaderProgram, shaderfile1);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(shaderfile0);
    glDeleteShader(shaderfile1);
    
    return shaderProgram;
}
















































































