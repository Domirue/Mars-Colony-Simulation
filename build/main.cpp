#include <GL/freeglut.h>
#include <cstdio>
#include <cmath>
#include <cstdlib>

GLuint texLazik, texRakieta, texKapsula;
GLuint texPodloze = 0;
GLuint texSkybox[6] = {0,0,0,0,0,0};

float camX = 0.0f, camY = 8.0f, camZ = 16.0f;
float cameraAngle = 0.0f;
float cameraPitch = 0.0f;
float ufoAngle = 0.0f;
float alienX = 45.0f, alienZ = 35.0f;
float alienAngle = 0.5f;
float alienWalk = 0.0f;

#define NPARTICLES 100
struct Particle { float x, y, z, vy, vx, vz, size, alpha; };
Particle parts[NPARTICLES];
float geyserX = -35.0f, geyserZ = 35.0f;
bool showUFO = true;
bool showGeyserParts = true;
bool showAlien = true;
bool showFog = true;
float lazikX = -14.0f, lazikZ = -18.0f;
float rakietaX = 12.0f, rakietaZ = -32.0f;
float lazikScale = 1.0f;
float rakietaScale = 4.0f;
bool rakietaFly = false;
float rakietaFlyY = 0.0f;
float origRakietaX = 12.0f, origRakietaZ = -32.0f;
float flameFlicker = 0.0f;

int dragMode = 0;
int winW = 800, winH = 600;
int lastMouseX = 0;
int lastMouseY = 0;
bool rotateCamera = false;
GLubyte tex_lazik[8][8][3] = {
    {{90,90,85},{115,115,105},{90,90,85},{115,115,105},{90,90,85},{115,115,105},{90,90,85},{115,115,105}},
    {{120,120,110},{150,150,135},{120,120,110},{150,150,135},{120,120,110},{150,150,135},{120,120,110},{150,150,135}},
    {{80,80,78},{80,80,78},{80,80,78},{80,80,78},{80,80,78},{80,80,78},{80,80,78},{80,80,78}},
    {{130,130,120},{160,160,145},{130,130,120},{160,160,145},{130,130,120},{160,160,145},{130,130,120},{160,160,145}},
    {{100,100,92},{130,130,118},{100,100,92},{130,130,118},{100,100,92},{130,130,118},{100,100,92},{130,130,118}},
    {{80,80,78},{80,80,78},{80,80,78},{80,80,78},{80,80,78},{80,80,78},{80,80,78},{80,80,78}},
    {{115,115,105},{145,145,132},{115,115,105},{145,145,132},{115,115,105},{145,145,132},{115,115,105},{145,145,132}},
    {{95,95,88},{120,120,110},{95,95,88},{120,120,110},{95,95,88},{120,120,110},{95,95,88},{120,120,110}}
};
GLubyte tex_rakieta[8][8][3] = {
    {{220,220,215},{230,230,225},{220,220,215},{230,230,225},{220,220,215},{230,230,225},{220,220,215},{230,230,225}},
    {{220,220,215},{230,230,225},{220,220,215},{230,230,225},{220,220,215},{230,230,225},{220,220,215},{230,230,225}},
    {{180,40,35},{180,40,35},{180,40,35},{180,40,35},{180,40,35},{180,40,35},{180,40,35},{180,40,35}},
    {{220,220,215},{230,230,225},{220,220,215},{230,230,225},{220,220,215},{230,230,225},{220,220,215},{230,230,225}},
    {{220,220,215},{230,230,225},{220,220,215},{230,230,225},{220,220,215},{230,230,225},{220,220,215},{230,230,225}},
    {{80,80,85},{80,80,85},{80,80,85},{80,80,85},{80,80,85},{80,80,85},{80,80,85},{80,80,85}},
    {{220,220,215},{230,230,225},{220,220,215},{230,230,225},{220,220,215},{230,230,225},{220,220,215},{230,230,225}},
    {{220,220,215},{230,230,225},{220,220,215},{230,230,225},{220,220,215},{230,230,225},{220,220,215},{230,230,225}}
};
GLubyte tex_kapsula[8][8][3] = {
    {{200,200,210},{200,200,210},{200,200,210},{220,220,230},{220,220,230},{200,200,210},{200,200,210},{200,200,210}},
    {{200,200,210},{200,200,210},{200,200,210},{220,220,230},{220,220,230},{200,200,210},{200,200,210},{200,200,210}},
    {{180,120,80},{180,120,80},{180,120,80},{180,120,80},{180,120,80},{180,120,80},{180,120,80},{180,120,80}},
    {{200,200,210},{200,200,210},{200,200,210},{220,220,230},{220,220,230},{200,200,210},{200,200,210},{200,200,210}},
    {{200,200,210},{200,200,210},{200,200,210},{220,220,230},{220,220,230},{200,200,210},{200,200,210},{200,200,210}},
    {{180,120,80},{180,120,80},{180,120,80},{180,120,80},{180,120,80},{180,120,80},{180,120,80},{180,120,80}},
    {{200,200,210},{200,200,210},{200,200,210},{220,220,230},{220,220,230},{200,200,210},{200,200,210},{200,200,210}},
    {{200,200,210},{200,200,210},{200,200,210},{220,220,230},{220,220,230},{200,200,210},{200,200,210},{200,200,210}}
};

