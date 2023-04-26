# System Design Document --SplatoonDemo

## Background Introduction

Splatoon is a third-person paint action shooter game developed and published by Nintendo. Its innovative and entertaining gameplay mechanics and unique, cool visual effects are deeply loved by players and have achieved excellent sales results and won multiple awards.

Based on Splatoon as its prototype, this project aims to apply the knowledge learned in computer graphics courses to implement some basic game mechanics and visual effects.

## Demo video link

https://youtu.be/KOZ5kqWw3yA

##  Implementation

### **(1) Game Mechanics**

1. The character can move freely and jump on the ground (non-linear), and collision detection exists between characters.
   
2. The character can shoot paint bullets.
3. The paint bullets will leave a painted area on the ground when they hit the ground.
4. The paint bullets (checked through collision detection) can cause damage to enemy units.
5. The player's unit can move faster by diving into their own colored area.
6. Entering an enemy's colored area will have a slowing effect.

### **(2) Visual Effects**

1. Basic scene and object (character, bullet) rendering.
   
2. Paint splatter effect on the ground when bullets hit, along with a paint-like texture.
3. Red flash effect when characters are hit.
4. Explosion effect when the character's health is depleted.
5. Shadows for characters and bullets.
6. Implementation of single light source lighting effect.
7. Particle effects generated behind the character as they move.

### **(3) Sound Effects**
1. Background music.

2. Sound effects for shooting bullets.
3. Sound effect for character explosion when they die.

## Framework Design

### Module design

##### PhysicalEngine

    This includes the definition of the base class of objects and collision detection.
    
    The basic properties of objects include position, gravity acceleration, color, number (object number and team number), and survival status.
    
    Objects are divided into rectangular objects and spherical objects, and rectangular/spherical objects have different properties (length, width, height/radius).
    
    Implementation of collision detection between different types of objects. (Based on the aforementioned rectangular and spherical object classes)

##### GameLogic

    The logical objects such as bullets and characters are derived from the base class of objects.
    
    Before each round of rendering, the physics engine is called to obtain collision detection information, and the information of the logical objects is updated and adjusted accordingly.
    
    For example, when a bullet hits a character/floor, the health is updated, the bullet is eliminated, and the object storage information is updated.


##### Renderer

    The provided object data is processed and rendered to generate images.
    
    For example, the painted floor is rendered, and the characters, bullets, and other objects are rendered, along with the shadow effect.


##### GameWindow

    Initialize window, initialize and handle mouse and keyboard events, manage camera, handle game logic module, renderer module, and some interactions of the camera.
    
    Main loop: Obtain time information, obtain projection and view matrices based on camera information, call the game logic module for logical object information update and adjustment, and then call the renderer module for rendering.


### Implementation of Main Modules

##### PhysicalEngine

**Collision Detection**

    Cube Collision Detection:
    
    In this project, there are two types of collision detection algorithms: sphere-cube collision and cube-cube collision. Sphere-cube collision is relatively simple and will not be elaborated here.
    
    For cube-cube collision detection, we can use the following algorithm:
    
    Take two cubes A and B, and first rotate B to be parallel to the coordinate axis, while rotating A accordingly.
    
    For each of the 12 edges of cube A, call the function that detects if the edge intersects with cube B, by treating cube B as a group of planes.
    
    If any of the edges intersect with cube B, the two cubes have collided.
    
    Swap the positions of cube A and B, and repeat the above process.
    
    This algorithm can be optimized to improve its efficiency and accuracy.

```c++
bool PhysicalEngine::intersect(glm::vec3 p1, glm::vec3 p2, glm::vec3 boxPoint, float halfLength) {
    float max_x = boxPoint.x + halfLength;
    float min_x = boxPoint.x - halfLength;
    float max_y = 100.0f;//boxPoint.y + halfLength;
    float min_y = 0.0f;//boxPoint.y - halfLength;
    float max_z = boxPoint.z + halfLength;
    float min_z = boxPoint.z - halfLength;
    glm::vec3 dir = p2 - p1;
    float t0 = 0.0f, t1 = 1.0f;
    if (fabs(dir.x) < eps) {
        if (p1.x > max_x + eps || p1.x < min_x - eps) return false;
    } else {
        float inv_dir = 1.0f / dir.x;
        float t_near = (max_x - p1.x) * inv_dir;
        float t_far = (min_x - p1.x) * inv_dir;
        if (t_near > t_far) swap(t_near, t_far);
        t0 = max(t0, t_near);
        t1 = min(t1, t_far);
        if (t0 > t1 + eps) return false;
    }
    if (fabs(dir.y) < eps) {
        if (p1.y > max_y + eps || p1.y < min_y - eps) return false;
    } else {
        float inv_dir = 1.0f / dir.y;
        float t_near = (max_y - p1.y) * inv_dir;
        float t_far = (min_y - p1.y) * inv_dir;
        if (t_near > t_far) swap(t_near, t_far);
        t0 = max(t0, t_near);
        t1 = min(t1, t_far);
        if (t0 > t1 + eps) return false;
    }
    if (fabs(dir.z) < eps) {
        if (p1.z > max_z + eps || p1.z < min_z - eps) return false;
    } else {
        float inv_dir = 1.0f / dir.z;
        float t_near = (max_z - p1.z) * inv_dir;
        float t_far = (max_z - p1.z) * inv_dir;
        if (t_near > t_far) swap(t_near, t_far);
        t0 = max(t0, t_near);
        t1 = min(t1, t_far);
        if (t0 > t1 + eps) return false;
    }
    return true;
}
```
    Intersection Detection between Line Segments and Cubes:
    
    Since the cube is now parallel to the coordinate axis, it is easy to project both the line segment and the cube onto the x, y, and z axes respectively for intersection detection.
    
    For each projection, we can calculate the range of values that the line segment and cube cover on that axis. If the two ranges overlap, then there is an intersection between the line segment and the cube on that axis. We can repeat this process for all three axes to determine whether there is an intersection between the line segment and the cube in 3D space.

