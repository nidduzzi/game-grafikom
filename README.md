# game-grafikom

opengl car obstacle game  

## How to use model_t class

include file: `model.h`  

For uniform initialization:  
`model_t foo{"path/to/model.obj", "path/to/texture.jpg"};`  

For copy initialization:  
`model_t foo = model_t("path/to/model.obj", "path/to/texture.jpg");`  

model_t class attributes:  
texture stbi data: `unsigned char *m_t_data`  
vertices data (x0, y0, z0, u0, v0, ...): `float *vertices`  
indeces data (triangles): `unsigned int *indeces`  
number of vertecies: `int num_vertices`  
number of indeces: `int num_faces`  
Vertex Array Object binder: `unsigned int VAO`  
Vertex Buffer Object binder: `unsigned int VBO`  
indeces buffer binder: `unsigned int EBO`  
Texture binder: `unsigned int VTO`  
Texture height: `int m_height`  
Texture width: `int m_width`  
Texture nrChannels: `int m_nrChannels`  

model_t class methods:  
WARNING: Some methods that needs to be private still are exposed
`void print()`: prints atributes head to stdout  
`void makeVerticesIndeces()` WARNING, do not call this: builds `vertices`, `indeces`, and their sepective nums  
`model_t(std::string model_address, std::string texture_address)`: constructor  
