#include <SDL2/SDL.h> // Библиотека SDL 2
#include <SDL2/SDL_image.h>

#include <GL/gl.h> // Библиотека OpenGL
#include <GL/glu.h> // Библиотека GLU

#include <iostream> // стандартная библиотека ввода-вывода в C++
#include <cstdio> // fscanf
#include <fstream>
#include <cstring> // strcmp
#include <vector> // Для типа vector
#include <time.h>

using namespace std;

SDL_Window *window; // задаем окно для SDL

const int width = 640; // ширина окна
const int height = 480; // высота окна
const int FPScap = 30;
GLuint Texture[1];

void init();
void displayOBJ();
void init_texture();

float a = 0;

GLuint texture;

class object
{
public:
    char* path;
    // Объявление мультиразмерного вектора (2-х размерного)
    //для вертексов.
    //(v)
    vector< vector<double> > vertex;
    // Объявление мультиразмерного вектора (2-х размерного)
    //для тексурной координаты вершины.
    //(vt)
    vector< vector<double> > normalVertex;
    // Объявление мультиразмерного вектора (2-х размерного)
    //для нормали вершины.
    //(vn)
    vector< vector<double> > textureVertex;
    // Объявление мультиразмерного вектора (2-х размерного)
    //для фейсов(треугольников).
    vector< vector<int> > face;
    // Объявление мультиразмерного вектора (2-х размерного)
    //для индексов текстурной координаты.
    vector< vector<int> > textureFace;
    // Объявление мультиразмерного вектора (2-х размерного)
    //для индексов нормалей.
    vector< vector<int> > normalFace;


    object(const char* inputPath)
    {
        path = (char*)inputPath;
    }
    object()
    {

    }

    void loadTexture(void) {
        //Load the image from the file into SDL's surface representation
        SDL_Surface* surface = IMG_Load("kikiHD.jpg");
        if (surface==NULL) { //If it failed, say why and don't continue loading the texture
            printf("Error: \"%s\"\n",SDL_GetError()); return;
        }
        //Generate an array of textures.  We only want one texture (one element array), so trick
        //it by treating "texture" as array of length one.
        glGenTextures(1,&texture);
        //Select (bind) the texture we just generated as the current 2D texture OpenGL is using/modifying.
        //All subsequent changes to OpenGL's texturing state for 2D textures will affect this texture.
        glBindTexture(GL_TEXTURE_2D,texture);
        //Specify the texture's data.  This function is a bit tricky, and it's hard to find helpful documentation.  A summary:
        //   GL_TEXTURE_2D:    The currently bound 2D texture (i.e. the one we just made)
        //               0:    The mipmap level.  0, since we want to update the base level mipmap image (i.e., the image itself,
        //                         not cached smaller copies)
        //         GL_RGBA:    The internal format of the texture.  This is how OpenGL will store the texture internally (kinda)--
        //                         it's essentially the texture's type.
        //      surface->w:    The width of the texture
        //      surface->h:    The height of the texture
        //               0:    The border.  Don't worry about this if you're just starting.
        //          GL_RGB:    The format that the *data* is in--NOT the texture!  Our test image doesn't have an alpha channel,
        //                         so this must be RGB.
        //GL_UNSIGNED_BYTE:    The type the data is in.  In SDL, the data is stored as an array of bytes, with each channel
        //                         getting one byte.  This is fairly typical--it means that the image can store, for each channel,
        //                         any value that fits in one byte (so 0 through 255).  These values are to be interpreted as
        //                         *unsigned* values (since 0x00 should be dark and 0xFF should be bright).
        // surface->pixels:    The actual data.  As above, SDL's array of bytes.
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w,surface->h, 0, GL_RGB,GL_UNSIGNED_BYTE,surface->pixels);
        //Set the minification and magnification filters.  In this case, when the texture is minified (i.e., the texture's pixels (texels) are
        //*smaller* than the screen pixels you're seeing them on, linearly filter them (i.e. blend them together).  This blends four texels for
        //each sample--which is not very much.  Mipmapping can give better results.  Find a texturing tutorial that discusses these issues
        //further.  Conversely, when the texture is magnified (i.e., the texture's texels are *larger* than the screen pixels you're seeing
        //them on), linearly filter them.  Qualitatively, this causes "blown up" (overmagnified) textures to look blurry instead of blocky.
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