** Nonlinear Movement**

    Objects have properties such as volume, mass, position, displacement direction, velocity, acceleration, etc.
    
    The ground and air have friction coefficients (smaller on own color paint, larger on enemy color paint).
    
    The keyboard is used to control player movement with the WASD keys, which is considered as having a certain corresponding direction of acceleration in a short period of time.
    
    When objects collide, their velocities are simply reversed (for convenience of implementation, but not realistic).
    
    Time passage is simulated by obtaining the program's running time.

##### GameLogic

**Keyboard and Mouse Interactions**
    

    Mainly, the movement of the player is indirectly controlled through the Camera class.
    
    After switching to the mode by pressing the "c" key, the camera is bound to the player, and any movements and changes in the camera's viewpoint are translated into corresponding movements and viewpoint changes of the player.

**Bullet Shooting**

    When firing bullets, random perturbations are added to the initial position and velocity of the bullets, achieving a scattering effect.
    
    The direction of the bullets is aligned with the player's viewpoint.
    
    The initial velocity of the bullets is relatively high to match the "pop-pop-pop" sound effect.

**Bullet Class**

    The Bullet class has an array specifically for storing currently unfired bullets.
    
    In each main loop, the update function of the bullets is called, mainly to simulate projectile motion.
    
    When a bullet is generated, the time of generation is recorded for simulation purposes.

**Player Class**

    There are many operations that the player can perform, such as jumping, shooting, diving, and moving. Each operation mainly involves checking and modifying the current state of the player (such as some physical properties).
    
    In the update function of the player, the position and other information of the player are updated according to the physical state and the passage of time.
    
    When the player is not under direct control, a simple AI is set up to allow the player to randomly move and perform actions.

##### Renderer

**Rendering of Characters and Bullets**

    The characters are represented by a cuboid, so an obj file is manually created to represent the character.
    
    To generate the spheres, the sphere class from a previous experiment was used, and the generated vertex and element information were stored in obj files.

**Drawing of the floor**

    The floor is covered by a square grid with a size of (301, 301), which spans the coordinate range of (-50..50, -50..50).
    
    Normally, each square can be drawn using two diagonal triangles to complete the grid.
    
    However, during actual debugging, it was found that if only 3/4 of each square is drawn, it can create a transparent floor effect and provide a better visual experience.

```C++
for(int i = 0; i < FLOOR_SIZE; ++i) {
    for (int j = 0; j < FLOOR_SIZE; ++j) {
        int idx = i * (FLOOR_SIZE + 1) + j;
        int idy = idx + 1;
        int idu = idx + (FLOOR_SIZE + 1);
        int idv = idy + (FLOOR_SIZE + 1);

        // a triangle
        floorElement.push_back(idx);
        floorElement.push_back(idy);
        floorElement.push_back(idu);

        // a triangle
//      floorElement.push_back(idu);
//      floorElement.push_back(idv);
//      floorElement.push_back(idy);

//      an amazing effect !!!
        floorElement.push_back(idx);
        floorElement.push_back(idy);
        floorElement.push_back(idv);
    }
}
```

**Determination of painted areas**

    This section describes the coloration area detection, which theoretically belongs to the floor drawing module, but is described separately due to its importance.
    
    For the coloration area, we define an outer radius (OUT_RADIUS) and an inner radius (IN_RADIUS).
    
    When a bullet hits the ground, it is first converted into a grid coordinate through corresponding mathematical operations, and then the grid coordinate is used as the starting point for BFS.
    
    Let d be the distance between a searched point and the initial search point:
    
    If d < IN_RADIUS: set the color intensity of the point to 1, and set the floor color to the color of the bullet.
    If d > OUT_RADIUS: continue
    Otherwise: interpolate the color intensity of the point based on the difference between d and IN_RADIUS, and set the floor color to the color of the bullet.
    The corresponding shader performs noise processing on the color intensity to achieve a more realistic paint texture.