void makeTexture(GLuint& tex, GLubyte data[8][8][3])
{
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 8, 8, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
}
bool loadBMP(const char* filename, GLuint& texture)
{
    FILE* file = fopen(filename, "rb");
    if (!file) return false;
    unsigned char header[54];
    fread(header, 1, 54, file);
    int width = *(int*)&header[18];
    int height = *(int*)&header[22];
    int offset = *(int*)&header[10];
    int rowSize = (width * 3 + 3) & (~3);
    unsigned char* row = new unsigned char[rowSize];
    unsigned char* data = new unsigned char[width * height * 3];
    fseek(file, offset, SEEK_SET);
    for (int y = 0; y < height; y++)
    {
        fread(row, 1, rowSize, file);
        for (int x = 0; x < width; x++)
        {
            int i = ((height - 1 - y) * width + x) * 3;
            int j = x * 3;
            data[i] = row[j + 2];
            data[i + 1] = row[j + 1];
            data[i + 2] = row[j];
        }
    }
    fclose(file);
    delete[] row;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    delete[] data;
    return true;
}
bool loadSkyboxTextures()
{
    const char* faces[6] = {"Right.bmp","Left.bmp","Top.bmp","Bottom.bmp","Front.bmp","Back.bmp"};
    bool ok = true;
    for (int i = 0; i < 6; i++)
    {
        if (!loadBMP(faces[i], texSkybox[i])) ok = false;
    }
    return ok;
}

void ustawMaterial()
{
    GLfloat diff[] = { 1, 1, 1, 1 };
    GLfloat spec[] = { 0.5f, 0.5f, 0.5f, 1 };
    GLfloat shin[] = { 35 };

    glMaterialfv(GL_FRONT, GL_DIFFUSE, diff);
    glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
    glMaterialfv(GL_FRONT, GL_SHININESS, shin);
}

void CubeTextured(GLuint tex)
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex);
    glColor3f(1, 1, 1);

    glBegin(GL_QUADS);

    glNormal3f(0, 0, 1);
    glTexCoord2f(0, 0); glVertex3f(-0.5, -0.5, 0.5);
    glTexCoord2f(1, 0); glVertex3f(0.5, -0.5, 0.5);
    glTexCoord2f(1, 1); glVertex3f(0.5, 0.5, 0.5);
    glTexCoord2f(0, 1); glVertex3f(-0.5, 0.5, 0.5);

    glNormal3f(0, 0, -1);
    glTexCoord2f(0, 0); glVertex3f(0.5, -0.5, -0.5);
    glTexCoord2f(1, 0); glVertex3f(-0.5, -0.5, -0.5);
    glTexCoord2f(1, 1); glVertex3f(-0.5, 0.5, -0.5);
    glTexCoord2f(0, 1); glVertex3f(0.5, 0.5, -0.5);

    glNormal3f(-1, 0, 0);
    glTexCoord2f(0, 0); glVertex3f(-0.5, -0.5, -0.5);
    glTexCoord2f(1, 0); glVertex3f(-0.5, -0.5, 0.5);
    glTexCoord2f(1, 1); glVertex3f(-0.5, 0.5, 0.5);
    glTexCoord2f(0, 1); glVertex3f(-0.5, 0.5, -0.5);

    glNormal3f(1, 0, 0);
    glTexCoord2f(0, 0); glVertex3f(0.5, -0.5, 0.5);
    glTexCoord2f(1, 0); glVertex3f(0.5, -0.5, -0.5);
    glTexCoord2f(1, 1); glVertex3f(0.5, 0.5, -0.5);
    glTexCoord2f(0, 1); glVertex3f(0.5, 0.5, 0.5);

    glNormal3f(0, 1, 0);
    glTexCoord2f(0, 0); glVertex3f(-0.5, 0.5, 0.5);
    glTexCoord2f(1, 0); glVertex3f(0.5, 0.5, 0.5);
    glTexCoord2f(1, 1); glVertex3f(0.5, 0.5, -0.5);
    glTexCoord2f(0, 1); glVertex3f(-0.5, 0.5, -0.5);

    glNormal3f(0, -1, 0);
    glTexCoord2f(0, 0); glVertex3f(-0.5, -0.5, -0.5);
    glTexCoord2f(1, 0); glVertex3f(0.5, -0.5, -0.5);
    glTexCoord2f(1, 1); glVertex3f(0.5, -0.5, 0.5);
    glTexCoord2f(0, 1); glVertex3f(-0.5, -0.5, 0.5);

    glEnd();
    glDisable(GL_TEXTURE_2D);
}
    float Height(float x, float z)
{
    float h = 0;
    h += sin(x * 0.08f) * 1.5f;
    h += cos(z * 0.06f) * 1.2f;
    h += sin((x + z) * 0.12f) * 0.8f;
    h += cos((x * 0.7f - z * 0.5f) * 0.15f) * 0.6f;
    float dx = x - 15.0f;
    float dz = z - 5.0f;
    float d = sqrt(dx*dx + dz*dz);
    h -= 2.0f * exp(-d * d * 0.005f);
    if (h < -3.0f) h = -3.0f;
    if (h > 4.5f) h = 4.5f;
    return h;
}