        //Unload SDL's copy of the data; we don't need it anymore because OpenGL now stores it in the texture.
        SDL_FreeSurface(surface);
    }
    int importOBJ(const char* inputPath)
    {
        // Переменная для получения строк из файла.
        char str_fileString[256];
        ifstream objFile; // Поток для файла obj.
        int dump; // Для проверки на соответствие строки маске.
        objFile.open(inputPath);
        if (objFile.fail())
        {
            cerr << "Error: cannot open OBJ file: " << path << endl;
            return 1;
        }
        while (!objFile.eof())
        {
            objFile.getline(str_fileString, 100);
            if (strcmp(str_fileString, "v") == 32) // Не спрашивайте почему 32. Это число возвращается strcmp
            {
                vertex.push_back(vector<double>(3)); // Создание новой строки.
                sscanf(str_fileString, "v %lf %lf %lf", &vertex.back()[0], &vertex.back()[1], &vertex.back()[2]);
                // Из прочитанной строчки записываем в вектор значения.
                //vertex.back()[n] означает что будет браться последняя строка в столбе n
            }else if (strcmp(str_fileString, "vn") == 32) // Не спрашивайте почему 32. Это число возвращается strcmp
            {
                normalVertex.push_back(vector<double>(3)); // Создание новой строки.
                sscanf(str_fileString, "vn %lf %lf %lf", &normalVertex.back()[0], &normalVertex.back()[1], &normalVertex.back()[2]);
            }else if (strcmp(str_fileString, "vt") == 32) // Не спрашивайте почему 32. Это число возвращается strcmp
            {
                textureVertex.push_back(vector<double>(2)); // Создание новой строки.
                sscanf(str_fileString, "vt %lf %lf", &textureVertex.back()[0], &textureVertex.back()[1]);
            }else if (strcmp(str_fileString, "f") == 32)
            {
                // Если строка имеет формат f 4//1 3//1 1//1 то
                if (sscanf(str_fileString, "f %d//%d %d//%d %d//%d", &dump, &dump, &dump, &dump, &dump, &dump) == 6)
                {
                    face.push_back(vector<int>(3)); // Создание новой строки.
                    normalFace.push_back(vector<int>(3));
                    sscanf(str_fileString, "f %d//%d %d//%d %d//%d", &face.back()[0], &normalFace.back()[0],
                            &face.back()[1], &normalFace.back()[1], &face.back()[2], &normalFace.back()[2]);
                } else // Иначе f 4/1/1 3/2/1 1/3/1
                {
                    face.push_back(vector<int>(3)); // Создание новой строки.
                    textureFace.push_back(vector<int>(3));
                    normalFace.push_back(vector<int>(3));
                    // Ну, тут всё понятно...
                    sscanf(str_fileString, "f %d/%d/%d %d/%d/%d %d/%d/%d", &face.back()[0], &textureFace.back()[0],
                            &normalFace.back()[0], &face.back()[1], &textureFace.back()[1], &normalFace.back()[1],
                            &face.back()[2], &textureFace.back()[2], &normalFace.back()[2]);
                }
            }
        }
        objFile.close();
        return 0;
    }

    // Возвращает 0 если удачно получилось открыть файл.
    int importOBJ()
    {
        // Переменная для получения строк из файла.
        char str_fileString[256];
        ifstream objFile; // Поток для файла obj.
        int dump; // Для проверки на соответствие строки маске.
        objFile.open(path);
        if (objFile.fail())
        {
            cerr << "Error: cannot open OBJ file: " << path << endl;
            return 1;
        }
        while (!objFile.eof())
        {
            objFile.getline(str_fileString, 100);
            if (strcmp(str_fileString, "v") == 32) // Не спрашивайте почему 32. Это число возвращается strcmp
            {
                vertex.push_back(vector<double>(3)); // Создание новой строки.
                sscanf(str_fileString, "v %lf %lf %lf", &vertex.back()[0], &vertex.back()[1], &vertex.back()[2]);
                // Из прочитанной строчки записываем в вектор значения.
                //vertex.back()[n] означает что будет браться последняя строка в столбе n
            }else if (strcmp(str_fileString, "vn") == 32) // Не спрашивайте почему 32. Это число возвращается strcmp
            {
                normalVertex.push_back(vector<double>(3)); // Создание новой строки.
                sscanf(str_fileString, "vn %lf %lf %lf", &normalVertex.back()[0], &normalVertex.back()[1], &normalVertex.back()[2]);
            }else if (strcmp(str_fileString, "vt") == 32) // Не спрашивайте почему 32. Это число возвращается strcmp
            {
                textureVertex.push_back(vector<double>(2)); // Создание новой строки.
                sscanf(str_fileString, "vt %lf %lf", &textureVertex.back()[0], &textureVertex.back()[1]);
            }else if (strcmp(str_fileString, "f") == 32)
            {
                // Если строка имеет формат f 4//1 3//1 1//1 то
                if (sscanf(str_fileString, "f %d//%d %d//%d %d//%d", &dump, &dump, &dump, &dump, &dump, &dump) == 6)
                {
                    face.push_back(vector<int>(3)); // Создание новой строки.
                    normalFace.push_back(vector<int>(3));
                    sscanf(str_fileString, "f %d//%d %d//%d %d//%d", &face.back()[0], &normalFace.back()[0],
                            &face.back()[1], &normalFace.back()[1], &face.back()[2], &normalFace.back()[2]);
                } else // Иначе f 4/1/1 3/2/1 1/3/1
                {
                    face.push_back(vector<int>(3)); // Создание новой строки.
                    textureFace.push_back(vector<int>(3));
                    normalFace.push_back(vector<int>(3));
                    // Ну, тут всё понятно...
                    sscanf(str_fileString, "f %d/%d/%d %d/%d/%d %d/%d/%d", &face.back()[0], &textureFace.back()[0],
                            &normalFace.back()[0], &face.back()[1], &textureFace.back()[1], &normalFace.back()[1],
                            &face.back()[2], &textureFace.back()[2], &normalFace.back()[2]);
                }
            }
        }
        objFile.close();
        return 0;
    }
    void display()
    {
        glRotatef(-1.5f, 0.0f, 1.0f, 0.0f);
        glBegin(GL_TRIANGLES);
        // Выводим модель на экран.
        for (unsigned int i = 0; i < face.size(); i++)
        {
            for (int j = 0; j < 3; j++)
            {
                glTexCoord2dv(textureVertex[textureFace[i][j] - 1].data());
                glNormal3dv(normalVertex[normalFace[i][j] - 1].data());
                glVertex3dv(vertex[face[i][j] - 1].data());
                // -1 потому что в OBJ нумирация идёт с 1, а не с 0. как
                //в C++.
                // vertex[].data возвращает вектор как массив.
            }
        }
        glEnd();
    }
};

