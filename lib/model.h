// Ahmad Izzuddin 1908919 C2 2019
// Fajar Zuliansyah Trihutama 1905394 C2 2019
// Muhammad Iqbal Zain 1901423 C2 2019
#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <bits/stdc++.h>
#include <fstream>
#include <sstream>
#include <iostream>

class model_t
{
private:
    std::vector<std::array<int, 9>> m_faces;
    std::vector<std::array<float, 2>> m_v_textures;
    std::vector<std::array<float, 3>> m_vertices;
    std::array<float, 3> m_color;

public:
    int m_height, m_width, m_nrChannels;
    unsigned char *m_t_data;
    // Gl binds
    float *vertices;
    unsigned int *indeces;
    int num_faces, num_vertices;
    unsigned int VAO, VBO, EBO, VTO;
    model_t(std::string model_address, std::string texture_address) : m_color{0.1f, 0.1f, 0.1f} /* Set model color*/, VAO{}, VBO{}, EBO{}, VTO{}
    {
        // TODO: make a constructor argument for color
        // Load texture
        // ============
        std::cout << "Loading texture file: " << texture_address.c_str() << "\n";
        m_t_data = stbi_load(texture_address.c_str(), &m_width, &m_height, &m_nrChannels, 0);

        // Load model from .obj file
        // =========================
        std::cout << "Loading model file: " << model_address.c_str() << "\n";
        if ((model_address.substr(model_address.size() - 4, model_address.size())).compare(".obj"))
        {
            std::cerr << "model file type not *.obj!\n";
            exit(1);
        }
        std::ifstream modelF{model_address};
        if (!modelF.good())
        {
            std::cerr << "Couldn't read model file!\n";
            exit(1);
        }
        float maxval = {}; //for normalizing the m_vertices vector
        while (!modelF.eof())
        {
            std::string line{};
            std::getline(modelF, line);
            // push the data into their respective vectors
            if ((line.substr(0, 2)).compare("v ") == 0)
            {
                std::array<float, 3> buffer{};
                // Create working copy of the line
                char *l{new (std::nothrow) char[line.size() + 1]{}};
                memcpy(l, line.c_str(), line.size() + 1);
                // Extract tokens upto first " "
                strtok(l, " "); // get first token eg. "v "
                for (int i = 0; i < 3; ++i)
                {
                    buffer[i] = atof(strtok(NULL, " "));
                    maxval = std::max(maxval, buffer[i]);
                }
                m_vertices.push_back(buffer);
                delete[] l;
            }
            else if ((line.substr(0, 2)).compare("vt") == 0)
            {
                std::array<float, 2> buffer{};
                // Create working copy of the line
                char *l{new (std::nothrow) char[line.size() + 1]{}};
                memcpy(l, line.c_str(), line.size() + 1);
                // Extract tokens upto first " "
                strtok(l, " "); // get first token eg. "v "
                for (int i = 0; i < 2; ++i)
                {
                    buffer[i] = atof(strtok(NULL, " "));
                }
                buffer[1] = 1 - buffer[1];
                m_v_textures.push_back(buffer);
                delete[] l;
            }
            else if ((line.substr(0, 2)).compare("f ") == 0)
            {
                std::array<int, 9> buffer{};
                // Create working copy of the line
                char *l{new (std::nothrow) char[line.size() + 1]{}};
                memcpy(l, line.c_str(), line.size() + 1);
                // Extract tokens upto first " "
                strtok(l, " "); // get first token eg. "v "
                for (int i = 0; i < 9; ++i)
                {
                    buffer[i] = atoi(strtok(NULL, " /"));
                }
                m_faces.push_back(buffer);
                delete[] l;
            }
        }
        // Normalize m_vertices
        for (int i = 0; i < static_cast<int>(m_vertices.size()); ++i)
        {
            for (int j = 0; j < 3; ++j)
                m_vertices[i][j] = m_vertices[i][j] / maxval;
        }
        modelF.close();
        this->makeVerticesIndeces();
    }

    void makeVerticesIndeces()
    {
        int len = static_cast<int>(m_faces.size());
        num_vertices = {static_cast<int>(m_vertices.size())}, num_faces = {len};
        std::vector<float> vertex{};
        vertex.resize(static_cast<int>(m_vertices.size()) * 8);
        std::vector<int> index{};
        index.resize(static_cast<int>(m_faces.size()) * 3);
        for (int i = 0; i < len; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                int vv = m_faces[i][j * 3] - 1;
                int vt = m_faces[i][j * 3 + 1] - 1;
                // assign vertex positions
                for (int k = 0; k < 3; ++k)
                {
                    vertex[vv * 8 + k] = m_vertices[vv][k];
                }
                // assign default color
                for (int k = 0; k < 3; ++k)
                {
                    vertex[vv * 8 + 3 + k] = m_color[k];
                }
                // assign texture coordinates
                for (int k = 0; k < 2; ++k)
                {
                    vertex[vv * 8 + 6 + k] = m_v_textures[vt][k];
                }
                // assign indices
                index[i * 3 + j] = vv;
            }
        }
        vertices = new float[static_cast<int>(vertex.size())]{};
        std::copy(vertex.begin(), vertex.end(), vertices);
        indeces = new unsigned int[static_cast<int>(index.size())]{};
        std::copy(index.begin(), index.end(), indeces);
    }

    ~model_t()
    {
        // deallocate from dynamic variables
        try
        {
           delete[] this->indeces;
           delete[] this->vertices;
        }
        catch(...)
        {
           std::cerr << "could't deallocate vertices and/or indeces\n";
        }
    }

    void print()
    {
        std::cout << "m_indicies: " << m_faces.size() << "\n";
        std::cout << "m_verticies: " << m_vertices.size() << "\n";
        std::cout << "m_v_textures: " << m_v_textures.size() << "\n";
        std::cout << "m_height: " << m_height << "\n";
        std::cout << "m_widht: " << m_width << "\n";
        std::cout << "m_nrChannels: " << m_nrChannels << "\n";
        std::cout << "num_faces: " << num_faces << "\n";
        std::cout << "num_vertices: " << num_vertices << "\n";
        std::cout << "vertices:\n";
        for (int i = 0; i < ((10 < num_vertices) ? 10 : num_vertices); ++i)
        {
            std::cout << vertices[i * 8] << ", " << vertices[i * 8 + 1] << ", " << vertices[i * 8 + 2] << "\n";
            std::cout << vertices[i * 8 + 3] << ", " << vertices[i * 8 + 4] << ", " << vertices[i * 8 + 5] << "\n";
            std::cout << vertices[i * 8 + 6] << ", " << vertices[i * 8 + 7] << "\n";
        }
        for (int i = 0; i < ((10 < num_faces) ? 10 : num_faces); ++i)
        {
            std::cout << indeces[i * 3] << ", " << indeces[i * 3 + 1] << ", " << indeces[i * 3 + 2] << "\n";
        }
    }
};
#endif
