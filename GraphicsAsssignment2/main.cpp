#define GL_SILENCE_DEPRECATION

#include <GLUT/glut.h>
#include <cmath>
#include <cstdio>
#include <vector>
#include <cstdlib>



#define GLUT_KEY_ESCAPE 27
#define DEG2RAD(a) (a * 0.0174532925)



class Vector3f {
public:
    float x, y, z;

    Vector3f(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) {
        x = _x;
        y = _y;
        z = _z;
    }

    Vector3f operator+(const Vector3f& v) const {
        return Vector3f(x + v.x, y + v.y, z + v.z);
    }

    Vector3f operator-(const Vector3f& v) const {
        return Vector3f(x - v.x, y - v.y, z - v.z);
    }

    Vector3f operator*(float n) const {
        return Vector3f(x * n, y * n, z * n);
    }

    Vector3f operator/(float n) const {
        return Vector3f(x / n, y / n, z / n);
    }

    Vector3f unit() const {
        float magnitude = sqrt(x * x + y * y + z * z);
        return Vector3f(x / magnitude, y / magnitude, z / magnitude);
    }

    Vector3f cross(const Vector3f& v) const {
        return Vector3f(
            y * v.z - z * v.y,
            z * v.x - x * v.z,
            x * v.y - y * v.x
        );
    }
};

class Camera {
public:
    Vector3f eye, center, up;

    Camera(float eyeX = 1.0f, float eyeY = 1.0f, float eyeZ = 1.0f, float centerX = 0.0f, float centerY = 0.0f, float centerZ = 0.0f, float upX = 0.0f, float upY = 1.0f, float upZ = 0.0f) {
        eye = Vector3f(eyeX, eyeY, eyeZ);
        center = Vector3f(centerX, centerY, centerZ);
        up = Vector3f(upX, upY, upZ);
    }

    void moveX(float d) {
        Vector3f right = up.cross(center - eye).unit();
        eye = eye + right * d;
        center = center + right * d;
    }

    void moveY(float d) {
        eye = eye + up.unit() * d;
        center = center + up.unit() * d;
    }

    void moveZ(float d) {
        Vector3f view = (center - eye).unit();
        eye = eye + view * d;
        center = center + view * d;
    }

    void rotateX(float a) {
        Vector3f view = (center - eye).unit();
        Vector3f right = up.cross(view).unit();
        view = view * cos(DEG2RAD(a)) + up * sin(DEG2RAD(a));
        up = view.cross(right);
        center = eye + view;
    }

    void rotateY(float a) {
        Vector3f view = (center - eye).unit();
        Vector3f right = up.cross(view).unit();
        view = view * cos(DEG2RAD(a)) + right * sin(DEG2RAD(a));
        right = view.cross(up);
        center = eye + view;
    }

    void look() {
        gluLookAt(
            eye.x, eye.y, eye.z,
            center.x, center.y, center.z,
            up.x, up.y, up.z
        );
    }
};

Camera camera;


float playerPosX = 0.0f;
float playerPosY = 0.0f;
float playerPosZ = 0.0f;
const float MOVEMENT_SPEED = 0.05f;
float playerRotationAngle = 0.0f;
bool animateLogo = false;
float logoRotation = 0.0f;
float goalRotation = 0.0f;
float coachScale = 1.0f;
float targetCoachScale = 2.0f;
float originalCoachScale = 1.0f;
bool isScaling = false;
bool animateCoachScaling = false;
float scaleSpeed = 0;
int scalingDirection = 1;



/////Draw//////
float xBall = -2.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (5.0f)));
float zBall = -1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (3.0f)));


const float FENCE_LEFT = -3.0f;
const float FENCE_RIGHT = 3.0f;
const float FENCE_FRONT = 2.0f;
const float FENCE_BACK = -2.0f;
GLfloat fenceHeight = 0.5f;


const float GOAL_LEFT = -2.5f;
const float GOAL_RIGHT = 2.5f;
const float GOAL_FRONT = 1.5f;
const float GOAL_BACK = -1.5f;

