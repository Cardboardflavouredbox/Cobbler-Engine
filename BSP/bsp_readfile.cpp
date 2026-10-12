#include "bsp_readfile.h"

#include "map.h"
#include "mapdata.h"

// Source - https://stackoverflow.com/a/35396994
// Posted by Jomi
// Retrieved 2026-10-12, License - CC BY-SA 3.0
bool linePlaneIntersection(float& contact, glm::vec3 ray, glm::vec3 rayOrigin,
                           glm::vec3 normal, glm::vec3 coord) {
  // get d value
  float d = glm::dot(normal, coord);
  if (glm::dot(normal, ray) == 0) {
    return false;  // No intersection, the line is parallel to the plane
  }
  // Compute the X value for the directed line ray intersecting the plane
  float x = (d - glm::dot(normal, rayOrigin)) / glm::dot(normal, ray);
  // output contact point
  contact = x;  // Make sure your ray vector is normalized
  return true;
}

bool readfile(std::filesystem::path path) {
  Mapdata tempmapdata;
  FILE* file = NULL;

  // check if file is a .map file.
  if (path.extension() == ".map") {
    file = fopen(path.c_str(), "r");
  }

  if (file == NULL) {
    printf("Impossible to open the file!");
    return false;
  }
  // time to read the contents of the file.
  while (true) {
    char lineHeader;
    // read the first word of the line
    if (fscanf(file, "%c\n", &lineHeader) == EOF) break;

    if (lineHeader == 'V') {  // Visual thing
      VisualObject tempobject;
      while (true) {  // Visual Points.
        if (fscanf(file, "%c", &lineHeader) == EOF) break;
        if (lineHeader == 'E') break;
        glm::vec3 temppoint;
        fscanf(file, "%f,%f,%f\n", &temppoint.x, &temppoint.y, &temppoint.z);
        tempobject.VisualPoints.push_back(temppoint);
      }
      fscanf(file, "\n");
      while (true) {  // Visual Faces.
        if (fscanf(file, "%c", &lineHeader) == EOF) break;
        if (lineHeader == 'E') break;
        Mapface tempface;
        int doublesided;
        fscanf(file, "%d ", &doublesided);
        char subHeader;
        while (true) {
          if (fscanf(file, "%c", &subHeader) == EOF) break;
          if (subHeader == 'E') break;
          Mapface::TUVthing tuv;
          char texture[64];
          fscanf(file, "%s %f,%f %f,%f %f,%f ", texture, &tuv.UVs[0].x,
                 &tuv.UVs[0].y, &tuv.UVs[1].x, &tuv.UVs[1].y, &tuv.UVs[2].x,
                 &tuv.UVs[2].y);
          tuv.texture = texture;
          tempface.TUVvector.push_back(tuv);
        }

        fscanf(file, "%u,%u,%u\n", &tempface.points[0], &tempface.points[1],
               &tempface.points[2]);

        tempface.doublesided = doublesided;
        for (int i = 0; i < 3; i++) tempface.points[i];
        tempobject.Visualmapfaces.push_back(tempface);
      }
      fscanf(file, "\n");
      tempmapdata.VisualObjectsVector.push_back(tempobject);
    } else if (lineHeader == 'H') {  // Hitbox thing
      uint32_t cnt = tempmapdata.HitboxPoints.size();
      while (true) {  // Hitbox Points.
        if (fscanf(file, "%c", &lineHeader) == EOF) break;
        if (lineHeader == 'E') break;
        glm::vec3 temppoint;
        fscanf(file, "%f,%f,%f\n", &temppoint.x, &temppoint.y, &temppoint.z);
        tempmapdata.HitboxPoints.push_back(temppoint);
      }
      fscanf(file, "\n");
      while (true) {  // Hitbox Faces.
        if (fscanf(file, "%c", &lineHeader) == EOF) break;
        if (lineHeader == 'E') break;
        std::array<uint32_t, 3> tempface;
        fscanf(file, "%u,%u,%u\n", &tempface[0], &tempface[1], &tempface[2]);

        for (int i = 0; i < 3; i++) tempface[i] += cnt;
        tempmapdata.Hitboxmapfaces.push_back(tempface);
      }
      fscanf(file, "\n");
    } else if (lineHeader == 'K') {  // Killbox thing
      uint32_t cnt = tempmapdata.KillboxPoints.size();
      while (true) {  // Killbox Points.
        if (fscanf(file, "%c", &lineHeader) == EOF) break;
        if (lineHeader == 'E') break;
        glm::vec3 temppoint;
        fscanf(file, "%f,%f,%f\n", &temppoint.x, &temppoint.y, &temppoint.z);
        tempmapdata.KillboxPoints.push_back(temppoint);
      }
      fscanf(file, "\n");
      while (true) {  // Killbox Faces.
        if (fscanf(file, "%c", &lineHeader) == EOF) break;
        if (lineHeader == 'E') break;
        std::array<uint32_t, 3> tempface;
        fscanf(file, "%u,%u,%u\n", &tempface[0], &tempface[1], &tempface[2]);

        for (int i = 0; i < 3; i++) tempface[i] += cnt;
        tempmapdata.KillboxFaces.push_back(tempface);
      }
      fscanf(file, "\n");
    } else if (lineHeader == 'C') {  // Skybox.
      char name[64];
      fscanf(file, "%s", name);
      tempmapdata.skybox = name;
      break;
    } else if (lineHeader == 'S') {  // SpawnPoints.
      glm::vec3 pos;
      fscanf(file, "%f,%f,%f\n", &pos.x, &pos.y, &pos.z);
      tempmapdata.SpawnPoints.push_back(pos);
    }
  }
  fclose(file);

  std::vector<glm::vec3> Points;
  std::vector<std::array<uint32_t, 3>> Faces;

  for (auto& i : tempmapdata.Hitboxmapfaces) {
    std::vector<glm::vec3> tempPoints = tempmapdata.HitboxPoints;
    std::vector<std::array<uint32_t, 3>> tempFaces = tempmapdata.Hitboxmapfaces;

    glm::vec3 v1 = tempPoints[i[1]] - tempPoints[i[0]],
              v2 = tempPoints[i[2]] - tempPoints[i[0]];
    glm::vec3 normal = glm::normalize(glm::cross(v1, v2));

    for (auto& face : tempFaces) {
      float temp;

      bool check =
          linePlaneIntersection(temp, tempPoints[face[2]], tempPoints[face[0]],
                                normal, tempPoints[i[0]]);

      if (check) {
        glm::vec3 intersection =
            tempPoints[face[0]] + glm::normalize(tempPoints[face[2]]) * temp;
      }
    }
  }
  return true;
}