struct tm *tblock;
time_t timer;
int sec, sec_old;
int FPS1;
int main(/*int argc, char *argv[]*/){
    Uint32 start = 0; // TEST FPS CAP
    init(); // инициализация
    init_texture();
    glBindTexture(GL_TEXTURE_2D,texture);
    //Tell OpenGL that all subsequent drawing operations should try to use the current 2D texture
    glEnable(GL_TEXTURE_2D);
    object kiki;
    kiki.importOBJ("cube.obj");
    glTranslatef(0.0f, -2.0f, -4.0f);
    bool running = true;

    while(running){
        start = SDL_GetTicks();
        SDL_Event event; // события SDL

        while ( SDL_PollEvent(&event) ){ // начинаем обработку событий
            switch(event.type){ // смотрим:
            case SDL_QUIT: // если произошло событие закрытия окна, то завершаем работу программы
                running = false;
                break;

            case SDL_KEYDOWN: // если нажата клавиша
                switch(event.key.keysym.sym){ // смотрим какая
                case SDLK_ESCAPE: // клавиша ESC
                    running = false; // завершаем работу программы
                    break;
                }
                break;
            }
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //glLoadIdentity();


        //glDisable(GL_TEXTURE_2D);
        //displayOBJ();

        kiki.display();

        glFlush();
        SDL_GL_SwapWindow(window);
        //FPS TRICK
        if(1000/FPScap > SDL_GetTicks()-start) {
            SDL_Delay(1000/FPScap-(SDL_GetTicks()-start));
        }

        timer = time(NULL);
        tblock = localtime(&timer);
        sec = tblock->tm_sec;
        FPS1++;
        if (sec_old != sec){
            sec_old = sec;
            cout << "FPS: " << FPS1 << endl;
            FPS1 = 0;
        }

    }

    SDL_Quit(); // завершаем работу SDL и выходим
    return 0;
}

void init(){
    // Инициализация SDL
    if ( SDL_Init(SDL_INIT_VIDEO) < 0 ){
        cout << "Unable to init SDL, error: " << SDL_GetError() << endl;
        exit(1);
    }

    // Включаем двойной буфер, настраиваем цвета

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 6);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);

    // Создаем окно с заголовком "Cube", размером 640х480 и расположенным по центру экрана.

    window = SDL_CreateWindow("OBJ loader", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

    SDL_GLContext glcontext = SDL_GL_CreateContext(window); // создаем контекст OpenGL

    if(window == NULL){ // если не получилось создать окно, то выходим
        exit(1);
    }

    // Инициализация OpenGL

    glClearColor(0.9f, 0.7f, 0.6f, 0.0f); // устанавливаем фоновый цвет на черный
    glClearDepth(1.0);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST); // включаем тест глубины
    glShadeModel(GL_SMOOTH);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90.0f, (float) width / (float) height, 0.1f, 300.0f); // настраиваем трехмерную перспективу
    glMatrixMode(GL_MODELVIEW); // переходим в трехмерный режим
    //glOrtho(-240, 240, -160, 160, -1.0, 1.0);
}

