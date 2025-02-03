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
Camera camera(glm::vec3(0.0f, 0.13f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//Estructura para Semi Esferas
struct SemiSphereCollider {
    glm::vec3 center;  // Centro de la semiesfera (x, y, z)
    float radius;      // Radio de la semiesfera
    float maxHeight;   // Altura máxima de la semiesfera
};

// Estructura para Bounding Box
struct BoundingBox {
    glm::vec3 min;
    glm::vec3 max;
};

// Coordenadas de choque
SemiSphereCollider skydomeCollider = {
    glm::vec3(0.0f, 0.0f, 0.0f),  // Centro del skydome
    19.3f,                        // Radio (basado en el scale de 20.0f)
    19.3f                         // Altura máxima hasta donde hay colisión
};

BoundingBox stadiumBoundingBox = {
    glm::vec3(-4.80f, 0.0f, -4.20f),   // min: Esquina inferior izquierda externa (muros)
    glm::vec3(4.80f, 1.70f, 8.10f)     // max: Esquina superior derecha externa (muros)
};

BoundingBox fieldBoundingBox = {
    glm::vec3(-1.75f, 0.0f, -1.30f),   // min: Esquina inferior izquierda interna (campo de juego)
    glm::vec3(1.90f, 1.70f, 5.20f)     // max: Esquina superior derecha interna (campo de juego)
};

//Función que verifica que esté dentro
bool checkSemiSphereCollision(glm::vec3 cameraPos, SemiSphereCollider collider) {
    float distanceXYZ = glm::distance(cameraPos, collider.center);  // Distancia real en 3D

    // La cámara debe estar dentro del radio, pero no debajo del suelo
    return (distanceXYZ <= collider.radius && cameraPos.y >= collider.center.y);
}

bool checkStadiumCollision(glm::vec3 cameraPos, BoundingBox bbox) {
    return (
        cameraPos.x >= bbox.min.x && cameraPos.x <= bbox.max.x &&  // Muros laterales
        cameraPos.z >= bbox.min.z && cameraPos.z <= bbox.max.z &&  // Muros frontales/traseros
        cameraPos.y >= bbox.min.y                                   // No bloquea la entrada por arriba
    );
}

bool checkFieldCollision(glm::vec3 cameraPos, BoundingBox bbox) {
    return (
        cameraPos.x >= bbox.min.x && cameraPos.x <= bbox.max.x &&  // Límites del campo
        cameraPos.z >= bbox.min.z && cameraPos.z <= bbox.max.z &&  // Límites del campo
        cameraPos.y >= bbox.min.y && cameraPos.y <= bbox.max.y     // Si está dentro del campo y debajo de la altura
    );
}



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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Examen Bimestral 2", NULL, NULL);
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
    Model ourModel("model/stadium/stadium2.obj");
    Model messiModel("model/messi/messi.obj");
    Model skydomModel("model/skydom/skydom.obj");
    Model fireworkModel("model/firework1/firework1.obj");
    Model fireworkModel2("model/firework2/firework2.obj");
    Model fireworkModel3("model/firework3/firework3.obj");
    Model fireworkModel4("model/firework4/firework4.obj");
    Model fireworkModel5("model/firework5/firework1.obj");
    Model terrenoModel("model/terreno/terreno.obj");
    Model balonModel("model/balon/balon.obj");
    Model copaModel("model/copa/copa.obj");
    Model moonModel("model/moon/moon.obj");


    // draw in wireframe

    camera.MovementSpeed = 3; //Optional. Modify the speed of the camera

    //dato camra position
    float lastPrintTime = 0.0f;

    //Posiciones de los puntos de luz de la luna
    glm::vec3 pointLightPosition = glm::vec3(10.0f, 10.0f, 1.0f);

    // Reflectores del estadio
    glm::vec3 reflectors[] = {
        glm::vec3(1.90f, 1.22f, 5.20f),
        glm::vec3(1.90f, 1.22f, -1.30f),
        glm::vec3(-1.75f, 1.22f, 5.20f),
        glm::vec3(-1.75f, 1.22f, -1.30f),
        glm::vec3(0.0f, 1.22f, 5.20f),
        glm::vec3(0.0f, 1.22f, -1.30f)
    };

    //Cargando shader
    ourShader.use();
    ourShader.setInt("material.diffuse", 0);
    ourShader.setInt("material.specular", 1);

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
        ourShader.setVec3("viewPos", camera.Position);
        ourShader.setFloat("material.shininess", 45.0f);

        // point light - luna
        ourShader.setVec3("pointLight.position", pointLightPosition);
        ourShader.setVec3("pointLight.ambient", 2.0f, 1.5f, 1.0f);
        ourShader.setVec3("pointLight.diffuse", 1.0f, 0.9f, 0.45f);
        ourShader.setVec3("pointLight.specular", 1.5f, 1.8f, 0.75f);
        ourShader.setFloat("pointLight.constant", 3.0f); // Factor de atenuacion constante de la luz: no cambia con la distancia.
        ourShader.setFloat("pointLight.linear", 0.09); //// Factor de atenuacion lineal de la luz: atenua la luz proporcionalmente a la distancia.
        ourShader.setFloat("pointLight.quadratic", 0.032); //// Factor de atenuacion cuadratica de la luz: atenua la luz de forma cuadratica con la distancia.

        ourShader.setVec3("stadiumPointLight0.position", reflectors[0]);
        ourShader.setVec3("stadiumPointLight0.ambient", 0.1f, 0.1f, 0.1f);
        ourShader.setVec3("stadiumPointLight0.diffuse", 0.4f, 0.4f, 0.4f);
        ourShader.setVec3("stadiumPointLight0.specular", 1.0f, 1.0f, 1.0f);
        ourShader.setFloat("stadiumPointLight0.constant", 1.0f);
        ourShader.setFloat("stadiumPointLight0.linear", 0.09f);
        ourShader.setFloat("stadiumPointLight0.quadratic", 0.032f);

        ourShader.setVec3("stadiumPointLight1.position", reflectors[1]);
        ourShader.setVec3("stadiumPointLight1.ambient", 0.1f, 0.1f, 0.1f);
        ourShader.setVec3("stadiumPointLight1.diffuse", 0.4f, 0.4f, 0.4f);
        ourShader.setVec3("stadiumPointLight1.specular", 1.0f, 1.0f, 1.0f);
        ourShader.setFloat("stadiumPointLight1.constant", 1.0f);
        ourShader.setFloat("stadiumPointLight1.linear", 0.09f);
        ourShader.setFloat("stadiumPointLight1.quadratic", 0.032f);

        ourShader.setVec3("stadiumPointLight2.position", reflectors[2]);
        ourShader.setVec3("stadiumPointLight2.ambient", 0.1f, 0.1f, 0.1f);
        ourShader.setVec3("stadiumPointLight2.diffuse", 0.4f, 0.4f, 0.4f);
        ourShader.setVec3("stadiumPointLight2.specular", 1.0f, 1.0f, 1.0f);
        ourShader.setFloat("stadiumPointLight2.constant", 1.0f);
        ourShader.setFloat("stadiumPointLight2.linear", 0.09f);
        ourShader.setFloat("stadiumPointLight2.quadratic", 0.032f);

        ourShader.setVec3("stadiumPointLight3.position", reflectors[3]);
        ourShader.setVec3("stadiumPointLight3.ambient", 0.1f, 0.1f, 0.1f);
        ourShader.setVec3("stadiumPointLight3.diffuse", 0.4f, 0.4f, 0.4f);
        ourShader.setVec3("stadiumPointLight3.specular", 1.0f, 1.0f, 1.0f);
        ourShader.setFloat("stadiumPointLight3.constant", 1.0f);
        ourShader.setFloat("stadiumPointLight3.linear", 0.09f);
        ourShader.setFloat("stadiumPointLight3.quadratic", 0.032f);

        ourShader.setVec3("stadiumPointLight4.position", reflectors[4]);
        ourShader.setVec3("stadiumPointLight4.ambient", 0.1f, 0.1f, 0.1f);
        ourShader.setVec3("stadiumPointLight4.diffuse", 0.4f, 0.4f, 0.4f);
        ourShader.setVec3("stadiumPointLight4.specular", 1.0f, 1.0f, 1.0f);
        ourShader.setFloat("stadiumPointLight4.constant", 1.0f);
        ourShader.setFloat("stadiumPointLight4.linear", 0.09f);
        ourShader.setFloat("stadiumPointLight4.quadratic", 0.032f);

        ourShader.setVec3("stadiumPointLight5.position", reflectors[5]);
        ourShader.setVec3("stadiumPointLight5.ambient", 0.1f, 0.1f, 0.1f);
        ourShader.setVec3("stadiumPointLight5.diffuse", 0.4f, 0.4f, 0.4f);
        ourShader.setVec3("stadiumPointLight5.specular", 1.0f, 1.0f, 1.0f);
        ourShader.setFloat("stadiumPointLight5.constant", 1.0f);
        ourShader.setFloat("stadiumPointLight5.linear", 0.09f);
        ourShader.setFloat("stadiumPointLight5.quadratic", 0.032f);


        ourShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        ourShader.setVec3("spotLight.diffuse", 0.0f, 0.0f, 0.0f);
        ourShader.setVec3("spotLight.specular", 0.0f, 0.0f, 0.0f);
        ourShader.setFloat("spotLight.constant", 0.5f); //Atenuacion de la luz  
        ourShader.setFloat("spotLight.linear", 0.05);
        ourShader.setFloat("spotLight.quadratic", 0.5);
    

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

        static double startTime = 0;
static bool activated = false;

double currentTime = glfwGetTime();
double elapsedTime = currentTime - startTime;

if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && !activated) {
    activated = true;
    startTime = glfwGetTime();
}

if (activated) {
    float initialSize = 0.1f;  // Tamaño inicial de los modelos

    if (elapsedTime >= 0 && elapsedTime < 4) {
        float maxTime = 4.0f;  // Duración de la fase
        float startOfPhase = 0.0f;
        float delays[5] = { 0.0f, 0.9f, 1.6f, 2.9f, 4.2f };
        glm::vec3 positions[5] = {
            glm::vec3(-3.66f, 3.03f, 4.48f),
            glm::vec3(3.66f, 3.03f, 4.48f),
            glm::vec3(3.66f, 3.03f, 0.15f),
            glm::vec3(-3.66f, 3.03f, 0.15f),
            glm::vec3(0.66769f, 3.03, 2.055f)
        };

        Model fireworkModels[5] = { fireworkModel, fireworkModel2, fireworkModel3, fireworkModel4, fireworkModel5 };

        for (int i = 0; i < 5; i++) {
            if (elapsedTime > delays[i]) {
                float adjustedTime = elapsedTime - startOfPhase - delays[i];
                float scaleFactor = initialSize + (adjustedTime / maxTime) * 0.2f; // Ahora crece correctamente

                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, positions[i]);
                model = glm::scale(model, glm::vec3(scaleFactor));

                ourShader.setMat4("model", model);
                fireworkModels[i].Draw(ourShader);
            }
        }
        ourShader.setVec3("spotLight.ambient", 1.0f, 0.0f, 0.0f);
    }

    else if (elapsedTime >= 4 && elapsedTime < 8) {
        float maxTime = 4.0f;
        float startOfPhase = 4.0f;
        float delays[5] = { 0.0f, 0.9f, 1.6f, 2.9f, 4.2f };
        glm::vec3 positions[5] = {
            glm::vec3(2.66f, 3.03f, 3.48f),
            glm::vec3(2.66f, 3.03f, 1.15f),
            glm::vec3(-2.66f, 3.03f, 3.48f),
            glm::vec3(-2.66f, 3.03f, 1.15f),
            glm::vec3(0.66769f, 3.03, 2.055f)
        };

        Model fireworkModels[5] = { fireworkModel, fireworkModel2, fireworkModel3, fireworkModel4, fireworkModel5 };

        for (int i = 0; i < 5; i++) {
            if (elapsedTime > delays[i]) {
                float adjustedTime = elapsedTime - startOfPhase - delays[i];
                float scaleFactor = initialSize + (adjustedTime / maxTime) * 0.2f;

                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, positions[i]);
                model = glm::scale(model, glm::vec3(scaleFactor));

                ourShader.setMat4("model", model);
                fireworkModels[i].Draw(ourShader);
            }
        }
    }

    else if (elapsedTime >= 8 && elapsedTime < 12) {
        float maxTime = 4.0f;
        float startOfPhase = 8.0f;
        float delays[5] = { 0.0f, 0.9f, 1.6f, 2.9f, 4.2f };
        glm::vec3 positions[5] = {
            glm::vec3(0.66769f, 3.03, 2.055f),
            glm::vec3(3.66f, 3.03f, 0.15f),
            glm::vec3(-3.66f, 3.03f, 0.15f),
            glm::vec3(-3.66f, 3.03f, 4.48f),
            glm::vec3(3.66f, 3.03f, 4.48f)
        };

        Model fireworkModels[5] = { fireworkModel, fireworkModel2, fireworkModel3, fireworkModel4, fireworkModel5 };

        for (int i = 0; i < 5; i++) {
            if (elapsedTime > delays[i]) {
                float adjustedTime = elapsedTime - startOfPhase - delays[i];
                float scaleFactor = initialSize + (adjustedTime / maxTime) * 0.2f;

                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, positions[i]);
                model = glm::scale(model, glm::vec3(scaleFactor));

                ourShader.setMat4("model", model);
                fireworkModels[i].Draw(ourShader);
            }
        }
    }
    else {
        activated = false; // Desactiva la animación
    }
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

        //Luna
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(10.0f, 10.0f, 1.0f)); //en lo alto, por eso se cambia y a 10
        model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
        ourShader.setMat4("model", model);
        moonModel.Draw(ourShader);

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

    // Obtenemos la siguiente posición iniciando con el valor actual
    glm::vec3 nextPosition = camera.Position;

    //Movimiento en distintas direcciones sin cambiar la altura
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        glm::vec3 forward = glm::normalize(glm::vec3(camera.Front.x, 0.0f, camera.Front.z)); // Ignorar la componente Y
        nextPosition += forward * (camera.MovementSpeed * deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        glm::vec3 forward = glm::normalize(glm::vec3(camera.Front.x, 0.0f, camera.Front.z)); // Ignorar la componente Y
        nextPosition -= forward * (camera.MovementSpeed * deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        glm::vec3 right = glm::normalize(glm::cross(camera.Front, glm::vec3(0.0f, 1.0f, 0.0f))); // Solo XZ
        nextPosition -= right * (camera.MovementSpeed * deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        glm::vec3 right = glm::normalize(glm::cross(camera.Front, glm::vec3(0.0f, 1.0f, 0.0f))); // Solo XZ
        nextPosition += right * (camera.MovementSpeed * deltaTime);
    }


    // Agregar movimiento arriba/abajo con Espacio y Ctrl izquierdo
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        nextPosition += glm::vec3(0.0f, camera.MovementSpeed * deltaTime, 0.0f); // Sube
    }

      // Si la altura es superior a 0.5 bajar, caso contrario no
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    {
        if(nextPosition.y > 0.12)
        nextPosition.y -= camera.MovementSpeed * deltaTime;
    }

    //Si está dentro de la esfera comprobar la colisión con el estadio
    if (checkSemiSphereCollision(nextPosition, skydomeCollider)) {

        // Comprobar que este dentro del estadio
        if (checkStadiumCollision(nextPosition, stadiumBoundingBox)) {

            // Si está dentro del estadio comprobar si está dentro de la parte "mobil"
            if (checkFieldCollision(nextPosition, fieldBoundingBox)) {
                camera.Position = nextPosition;  // Solo puede moverse dentro del campo
            }
            // Si está sobre el campo, puede moverse libremente dentro del estadio
            else if (nextPosition.y > fieldBoundingBox.max.y) {
                camera.Position = nextPosition;
            }
        }
        else {
            camera.Position = nextPosition;
        }
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