bool checkCollisionWithFence(float x, float z) {
    return x < FENCE_LEFT || x > FENCE_RIGHT || z < FENCE_BACK || z > FENCE_FRONT;
}




void movePlayer(float dx, float dy, float dz) {
    float newX = playerPosX + dx * MOVEMENT_SPEED;
    float newZ = playerPosZ + dz * MOVEMENT_SPEED;

    if (dx != 0.0f || dz != 0.0f) {
        playerRotationAngle = atan2(dz, dx) * 180.0f / M_PI;
    }

    if (checkCollisionWithFence(newX, newZ)) {
        printf("Collision with fence!\n");
    } else {
        playerPosX = newX;
        playerPosZ = newZ;
    }
}



bool isColorChangingFence = true;
float colorTimerFence = 0.0f;


void drawHandballCourt() {
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex3f(-3.0f, 0.01f, -2.0f);
    glVertex3f(3.0f, 0.01f, -2.0f);
    glVertex3f(3.0f, 0.01f, 2.0f);
    glVertex3f(-3.0f, 0.01f, 2.0f);
    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(2.0);
    glBegin(GL_LINES);

    glVertex3f(-2.5f, 0.02f, -1.5f);
    glVertex3f(-2.5f, 0.02f, 1.5f);

    glVertex3f(2.5f, 0.02f, -1.5f);
    glVertex3f(2.5f, 0.02f, 1.5f);

    glVertex3f(-2.5f, 0.02f, -1.5f);
    glVertex3f(2.5f, 0.02f, -1.5f);

    glVertex3f(-2.5f, 0.02f, 1.5f);
    glVertex3f(2.5f, 0.02f, 1.5f);

    glVertex3f(0.0f, 0.02f, -1.5f);
    glVertex3f(0.0f, 0.02f, 1.5f);

    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);
    GLfloat radius = 0.2f;
    GLint numSegments = 100;
    GLfloat centerX = 0.0f;
    GLfloat centerZ = 0.0f;

    glBegin(GL_LINE_LOOP);
    for (int i = 0; i <= numSegments; i++) {
        GLfloat angle = 2.0f * 3.14159f * i / numSegments;
        GLfloat x = centerX + (radius * cos(angle));
        GLfloat z = centerZ + (radius * sin(angle));
        glVertex3f(x, 0.02f, z);
    }
    glEnd();

    float red = 0.8f;
    float green = 0.5f;
    float blue = 0.2f;

    if (isColorChangingFence) {
        colorTimerFence += 0.01f;
        if (colorTimerFence > 1.0f) colorTimerFence = 0.0f;

        red = 0.8f + 0.2f * sin(colorTimerFence * 2 * M_PI);
        green = 0.5f + 0.2f * sin(colorTimerFence * 2 * M_PI + 1.0f);
        blue = 0.2f + 0.2f * sin(colorTimerFence * 2 * M_PI + 2.0f);
    }

    glColor3f(red, green, blue);

    glBegin(GL_QUADS);
    glVertex3f(-3.0f, 0.0f, 2.0f);
    glVertex3f(3.0f, 0.0f, 2.0f);
    glVertex3f(3.0f, fenceHeight, 2.0f);
    glVertex3f(-3.0f, fenceHeight, 2.0f);

    glVertex3f(-3.0f, 0.0f, -2.0f);
    glVertex3f(3.0f, 0.0f, -2.0f);
    glVertex3f(3.0f, fenceHeight, -2.0f);
    glVertex3f(-3.0f, fenceHeight, -2.0f);
    glEnd();

    glBegin(GL_QUADS);
    glVertex3f(-3.0f, 0.0f, -2.0f);
    glVertex3f(-3.0f, 0.0f, 2.0f);
    glVertex3f(-3.0f, fenceHeight, 2.0f);
    glVertex3f(-3.0f, fenceHeight, -2.0f);

    glVertex3f(3.0f, 0.0f, -2.0f);
    glVertex3f(3.0f, 0.0f, 2.0f);
    glVertex3f(3.0f, fenceHeight, 2.0f);
    glVertex3f(3.0f, fenceHeight, -2.0f);
    glEnd();

    glColor3f(0.4f, 0.4f, 0.4f);

    glBegin(GL_TRIANGLES);
    glVertex3f(-3.0f, 0.0f, -2.0f);
    glVertex3f(-3.0f, fenceHeight / 2.0f, 0.0f);
    glVertex3f(-3.0f, 0.0f, 2.0f);
    glEnd();

    glBegin(GL_TRIANGLES);
    glVertex3f(3.0f, 0.0f, -2.0f);
    glVertex3f(3.0f, fenceHeight / 2.0f, 0.0f);
    glVertex3f(3.0f, 0.0f, 2.0f);
    glEnd();
}



