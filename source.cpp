#include <GLFW/glfw3.h>
#include <cmath>
#include <iostream>

// Constants for the simulation
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;
const float PI = 3.14159265359f;

// Camera properties
float cameraDistance = 5.0f;
float cameraAngleX = 30.0f;
float cameraAngleY = 45.0f;

// Function to set up perspective projection without gluPerspective
void setPerspective(float fovY, float aspect, float near, float far) {
    float f = 1.0f / tan(fovY * PI / 360.0f);
    float matrix[16] = {
        f / aspect, 0, 0, 0,
        0, f, 0, 0,
        0, 0, (far + near) / (near - far), -1,
        0, 0, (2 * far * near) / (near - far), 0
    };
    glMultMatrixf(matrix);
}

// Function to set camera position without gluLookAt
void setCamera(float eyeX, float eyeY, float eyeZ,
    float centerX, float centerY, float centerZ,
    float upX, float upY, float upZ) {
    float forward[3], side[3], up[3];

    // Calculate forward vector
    forward[0] = centerX - eyeX;
    forward[1] = centerY - eyeY;
    forward[2] = centerZ - eyeZ;

    // Normalize forward vector
    float forwardLength = sqrt(forward[0] * forward[0] + forward[1] * forward[1] + forward[2] * forward[2]);
    forward[0] /= forwardLength;
    forward[1] /= forwardLength;
    forward[2] /= forwardLength;

    // Calculate side vector
    side[0] = forward[1] * upZ - forward[2] * upY;
    side[1] = forward[2] * upX - forward[0] * upZ;
    side[2] = forward[0] * upY - forward[1] * upX;

    // Normalize side vector
    float sideLength = sqrt(side[0] * side[0] + side[1] * side[1] + side[2] * side[2]);
    side[0] /= sideLength;
    side[1] /= sideLength;
    side[2] /= sideLength;

    // Calculate up vector
    up[0] = side[1] * forward[2] - side[2] * forward[1];
    up[1] = side[2] * forward[0] - side[0] * forward[2];
    up[2] = side[0] * forward[1] - side[1] * forward[0];

    float matrix[16] = {
        side[0], up[0], -forward[0], 0,
        side[1], up[1], -forward[1], 0,
        side[2], up[2], -forward[2], 0,
        0, 0, 0, 1
    };

    glMultMatrixf(matrix);
    glTranslatef(-eyeX, -eyeY, -eyeZ);
}

// Planet structure with additional 3D properties
struct Planet {
    float distance;    // Distance from the Sun
    float size;       // Radius of the planet
    float orbitSpeed; // Speed of orbit
    float angle;      // Current angle in orbit
    float tilt;       // Axial tilt
    float rotation;   // Self-rotation angle
};

// Initialize the Sun, planets, and Moon with faster speeds
Planet sun = { 0.0f, 0.2f, 0.0f, 0.0f, 0.0f, 0.0f };
Planet planets[7] = {
    {0.5f, 0.05f, 0.5f, 0.0f, 7.0f, 0.0f},     // Mercury
    {0.8f, 0.07f, 0.4f, 0.0f, 3.0f, 0.0f},     // Venus
    {1.1f, 0.09f, 0.3f, 0.0f, 23.5f, 0.0f},    // Earth
    {1.4f, 0.06f, 0.25f, 0.0f, 25.0f, 0.0f},   // Mars
    {1.7f, 0.1f, 0.2f, 0.0f, 3.0f, 0.0f},      // Jupiter
    {2.0f, 0.08f, 0.15f, 0.0f, 27.0f, 0.0f},   // Saturn
    {2.3f, 0.07f, 0.1f, 0.0f, 98.0f, 0.0f}     // Uranus
};
Planet moon = { 0.15f, 0.02f, 0.8f, 0.0f, 6.7f, 0.0f };

// Function to draw orbit paths
void drawOrbitPath(float radius) {
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 100; i++) {
        float angle = 2.0f * PI * i / 100;
        glVertex3f(radius * cos(angle), 0.0f, radius * sin(angle));
    }
    glEnd();
}

// Function to draw a sphere using triangle strips
void drawSphere(float radius, int segments) {
    for (int i = 0; i < segments; i++) {
        float lat0 = PI * (-0.5f + (float)(i) / segments);
        float lat1 = PI * (-0.5f + (float)(i + 1) / segments);

        glBegin(GL_TRIANGLE_STRIP);
        for (int j = 0; j <= segments; j++) {
            float lng = 2 * PI * (float)(j) / segments;
            float x, y, z;

            // First vertex
            x = cos(lng) * cos(lat0);
            y = sin(lat0);
            z = sin(lng) * cos(lat0);
            glNormal3f(x, y, z);
            glVertex3f(x * radius, y * radius, z * radius);

            // Second vertex
            x = cos(lng) * cos(lat1);
            y = sin(lat1);
            z = sin(lng) * cos(lat1);
            glNormal3f(x, y, z);
            glVertex3f(x * radius, y * radius, z * radius);
        }
        glEnd();
    }
}

