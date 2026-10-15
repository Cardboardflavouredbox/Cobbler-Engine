#include "bsp_readfile.h"

#include "map.h"
#include "mapdata.h"

MapBSP BSPResult;

// Source - https://stackoverflow.com/a/35396994
// Posted by Jomi
// Retrieved 2026-10-12, License - CC BY-SA 3.0
bool linePlaneIntersection(float& contact, glm::vec3 ray, glm::vec3 rayOrigin,
                           glm::vec3 normal, glm::vec3 coord) {
  // get d value
  ray -= rayOrigin;
  float d = glm::dot(normal, coord);
  if (glm::dot(normal, ray) <= 0.0000001f) {
    return false;  // No intersection, the line is parallel to the plane
  }
  // Compute the X value for the directed line ray intersecting the plane
  float x = (d - glm::dot(normal, rayOrigin)) / glm::dot(normal, ray);
  // output contact point
  contact = x;  // Make sure your ray vector is normalized
  if (x <= 0) return false;
  return true;
}

float pointSideCheck(glm::vec3 point, glm::vec3 normal, glm::vec3 coord) {
  glm::vec3 toTarget = point - coord;

  return glm::dot(glm::normalize(normal), toTarget);
}

struct ProcessResult {
  std::vector<std::array<uint32_t, 3>> ResultFacesA, ResultFacesB, currentfaces;
  glm::vec3 normal, point;
  int cnt = 0, amountoffacesadded = 1;
};

ProcessResult ProcessFace(std::vector<std::array<uint32_t, 3>> Faces,
                          int index) {
  ProcessResult result;
  std::array<uint32_t, 3> currentface = Faces[index];
  result.currentfaces.push_back(currentface);

  glm::vec3 v1 = BSPResult.Points[currentface[1]] -
                 BSPResult.Points[currentface[0]],
            v2 = BSPResult.Points[currentface[2]] -
                 BSPResult.Points[currentface[0]];
  glm::vec3 normal = glm::normalize(glm::cross(v1, v2));

  result.normal = normal;
  result.point = BSPResult.Points[currentface[0]];
  if (std::isnan(normal.x) || std::isnan(normal.y) || std::isnan(normal.z)) {
    printf("isnan detected! face:");
    for (int i = 0; i < 3; i++)
      printf("%f %f %f  ", BSPResult.Points[currentface[i]].x,
             BSPResult.Points[currentface[i]].y,
             BSPResult.Points[currentface[i]].z);
    printf("\n");
    return result;
  }

  for (int a = 0; a < Faces.size(); a++) {
    if (index != a) {
      auto& face = Faces[a];
      float dummy;

      glm::vec3 v3 = BSPResult.Points[face[1]] - BSPResult.Points[face[0]],
                v4 = BSPResult.Points[face[2]] - BSPResult.Points[face[0]];
      glm::vec3 tempnormal = glm::normalize(glm::cross(v3, v4));
      if (1.f - std::fabs(glm::dot(tempnormal, normal)) <=
          0.00001f) {  // if normals are parallel
        float dist = pointSideCheck(BSPResult.Points[face[0]], normal,
                                    BSPResult.Points[currentface[0]]);
        if (std::fabsf(dist) <= 0.00001f) {
          result.amountoffacesadded++;
          result.currentfaces.push_back(face);
        } else {
          if (dist >= 0.f)
            result.ResultFacesA.push_back(face);
          else
            result.ResultFacesB.push_back(face);
        }
      } else {
        bool facecheck = false;
        for (int i = 0; i < 3; i++) {
          uint32_t face0 = face[(i) % 3], face1 = face[(i + 1) % 3],
                   face2 = face[(i + 2) % 3];

          float temp, temp2;

          bool check = linePlaneIntersection(temp, BSPResult.Points[face1],
                                             BSPResult.Points[face0], normal,
                                             BSPResult.Points[currentface[0]]),
               check2 = linePlaneIntersection(temp2, BSPResult.Points[face2],
                                              BSPResult.Points[face0], normal,
                                              BSPResult.Points[currentface[0]]);

          if (check && check2 && temp > 0.f && temp < 1.0f && temp2 > 0.f &&
              temp2 < 1.0f) {
            glm::vec3 A = BSPResult.Points[face0] +
                          (BSPResult.Points[face1] - BSPResult.Points[face0]) *
                              temp,
                      B = BSPResult.Points[face0] +
                          (BSPResult.Points[face2] - BSPResult.Points[face0]) *
                              temp2;
            float epsilon = 0.001f;
            if (glm::distance(A, B) < epsilon ||
                glm::distance(BSPResult.Points[face0], A) < epsilon ||
                glm::distance(BSPResult.Points[face0], B) < epsilon ||
                glm::distance(BSPResult.Points[face1], A) < epsilon ||
                glm::distance(BSPResult.Points[face1], B) < epsilon ||
                glm::distance(BSPResult.Points[face2], A) < epsilon ||
                glm::distance(BSPResult.Points[face2], B) < epsilon) {
              result.amountoffacesadded++;
              result.currentfaces.push_back(face);
            } else {
              BSPResult.Points.push_back(A);
              BSPResult.Points.push_back(B);

              std::array<std::array<uint32_t, 3>, 3> splitfaces;

              splitfaces[0][0] = face0;
              splitfaces[0][1] = BSPResult.Points.size() - 2;
              splitfaces[0][2] = BSPResult.Points.size() - 1;

              splitfaces[1][0] = face1;
              splitfaces[1][1] = face2;
              splitfaces[1][2] = BSPResult.Points.size() - 1;

              splitfaces[2][0] = face1;
              splitfaces[2][1] = BSPResult.Points.size() - 1;
              splitfaces[2][2] = BSPResult.Points.size() - 2;

              result.cnt += 1;

              for (int j = 0; j < 3; j++) {
                float dist =
                    pointSideCheck(BSPResult.Points[splitfaces[j][0]], normal,
                                   BSPResult.Points[currentface[0]]);
                if (dist >= 0.f)
                  result.ResultFacesA.push_back(splitfaces[j]);
                else
                  result.ResultFacesB.push_back(splitfaces[j]);
              }
            }
            facecheck = true;
            break;
          }
        }
        if (!facecheck) {
          float dist = pointSideCheck(BSPResult.Points[face[0]], normal,
                                      BSPResult.Points[currentface[0]]);
          if (dist >= 0.f)
            result.ResultFacesA.push_back(face);
          else
            result.ResultFacesB.push_back(face);
        }
      }
    }
  }

  return result;
}

