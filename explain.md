

### **Header Files**
```cpp
#include <GLFW/glfw3.h>
#include <cmath>
#include <iostream>
```
- `GLFW/glfw3.h`: Includes the GLFW library for window management, input handling, and OpenGL context creation.
- `cmath`: Provides mathematical functions like `cos`, `sin`, and `sqrt`.
- `iostream`: Used for debugging and logging to the console.

---

### **Global Constants**
```cpp
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;
const float PI = 3.14159265359f;
```
- `WINDOW_WIDTH` and `WINDOW_HEIGHT`: Define the dimensions of the application window.
- `PI`: A constant representing π, used for trigonometric calculations.

---

### **Camera Properties**
```cpp
float cameraDistance = 7.0f;
float cameraAngleX = 30.0f;
float cameraAngleY = 45.0f;
```
- `cameraDistance`: Distance of the camera from the origin.
- `cameraAngleX` and `cameraAngleY`: Control the vertical and horizontal angles of the camera.

---

### **Perspective Setup**
```cpp
void setPerspective(float fovY, float aspect, float near, float far)
```
- Implements a perspective projection matrix manually without using external libraries.
- Parameters:
  - `fovY`: Field of view in the vertical direction.
  - `aspect`: Aspect ratio (width/height) of the window.
  - `near` and `far`: The near and far clipping planes.
- Uses trigonometry (`tan`) to compute the matrix values and sets the OpenGL projection matrix.

---

### **Camera Setup**
```cpp
void setCamera(float eyeX, float eyeY, float eyeZ, float centerX, float centerY, float centerZ, float upX, float upY, float upZ)
```
- Manages the camera's position and orientation.
- Parameters:
  - `eyeX, eyeY, eyeZ`: Position of the camera.
  - `centerX, centerY, centerZ`: Point the camera looks at.
  - `upX, upY, upZ`: Defines the upward direction of the camera.
- Computes:
  - Forward vector: Direction the camera faces.
  - Side vector: Orthogonal to the forward and up vectors.
  - Up vector: Adjusted upward direction.
- Normalizes these vectors and applies the transformation.

---

### **Planet Structure**
```cpp
struct Planet {
    float distance;
    float size;
    float orbitSpeed;
    float angle;
    float tilt;
    float rotation;
};
```
- Represents the properties of planets:
  - `distance`: Distance from the sun.
  - `size`: Radius of the planet.
  - `orbitSpeed`: Speed of the planet's revolution.
  - `angle`: Current position in the orbit (in degrees).
  - `tilt`: Axial tilt of the planet.
  - `rotation`: Current rotation of the planet.

---

### **Planet Initialization**
```cpp
Planet sun = {0.0f, 0.2f, 0.0f, 0.0f, 0.0f, 0.0f};
Planet planets[7] = { ... };
Planet moon = {0.15f, 0.02f, 0.8f, 0.0f, 6.7f, 0.0f};
```
- Initializes the Sun, seven planets, and Earth's moon with specific properties.

---

### **Drawing Functions**
1. **Draw Orbit Path**
```cpp
void drawOrbitPath(float radius)
```
- Draws a circular orbit path for planets using a line loop.
- Uses trigonometric functions to compute points along the circle.

2. **Draw Sphere**
```cpp
void drawSphere(float radius, int segments)
```
- Renders a sphere using triangle strips.
- Divides the sphere into latitudinal and longitudinal segments.

---

### **Lighting Setup**
```cpp
void setupLighting()
```
- Configures lighting for the scene:
  - `glEnable(GL_LIGHTING)`: Enables lighting.
  - `glEnable(GL_LIGHT0)`: Activates the first light source.
  - Sets the light's position, ambient, and diffuse properties.

---

### **Rendering the Solar System**
```cpp
void renderSolarSystem()
```
- Renders the Sun, planets, and Earth's moon:
  - Draws orbit paths.
  - Applies material properties (e.g., emission for the Sun).
  - Positions and orients planets based on their distance, angle, tilt, and rotation.
  - Adds the moon around Earth.

---

### **Updating Positions**
```cpp
void updatePositions()
```
- Updates the orbital and rotational angles of planets and the moon.
- Uses `deltaTime` to ensure smooth animation regardless of frame rate.

---

### **Keyboard Controls**
```cpp
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
```
- Handles user input to control the camera:
  - Arrow keys: Adjust camera angles.
  - `W` and `S`: Zoom in and out by changing `cameraDistance`.

---

### **Main Function**
```cpp
int main()
```
1. **Initialize GLFW**:
   - Sets up the GLFW library and creates a window.
2. **Set Up OpenGL**:
   - Configures projection, lighting, and background color.
3. **Main Loop**:
   - Clears the screen, sets the camera, renders the solar system, updates positions, and processes input.
4. **Cleanup**:
   - Terminates GLFW and cleans up resources.

---

### **How the Code Works**
- **Visualization**: Models a 3D solar system with the Sun, planets, and Earth's moon.
- **User Interaction**: Allows camera movement and zooming using keyboard input.
- **Animation**: Simulates planetary orbits and rotations over time.

If you encounter issues running the program, ensure:
1. You have GLFW installed and linked correctly.
2. OpenGL drivers are up to date.
3. The system supports OpenGL.