void Podloze()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texPodloze);
    glColor3f(1, 1, 1);

    const int N = 100;
    const float SIZE = 90.0f;

    for (int z = 0; z < N; z++)
    {
        glBegin(GL_TRIANGLE_STRIP);

        for (int x = 0; x <= N; x++)
        {
            float worldX = -SIZE + (2.0f * SIZE * x) / N;

            float worldZ1 = -SIZE + (2.0f * SIZE * z) / N;
            float worldZ2 = -SIZE + (2.0f * SIZE * (z + 1)) / N;

            float h1 = Height(worldX, worldZ1);
            float h2 = Height(worldX, worldZ2);

            glNormal3f(0, 1, 0);

            glTexCoord2f((float)x / N,
                         (float)z / N);
            glVertex3f(worldX, h1, worldZ1);

            glTexCoord2f((float)x / N,
                         (float)(z + 1) / N);
            glVertex3f(worldX, h2, worldZ2);
        }

        glEnd();
    }

    glDisable(GL_TEXTURE_2D);
}

void Tlo()
{
    if (!texSkybox[0]) return;

    glDisable(GL_LIGHTING);
    glDepthMask(GL_FALSE);

    float s = 90.0f;
    float top = 60.0f;
    float bot = -10.0f;

    glEnable(GL_TEXTURE_2D);
    glColor3f(1, 1, 1);

    glBindTexture(GL_TEXTURE_2D, texSkybox[0]);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 1); glVertex3f(s, bot, s);
    glTexCoord2f(1, 1); glVertex3f(s, bot, -s);
    glTexCoord2f(1, 0); glVertex3f(s, top, -s);
    glTexCoord2f(0, 0); glVertex3f(s, top, s);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, texSkybox[1]);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 1); glVertex3f(-s, bot, -s);
    glTexCoord2f(1, 1); glVertex3f(-s, bot, s);
    glTexCoord2f(1, 0); glVertex3f(-s, top, s);
    glTexCoord2f(0, 0); glVertex3f(-s, top, -s);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, texSkybox[2]);
    glBegin(GL_QUADS);
    glTexCoord2f(1, 1); glVertex3f(-s, top, -s);
    glTexCoord2f(0, 1); glVertex3f(s, top, -s);
    glTexCoord2f(0, 0); glVertex3f(s, top, s);
    glTexCoord2f(1, 0); glVertex3f(-s, top, s);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, texSkybox[3]);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 1); glVertex3f(-s, bot, s);
    glTexCoord2f(1, 1); glVertex3f(s, bot, s);
    glTexCoord2f(1, 0); glVertex3f(s, bot, -s);
    glTexCoord2f(0, 0); glVertex3f(-s, bot, -s);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, texSkybox[4]);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 1); glVertex3f(s, bot, -s);
    glTexCoord2f(1, 1); glVertex3f(-s, bot, -s);
    glTexCoord2f(1, 0); glVertex3f(-s, top, -s);
    glTexCoord2f(0, 0); glVertex3f(s, top, -s);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, texSkybox[5]);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 1); glVertex3f(-s, bot, s);
    glTexCoord2f(1, 1); glVertex3f(s, bot, s);
    glTexCoord2f(1, 0); glVertex3f(s, top, s);
    glTexCoord2f(0, 0); glVertex3f(-s, top, s);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glDepthMask(GL_TRUE);
    glEnable(GL_LIGHTING);
}

void Skala(float x, float z, float sx, float sy, float sz)
{
    glPushMatrix();
    float terrainY = Height(x, z);
    glTranslatef(x, terrainY, z);
    glScalef(sx, sy, sz);
    CubeTextured(texPodloze);
    glPopMatrix();
}

void Kolo(float x, float y, float z)
{
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(90, 0, 1, 0);
    glColor3f(0.05f, 0.05f, 0.05f);
    glutSolidTorus(0.08, 0.28, 12, 20);
    glPopMatrix();
}

