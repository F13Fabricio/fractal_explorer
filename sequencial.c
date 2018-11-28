/* 
 * Fractal explorer sequencial version
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 720
#define ITERATIONS 100

// fractal types
#define MANDELBROT 1
#define JULIA 2


typedef struct _point {
    float x, y;
    unsigned char r, g, b, a;
} Point;

/* global */
float c_real = 0;
float c_imag = 0;
float offsetx = 0;
float offsety = 0;
float zoom = 0.001;

float complex_fator = 0.001;
float move_fator = 0.01;
float zoom_fator = 0.0001;

int frac_type = MANDELBROT;

int getColorJulia(float _x, float _y);
int getColorMandelbrot(float _x, float _y);
void fractal(size_t length, Point *points);
void render(size_t length, Point *points);
void keyCallback( GLFWwindow *window, int key, int scancode, int action, int mods );

void keyCallback( GLFWwindow *window, int key, int scancode, int action, int mods ) {
    switch (key) {

        /* navegation */
        case GLFW_KEY_A:
            offsetx -= move_fator;
            break;
        case GLFW_KEY_D:
            offsetx += move_fator;
            break;
        case GLFW_KEY_W:
            offsety -= move_fator;
            break;
        case GLFW_KEY_S:
            offsety += move_fator;
            break;
        case GLFW_KEY_KP_ADD:
            zoom -= zoom_fator;
            break;
        case GLFW_KEY_KP_SUBTRACT:
            zoom += zoom_fator;
            break;

        /* specific for julia fractal */
        case GLFW_KEY_UP:
            if (frac_type == JULIA) c_real += complex_fator;
            break;
        case GLFW_KEY_DOWN:
            if (frac_type == JULIA) c_real -= complex_fator;
            break;
        case GLFW_KEY_LEFT:
            if (frac_type == JULIA) c_imag += complex_fator;
            break;
        case GLFW_KEY_RIGHT:
            if (frac_type == JULIA) c_imag -= complex_fator;
            break;
        
        /* select fractal type */
        case GLFW_KEY_1:
            frac_type = MANDELBROT;
            break;
        case GLFW_KEY_2:
            frac_type = JULIA;
            break;
        
        /* other controls */
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, GLFW_TRUE);
            break;
        default:
            //printf("%i\n", key);
            break;
    }
}

int getColorMandelbrot(float _x, float _y) {
    int i = 0;
    float x = _x;
    float y = _y;
    float temp = 0;

    while (x*x + y*y < 16 && i < ITERATIONS) {
        temp = x*x - y*y + _x;
        y = 2 * y * x + _y;
        x = temp;
        i++;
    }
    return i;
}

int getColorJulia(float _x, float _y) {
    int i = 0;
    float x = _x;
    float y = _y;
    float temp = 0;

    while ( x*x + y*y < 16 && i < ITERATIONS) {
        temp = x*x - y*y + c_real;
        y = 2 * y * x + c_imag;
        x = temp;
        i++;
    }
    return i;
}

void fractal(size_t length, Point *points) {
    int i, j, color;
    float x, y;
    float fator = 70;
    float delta  = (2.0 / SCREEN_HEIGHT) + zoom;
    for (i = 0; i < SCREEN_HEIGHT; i++) {
        for (j = 0; j < SCREEN_WIDTH; j++) {
            x = delta * (j - SCREEN_WIDTH / 2) + offsetx;
            y = delta * (i - SCREEN_HEIGHT / 2) + offsety;

            // select fractal function
            switch(frac_type) {
                case MANDELBROT:
                    color = getColorMandelbrot(x, y);
                    break;
                case JULIA:
                    color = getColorJulia(x, y);
                    break;
                default:
                    break;
            }

            color = color * (log(color)*10);
            points[i * SCREEN_WIDTH + j].x = j;
            points[i * SCREEN_WIDTH + j].y = i;
            points[i * SCREEN_WIDTH + j].r = color;
            points[i * SCREEN_WIDTH + j].g = 0;
            points[i * SCREEN_WIDTH + j].b = 100;
            points[i * SCREEN_WIDTH + j].a = 0;
        }
    }
}

void render(size_t length, Point *points) {
    fractal(length, points);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, SCREEN_WIDTH, 0.0, SCREEN_HEIGHT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // draw
    glColor3ub(255, 255, 255);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glVertexPointer(2, GL_FLOAT, sizeof(Point), &points[0].x);
    glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Point), &points[0].r);
    glPointSize(1.0);
    glDrawArrays(GL_POINTS, 0, length);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    glFlush();
}

int main(void) {
    GLFWwindow* window;
    GLuint bufferObj;

    size_t length = SCREEN_WIDTH * SCREEN_HEIGHT;
    Point *points = (Point*) malloc(sizeof(Point) * length);

    if (!glfwInit() || !glewInit()) return -1;

    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT,
                              "Fractal Explorer - Sequencial", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_FALSE);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        render(length, points);
        glfwSwapBuffers(window);
        glfwPollEvents();
        //glfwWaitEventsTimeout(0.7);
        //glfwWaitEvents();
    }

    glfwTerminate();
    free(points);
    return 0;
}