```c++
    while(!q.empty()){
        BulletPos nowPos = q.front();
        q.pop();

        if(!nowPos.checkPos()) continue;

        // distance between this position and bullet position
        float dis = BulletPos::distance(nowPos, src);

        // check whether need render
        if(dis > OUT_RADIUS*OUT_RADIUS) continue;

        int id = nowPos.mapToId();
        int idStartPos = id * FLOOR_ELEMENT_COUNT;

        float& colorAlpha = floorVertices[idStartPos + 6];
        if(dis <= IN_RADIUS*IN_RADIUS){
            colorAlpha = 1.0f;
        }else{ // calc difference
            float tFactor = 1.0f/ (OUT_RADIUS - IN_RADIUS);
            float d = sqrt(dis);
            float t = 1.0f - (d - IN_RADIUS)*tFactor;

            colorAlpha = std::min(colorAlpha + t, 1.0f);
        }

        floorVertices[idStartPos + 3] = color[0];
        floorVertices[idStartPos + 4] = color[1];
        floorVertices[idStartPos + 5] = color[2];

        // find adjacent
        for(int i = 0;i < 4; ++ i){
            BulletPos nxt(nowPos + BulletPos(dx[i], dy[i]));
            if(s.find(nxt) == s.end()){
                s.insert(nxt);
                q.push(nxt);
            }
        }
    }
```

**Light**

[Reference(learnOpenGL)](https://learnopengl-cn.github.io/05%20Advanced%20Lighting/01%20Advanced%20Lighting/)

    The Blinn-Phong lighting model was used for lighting in this project, and the code was based on the tutorials from learnOpenGL. The light source was set to the position of the sun indicated by the skybox used in this project.
    
    When the floor is transparent, the reflection of the lighting and the reflection of the water surface under the skybox blend together, creating a good visual effect.

**Shadow**

[Reference(learnOpenGL)](https://learnopengl-cn.github.io/05%20Advanced%20Lighting/03%20Shadows/01%20Shadow%20Mapping/)

    Create a new frame buffer (depth buffer) and position the view at the light source position. Adjust the parameters so that the projection matrix at this time is an orthogonal projection matrix of the light source looking at the plane.
    
    Render all objects that need to be shadowed into this frame buffer.
    
    After rendering, pass the depth buffer of this buffer as a texture to the floor rendering section.
    
    In the floor rendering, map the points to be rendered through the corresponding orthogonal matrix, and compare the depth of the point with the depth in the texture to determine if the point will be occluded.

**Game sound**

    The game sound effects are implemented using a cross-platform audio library. irrKlang
    
    The sound effect is played at the beginning of the game. 
    
    Play shooting sound effects when the player shoots while controlling their character, and play explosion sound effects when a character dies.

Paint

    For each sampling point on the floor, a color strength is provided to the shader. The shader will calculate the color based on the color strength using different methods.
    
    To represent the effect of irregular spray paint, we introduce noise textures. The color strength of the point is subtracted from the noise strength, and two threshold values are set to divide the difference into three parts: strong color, edge color, and no color.
    
    For the strongly colored part, in order to show the unevenness of the paint spray, the color is blended with the noise. For the edge color part, a noise threshold is set. If the noise value is high, the color is blended with the floor color based on the color strength to produce a gradient effect. If the noise is small, the floor color is directly taken to produce an irregular spray effect. For the uncolored part, the floor color is directly taken.

**Particle**

    Particles are objects with a duration and can inherit properties from the Object class. In this project, the wake effect under the water uses particles.
    
    At the beginning of each frame, the particle's position, velocity, and other basic attributes, as well as the life attribute, are updated based on the time interval. If the life attribute is less than 0, the particle is destroyed.
    
    The life value can be used to specify different drawing methods and position changes for particles, thereby creating dynamic effects.

## Deployment and operation

This project uses the following libraries:`glew 2.2.0`, `glfw3.3.2`, `irrKlang 1.6`, `glm 0.9.8.5`, `stb_image`

Compilation: Using Cmake to compile projects for cross-platform compatibility

## Operating Instructions

Keyboard control
    W: Control camera move forward/give player forward speed
    
    S: Control camera move backward/give player backward speed
    
    A: Control camera move left/give player leftward speed
    
    D: Control camera move right/give player rightward speed
    
    C: Switch between global camera/player camera
    
    Q: Switch to the previous player (when in player camera mode)
    
    E: Switch to the next player (when in player camera mode)
    
    Space: Control camera move upward/control player jump
    
    Shift: Control camera move downward/control player dive (only effective on the player with the same color as the paint)

Mouse control

    Mouse movement controls the camera angle.
    
    Left mouse button fires bullets.
    
    Mouse scroll wheel for zooming.