void Lazik()
{
    glPushMatrix();
    float terrainY = Height(lazikX, lazikZ);
glTranslatef(lazikX,
             terrainY + 0.8f * lazikScale,
             lazikZ);
    glScalef(lazikScale, lazikScale, lazikScale);

    glPushMatrix();
    glScalef(3.5f, 0.8f, 2.0f);
    CubeTextured(texLazik);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.3f, 0.75f, 0.0f);
    glScalef(1.4f, 0.7f, 1.2f);
    CubeTextured(texLazik);
    glPopMatrix();

    Kolo(-1.4f, -0.35f, 1.05f);
    Kolo(0.0f, -0.35f, 1.05f);
    Kolo(1.4f, -0.35f, 1.05f);
    Kolo(-1.4f, -0.35f, -1.05f);
    Kolo(0.0f, -0.35f, -1.05f);
    Kolo(1.4f, -0.35f, -1.05f);

    glDisable(GL_TEXTURE_2D);
    glColor3f(0.2f, 0.2f, 0.2f);

    glPushMatrix();
    glTranslatef(1.5f, 1.5f, 0.0f);
    glScalef(0.12f, 1.8f, 0.12f);
    glutSolidCube(1.0);
    glPopMatrix();

    glColor3f(0.1f, 0.1f, 0.1f);

    glPushMatrix();
    glTranslatef(1.5f, 2.45f, 0.0f);
    glScalef(1.0f, 0.12f, 0.7f);
    glutSolidCube(1.0);
    glPopMatrix();

    glPopMatrix();
}

void Rakieta()
{
    glPushMatrix();
    float terrainY = Height(rakietaX, rakietaZ);
glTranslatef(rakietaX,
             terrainY + rakietaFlyY,
             rakietaZ);
    glScalef(rakietaScale, rakietaScale, rakietaScale);

    glPushMatrix();
    glTranslatef(0.0f, 3.0f, 0.0f);
    glScalef(1.4f, 6.0f, 1.4f);
    CubeTextured(texRakieta);
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
    glColor3f(0.8f, 0.1f, 0.08f);

    glPushMatrix();
    glTranslatef(0.0f, 6.0f, 0.0f);
    glRotatef(-90, 1, 0, 0);
    glutSolidCone(0.9, 1.6, 24, 12);
    glPopMatrix();

    glColor3f(0.75f, 0.1f, 0.08f);

    glBegin(GL_TRIANGLES);
    glVertex3f(-0.7f, 0.0f, 0.7f); glVertex3f(-2.0f, 0.0f, 0.7f); glVertex3f(-0.7f, 2.0f, 0.7f);
    glVertex3f(0.7f, 0.0f, 0.7f); glVertex3f(2.0f, 0.0f, 0.7f); glVertex3f(0.7f, 2.0f, 0.7f);
    glVertex3f(-0.7f, 0.0f, -0.7f); glVertex3f(-2.0f, 0.0f, -0.7f); glVertex3f(-0.7f, 2.0f, -0.7f);
    glVertex3f(0.7f, 0.0f, -0.7f); glVertex3f(2.0f, 0.0f, -0.7f); glVertex3f(0.7f, 2.0f, -0.7f);
    glEnd();

    glColor3f(0.05f, 0.05f, 0.07f);

    glPushMatrix();
    glTranslatef(0.0f, 3.8f, 0.72f);
    glutSolidSphere(0.35, 20, 20);
    glPopMatrix();

    if (rakietaFly)
    {
        glDisable(GL_LIGHTING);
        float f = sin(flameFlicker) * 0.15f + 1.0f;
        glColor3f(1.0f, 0.4f, 0.0f);
        glPushMatrix();
        glTranslatef(0.0f, -0.2f, 0.0f);
        glRotatef(-90, 1, 0, 0);
        glScalef(1.0f, 1.0f, f);
        glutSolidCone(0.5f, 2.5f, 12, 6);
        glPopMatrix();
        glColor3f(1.0f, 0.9f, 0.3f);
        glPushMatrix();
        glTranslatef(0.0f, 0.2f, 0.0f);
        glRotatef(-90, 1, 0, 0);
        glScalef(1.0f, 1.0f, f * 1.2f);
        glutSolidCone(0.25f, 1.8f, 12, 6);
        glPopMatrix();
        glEnable(GL_LIGHTING);
    }

    glPopMatrix();
}

void PolskaFlaga()
{
    glDisable(GL_TEXTURE_2D);

    glPushMatrix();
    float terrainY = Height(-4.0f, -28.0f) + 0.35f;

    glTranslatef(-4.0f, terrainY, -28.0f);

    glColor3f(0.25f, 0.25f, 0.25f);

    glPushMatrix();
    glTranslatef(0.0f, 3.0f, 0.0f);
    glScalef(0.16f, 6.0f, 0.16f);
    glutSolidCube(1.0);
    glPopMatrix();

    glDisable(GL_LIGHTING);

    glBegin(GL_QUADS);

    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex3f(0.1f, 5.2f, 0.0f);
    glVertex3f(3.2f, 5.2f, 0.0f);
    glVertex3f(3.2f, 4.3f, 0.0f);
    glVertex3f(0.1f, 4.3f, 0.0f);

    glColor3f(0.85f, 0.0f, 0.0f);
    glVertex3f(0.1f, 4.3f, 0.0f);
    glVertex3f(3.2f, 4.3f, 0.0f);
    glVertex3f(3.2f, 3.4f, 0.0f);
    glVertex3f(0.1f, 3.4f, 0.0f);

    glEnd();

    glEnable(GL_LIGHTING);

    glPopMatrix();
}

