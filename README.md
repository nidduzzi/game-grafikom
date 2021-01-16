# game-grafikom

opengl car obstacle game

Kelompok 7
Grafika Komputer Semrster Genap tahun 2020 - 2021
- Ahmad Izzuddin 		   1908919 
- Muhammad Iqbal Zain	   1901423
- Fajar Zuliansyah Trihutama 1905394
- Non Alyya Yan Hari 	   1804430
 
## PROGRAM DESCRIPTION

**(English)**  
Obstacle car is a simulation that uses mesh models from blender and uses opengl to simulate objects controlled by a user.

The mesh models used needs to be exported in the wavefront format (\*.obj) with triangulated faces. The model must put into the Models folder with the naming format "model<model number>.obj". In addition the model also needs a texture file in the same folder with the naming format "model<model number>.png". The program will first cunstruct `model_t` objects that will contain the required vertex, face, and texture data. Then this data is loaded into the GPU. The `model_t` object will then be used by `object_t` class for calculating the model and view matricies for cameras relative to the object (1st & 3rd person cameras).

**(Indonesian)**  
Obstacle car merupakan sebuah game simulasi yang menggunakan model dari blender dan untuk proses menggambar menggunakan opengl yang mensimulasikan benda-benda yang dapat dikontrol dan bergerak sesuai kehendak user.  

Mesh model sendiri perlu diexport dalam format .obj dan semua face sudah dijadikan triangle. Model harus memiliki nama file "model<nomor model>.obj" seperti (model0.obj). Selain itu, texture harus dalam format .png, dan memiliki nama file "model<nomor model>.png" seperti (model0.png). Program akan membuat objek `model_t` untuk setiap mesh model. Objek tersebut akan mengambil data vertex, elemen, texture coordinat, dan texture dari file model dan menyimpan data tersebut. Kemudian data akan diload ke opengl pada fungsi main karena masalah opengl dengan fungsi luar/ method object. Setelah itu posisi & transformasi instance `model_t` yang aktif akan diambil dari user, kemudian seluruh object digambarkan ke window oleh opengl. Sebuah object `model_t` dapat digunakan oleh class `object_t` menggunakan method dari class `object_t` yang akan menghasilkan matriks model dan matriks view utk kamera relatif terhadap objek (first person view & third person view).

## CONTROL : control yang dapat dilakukan oleh user sebagai berikut:  

### Select object  

- Model 1 (Mobil Hitam - default) : 1  
- Model 2 (Kursi)         	      : 2  
- Model 3 (Badak)		              : 3  
- Model 4 (Mobil Hijau)           : 4  
- Model 5 (Mobil Merah)           : 5  
- Model 6 (Apel)      		        : 6  

### Translate
Menggunakan arrow keys dan WSAD  
- Up/W    : menggerakan object kearah depan object tersebut.  
- Down/S  : menggerakan object kearah belakang(mundur) object tersebut.  
- Left/A  : Membuat object berbelok arah ke kiri.  
- Right/D : Membuat object belok ke arah kanan.  

### Scale

- Bigger : Q  
- Smaller: E  

### Rotate

- x-axis: V & B  
- z-axis: R & T  

### Point Of View
User dapat mengubah setting camera untuk mendapatkan point of view lain dengan menggunakan tombol C, point of view yang tersedia dalam game ini antara lain:
1. eagle eye, untuk melihat keseluruhan lingkungan dari game dari atas.
2. first person, untuk melihat keadaan lingkungan berdasarkan pandangan object yang sedang dikontrol(aktif)
3. thrid person, untuk melihat keadaan lingkungan berdasarkan pandangan lain selain object yang sedang aktif.

## How to use `model_t` class

include file: `model.hpp`  

model_t only accepts models from wavefront(.obj) files with the components vertex positions, vertex normals, texture cooredinates, and triangular faces.  
model_t only accepts textures from .png files with up to 16-bit channels.  

For uniform initialization:  
`model_t foo{"path/to/model.obj", "path/to/texture.png"};`  

For copy initialization:  
`model_t foo = model_t("path/to/model.obj", "path/to/texture.png");`  

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
`~model_t()`: deconstructor  

## How to use `object_t` class

include file: `object.hpp`  

constructor requres a pointer a `model_t` instance, and accepts initial transformations (position, rotatian, scaling, etc).

#### Transformation methods
`move_by()` method accepts `glm::vec3` or delta x, y, and z then translates relative to current position.  
`move_to()` method accepts `glm::vec3` or delta x, y, and z then translates relative to origin.  
`rotate_by()` method accepts euler and quaternion rotations in the form of axis and angle of rotation realtive ot current orientation.  
`rotate_to()` method accepts euler and quaternion rotations in the form of axis and angle of rotation relative to initial orientation.  
`scale_by()` method accepts `glm::vec3` or delta x, y, and z scaling factors then scales relative to the current scale.  
`scale_by()` method accepts `glm::vec3` or delta x, y, and z scaling factors then scales relative to the initial scale.  

#### Attribute retreival
`get_pos()`  returns a `glm::vec3` of object coordinates  
`get_axis()` returns a `glm::f32` angle in radians of the current rotation around an axis  
`get_axis()` returns a `glm::vec3` of object rotation axis    
`get_rotation_quat()` returns a `glm::quat` quaternion of the object rotation transformation  
`get_model_mat()` returns a `glm::mat4` of the object's transformed model matrix  
`get_chase_cam_view_mat` returns a `glm::mat4` of the object's view matrix for a chase camera (3rd person)  
`get_fp_cam_view_mat` returns a `glm::mat4` of the object's view matrix for a first person camera  