void drawOlympicLogo(float scale, float rotation) {
    float ringRadius = 0.1f * scale;
    float ringThickness = 0.02f * scale;

    struct Ring {
        float offsetX, offsetY;
        float color[3];
    };

    Ring rings[] = {
        { -0.22f, 0.0f, { 0.0f, 0.4f, 1.0f } },
        { 0.0f, 0.0f, { 0.0f, 0.0f, 0.0f } },
        { 0.22f, 0.0f, { 1.0f, 0.0f, 0.0f } },
        { -0.11f, -0.12f, { 1.0f, 0.85f, 0.0f } },
        { 0.11f, -0.12f, { 0.0f, 1.0f, 0.0f } }
    };

    glPushMatrix();
    glTranslatef(0.0f, fenceHeight + 0.1f, -1.5f);

    glRotatef(rotation, 0.0f, 0.0f, 1.0f);

    for (const auto& ring : rings) {
        glPushMatrix();
        glTranslatef(ring.offsetX * scale, ring.offsetY * scale, 0.0f);
        glColor3fv(ring.color);
        glutSolidTorus(ringThickness, ringRadius, 30, 30);
        glPopMatrix();
    }

    glPopMatrix();
}




void drawGoal(float x, float z, float rotation) {
    glColor3f(0.0f, 0.0f, 1.0f);

    glPushMatrix();
    glTranslated(x, 0.15, z);
    glRotatef(rotation, 0.0f, 1.0f, 0.0f);

   
    glPushMatrix();
    glScaled(0.05, 0.3, 0.05);
    glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glTranslated(0.0, 0.0, -0.5);
    glScaled(0.05, 0.3, 0.05);
    glutSolidCube(1.0);
    glPopMatrix();

   
    glPushMatrix();
    glTranslated(0.0, 0.15, -0.25);
    glScaled(0.05, 0.05, 0.5);
    glutSolidCube(1.0);
    glPopMatrix();


    glPopMatrix();
}