void UFO()
{
    float flyY = 20.0f;
    float x = sin(ufoAngle) * 25.0f;
    float z = sin(ufoAngle * 2.0f) * 12.0f;
    float y = flyY + sin(ufoAngle * 3.0f) * 2.0f;

    float dx = cos(ufoAngle);
    float dz = cos(ufoAngle * 2.0f) * 2.0f;
    float yaw = atan2(dz, dx) * 180.0f / 3.14159f;
    float roll = sin(ufoAngle) * 35.0f;

    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(yaw, 0, 1, 0);
    glRotatef(roll, 0, 0, 1);

    glDisable(GL_LIGHTING);
    glColor3f(0.7f, 0.7f, 0.75f);
    glPushMatrix();
    glScalef(2.5f, 0.5f, 2.5f);
    glutSolidSphere(1.0, 20, 20);
    glPopMatrix();

    glColor3f(0.4f, 0.6f, 0.9f);
    glPushMatrix();
    glTranslatef(0.0f, 0.4f, 0.0f);
    glScalef(0.8f, 0.4f, 0.8f);
    glutSolidSphere(1.0, 20, 20);
    glPopMatrix();

    glColor3f(0.0f, 0.8f, 1.0f);
    glPushMatrix();
    glTranslatef(0.0f, 0.7f, 0.0f);
    glScalef(0.4f, 0.25f, 0.4f);
    glutSolidSphere(1.0, 16, 16);
    glPopMatrix();

    glColor3f(1.0f, 0.2f, 0.1f);
    glPushMatrix();
    glTranslatef(0.8f, 0.0f, 0.8f);
    glutSolidSphere(0.12, 8, 8);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-0.8f, 0.0f, 0.8f);
    glutSolidSphere(0.12, 8, 8);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.8f, 0.0f, -0.8f);
    glutSolidSphere(0.12, 8, 8);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-0.8f, 0.0f, -0.8f);
    glutSolidSphere(0.12, 8, 8);
    glPopMatrix();

    glEnable(GL_LIGHTING);
    glPopMatrix();
}

void Kapsula()
{
    float x = 30.0f, z = 5.0f;
    glPushMatrix();
    float ty = Height(x, z);
    glTranslatef(x, ty, z);

    float s = 3.5f;
    glScalef(s, s, s);

    GLUquadricObj *q = gluNewQuadric();
    gluQuadricTexture(q, GL_TRUE);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texKapsula);
    glColor3f(1,1,1);

    
    glPushMatrix();
    glRotatef(-90, 1, 0, 0);
    gluCylinder(q, 1.5f, 1.4f, 2.5f, 24, 6);
    
    gluDisk(q, 0, 1.4f, 24, 6);
    glPopMatrix();

    
    glPushMatrix();
    glTranslatef(0, 0.005f, 0);
    glRotatef(90, 1, 0, 0);
    gluDisk(q, 0, 1.5f, 24, 6);
    glPopMatrix();

    
    glDisable(GL_TEXTURE_2D);
    glColor3f(0.85f, 0.85f, 0.9f);
    glPushMatrix();
    glTranslatef(0, 2.5f, 0);
    glScalef(1.4f, 0.6f, 1.4f);
    glutSolidSphere(1.0, 20, 20);
    glPopMatrix();

    
    glColor3f(0.6f, 0.6f, 0.6f);
    glPushMatrix();
    glTranslatef(0, 3.2f, 0);
    glRotatef(-90, 1, 0, 0);
    glutSolidCone(0.05f, 0.6f, 8, 4);
    glPopMatrix();

    
    glColor3f(1.0f, 0.3f, 0.1f);
    glPushMatrix();
    glTranslatef(0, 3.8f, 0);
    glutSolidSphere(0.12f, 8, 8);
    glPopMatrix();

    
    glColor3f(0.5f, 0.5f, 0.5f);
    float legR = 0.1f;
    float legH = 0.6f;
    for (int i = 0; i < 4; i++)
    {
        float a = i * 3.14159f / 2 + 3.14159f / 4;
        float lx = cos(a) * 1.3f;
        float lz = sin(a) * 1.3f;
        glPushMatrix();
        glTranslatef(lx, -legH, lz);
        glRotatef(-90, 1, 0, 0);
        glutSolidCone(legR, legH, 8, 4);
        glPopMatrix();
    }

    
    glColor3f(0.4f, 0.4f, 0.4f);
    for (int i = 0; i < 4; i++)
    {
        float a = i * 3.14159f / 2 + 3.14159f / 4;
        float lx = cos(a) * 1.3f;
        float lz = sin(a) * 1.3f;
        glPushMatrix();
        glTranslatef(lx, -legH - 0.05f, lz);
        glScalef(0.25f, 0.05f, 0.25f);
        glutSolidCube(1.0);
        glPopMatrix();
    }

    
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.5f, 0.85f, 1.0f, 0.55f);
    glPushMatrix();
    glTranslatef(0.0f, 1.4f, 1.55f);
    glutSolidSphere(0.4f, 16, 16);
    glPopMatrix();
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);

    
    glColor3f(0.35f, 0.3f, 0.25f);
    glBegin(GL_QUADS);
    glVertex3f(-0.25f, 0.0f, 1.41f);
    glVertex3f(0.25f, 0.0f, 1.41f);
    glVertex3f(0.25f, 0.9f, 1.41f);
    glVertex3f(-0.25f, 0.9f, 1.41f);
    glEnd();

    glColor3f(0.5f, 0.45f, 0.4f);
    glBegin(GL_LINE_LOOP);
    glVertex3f(-0.25f, 0.0f, 1.41f);
    glVertex3f(0.25f, 0.0f, 1.41f);
    glVertex3f(0.25f, 0.9f, 1.41f);
    glVertex3f(-0.25f, 0.9f, 1.41f);
    glEnd();

    
    glColor3f(0.7f, 0.7f, 0.5f);
    glPushMatrix();
    glTranslatef(0.18f, 0.45f, 1.42f);
    glutSolidSphere(0.04f, 6, 6);
    glPopMatrix();

    glEnable(GL_TEXTURE_2D);
    gluDeleteQuadric(q);
    glPopMatrix();
}

