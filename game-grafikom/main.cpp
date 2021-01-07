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
int activeCam{0};
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window, std::vector<object_t> &Objects, double &old_time, int &activeObject, int &activeCam);
void toggle_key(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    static bool c_pressed;
    if(key == GLFW_KEY_C && action == GLFW_PRESS && !c_pressed)
    {
        activeCam = (activeCam + 1) % 2;
        c_pressed = !c_pressed;
    }
    else c_pressed = !c_pressed;
}
// settings
unsigned int SCR_WIDTH = 1000;
unsigned int SCR_HEIGHT = 1000;

int main()
{
    // pre-load models
    const int numModels{7};
    model_t *Models[numModels];
    { //localize variable scope
        char tmpstr[2][50]{"", ""};
        for (int i = 0; i < numModels; ++i)
        {
            std::sprintf(tmpstr[0], "../Models/model%d.obj", i);
            std::sprintf(tmpstr[1], "../Models/model%d.png", i);
            Models[i] = new model_t{tmpstr[0], tmpstr[1]}; //allocate a new model object to the array
        }
    }
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
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_FALSE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    // glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "UAS Grafikom", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, toggle_key);
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    std::cerr << "glEnable & gladLoad: " << glGetError() << "\n";
    // build and compile our shader zprogram
    // ------------------------------------
    Shader ourShader("vs.cu", "fs.cu");
    std::cerr << "shader: " << glGetError() << "\n";
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    for (int i = 0; i < numModels; ++i)
    {
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
    std::vector<object_t> Objects{
        {Models[1], glm::vec3{0.0f, 0.0f, 0.0f}},    // Mobil item
        {Models[6], glm::vec3{-2.0f, 0.0f, -10.0f}}, // apel
        {Models[5], glm::vec3{5.5f, 0.0f, 10.0f}},   // tamia
        {Models[4], glm::vec3{8.0f, 0.0f, 8.5f}},    // Mobil tua
        {Models[3], glm::vec3{-6.0f, 0.0f, -8.5f}},  // Badak
        {Models[2], glm::vec3{3.0f, 0.3f, -5.0f}}    // Kursi

    };
    const int arena_width = 20;
    const int arena_length = 20;
    const float FOV = 45.0f;
    std::vector<object_t> Asphalt{};
    for (int i = -(arena_width / 2 + arena_width % 2); i < arena_width / 2; ++i)
    {
        for (int j = -(arena_length / 2 + arena_length % 2); j < arena_length / 2; ++j)
            Asphalt.push_back(object_t{Models[0], glm::vec3{2.0f * static_cast<float>(i) + 1.0f, -0.505f, 2.0f * static_cast<float>(j) + 1.0f}});
    }
    glm::mat4 eagle_view{glm::lookAt(glm::vec3{0.0f, std::max(arena_length, arena_width) / (std::tan(glm::radians(FOV / 2.0f))), 0.0f}, glm::vec3{}, glm::vec3{0.0, 0.0, 1.0})};
    const std::size_t numObjects{Objects.size()};
    double old_time{glfwGetTime()}, fps_timer{glfwGetTime()};
    int fps_count{}, activeObject{0}, numCams{2};
    unsigned int err{glGetError()};

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window, Objects, old_time, activeObject, activeCam);
        // render
        // ------
        glClearColor(191.0f / 255.0f, 224.0f / 255.0f, 1.0f, 1.0f);

        if ((err = glGetError()))
            std::cerr << "\n\nglClearColor(191.0f / 255.0f, 224.0f / 255.0f, 1.0f, 1.0f): " << err << "\n\n";
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!
        if ((err = glGetError()))
            std::cerr << "\n\nglClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT): " << err << "\n\n";

        // activate shader
        ourShader.use();
        // create transformations
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(FOV), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view{};
        if (activeCam == 0)
            view = Objects[activeObject].get_chase_cam_view_mat();
        else
            view = eagle_view;

        // pass transformation matrices to the shader
        ourShader.setMat4("projection", projection); // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
        if ((err = glGetError()))
            std::cerr << "\n\nourShader.setMat4(\"projection\", projection): " << err << "\n\n";
        ourShader.setMat4("view", view);
        if ((err = glGetError()))
            std::cerr << "\n\nourShader.setMat4(\"view\", view): " << err << "\n\n";
        // render models
        for (auto i : Asphalt)
        {
            glBindVertexArray((i.m_model->VAO));

            if ((err = glGetError()))
                std::cerr << "\n\nglBindVertexArray((i.m_model->VAO)): " << err << "\n\n";
            glBindTexture(GL_TEXTURE_2D, (i.m_model->VTO));
            if ((err = glGetError()))
                std::cerr << "\n\nglBindTexture(GL_TEXTURE_2D, (i.m_model->VTO)): " << err << "\n\n";

            // calculate the model matrix for each object and pass it to shader before drawing
            // rotate models not currently controled by the user
            ourShader.setMat4("model", i.get_model_mat());
            if ((err = glGetError()))
                std::cerr << "\n\nourShader.setMat4(\"model\", i.get_model_mat()): " << err << "\n\n";

            glDrawElements(GL_TRIANGLES, 3 * (i.m_model->num_faces), GL_UNSIGNED_INT, 0);
            if ((err = glGetError()))
                std::cerr << "\n\nglDrawElements(GL_TRIANGLES, 3 * (i.m_model->num_faces), GL_UNSIGNED_INT, 0): " << err << "\n\n";
        }
        for (std::size_t i = 0; i < numObjects; i++)
        {
            glBindVertexArray((Objects[i].m_model->VAO));

            if ((err = glGetError()))
                std::cerr << "\n\nglBindVertexArray((Objects[" << i << "].m_model->VAO)): " << err << "\n\n";
            glBindTexture(GL_TEXTURE_2D, (Objects[i].m_model->VTO));
            if ((err = glGetError()))
                std::cerr << "\n\nglBindTexture(GL_TEXTURE_2D, (Objects[" << i << "].m_model->VTO)): " << err << "\n\n";

            // calculate the model matrix for each object and pass it to shader before drawing
            // rotate models not currently controled by the user
            ourShader.setMat4("model", Objects[i].get_model_mat());
            if ((err = glGetError()))
                std::cerr << "\n\nourShader.setMat4(\"model\", Objects[" << i << "].get_model_mat()): " << err << "\n\n";

            glDrawElements(GL_TRIANGLES, 3 * (Objects[i].m_model->num_faces), GL_UNSIGNED_INT, 0);
            if ((err = glGetError()))
                std::cerr << "\n\nglDrawElements(GL_TRIANGLES, 3 * (Objects[" << i << "].m_model->num_faces), GL_UNSIGNED_INT, 0): " << err << "\n\n";
        }

        // Catch errors
        if (glfwGetTime() - fps_timer >= 2.0)
        {
            glm::vec3 pos{Objects[activeObject].get_pos()}, axis{Objects[activeObject].get_axis()};
            float angle{Objects[activeObject].get_angle()};
            std::cout << "Active Object: " << activeObject << "\n"
                      << "Postion: " << pos.x << " " << pos.y << " " << pos.z << " \n"
                      << "Angle: " << angle << " Axis: " << axis.x << " " << axis.y << " " << axis.z << " \n"
                      << "Draw rate: " << fps_count;
            if ((err = glGetError()))
                std::cout << " glError: (" << err << ") ";
            std::cout << "\n\n";
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
void processInput(GLFWwindow *window, std::vector<object_t> &Objects, double &old_time, int &activeObject, int &activeCam)
{
    // Set active model
    // ================
    if ((Objects.size() > 0) && (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS))
    {
        activeObject = 0;
    }
    else if ((Objects.size() > 1) && (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS))
    {
        activeObject = 1;
    }
    else if ((Objects.size() > 2) && (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS))
    {
        activeObject = 2;
    }
    else if ((Objects.size() > 3) && (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS))
    {
        activeObject = 3;
    }
    else if ((Objects.size() > 4) && (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS))
    {
        activeObject = 4;
    }
    else if ((Objects.size() > 5) && (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS))
    {
        activeObject = 5;
    }
    else if ((Objects.size() > 6) && (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS))
    {
        activeObject = 6;
    }
    else if ((Objects.size() > 7) && (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS))
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
        //ARROW LEFT RIGHT - swapped with  F G ----------------------------------

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
            Objects[activeObject].scale_by(glm::vec3(1.02, 1.02, 1.02));
        }
        else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            Objects[activeObject].scale_by(glm::vec3(0.98, 0.98, 0.98));
        }
        // Translate
        // =========
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        {
            if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            {
                Objects[activeObject].rotate_by(glm::radians(2.0f), glm::vec3(0.0, 1.0, 0.0));
            }
            else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            {
                Objects[activeObject].rotate_by(glm::radians(2.0f), glm::vec3(0.0, -1.0, 0.0));
            }
            Objects[activeObject].move_by(glm::vec3{Objects[activeObject].get_rotation_quat() * glm::vec3{0.0, 0.0, 0.2}});
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            {
                Objects[activeObject].rotate_by(glm::radians(2.0f), glm::vec3(0.0, -1.0, 0.0));
            }
            else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            {
                Objects[activeObject].rotate_by(glm::radians(2.0f), glm::vec3(0.0, 1.0, 0.0));
            }
            Objects[activeObject].move_by(glm::vec3{Objects[activeObject].get_rotation_quat() * glm::vec3{0.0, 0.0, -0.2}});
        }
        // if ((glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) && !camChanged)
        // {
        //     activeCam = (activeCam + 1) % 2;
        //     camChanged = !camChanged;
        // }
        // else if(glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE)
        //     camChanged = !camChanged;
        // else camChanged = false;
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
    if (width && height)
    {
        glfwSetWindowAspectRatio(window, width, height);
        SCR_HEIGHT = height;
        SCR_WIDTH = width;
    }
}