void drawPlayer() {
    
    glPushMatrix();
    
    glTranslatef(playerPosX, playerPosY, playerPosZ);
    glRotatef(playerRotationAngle, 0.0f, 1.0f, 0.0f);

    glColor3f(0.8f, 0.52f, 0.25f);
    glPushMatrix();
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    glTranslatef(-0.03f, 0.1f, 0.0f);
    glRotatef(-90, 1.0f, 0.0f, 0.0f);
    glScalef(0.5f, 0.5f, 0.5f);
    gluCylinder(gluNewQuadric(), 0.03, 0.03, 0.2, 10, 10);
    glPopMatrix();

    glColor3f(0.8f, 0.52f, 0.25f);
    glPushMatrix();
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    glTranslatef(0.03f, 0.1f, 0.0f);
    glRotatef(-90, 1.0f, 0.0f, 0.0f);
    glScalef(0.5f, 0.5f, 0.5f);
    gluCylinder(gluNewQuadric(), 0.03, 0.03, 0.2, 10, 10);
    glPopMatrix();

    glColor3f(0.2f, 0.2f, 0.5f);
    glPushMatrix();
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    glTranslatef(0.0f, 0.211111f, 0.0f);
    glScalef(0.079f, 0.04f, 0.06f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor3f(0.1f, 0.5f, 0.1f);
    glPushMatrix();
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    glTranslatef(0.0f, 0.27f, 0.0f);
    glScalef(0.05f, 0.1f, 0.05f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor3f(0.8f, 0.52f, 0.25f);
    glPushMatrix();
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    glTranslatef(-0.085f, 0.25f, 0.0f);
    glRotatef(-90, 1.0f, 0.0f, 0.0f);
    glRotatef(45, 0.0f, 1.0f, 0.0f);
    glScalef(0.2f, 0.2f, 0.4f);
    gluCylinder(gluNewQuadric(), 0.02, 0.02, 0.2, 10, 10);
    glPopMatrix();

    glColor3f(0.8f, 0.52f, 0.25f);
    glPushMatrix();
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    glTranslatef(0.085f, 0.25f, 0.0f);
    glRotatef(-90, 1.0f, 0.0f, 0.0f);
    glRotatef(-45, 0.0f, 1.0f, 0.0f);
    glScalef(0.2f, 0.2f, 0.4f);
    gluCylinder(gluNewQuadric(), 0.02, 0.02, 0.2, 10, 10);
    glPopMatrix();

    glColor3f(0.8f, 0.52f, 0.25f);
    glPushMatrix();
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    glTranslatef(0.0f, 0.37f, 0.0f);
    glScalef(0.12f, 0.12f, 0.12f);
    glutSolidSphere(0.5f, 10, 10);
    glPopMatrix();
    
    glPopMatrix();
}


void drawCoach() {
    glPushMatrix();
    glScalef(coachScale, coachScale, coachScale);

    glTranslatef(-1.5, 0, -1.5);
    glRotatef(playerRotationAngle, 0.0f, 1.0f, 0.0f);

    glColor3f(0.8f, 0.52f, 0.25f);
    glPushMatrix();
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    glTranslatef(-0.03f, 0.1f, 0.0f);
    glRotatef(-90, 1.0f, 0.0f, 0.0f);
    glScalef(0.5f, 0.5f, 0.5f);
    gluCylinder(gluNewQuadric(), 0.03, 0.03, 0.2, 10, 10);
    glPopMatrix();

    glColor3f(0.8f, 0.52f, 0.25f);
    glPushMatrix();
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    glTranslatef(0.03f, 0.1f, 0.0f);
    glRotatef(-90, 1.0f, 0.0f, 0.0f);
    glScalef(0.5f, 0.5f, 0.5f);
    gluCylinder(gluNewQuadric(), 0.03, 0.03, 0.2, 10, 10);
    glPopMatrix();

    glColor3f(0.0f, 0.0f, 0.0f);
    glPushMatrix();
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    glTranslatef(0.0f, 0.211111f, 0.0f);
    glScalef(0.079f, 0.04f, 0.06f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor3f(1.0f, 1.0f, 1.0f);
    glPushMatrix();
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    glTranslatef(0.0f, 0.27f, 0.0f);
    glScalef(0.05f, 0.1f, 0.05f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor3f(0.8f, 0.52f, 0.25f);
    glPushMatrix();
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    glTranslatef(-0.085f, 0.25f, 0.0f);
    glRotatef(-90, 1.0f, 0.0f, 0.0f);
    glRotatef(45, 0.0f, 1.0f, 0.0f);
    glScalef(0.2f, 0.2f, 0.4f);
    gluCylinder(gluNewQuadric(), 0.02, 0.02, 0.2, 10, 10);
    glPopMatrix();

    glColor3f(0.8f, 0.52f, 0.25f);
    glPushMatrix();
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    glTranslatef(0.085f, 0.25f, 0.0f);
    glRotatef(-90, 1.0f, 0.0f, 0.0f);
    glRotatef(-45, 0.0f, 1.0f, 0.0f);
    glScalef(0.2f, 0.2f, 0.4f);
    gluCylinder(gluNewQuadric(), 0.02, 0.02, 0.2, 10, 10);
    glPopMatrix();

    glColor3f(0.8f, 0.52f, 0.25f);
    glPushMatrix();
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    glTranslatef(0.0f, 0.37f, 0.0f);
    glScalef(0.12f, 0.12f, 0.12f);
    glutSolidSphere(0.5f, 10, 10);
    glPopMatrix();
    
    glPopMatrix();
}

float ballScale = 1.0f;
float scaleDirection = 1.0f;

void animateBallScale() {
    ballScale += scaleDirection * scaleSpeed;

    if (ballScale >= 1.5f) scaleDirection = -1.0f;
    if (ballScale <= 0.5f) scaleDirection = 1.0f;
}


void drawRandomBall() {
    glColor3f(1.0f, 0.5f, 0.0f);

    glPushMatrix();
    glTranslatef(xBall, 0.1f, zBall);
    glScalef(ballScale, ballScale, ballScale);

    glutSolidSphere(0.05f, 20, 20);

    glColor3f(1.0f, 1.0f, 1.0f);

    glPushMatrix();
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
        glVertex3f(0.0f, 0.0f, -0.05f);
        glVertex3f(0.0f, 0.0f, 0.05f);
    glEnd();
    glPopMatrix();

    glPushMatrix();
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    glBegin(GL_LINES);
        glVertex3f(0.0f, 0.0f, -0.05f);
        glVertex3f(0.0f, 0.0f, 0.05f);
    glEnd();
    glPopMatrix();

    glPopMatrix();
}

bool isColorChanging = false;
float colorTimer = 0.0f;


void drawTable() {
    float red = 0.8f;
    float green = 0.5f;
    float blue = 0.2f;

    if (isColorChanging) {
        colorTimer += 0.01f;
        if (colorTimer > 1.0f) colorTimer = 0.0f;

        red = 0.8f + 0.2f * sin(colorTimer * 2 * M_PI);
        green = 0.5f + 0.2f * sin(colorTimer * 2 * M_PI + 1.0f);
        blue = 0.2f + 0.2f * sin(colorTimer * 2 * M_PI + 2.0f);
    }

    glColor3f(red, green, blue);

    glPushMatrix();
    glTranslatef(0.0f, 0.1f, -1.7f);
    glScalef(0.5f, 0.1f, 0.4f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor3f(0.5f, 0.3f, 0.1f);

    glPushMatrix();
    glTranslatef(-0.4f, -0.45f, -0.3f);
    glScalef(0.1f, 0.9f, 0.1f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.4f, -0.45f, -0.3f);
    glScalef(0.1f, 0.9f, 0.1f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.4f, -0.45f, 0.3f);
    glScalef(0.1f, 0.9f, 0.1f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.4f, -0.45f, 0.3f);
    glScalef(0.1f, 0.9f, 0.1f);
    glutSolidCube(1.0f);
    glPopMatrix();
}

bool isAnimating = false;
float chairOffset = 0.0f;

void drawChair() {
    if (isAnimating) {
        chairOffset = sin(glutGet(GLUT_ELAPSED_TIME) / 500.0f) * 0.5f;
    }

    glPushMatrix();
    glTranslatef(0.2f + chairOffset, 0.0f, -1.5f);
    glScalef(0.2f, 0.3f, 0.2f);

    glColor3f(0.8f, 0.5f, 0.2f);
    glPushMatrix();
    glTranslatef(0.7f, 0.3f, -1.5f);
    glScalef(0.6f, 0.1f, 0.6f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor3f(0.5f, 0.3f, 0.1f);
    glPushMatrix();
    glTranslatef(0.7f, 0.7f, -1.5f);
    glScalef(0.6f, 0.5f, 0.1f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor3f(0.5f, 0.3f, 0.1f);

    glPushMatrix();
    glTranslatef(0.4f, 0.0f, -1.2f);
    glScalef(0.1f, 0.6f, 0.1f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1.0f, 0.0f, -1.2f);
    glScalef(0.1f, 0.6f, 0.1f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.4f, 0.0f, -1.8f);
    glScalef(0.1f, 0.6f, 0.1f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1.0f, 0.0f, -1.8f);
    glScalef(0.1f, 0.6f, 0.1f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPopMatrix();
}


/////END Draw//////
int ballsCollected = 0;

void renderBitmapString(float x, float y, void *font, const char *string) {
    glRasterPos2f(x, y);
    while (*string) {
        glutBitmapCharacter(font, *string);
        string++;
    }
}

void displayBallCounter() {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 100, 0, 100);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glColor3f(1.0f, 1.0f, 1.0f);
    char buffer[50];
    sprintf(buffer, "Balls Collected: %d", ballsCollected);
    renderBitmapString(5, 95, GLUT_BITMAP_HELVETICA_18, buffer);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}


int gameTime = 20;
bool isGameOver = false;
bool gameWon = false;

void drawTimer() {
    
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.0f, 1.0f, 0.0f, 1.0f);

   
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glColor3f(0.0f, 0.0f, 0.0f);

   
    char timerText[20];
    sprintf(timerText, "Time: %d", gameTime);

   
    glRasterPos2f(0.85f, 0.95f);

   
    for (char* c = timerText; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }

    
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void updateTimer(int value) {
    if (gameTime > 0) {
        gameTime--;
        glutTimerFunc(1000, updateTimer, 0);
    } else {
        isGameOver = true;
    }
    glutPostRedisplay();
}

void drawGameOverScreen() {
    glClear(GL_COLOR_BUFFER_BIT);

   
    glColor3f(0.0f, 0.0f, 0.0f);

   
    glRasterPos2f(-0.2f, 0.0f);

    
    const char* gameOverText = "Time's Up, GAME LOSE";
    for (const char* c = gameOverText; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }

    glutSwapBuffers();
}

void renderWinScreen() {
   
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(-1.0f, 1.0f, -1.0f, 1.0f);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    
    glColor3f(0.0f, 1.0f, 0.0f);
    glRasterPos2f(-0.2f, 0.0f);
    const char* winText = "GAME WIN!";
    for (const char* c = winText; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }

    
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    
    glutSwapBuffers();
}



void checkWinCondition() {
    if (ballsCollected >= 2) {
        gameWon = true;
    }
}
void checkBallCollision() {
    if (playerPosX + 0.2f > xBall && playerPosX - 0.2f < xBall &&
        playerPosZ + 0.2f > zBall && playerPosZ - 0.2f < zBall) {
        ballsCollected += 1;
        xBall = -2.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (5.0f)));
        zBall = -1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (3.0f)));
    }
    checkWinCondition();
}

void Special(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_LEFT:
        movePlayer(0.0f, 0.0f, 1.0f);
        break;
    case GLUT_KEY_RIGHT:
        movePlayer(0.0f, 0.0f, -1.0f);
        break;
    case GLUT_KEY_UP:
        movePlayer(-1.0f, 0.0f, 0.0f);
        break;
    case GLUT_KEY_DOWN:
        movePlayer(1.0f, 0.0f, 0.0f);
        break;
    }
    glutPostRedisplay();
}





void setupLights() {
    GLfloat ambient[] = { 0.7f, 0.7f, 0.7, 1.0f };
    GLfloat diffuse[] = { 0.6f, 0.6f, 0.6, 1.0f };
    GLfloat specular[] = { 1.0f, 1.0f, 1.0, 1.0f };
    GLfloat shininess[] = { 50 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

    GLfloat lightIntensity[] = { 0.7f, 0.7f, 1, 1.0f };
    GLfloat lightPosition[] = { -7.0f, 6.0f, 3.0f, 0.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightIntensity);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightIntensity);
}

void setupCamera() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, 640.0 / 480.0, 0.001, 100);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    camera.look();
}


