#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader_m.h>

#include <cstdio>
#include <iostream>
#include "../lib/model.hpp"
#include "../lib/object.hpp"

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window, object_t Objects[], double &old_time, int &activeObject);

// !DEBUG
unsigned int getError_from_GL()
{
    return glGetError();
}
// !DEBUG

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1000;

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
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "UTS", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::cerr << "enable gl & load: " << glGetError() << "\n";
    // build and compile our shader zprogram
    // ------------------------------------
    Shader ourShader("vs.cu", "fs.cu");
    std::cerr << "shader: " << glGetError() << "\n";
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    const int numModels{7};
    model_t *Models[numModels];
    char tmpstr[2][50]{"", ""};
    for (int i = 0; i < numModels; ++i)
    {
        std::sprintf(tmpstr[0], "../Models/model%d.obj", i);
        std::sprintf(tmpstr[1], "../Models/model%d.png", i);
        Models[i] = new model_t{tmpstr[0], tmpstr[1]}; //allocate a new model object to the array
        glGenVertexArrays(1, &((*(Models[i])).VAO));
        glGenBuffers(1, &((*(Models[i])).VBO));
        glGenBuffers(1, &((*(Models[i])).EBO));

        glBindVertexArray((*(Models[i])).VAO);

        glBindBuffer(GL_ARRAY_BUFFER, (*(Models[i])).VBO);
        glBufferData(GL_ARRAY_BUFFER, (*(Models[i])).num_vertices * 8 * sizeof(float), (*(Models[i])).vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*(Models[i])).EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, (*(Models[i])).num_faces * 3 * sizeof(unsigned int), (*(Models[i])).indeces, GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        // color attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        // texture coord attribute
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        // load texture to gl
        glGenTextures(1, &((*(Models[i])).VTO));
        glBindTexture(GL_TEXTURE_2D, (*(Models[i])).VTO); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        // load image from texture file address
        if ((*(Models[i])).m_t_data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (*(Models[i])).m_width, (*(Models[i])).m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (*(Models[i])).m_t_data);
            glGenerateMipmap(GL_TEXTURE_2D);
            if (i == 6)
            {
                for (int j = 161; j < 171; ++j)
                {
                    unsigned char *offset = (*(Models[i])).m_t_data + (738 + (*(Models[i])).m_height * j) * (*(Models[i])).m_nrChannels;
                    std::cout << "pixel data: r=" << static_cast<int>(offset[0]) << " b=" << static_cast<int>(offset[1]) << " g=" << static_cast<int>(offset[2]) << " a=" << static_cast<int>(offset[3]) << "\n";
                }
            }
        }
        else
        {
            std::cerr << "Failed to load texture"
                      << "\n";
            exit(1);
        }
        std::cerr << "loading model(" << i << "): " << glGetError() << "\n";
    }
    std::cerr << "model loading: " << glGetError() << "\n";

    // loop vars
    const int numObjects{8};
    object_t Objects[numObjects]{{Models[0], glm::vec3(0.0f, 0.0f, 0.0f)}, {Models[1], glm::vec3(2.0f, 5.0f, -11.0f)}, {Models[2], glm::vec3(-1.5f, -2.2f, -2.5f)}, {Models[3], glm::vec3(0.5f, 2.2f, -5.5f)}, {Models[0], glm::vec3(-3.8f, -2.0f, -8.3f)}, {Models[4], glm::vec3(1.5f, 2.2f, -2.5f)}, {Models[5], glm::vec3(-15.0f, 3.2f, -5.5f)}, {Models[6], glm::vec3(-3.5f, 10.2f, -25.5f)}};
    double old_time{glfwGetTime()}, fps_timer{glfwGetTime()}, rot_timer[numObjects]{glfwGetTime()}, rot_time[numObjects]{glfwGetTime()};
    int fps_count{}, activeObject{3}, old_active{3};
    unsigned int err{glGetError()};
    bool activeTimer_started{false};
    for (int i = 0; i < numObjects; ++i)
    {
        float angle = 20.0f * static_cast<float>(i);
        Objects[i].rotate_to(glm::radians(angle) + glm::radians(static_cast<glm::f32>(glfwGetTime())), glm::vec3(1.0f, 0.3f, 0.5f));
    }
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window, Objects, old_time, activeObject);
        if (activeObject != old_active)
        {
            rot_time[old_active] -= (glfwGetTime() - rot_timer[old_active]);
            activeTimer_started = false;
            old_active = activeObject;
        }
        else
        {
            rot_timer[old_active] = glfwGetTime();
            activeTimer_started = true;
        }
        // render
        // ------
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        if ((err = glGetError()))
            std::cout << "\n\nglClearColor(0.2f, 0.2f, 0.2f, 1.0f): " << err << "\n\n";
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!
        if ((err = glGetError()))
            std::cout << "\n\nglClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT): " << err << "\n\n";
        // activate shader
        ourShader.use();
        // create transformations
        glm::mat4 view = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(95.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        // pass transformation matrices to the shader
        ourShader.setMat4("projection", projection); // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
        if ((err = glGetError()))
            std::cout << "\n\nourShader.setMat4(\"projection\", projection): " << err << "\n\n";
        ourShader.setMat4("view", view);
        if ((err = glGetError()))
            std::cout << "\n\nourShader.setMat4(\"view\", view): " << err << "\n\n";
        // render models
        for (int i = 0; i < numObjects; i++)
        {
            glBindVertexArray((Objects[i].m_model->VAO));
            if ((err = glGetError()))
                std::cout << "\n\nglBindVertexArray((Objects[" << i << "].m_model->VAO)): " << err << "\n\n";
            glBindTexture(GL_TEXTURE_2D, (Objects[i].m_model->VTO));
            if ((err = glGetError()))
                std::cout << "\n\nglBindTexture(GL_TEXTURE_2D, (Objects[" << i << "].m_model->VTO)): " << err << "\n\n";
            // calculate the model matrix for each object and pass it to shader before drawing
            // rotate models not currently controled by the user

            if (i != activeObject)
            {
                rot_time[i] = glfwGetTime();
                if (fabs(rot_time[i] - rot_timer[i]) > 0.001)
                {
                    Objects[i].rotate_by(glm::radians(static_cast<glm::f32>((rot_time[i] - rot_timer[i]) * 100.0)), glm::vec3(1.0f, 0.3f * i, 0.5f));
                    rot_timer[i] = rot_time[i];
                }
            }
            ourShader.setMat4("model", Objects[i].get_model_mat());
            if ((err = glGetError()))
                std::cout << "\n\nourShader.setMat4(\"model\", Objects[" << i << "].get_model_mat()): " << err << "\n\n";

            glDrawElements(GL_TRIANGLES, 3 * (Objects[i].m_model->num_faces), GL_UNSIGNED_INT, 0);
            if ((err = glGetError()))
                std::cout << "\n\nglDrawElements(GL_TRIANGLES, 3 * (Objects[" << i << "].m_model->num_faces), GL_UNSIGNED_INT, 0): " << err << "\n\n";
        }
        // Catch errors
        if (glfwGetTime() - fps_timer >= 2.0)
        {
            std::cout << "Active Object: " << activeObject << "\n"
                      << "Postion: " << Objects[activeObject].get_pos().x << " " << Objects[activeObject].get_pos().y << " " << Objects[activeObject].get_pos().z << " \n"
                      << "Draw rate: " << fps_count << " glError: (" << glGetError() << ") "
                      << "\n\n";
            fps_count = 0;
            fps_timer = glfwGetTime();
        }
        ++fps_count;
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------

    for (int i = 0; i < numModels; ++i)
    {
        glDeleteVertexArrays(1, &((*(Models[i])).VAO));
        glDeleteBuffers(1, &((*(Models[i])).VBO));
        glDeleteBuffers(1, &((*(Models[i])).EBO));
        glDeleteTextures(1, &((*(Models[i])).VTO));
        delete Models[i];
    }
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    const char *str;
    glfwGetError(&str);
    std::cout << str << "\n";
    delete str;
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window, object_t Objects[], double &old_time, int &activeObject)
{
    // Set active model
    // ================
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    {
        activeObject = 0;
    }
    else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
    {
        activeObject = 1;
    }
    else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
    {
        activeObject = 2;
    }
    else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
    {
        activeObject = 3;
    }
    else if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
    {
        activeObject = 4;
    }
    else if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
    {
        activeObject = 5;
    }
    else if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
    {
        activeObject = 6;
    }
    else if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS)
    {
        activeObject = 7;
    }

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetTime() - old_time > 1.0 / 60.0)
    {
        // Rotate
        // ======
        // z-axis
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        {
            Objects[activeObject].rotate_by(glm::radians(2.0f), glm::vec3(0.0, 0.0, -1.0));
        }
        else if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
        {
            Objects[activeObject].rotate_by(glm::radians(2.0f), glm::vec3(0.0, 0.0, 1.0));
        }
        // y-axis
        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
        {
            Objects[activeObject].rotate_by(glm::radians(2.0f), glm::vec3(0.0, -1.0, 0.0));
        }
        else if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
        {
            Objects[activeObject].rotate_by(glm::radians(2.0f), glm::vec3(0.0, 1.0, 0.0));
        }
        // x-axis
        if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
        {
            Objects[activeObject].rotate_by(glm::radians(2.0f), glm::vec3(-1.0, 0.0, 0.0));
        }
        else if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
        {
            Objects[activeObject].rotate_by(glm::radians(2.0f), glm::vec3(1.0, 0.0, 0.0));
        }
        // Scale
        // =====
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            Objects[activeObject].scale_by(glm::vec3(1.2, 1.2, 1.2));
        }
        else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            Objects[activeObject].scale_by(glm::vec3(0.8, 0.8, 0.8));
        }
        // Translate
        // =========
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        {
            Objects[activeObject].move_by(glm::vec3(0.0, 0.1, 0.0));
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            Objects[activeObject].move_by(glm::vec3(0.0, -0.1, 0.0));
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            Objects[activeObject].move_by(glm::vec3(-0.1, 0.0, 0.0));
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            Objects[activeObject].move_by(glm::vec3(0.1, 0.0, 0.0));
        }
        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
        {
            Objects[activeObject].move_by(glm::vec3(0.0, 0.0, 0.1));
        }
        if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        {
            Objects[activeObject].move_by(glm::vec3(0.0, 0.0, -0.1));
        }
        old_time = glfwGetTime();
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