void BSPcreate(std::vector<std::array<uint32_t, 3>> Faces, bool isA,
               int parent) {
  if (!Faces.empty()) {
    printf("%lu\n", Faces.size());
    ProcessResult result;
    bool checkthing = false;
    for (int i = 0; i < Faces.size(); i++) {
      ProcessResult temp = ProcessFace(Faces, i);
      if (!std::isnan(temp.normal.x) && !std::isnan(temp.normal.y) &&
          !std::isnan(temp.normal.z) &&
          (!checkthing || ((std::abs((int)result.ResultFacesA.size() -
                                     (int)result.ResultFacesB.size()) +
                                result.cnt * 4 + result.amountoffacesadded * 5 >
                            std::abs((int)temp.ResultFacesA.size() -
                                     (int)temp.ResultFacesB.size()) +
                                temp.cnt * 4 + temp.amountoffacesadded * 5)))) {
        checkthing = true;
        result = temp;
      }
    }
    if (!checkthing) {
      printf("error! %d\n", Faces.size());
      return;
    }
    // printf("%lu %lu\n", result.ResultFacesA.size(),
    // result.ResultFacesB.size());
    for (auto& i : result.currentfaces) BSPResult.Faces.push_back(i);
    MapBSP::BSPnode node;
    for (int i = 0; i < result.amountoffacesadded; i++) {
      node.Faces.push_back(BSPResult.Faces.size() - 1 - i);
    }
    node.normal = result.normal;
    node.point = result.point;
    BSPResult.BSPtree.push_back(node);
    int nodeindex = BSPResult.BSPtree.size() - 1;
    if (parent > -1) {
      if (isA)
        BSPResult.BSPtree[parent].A = nodeindex;
      else
        BSPResult.BSPtree[parent].B = nodeindex;
    }
    BSPcreate(result.ResultFacesA, true, nodeindex);
    BSPcreate(result.ResultFacesB, false, nodeindex);
  }
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
      while (true) {  // VisualBSPResult.Points.
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
      while (true) {  // HitboxBSPResult.Points.
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
      while (true) {  // KillboxBSPResult.Points.
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

  BSPResult.Points = tempmapdata.HitboxPoints;
  std::vector<std::array<uint32_t, 3>> Faces = tempmapdata.Hitboxmapfaces;
  BSPcreate(Faces, false, -1);

  std::string paththing = path.string();
  for (int i = 0; i < 3; i++) paththing.pop_back();
  paththing += "bsp";

  file = fopen(paththing.c_str(), "w");

  if (file == NULL) {
    printf("Impossible to write the file!");
    return false;
  }

  printf("writing to %s", paththing.c_str());

  fprintf(file, "V\n");
  for (int i = 0; i < BSPResult.Points.size(); i++) {
    fprintf(file, "D%f %f %f\n", BSPResult.Points[i][0], BSPResult.Points[i][1],
            BSPResult.Points[i][2]);
  }
  fprintf(file, "E\n");

  fprintf(file, "F\n");
  for (int i = 0; i < BSPResult.Faces.size(); i++) {
    fprintf(file, "D%u %u %u\n", BSPResult.Faces[i][0], BSPResult.Faces[i][1],
            BSPResult.Faces[i][2]);
  }
  fprintf(file, "E\n");

  fprintf(file, "B\n");
  for (int i = 0; i < BSPResult.BSPtree.size(); i++) {
    MapBSP::BSPnode& bspthing = BSPResult.BSPtree[i];
    fprintf(file, "D%d %d %f %f %f %f %f %f ", bspthing.A, bspthing.B,
            bspthing.normal[0], bspthing.normal[1], bspthing.normal[2],
            bspthing.point[0], bspthing.point[1], bspthing.point[2]);
    for (auto& j : bspthing.Faces) {
      fprintf(file, "D%u", j);
    }
    fprintf(file, "E\n");
  }
  fprintf(file, "E");

  fclose(file);

  return true;
}