void init_texture(void) {
    //Load the image from the file into SDL's surface representation
    SDL_Surface* surface = IMG_Load("kikiHD.jpg");
    if (surface==NULL) { //If it failed, say why and don't continue loading the texture
        printf("Error: \"%s\"\n",SDL_GetError()); return;
    }
    //Generate an array of textures.  We only want one texture (one element array), so trick
    //it by treating "texture" as array of length one.
    glGenTextures(1,&texture);
    //Select (bind) the texture we just generated as the current 2D texture OpenGL is using/modifying.
    //All subsequent changes to OpenGL's texturing state for 2D textures will affect this texture.
    glBindTexture(GL_TEXTURE_2D,texture);
    //Specify the texture's data.  This function is a bit tricky, and it's hard to find helpful documentation.  A summary:
    //   GL_TEXTURE_2D:    The currently bound 2D texture (i.e. the one we just made)
    //               0:    The mipmap level.  0, since we want to update the base level mipmap image (i.e., the image itself,
    //                         not cached smaller copies)
    //         GL_RGBA:    The internal format of the texture.  This is how OpenGL will store the texture internally (kinda)--
    //                         it's essentially the texture's type.
    //      surface->w:    The width of the texture
    //      surface->h:    The height of the texture
    //               0:    The border.  Don't worry about this if you're just starting.
    //          GL_RGB:    The format that the *data* is in--NOT the texture!  Our test image doesn't have an alpha channel,
    //                         so this must be RGB.
    //GL_UNSIGNED_BYTE:    The type the data is in.  In SDL, the data is stored as an array of bytes, with each channel
    //                         getting one byte.  This is fairly typical--it means that the image can store, for each channel,
    //                         any value that fits in one byte (so 0 through 255).  These values are to be interpreted as
    //                         *unsigned* values (since 0x00 should be dark and 0xFF should be bright).
    // surface->pixels:    The actual data.  As above, SDL's array of bytes.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w,surface->h, 0, GL_RGB,GL_UNSIGNED_BYTE,surface->pixels);
    //Set the minification and magnification filters.  In this case, when the texture is minified (i.e., the texture's pixels (texels) are
    //*smaller* than the screen pixels you're seeing them on, linearly filter them (i.e. blend them together).  This blends four texels for
    //each sample--which is not very much.  Mipmapping can give better results.  Find a texturing tutorial that discusses these issues
    //further.  Conversely, when the texture is magnified (i.e., the texture's texels are *larger* than the screen pixels you're seeing
    //them on), linearly filter them.  Qualitatively, this causes "blown up" (overmagnified) textures to look blurry instead of blocky.
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    //Unload SDL's copy of the data; we don't need it anymore because OpenGL now stores it in the texture.
    SDL_FreeSurface(surface);
}