void Alien()
{
    float y = Height(alienX, alienZ);
    float legSwing = sin(alienWalk) * 25.0f;

    glPushMatrix();
    glTranslatef(alienX, y, alienZ);
    glRotatef(-alienAngle * 180.0f / 3.14159f, 0, 1, 0);

    glDisable(GL_LIGHTING);

    glColor3f(0.2f, 0.5f, 0.15f);
    glPushMatrix();
    glTranslatef(0.0f, 0.55f, 0.0f);
    glScalef(0.45f, 0.45f, 0.35f);
    glutSolidSphere(1.0, 16, 16);
    glPopMatrix();

    glColor3f(0.25f, 0.55f, 0.2f);
    glPushMatrix();
    glTranslatef(0.0f, 0.95f, 0.0f);
    glScalef(0.3f, 0.25f, 0.25f);
    glutSolidSphere(1.0, 16, 16);
    glPopMatrix();

    glColor3f(1.0f, 1.0f, 1.0f);
    glPushMatrix();
    glTranslatef(-0.12f, 1.05f, -0.2f);
    glutSolidSphere(0.1, 8, 8);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.12f, 1.05f, -0.2f);
    glutSolidSphere(0.1, 8, 8);
    glPopMatrix();

    glColor3f(0.0f, 0.0f, 0.0f);
    glPushMatrix();
    glTranslatef(-0.12f, 1.05f, -0.24f);
    glutSolidSphere(0.05, 6, 6);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.12f, 1.05f, -0.24f);
    glutSolidSphere(0.05, 6, 6);
    glPopMatrix();

    glColor3f(0.15f, 0.4f, 0.1f);
    glPushMatrix();
    glTranslatef(-0.18f, 0.15f, 0.0f);
    glRotatef(legSwing, 1, 0, 0);
    glTranslatef(0.0f, -0.15f, 0.0f);
    glScalef(0.1f, 0.3f, 0.1f);
    glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.18f, 0.15f, 0.0f);
    glRotatef(-legSwing, 1, 0, 0);
    glTranslatef(0.0f, -0.15f, 0.0f);
    glScalef(0.1f, 0.3f, 0.1f);
    glutSolidCube(1.0);
    glPopMatrix();

    glEnable(GL_LIGHTING);
    glPopMatrix();
}

void Geyser()
{
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);

    for (int i = 0; i < NPARTICLES; i++)
    {
        if (parts[i].alpha <= 0) continue;
        float a = parts[i].alpha;
        if (a > 1.0f) a = 1.0f;
        float s = parts[i].size;
        glColor4f(0.75f, 0.8f, 0.85f, a * 0.6f);
        glPushMatrix();
        glTranslatef(parts[i].x, parts[i].y, parts[i].z);
        glutSolidSphere(s, 8, 8);
        glPopMatrix();
        glColor4f(0.8f, 0.85f, 0.9f, a * 0.5f);
        glPushMatrix();
        glTranslatef(parts[i].x + s * 0.3f, parts[i].y + s * 0.2f, parts[i].z - s * 0.2f);
        glutSolidSphere(s * 0.8f, 8, 8);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(parts[i].x - s * 0.25f, parts[i].y - s * 0.15f, parts[i].z + s * 0.3f);
        glutSolidSphere(s * 0.7f, 8, 8);
        glPopMatrix();
    }

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

