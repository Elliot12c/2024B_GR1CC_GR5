
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION 
#include <learnopengl/stb_image.h>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 2200;
const unsigned int SCR_HEIGHT = 1000;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//llamada a la funcion que retorna la posisicon de la camara
void printCameraCoordinates(const Camera& camera);

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Exercise 16 Task 3", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    //stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    Shader ourShader("shaders/shader_exercise16_mloading.vs", "shaders/shader_exercise16_mloading.fs");
    //Shader ourShaderSky("shaders/VertexsShader_TareaB2T3.vs", "shaders/FragmentShader_TareaB2T3.fs");

    // load models
    // -----------
    //Model ourModel(FileSystem::getPath("resources/objects/backpack/backpack.obj"));
    Model ourModel("model/stadium/stadium2.obj");
    Model messiModel("model/messi/messi.obj");
    Model skydomModel("model/skydom/skydom.obj");
    Model fireworkModel("model/firework1/firework1.obj");
    Model fireworkModel2("model/firework2/firework2.obj");
    Model fireworkModel3("model/firework3/firework3.obj");
    Model fireworkModel4("model/firework4/firework4.obj");
    Model fireworkModel5("model/firework5/firework5.obj");
    Model terrenoModel("model/terreno/terreno.obj");
    Model balonModel("model/balon/balon.obj");
    Model copaModel("model/copa/copa.obj");
    //Model ourModel("model/backpack/backpack.obj");


    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    camera.MovementSpeed = 3; //Optional. Modify the speed of the camera

    //dato camra position
    float lastPrintTime = 0.0f;

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //funcion posicion camara
        if (currentFrame - lastPrintTime >= 2.0f) {
            printCameraCoordinates(camera);
            lastPrintTime = currentFrame;
        }

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms
        ourShader.use();

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // render the loaded model
        //Stadium
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.3f, 0.0f, 2.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        ourShader.setMat4("model", model);
        ourModel.Draw(ourShader);

        //Terreno
        glm::mat4 modelTerreno = glm::mat4(1.0f);
        modelTerreno = glm::translate(modelTerreno, glm::vec3(0.0f, -0.8f, -10.0f)); // translate it down so it's at the center of the scene
        modelTerreno = glm::scale(modelTerreno, glm::vec3(50.0f, 50.0f, 50.0f));	// it's a bit too big for our scene, so scale it down
        ourShader.setMat4("model", modelTerreno);
        terrenoModel.Draw(ourShader);

        //Players
        
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {

            glm::mat4 modelMessi = glm::mat4(1.0f);
            modelMessi = glm::translate(modelMessi, glm::vec3(0.117488f, 0.0f, 2.1629f)); // translate it down so it's at the center of the scene
            modelMessi = glm::scale(modelMessi, glm::vec3(0.0012f, 0.0012f, 0.0012f));	// it's a bit too big for our scene, so scale it down
            ourShader.setMat4("model", modelMessi);
            messiModel.Draw(ourShader);

            glm::mat4 modelMessi1 = glm::mat4(1.0f);
            modelMessi1 = glm::translate(modelMessi1, glm::vec3(0.228094f, 0.0f, 4.38508f)); // translate it down so it's at the center of the scene
            modelMessi1 = glm::scale(modelMessi1, glm::vec3(0.0012f, 0.0012f, 0.0012f));	// it's a bit too big for our scene, so scale it down
            ourShader.setMat4("model", modelMessi1);
            messiModel.Draw(ourShader);



            for (unsigned int i = 0; i < 2; i++)
            {
                for (unsigned j = 0; j < 4; j++)
                {
                    glm::mat4 modelMessi2 = glm::mat4(1.0f);
                    modelMessi2 = glm::translate(modelMessi2, glm::vec3(-1.15691f + j, 0.0f, 3.87968f - i));
                    //modelMessi2 = glm::rotate(modelMessi2, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                    modelMessi2 = glm::scale(modelMessi2, glm::vec3(0.0012f, 0.0012f, 0.0012f));
                    ourShader.setMat4("model", modelMessi2);
                    messiModel.Draw(ourShader);
                }

            }

            for (unsigned int i = 0; i < 2; i++)
            {
                for (unsigned j = 0; j < 4; j++)
                {
                    glm::mat4 modelMessi3 = glm::mat4(1.0f);
                    modelMessi3 = glm::translate(modelMessi3, glm::vec3(-1.06649 + j, 0.0f, 0.108762 + i));
                    //modelMessi2 = glm::rotate(modelMessi2, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                    modelMessi3 = glm::scale(modelMessi3, glm::vec3(0.0012f, 0.0012f, 0.0012f));
                    ourShader.setMat4("model", modelMessi3);
                    messiModel.Draw(ourShader);
                }

            }

            glm::mat4 modelMessi4 = glm::mat4(1.0f);
            modelMessi4 = glm::translate(modelMessi4, glm::vec3(0.0881392f, 0.0f, -0.396865f)); // translate it down so it's at the center of the scene
            modelMessi4 = glm::scale(modelMessi4, glm::vec3(0.0012f, 0.0012f, 0.0012f));	// it's a bit too big for our scene, so scale it down
            ourShader.setMat4("model", modelMessi4);
            messiModel.Draw(ourShader);

            for (unsigned i = 0; i < 2; i++)
            {
                glm::mat4 modelMessi3 = glm::mat4(1.0f);
                modelMessi3 = glm::translate(modelMessi3, glm::vec3(-0.0881392 + i, 0.0f, 1.65189f));
                //modelMessi2 = glm::rotate(modelMessi2, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                modelMessi3 = glm::scale(modelMessi3, glm::vec3(0.0012f, 0.0012f, 0.0012f));
                ourShader.setMat4("model", modelMessi3);
                messiModel.Draw(ourShader);
            }
        }


        //Sky

        glm::mat4 modelSkydom = glm::mat4(1.0f);
        modelSkydom = glm::translate(modelSkydom, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        modelSkydom = glm::scale(modelSkydom, glm::vec3(20.0f, 20.0f, 20.0f));	// it's a bit too big for our scene, so scale it down
        ourShader.setMat4("model", modelSkydom);
        skydomModel.Draw(ourShader);

        //Fireworks
        //al mantener presionada la tecla 1 aparecen los juegos pirotecnicos

        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {

            glm::mat4 modelFirework2 = glm::mat4(1.0f);
            modelFirework2 = glm::translate(modelFirework2, glm::vec3(2.66769f, 1.78559f, 0.45395f)); // translate it down so it's at the center of the scene
            modelFirework2 = glm::scale(modelFirework2, glm::vec3(0.1f, 0.1f, 0.1f));	// it's a bit too big for our scene, so scale it down
            ourShader.setMat4("model", modelFirework2);
            fireworkModel2.Draw(ourShader);

            glm::mat4 modelFirework = glm::mat4(1.0f);
            modelFirework = glm::translate(modelFirework, glm::vec3(2.82727f, 1.89701f, 3.29813f)); // translate it down so it's at the center of the scene
            modelFirework = glm::scale(modelFirework, glm::vec3(0.1f, 0.1f, 0.1f));	// it's a bit too big for our scene, so scale it down
            ourShader.setMat4("model", modelFirework);
            fireworkModel.Draw(ourShader);

            glm::mat4 modelFirework3 = glm::mat4(1.0f);
            modelFirework3 = glm::translate(modelFirework3, glm::vec3(-2.68553f, 1.87859f, 3.6442f)); // translate it down so it's at the center of the scene
            modelFirework3 = glm::scale(modelFirework3, glm::vec3(0.1f, 0.1f, 0.1f));	// it's a bit too big for our scene, so scale it down
            ourShader.setMat4("model", modelFirework3);
            fireworkModel3.Draw(ourShader);

            glm::mat4 modelFirework4 = glm::mat4(1.0f);
            modelFirework4 = glm::translate(modelFirework4, glm::vec3(-2.9926f, 1.80539f, 0.855825f)); // translate it down so it's at the center of the scene
            modelFirework4 = glm::scale(modelFirework4, glm::vec3(0.1f, 0.1f, 0.1f));	// it's a bit too big for our scene, so scale it down
            ourShader.setMat4("model", modelFirework4);
            fireworkModel4.Draw(ourShader);

            glm::mat4 modelFirework5 = glm::mat4(1.0f);
            modelFirework5 = glm::translate(modelFirework5, glm::vec3(-0.230177f, 3.02766f, 1.84074f)); // translate it down so it's at the center of the scene
            modelFirework5 = glm::scale(modelFirework5, glm::vec3(0.1f, 0.1f, 0.1f));	// it's a bit too big for our scene, so scale it down
            ourShader.setMat4("model", modelFirework5);
            fireworkModel5.Draw(ourShader);


        }


        //balon
        // 

        glm::mat4 modelBalon = glm::mat4(1.0f);
        modelBalon = glm::translate(modelBalon, glm::vec3(0.229368f, 0.0f, 1.97711f)); // translate it down so it's at the center of the scene
        modelBalon = glm::scale(modelBalon, glm::vec3(0.05f, 0.05f, 0.05f));	// it's a bit too big for our scene, so scale it down
        ourShader.setMat4("model", modelBalon);
        balonModel.Draw(ourShader);

        //copa

        glm::mat4 modelCopa = glm::mat4(1.0f);
        modelCopa = glm::translate(modelCopa, glm::vec3(0.229368f, 0.0f, 1.97711f)); // translate it down so it's at the center of the scene
        modelCopa = glm::scale(modelCopa, glm::vec3(0.05f, 0.05f, 0.05f));	// it's a bit too big for our scene, so scale it down
        ourShader.setMat4("model", modelCopa);
        copaModel.Draw(ourShader);








        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    //---------------------------------------------------------------------

        // glfw: terminate, clearing all previously allocated GLFW resources.
        // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    
    // Agregar movimiento arriba/abajo con Espacio y Ctrl izquierdo
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.Position += glm::vec3(0.0f, camera.MovementSpeed * deltaTime , 0.0f); // Sube
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        camera.Position -= glm::vec3(0.0f, camera.MovementSpeed * deltaTime, 0.0f); // Baja
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

//funcion camara position
void printCameraCoordinates(const Camera& camera) {
    glm::vec3 position = camera.Position;
    std::cout << "Camera Position: (" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
}
