#include "map.h"

#include <SDL3/SDL_log.h>
#include <glad/gl.h>

#include <glm/glm.hpp>

#include "files.h"
#include "render.h"

void LoadMapGL(bool IsOldGL) {
  if (IsOldGL) {
    RendererGlobal->GLstuff->MapGLlist = glGenLists(1);
    glNewList(RendererGlobal->GLstuff->MapGLlist, GL_COMPILE);

    for (auto& obj : GlobalMapStuff->VisualObjectsVector) {
      for (int i = 0; i < obj.Visualmapfaces.size(); i++) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(
            GL_TEXTURE_2D,
            RendererGlobal->GLstuff
                ->textures[obj.Visualmapfaces[i].TUVvector.front().texture]);
        glBegin(GL_TRIANGLES);
        for (int j = 2; j >= 0; j--) {
          glm::vec3 pos = obj.VisualPoints[obj.Visualmapfaces[i].points[j]];
          glm::vec2 uvw = obj.Visualmapfaces[i].TUVvector.front().UVs[j];
          glColor3f(1, 1, 1);
          glTexCoord2f(uvw.x, 1 - uvw.y);
          glVertex3f(pos.x, pos.y, pos.z);
        }
        glEnd();
      }
    }

    glEnd();
    glEndList();
  } else {
    GLuint shadertemp = LoadShaders(std::vector<std::filesystem::path>(
        {"mapshader.vert", "lightingmap.frag"}));

    RendererGlobal->GLstuff->shaders["map"] = shadertemp;
    for (auto& obj : GlobalMapStuff->VisualObjectsVector) {
      RendererStuff::OpenGLRenderer::GLMapObject globjectthing;
      globjectthing.shader = shadertemp;

      globjectthing.texture =
          RendererGlobal->GLstuff
              ->textures[obj.Visualmapfaces[0].TUVvector[0].texture];
      globjectthing.shadowmaptexture =
          RendererGlobal->GLstuff
              ->textures[obj.Visualmapfaces[0].TUVvector[1].texture];

      std::vector<float> vertices;

      for (auto& i : obj.Visualmapfaces) {
        glm::vec3 tri[3];
        for (int a = 2; a >= 0; a--) {
          tri[a] = obj.VisualPoints[i.points[a]];
        }
        glm::vec3 normal =
            glm::normalize(glm::cross(tri[1] - tri[0], tri[2] - tri[0]));
        for (int a = 2; a >= 0; a--) {
          for (int j = 0; j < 3; j++) {
            vertices.push_back(obj.VisualPoints[i.points[a]][j]);
          }
          for (int j = 0; j < 3; j++) {
            vertices.push_back(normal[j]);
          }
          for (int j = 0; j < 2; j++) {
            vertices.push_back(i.TUVvector[0].UVs[a][j]);
          }
          for (int j = 0; j < 2; j++) {
            vertices.push_back(i.TUVvector[1].UVs[a][j]);
          }
          globjectthing.size++;
        }
      }
      glGenVertexArrays(1, &globjectthing.VAOthing);
      glGenBuffers(1, &globjectthing.VBOthing);

      glBindVertexArray(globjectthing.VAOthing);

      glBindBuffer(GL_ARRAY_BUFFER, globjectthing.VBOthing);
      glBufferData(GL_ARRAY_BUFFER, sizeof(float) * globjectthing.size * 10,
                   &vertices[0], GL_STATIC_DRAW);

      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), 0);
      glEnableVertexAttribArray(0);

      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float),
                            (void*)(3 * sizeof(GLfloat)));
      glEnableVertexAttribArray(1);

      glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 10 * sizeof(float),
                            (void*)(6 * sizeof(GLfloat)));
      glEnableVertexAttribArray(2);

      glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 10 * sizeof(float),
                            (void*)(8 * sizeof(GLfloat)));
      glEnableVertexAttribArray(3);

      RendererGlobal->GLstuff->GlMapObjects.push_back(globjectthing);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glBindVertexArray(0);
    }
  }
}