void GeyserBase()
{
    float gy = Height(geyserX, geyserZ);
    glDisable(GL_LIGHTING);
    glColor3f(0.15f, 0.15f, 0.15f);
    glPushMatrix();
    glTranslatef(geyserX, gy, geyserZ);
    glRotatef(-90, 1, 0, 0);
    glutSolidCone(0.5f, 0.5f, 12, 1);
    glPopMatrix();
    glColor3f(0.2f, 0.2f, 0.2f);
    for (int i = 0; i < 6; i++)
    {
        float a = (float)i * 3.14159f * 2.0f / 6.0f;
        glPushMatrix();
        glTranslatef(geyserX + cos(a) * 0.6f, gy + (float)(rand()%50)*0.002f, geyserZ + sin(a) * 0.6f);
        glScalef(0.2f, 0.12f, 0.18f);
        glutSolidSphere(1.0, 8, 8);
        glPopMatrix();
    }
    glEnable(GL_LIGHTING);
}

void initFog()
{
    GLfloat fogColor[] = { 0.55f, 0.30f, 0.25f, 1.0f };
    glEnable(GL_FOG);
    glFogfv(GL_FOG_COLOR, fogColor);
    glFogi(GL_FOG_MODE, GL_EXP);
    glFogf(GL_FOG_DENSITY, 0.018f);
    glHint(GL_FOG_HINT, GL_NICEST);
    glClearColor(0.55f, 0.30f, 0.25f, 1.0f);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (showFog) glEnable(GL_FOG);
    else glDisable(GL_FOG);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    float lookX = camX + cos(cameraPitch) * sin(cameraAngle) * 10.0f;
    float lookY = camY + sin(cameraPitch) * 10.0f;
    float lookZ = camZ - cos(cameraPitch) * cos(cameraAngle) * 10.0f;

    gluLookAt(camX, camY, camZ, lookX, lookY, lookZ, 0, 1, 0);

    GLfloat light_pos[] = { 10, 25, 10, 1 };
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

    ustawMaterial();

    Tlo();
    Podloze();
    Skala(-4.0f, -28.0f, 3.0f, 0.7f, 2.5f);
    Lazik();
    Rakieta();
    if (showUFO) UFO();
    GeyserBase();
    if (showGeyserParts) Geyser();
    if (showAlien) Alien();
    Kapsula();
    PolskaFlaga();

    glutSwapBuffers();
}

void reshape(int w, int h)
{
    if (h == 0) h = 1;
    winW = w;
    winH = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)w / (double)h, 1.0, 300.0);
    glMatrixMode(GL_MODELVIEW);
}

void mouse(int button, int state, int x, int y)
{
    if (state == GLUT_UP)
    {
        dragMode = 0;
        rotateCamera = false;
        return;
    }

    if (button == GLUT_LEFT_BUTTON) dragMode = 1;
    if (button == GLUT_RIGHT_BUTTON) dragMode = 2;

    if (button == GLUT_MIDDLE_BUTTON)
    {
        rotateCamera = true;
        lastMouseX = x;
        lastMouseY = y;
    }
}

void motion(int x, int y)
{
    if (rotateCamera)
    {
        cameraAngle += (x - lastMouseX) * 0.01f;
        cameraPitch += (lastMouseY - y) * 0.01f;
        if (cameraPitch > 89.0f) cameraPitch = 89.0f;
        if (cameraPitch < -89.0f) cameraPitch = -89.0f;
        lastMouseX = x;
        lastMouseY = y;
    }
    else
    {
        float nx = ((float)x / winW) * 40.0f - 20.0f;
        float nz = -5.0f - ((float)y / winH) * 40.0f;

        if (dragMode == 1)
        {
            lazikX = nx;
            lazikZ = nz;
        }

        if (dragMode == 2)
        {
            rakietaX = nx;
            rakietaZ = nz;
        }
    }

    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
    if (key == '=' || key == '+') camZ -= 1.0f;
    if (key == '-') camZ += 1.0f;

    if (key == 'q' || key == 'Q') lazikScale += 0.1f;

    if (key == 'a' || key == 'A')
    {
        lazikScale -= 0.1f;
        if (lazikScale < 0.2f) lazikScale = 0.2f;
    }

    if (key == 'w' || key == 'W') rakietaScale += 0.1f;

    if (key == 's' || key == 'S')
    {
        rakietaScale -= 0.1f;
        if (rakietaScale < 0.2f) rakietaScale = 0.2f;
    }

    if (key == 'r' || key == 'R')
    {
        if (rakietaFly)
        {
            rakietaFly = false;
            rakietaFlyY = 0.0f;
            rakietaX = origRakietaX;
            rakietaZ = origRakietaZ;
            rakietaScale = 4.0f;
        }
        else
        {
            rakietaFly = true;
            rakietaFlyY = 0.0f;
            rakietaX = origRakietaX;
            rakietaZ = origRakietaZ;
            rakietaScale = 4.0f;
        }
    }

    glutPostRedisplay();
}

