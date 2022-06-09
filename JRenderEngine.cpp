#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"
#include "model.h"
#include "skybox.h"
#include "BRDFLUT.h"
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

void drawLight(Shader &shader);
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 30.0f, 0.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
// lights
// ------
glm::vec3 lightPositions[] = {
    glm::vec3(0.0, 30.0, 0),
};
glm::vec3 lightColors[] = {
    glm::vec3(10.0f, 10.0f, 10.0f),
};
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
  GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
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
  stbi_set_flip_vertically_on_load(true);

  // configure global opengl state
  // -----------------------------
  glEnable(GL_DEPTH_TEST);glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
  // build and compile shaders
  // -------------------------
  BRDFLUT LUT(512);LUT.SetupGL();LUT.CalculateLUT();
  Shader modelPBRShader("./shaders/1.model_loading.vert", "./shaders/1.model_loading.frag");
  Shader lightShader("./shaders/light.vert","./shaders/light.frag");
  // load models
  // -----------
  Model sceneModel("./models/sponza/Sponza.gltf");
//  Model sceneModel("./models/DamagedHelmet/DamagedHelmet.gltf");
//  Model sceneModel("./models/MetalRoughSpheres/MetalRoughSpheres.gltf");
  sceneModel.SetupGL();
  shared_ptr<CubeMap> skyboxCubeMap=make_shared<CubeMap>(1024);
  skyboxCubeMap->SetupGL();
  skyboxCubeMap->GenerateFromHDRTex("./skyboxes/Barcelona_Rooftops/Barce_Rooftop_C_3k.hdr");
  glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxCubeMap->id);
  glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

  // calculate diffuse irrandiance map from skybox
  shared_ptr<CubeMap> diffuseIrradiance=make_shared<CubeMap>(32);
  diffuseIrradiance->SetupGL();
  diffuseIrradiance->DiffuseIrradianceCalFrom(skyboxCubeMap);

  shared_ptr<CubeMap> prefilterMap=make_shared<CubeMap>(128);
  prefilterMap->SetupGL(true);
  prefilterMap->PrefilterEnvMap(skyboxCubeMap);

  Skybox skybox(skyboxCubeMap);


  int scrWidth, scrHeight;
  glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
  glViewport(0, 0, scrWidth, scrHeight);
  // render loop
  // -----------
  while (!glfwWindowShouldClose(window))
  {
    // per-frame time logic
    // --------------------
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // input
    // -----
    processInput(window);

    // render
    // ------
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // view/projection transformations
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
    glm::mat4 view = camera.GetViewMatrix();
    // don't forget to enable shader before setting uniforms
    modelPBRShader.use();
    modelPBRShader.setVec3("camPos", camera.Position);
    for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i) {
      modelPBRShader.setVec3("lightPositions[" + std::to_string(i) + "]", lightPositions[i]);
      modelPBRShader.setVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);
    }
    modelPBRShader.setMat4("projection", projection);
    modelPBRShader.setMat4("view", view);

    // render the loaded model
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
    model = glm::scale(model, glm::vec3(0.1F));	// it's a bit too big for our scene, so scale it down
    modelPBRShader.setMat4("model", model);
    sceneModel.Draw(modelPBRShader, diffuseIrradiance->id,prefilterMap->id,LUT.id);


    //render the lights
    drawLight(lightShader);

    skybox.Draw(projection,view);
    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    // -------------------------------------------------------------------------------
    glfwSwapBuffers(window);
    glfwPollEvents();
  }


  // glfw: terminate, clearing all previously allocated GLFW resources.
  // ------------------------------------------------------------------
  glfwTerminate();
  return 0;
}
void drawLight(Shader &shader) {
  glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
  glm::mat4 view = camera.GetViewMatrix();
  shader.use();
  shader.setMat4("projection", projection);
  shader.setMat4("view", view);
  shader.setVec3("camPos",camera.Position);
  // render the loaded model
  for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, lightPositions[i]); // translate it down so it's at the center of the scene
    model = glm::scale(model, glm::vec3(1.0f));	// it's a bit too big for our scene, so scale it down
    shader.setMat4("model", model);
    shader.setVec3("lightColor",lightColors[i]);
    glEnable(GL_CULL_FACE);
    Mesh::GetUnitSphereInstance()->Draw();
    // always good practice to set everything back to defaults once configured.
    glBindVertexArray(0);
  }
  glDisable(GL_CULL_FACE);

}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
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
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
  float xpos = static_cast<float>(xposIn);
  float ypos = static_cast<float>(yposIn);

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
  camera.ProcessMouseScroll(static_cast<float>(yoffset));
}