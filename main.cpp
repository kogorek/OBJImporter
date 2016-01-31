#include <SDL2/SDL.h> // Библиотека SDL 2

#include <GL/gl.h> // Библиотека OpenGL
#include <GL/glu.h> // Библиотека GLU

#include <iostream> // стандартная библиотека ввода-вывода в C++
#include <cstdio> // fscanf
#include <fstream>

using namespace std;

SDL_Window *window; // задаем окно для SDL

const int width = 640; // ширина окна
const int height = 480; // высота окна

void init();
void displayOBJ();

float a = 0;

int main(int argc, char *argv[]){

    init(); // инициализация

    bool running = true;

    while(running){

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

        displayOBJ();
        glFlush();
        SDL_GL_SwapWindow(window);
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

    window = SDL_CreateWindow("Sol system", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

    SDL_GLContext glcontext = SDL_GL_CreateContext(window); // создаем контекст OpenGL

    if(window == NULL){ // если не получилось создать окно, то выходим
        exit(1);
    }

    // Инициализация OpenGL

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // устанавливаем фоновый цвет на черный
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

void displayOBJ()
{
    char vertex[100];
    double normal[3];
    double triangle[3];
    ifstream objFile;

    objFile.open("cube.obj");
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -10.0f);
    glBegin(GL_TRIANGLES);
    while (!objFile.eof())
    {
        objFile.getline(vertex, 100);
        if (strcmp(vertex, "v") == 32) // Не спрашивайте почему 32. Это число возвращается strcmp
        {
            sscanf(vertex, "v %lf %lf %lf\n", &triangle[0], &triangle[1], &triangle[2]);
            glVertex3dv(triangle);
        }
        else if (strcmp(vertex, "vn") == 32)
        {
            sscanf(vertex, "vn %lf %lf %lf\n", &normal[0], &normal[1], &normal[2]);
            glNormal3dv(normal);
        }
    }
    objFile.close();

    /*glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -200.0f);
    glBegin(GL_TRIANGLES);
*/
    glEnd();

}