void special(int key, int x, int y)
{
    float speed = 1.0f;

    if (key == GLUT_KEY_LEFT) cameraAngle -= 0.08f;
    if (key == GLUT_KEY_RIGHT) cameraAngle += 0.08f;

    if (key == GLUT_KEY_UP)
    {
        camX += sin(cameraAngle) * speed;
        camZ -= cos(cameraAngle) * speed;
    }

    if (key == GLUT_KEY_DOWN)
    {
        camX -= sin(cameraAngle) * speed;
        camZ += cos(cameraAngle) * speed;
    }

    glutPostRedisplay();
}

void idle()
{
    ufoAngle += 0.005f;

    float speed = 0.03f;
    alienX += sin(alienAngle) * speed;
    alienZ += cos(alienAngle) * speed;
    alienWalk += 0.04f;

    float h = Height(alienX, alienZ);
    if (alienX > 80 || alienX < -80 || alienZ > 80 || alienZ < -80 || h < -2.0f)
        alienAngle += 3.14f;

    if (rand() % 200 == 0)
        alienAngle += (float)(rand() % 628 - 314) / 100.0f;

    float gy = Height(geyserX, geyserZ);
    for (int i = 0; i < NPARTICLES; i++)
    {
        if (parts[i].alpha <= 0)
        {
            parts[i].x = geyserX + (float)(rand()%1000-500)*0.001f;
            parts[i].z = geyserZ + (float)(rand()%1000-500)*0.001f;
            parts[i].y = gy + 0.3f;
            parts[i].vx = (float)(rand()%1000-500)*0.0002f;
            parts[i].vz = (float)(rand()%1000-500)*0.0002f;
            parts[i].vy = 0.035f + (float)(rand()%100)*0.004f;
            parts[i].size = 0.08f + (float)(rand()%100)*0.006f;
            parts[i].alpha = 0.7f + (float)(rand()%100)*0.003f;
        }
        parts[i].x += parts[i].vx;
        parts[i].z += parts[i].vz;
        parts[i].y += parts[i].vy;
        parts[i].vy += 0.0005f;
        parts[i].size += 0.003f;
        parts[i].alpha -= 0.003f;
    }

    flameFlicker += 0.05f;

    if (rakietaFly)
    {
        rakietaFlyY += 0.08f;
        rakietaZ -= 0.04f;
    }

    glutPostRedisplay();
}

void menuProc(int id)
{
    switch (id)
    {
        case 1: showUFO = !showUFO; break;
        case 2: showGeyserParts = !showGeyserParts; break;
        case 3: showAlien = !showAlien; break;
        case 4: showFog = !showFog; break;
        case 5: camX=0; camY=8; camZ=16; cameraAngle=0; cameraPitch=0; break;
    }
    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Mars");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_NORMALIZE);

    GLfloat light_diff[] = { 1, 0.9f, 0.8f, 1 };
    GLfloat light_amb[] = { 0.25f, 0.18f, 0.16f, 1 };

    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diff);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_amb);

    if (!loadBMP("Bottom.bmp", texPodloze))
    {
        static GLubyte fallback[8][8][3] = {{{120,70,40}}};
        makeTexture(texPodloze, fallback);
    }

    if (!loadBMP("lazik.bmp", texLazik))
        makeTexture(texLazik, tex_lazik);

    makeTexture(texRakieta, tex_rakieta);

    makeTexture(texKapsula, tex_kapsula);
    loadSkyboxTextures();

    initFog();

    float gy = Height(geyserX, geyserZ);
    for (int i = 0; i < NPARTICLES; i++)
    {
        parts[i].x = geyserX + (float)(rand()%1000-500)*0.001f;
        parts[i].z = geyserZ + (float)(rand()%1000-500)*0.001f;
        parts[i].y = gy + 0.3f;
        parts[i].vx = (float)(rand()%1000-500)*0.0002f;
        parts[i].vz = (float)(rand()%1000-500)*0.0002f;
        parts[i].vy = 0.035f + (float)(rand()%100)*0.004f;
        parts[i].size = 0.08f + (float)(rand()%100)*0.006f;
        parts[i].alpha = 0;
    }

    glutDisplayFunc(display);
    glutIdleFunc(idle);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);

    glutCreateMenu(menuProc);
    glutAddMenuEntry("Pokaz/ukryj UFO", 1);
    glutAddMenuEntry("Pokaz/ukryj dym gejzeru", 2);
    glutAddMenuEntry("Pokaz/ukryj kosmite", 3);
    glutAddMenuEntry("Wlacz/wylacz mgle", 4);
    glutAddMenuEntry("Reset kamery", 5);
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    glutMainLoop();
    return 0;
}