void Display() {
   

   
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (gameWon)
            renderWinScreen();
    else{
        
        if (isGameOver) {
            
            glClearColor(1.7f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            
           
            glColor3f(0.0f, 0.0f, 0.0f);
            char message[] = "Time's Up, GAME LOSE!";
            glRasterPos2f(-0.15f, 0.0f);
            for (char* c = message; *c != '\0'; c++) {
                glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
            }
        } else {
            setupCamera();
            setupLights();
           
            drawHandballCourt();
            drawTimer();
            drawRandomBall();
            drawPlayer();
            drawCoach();
            drawTable();
            drawChair();
            checkBallCollision();
            displayBallCounter();
            drawOlympicLogo(1.0f, logoRotation);
            if (animateLogo) {
                    logoRotation += 0.5f;
                    if (logoRotation >= 360.0f) {
                        logoRotation = 0.0f;
                    }
                }
            if (animateLogo) {
                goalRotation += 0.5f;
                if (goalRotation >= 360.0f) {
                    goalRotation = 0.0f;
                }
            }

          
            drawGoal(-2.8f, 0.0f, goalRotation);
            drawGoal(2.8f, 0.0f, goalRotation);
        }
    }

    glFlush();
}

void updateCoachScale() {
    if (animateCoachScaling) {
     
        coachScale += scaleSpeed * scalingDirection;

        if (coachScale >= targetCoachScale) {
            scalingDirection = -1;
        } else if (coachScale <= originalCoachScale) {
            scalingDirection = 1;
        }

       
        glutPostRedisplay();
    }
}



void Keyboard(unsigned char key, int x, int y) {
   
    float moveSensitivity = 0.1f;

    float rotateSensitivity = 1.3f;

    switch (key) {
        
        case 'w':
            camera.moveY(moveSensitivity);
            break;
        case 's':
            camera.moveY(-moveSensitivity);
            break;
        case 'a':
            camera.moveX(moveSensitivity);
            break;
        case 'd':
            camera.moveX(-moveSensitivity);
            break;
        case 'q':
            camera.moveZ(moveSensitivity);
            break;
        case 'e':
            camera.moveZ(-moveSensitivity);
            break;

       
        case 't':
            camera.rotateX(-rotateSensitivity);
            break;
        case 'g':
            camera.rotateX(rotateSensitivity);
            break;
        case 'f':
            camera.rotateY(-rotateSensitivity);
            break;
        case 'h':
            camera.rotateY(rotateSensitivity);
            break;

       
        case 'p':
            animateLogo = !animateLogo;
            if (animateLogo) {
                logoRotation = 0.0f;
            }
            break;
        case 'o':
                   if (!isScaling) {
                      
                       animateCoachScaling = true;
                       isScaling = true;
                       scalingDirection = 1;
                   } else {
                      
                       coachScale = originalCoachScale;
                       animateCoachScaling = false;
                       isScaling = false;
                   }
                   break;
        case 'i':
                   isAnimating = !isAnimating;
                   break;
        case 'u':
                    isColorChanging = !isColorChanging;
                    colorTimer = 0.0f;
                    break;
        case '1':
                   camera.eye = Vector3f(0.0f, 1.0f, 3.0f);  
                   camera.center = Vector3f(0.0f, 1.0f, 0.0f);
                   camera.up = Vector3f(0.0f, 1.0f, 0.0f);
                   break;
               case '2':
                   camera.eye = Vector3f(0.0f, 5.0f, 0.0f);
                   camera.center = Vector3f(0.0f, 0.0f, 0.0f);
                   camera.up = Vector3f(0.0f, 0.0f, 1.0f);
                   break;
               case '3':
                   camera.eye = Vector3f(3.0f, 1.0f, 0.0f);
                   camera.center = Vector3f(0.0f, 1.0f, 0.0f);
                   camera.up = Vector3f(0.0f, 1.0f, 0.0f);
                   break;


        
        case 27:
            exit(EXIT_SUCCESS);
            break;

        default:
            break;
    }

   
    glutPostRedisplay();
}

void init() {
    glEnable(GL_DEPTH_TEST);

    glClearColor(0.53f, 0.81f, 0.92f, 1.0f);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, 1.0f, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

void update(int value) {
    animateBallScale();
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    
    glutInitWindowSize(640, 480);
    glutInitWindowPosition(150, 150);

    glutCreateWindow("Handball Game");

    glutDisplayFunc(Display);
    glutKeyboardFunc(Keyboard);
    glutIdleFunc(updateCoachScale);
    glutSpecialFunc(Special);
    glutTimerFunc(16, update, 0);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glutTimerFunc(1000, updateTimer, 0);
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);

    glShadeModel(GL_SMOOTH);

    glutMainLoop();
}
