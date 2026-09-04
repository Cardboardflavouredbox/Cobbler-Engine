#include "map.h"

#include <glad/glad.h>

#include <glm/glm.hpp>

#include "files.h"
#include "render.h"

void LoadMapGL() {
  RendererGlobal->GLstuff->MapGLlist = glGenLists(1);

  glNewList(RendererGlobal->GLstuff->MapGLlist, GL_COMPILE);

  for (int i = 0; i < GlobalMapStuff->mapfaces.size(); i++) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(
        GL_TEXTURE_2D,
        RendererGlobal->GLstuff->textures[GlobalMapStuff->mapfaces[i].texture]);
    glBegin(GL_TRIANGLES);
    for (int j = 2; j >= 0; j--) {
      glm::vec3 pos =
          GlobalMapStuff->Points[GlobalMapStuff->mapfaces[i].points[j]].pos;
      glm::vec2 uvw = GlobalMapStuff->mapfaces[i].UVs[j];
      glColor3f(GlobalMapStuff->Points[GlobalMapStuff->mapfaces[i].points[j]]
                    .shade[0],
                GlobalMapStuff->Points[GlobalMapStuff->mapfaces[i].points[j]]
                    .shade[1],
                GlobalMapStuff->Points[GlobalMapStuff->mapfaces[i].points[j]]
                    .shade[2]);
      glTexCoord2f(uvw.x, uvw.y);
      glVertex3f(pos.x, pos.y, pos.z);
    }
    glEnd();
  }

  glEnd();
  glEndList();
}