// Function to set up lighting
void setupLighting() {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);

    float lightPos[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    float lightAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    float lightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
}

// Function to render the solar system
void renderSolarSystem() {
    float sunEmission[] = { 1.0f, 1.0f, 0.0f, 1.0f };
    float noEmission[] = { 0.0f, 0.0f, 0.0f, 1.0f };

    // Draw orbit paths
    glDisable(GL_LIGHTING);
    glColor3f(0.3f, 0.3f, 0.3f);
    for (int i = 0; i < 7; i++) {
        drawOrbitPath(planets[i].distance);
    }
    glEnable(GL_LIGHTING);

    // Render the Sun
    glMaterialfv(GL_FRONT, GL_EMISSION, sunEmission);
    glColor3f(1.0f, 1.0f, 0.0f);
    drawSphere(sun.size, 30);
    glMaterialfv(GL_FRONT, GL_EMISSION, noEmission);

    // Render planets
    for (int i = 0; i < 7; i++) {
        glPushMatrix();

        // Convert angle to radians for actual position calculation
        float angleRad = planets[i].angle * PI / 180.0f;
        float x = planets[i].distance * cos(angleRad);
        float z = planets[i].distance * sin(angleRad);

        glTranslatef(x, 0.0f, z);
        glRotatef(planets[i].tilt, 0.0f, 0.0f, 1.0f);
        glRotatef(planets[i].rotation, 0.0f, 1.0f, 0.0f);

        float planetColor[] = { 0.2f + 0.1f * i, 0.3f + 0.1f * i, 0.6f, 1.0f };
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, planetColor);
        drawSphere(planets[i].size, 20);

        if (i == 2) {  // Earth's moon
            glPushMatrix();
            float moonAngleRad = moon.angle * PI / 180.0f;
            float moonX = moon.distance * cos(moonAngleRad);
            float moonZ = moon.distance * sin(moonAngleRad);

            glTranslatef(moonX, 0.0f, moonZ);
            glRotatef(moon.tilt, 0.0f, 0.0f, 1.0f);
            glRotatef(moon.rotation, 0.0f, 1.0f, 0.0f);

            float moonColor[] = { 0.8f, 0.8f, 0.8f, 1.0f };
            glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, moonColor);
            drawSphere(moon.size, 15);
            glPopMatrix();
        }

        glPopMatrix();
    }
}

// Function to update positions and rotations
void updatePositions() {
    static float lastTime = glfwGetTime();
    float currentTime = glfwGetTime();
    float deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    for (int i = 0; i < 7; i++) {
        planets[i].angle += planets[i].orbitSpeed * deltaTime * 100.0f;
        planets[i].rotation += (0.5f + i * 0.1f) * deltaTime * 100.0f;
        if (planets[i].angle > 360.0f) planets[i].angle -= 360.0f;
        if (planets[i].rotation > 360.0f) planets[i].rotation -= 360.0f;
    }

    moon.angle += moon.orbitSpeed * deltaTime * 100.0f;
    moon.rotation += 0.3f * deltaTime * 100.0f;
    if (moon.angle > 360.0f) moon.angle -= 360.0f;
    if (moon.rotation > 360.0f) moon.rotation -= 360.0f;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        switch (key) {
        case GLFW_KEY_UP:
            cameraAngleX += 5.0f;
            break;
        case GLFW_KEY_DOWN:
            cameraAngleX -= 5.0f;
            break;
        case GLFW_KEY_LEFT:
            cameraAngleY -= 5.0f;
            break;
        case GLFW_KEY_RIGHT:
            cameraAngleY += 5.0f;
            break;
        case GLFW_KEY_W:
            cameraDistance -= 0.1f;
            break;
        case GLFW_KEY_S:
            cameraDistance += 0.1f;
            break;
        }
    }
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "3D Solar System", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, keyCallback);
    glfwSwapInterval(0); // Disable vsync

    // Set up perspective projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    setPerspective(45.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);

    // Enable depth testing and lighting
    glEnable(GL_DEPTH_TEST);
    setupLighting();

    // Set background color
    glClearColor(0.0f, 0.0f, 0.1f, 1.0f);  // Dark blue background

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();

        // Set up camera position
        float camX = cameraDistance * cos(cameraAngleY * PI / 180.0f) * cos(cameraAngleX * PI / 180.0f);
        float camY = cameraDistance * sin(cameraAngleX * PI / 180.0f);
        float camZ = cameraDistance * sin(cameraAngleY * PI / 180.0f) * cos(cameraAngleX * PI / 180.0f);
        setCamera(camX, camY, camZ, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

        renderSolarSystem();
        updatePositions();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
