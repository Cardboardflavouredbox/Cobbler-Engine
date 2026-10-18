#include "bsp_readfile.h"

#include "map.h"
#include "mapdata.h"

MapBSP BSPResult;

static float epsilon = glm::epsilon<float>();

glm::vec4 CreatePlane(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2) {
  glm::vec3 edge1 = v1 - v0;
  glm::vec3 edge2 = v2 - v0;

  glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

  float d = -glm::dot(normal, v0);

  return glm::vec4(normal.x, normal.y, normal.z, d);
}

// Source - https://stackoverflow.com/a/35396994
// Posted by Jomi
// Retrieved 2026-10-12, License - CC BY-SA 3.0
bool linePlaneIntersection(float& contact, glm::vec3 ray, glm::vec3 rayOrigin,
                           glm::vec4 plane) {
  glm::vec3 normal = glm::vec3(plane), coord = normal * -plane.w;

  // get d value
  ray -= rayOrigin;
  float normalray = glm::dot(normal, ray);
  if (std::fabs(normalray) < epsilon) {
    return false;  // No intersection, the line is parallel to the plane
  }
  // Compute the X value for the directed line ray intersecting the plane
  float x = (glm::dot(normal, coord) - glm::dot(normal, rayOrigin)) / normalray;
  // output contact point
  contact = x;  // Make sure your ray vector is normalized
  if (x <= 0 || x >= 1) return false;
  return true;
}

float pointSideCheck(glm::vec3 point, glm::vec4 plane) {
  return glm::dot(point, glm::vec3(plane)) + plane.w;
}

struct ProcessResult {
  std::vector<std::array<uint32_t, 3>> ResultFacesA, ResultFacesB, currentfaces;
  glm::vec4 plane;
  int cnt = 0, amountoffacesadded = 1;
};

ProcessResult ProcessFace(std::vector<std::array<uint32_t, 3>> Faces,
                          int index) {
  ProcessResult result;
  std::array<uint32_t, 3> currentface = Faces[index];
  result.currentfaces.push_back(currentface);

  result.plane = CreatePlane(BSPResult.Points[currentface[0]],
                             BSPResult.Points[currentface[1]],
                             BSPResult.Points[currentface[2]]);

  glm::vec3 normal =
      glm::vec3(result.plane[0], result.plane[1], result.plane[2]);

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
    if (index == a) continue;
    auto& face = Faces[a];

    float dists[3];
    int frontCount = 0, backCount = 0, onPlaneCount = 0;

    for (int i = 0; i < 3; i++) {
      dists[i] = pointSideCheck(BSPResult.Points[face[i]], result.plane);
      if (dists[i] > epsilon)
        frontCount++;
      else if (dists[i] < -epsilon)
        backCount++;
      else {
        dists[i] = 0.0f;  // Force strict zero
        onPlaneCount++;
      }
    }

    float dummy;

    glm::vec3 v3 = BSPResult.Points[face[1]] - BSPResult.Points[face[0]],
              v4 = BSPResult.Points[face[2]] - BSPResult.Points[face[0]];
    glm::vec3 tempnormal = glm::normalize(glm::cross(v3, v4));

    if (backCount == 0 && frontCount > 0) {
      result.ResultFacesA.push_back(face);
    } else if (frontCount == 0 && backCount > 0) {
      result.ResultFacesB.push_back(face);
    } else if (onPlaneCount == 3) {
      result.amountoffacesadded++;
      result.currentfaces.push_back(face);
    } else {
      // Re-order vertices so the isolated single vertex is always index 0
      uint32_t idx0 = 0, idx1 = 1, idx2 = 2;
      if ((frontCount == 1 && dists[1] > epsilon) ||
          (backCount == 1 && dists[1] < -epsilon)) {
        idx0 = 1;
        idx1 = 2;
        idx2 = 0;
      } else if ((frontCount == 1 && dists[2] > epsilon) ||
                 (backCount == 1 && dists[2] < -epsilon)) {
        idx0 = 2;
        idx1 = 0;
        idx2 = 1;
      }

      glm::vec3 v0 = BSPResult.Points[face[idx0]];
      glm::vec3 v1 = BSPResult.Points[face[idx1]];
      glm::vec3 v2 = BSPResult.Points[face[idx2]];

      float tA = 0.5f, tB = 0.5f;
      linePlaneIntersection(tA, v0, v1, result.plane);
      linePlaneIntersection(tB, v0, v2, result.plane);

      glm::vec3 ptA = v0 + (v1 - v0) * tA;
      glm::vec3 ptB = v0 + (v2 - v0) * tB;
      BSPResult.Points.push_back(ptA);
      BSPResult.Points.push_back(ptB);

      uint32_t iA = BSPResult.Points.size() - 2;
      uint32_t iB = BSPResult.Points.size() - 1;

      // Construct split sub-triangles safely
      std::array<uint32_t, 3> tris[3] = {{face[idx0], iA, iB},
                                         {iA, face[idx1], face[idx2]},
                                         {iA, face[idx2], iB}};

      result.cnt += 1;

      for (auto& tri : tris) {
        glm::vec3 vert[3];

        for (int i = 0; i < 3; i++) vert[i] = BSPResult.Points[tri[i]];

        glm::vec3 center = (vert[0] + vert[1] + vert[2]) / 3.0f;

        float faceepsilon = 0.0001f;
        if (glm::distance(vert[0], vert[1]) <= faceepsilon ||
            glm::distance(vert[0], vert[2]) <= faceepsilon ||
            glm::distance(vert[1], vert[2]) <= faceepsilon) {
          result.amountoffacesadded++;
          result.currentfaces.push_back(tri);
        } else if (pointSideCheck(center, result.plane) >= 0.0f) {
          result.ResultFacesA.push_back(tri);
        } else {
          result.ResultFacesB.push_back(tri);
        }
      }
    }
  }

  return result;
}

void BSPcreate(std::vector<std::array<uint32_t, 3>> Faces, bool isA,
               int parent) {
  if (Faces.empty()) {
    MapBSP::BSPnode node;
    node.type = isA ? MapBSP::BSPnode::LEAF_EMPTY : MapBSP::BSPnode::LEAF_SOLID;
    BSPResult.BSPtree.push_back(node);
    int nodeindex = BSPResult.BSPtree.size() - 1;
    if (parent > -1) {
      if (isA)
        BSPResult.BSPtree[parent].A = nodeindex;
      else
        BSPResult.BSPtree[parent].B = nodeindex;
    }
  } else {
    printf("%lu\n", Faces.size());
    ProcessResult result;
    bool checkthing = false;
    for (int i = 0; i < Faces.size(); i++) {
      ProcessResult temp = ProcessFace(Faces, i);
      if (!std::isnan(temp.plane.x) && !std::isnan(temp.plane.y) &&
          !std::isnan(temp.plane.z) &&
          (!checkthing ||
           ((std::abs((int)result.ResultFacesA.size() -
                      (int)result.ResultFacesB.size()) +
                 result.cnt * 3.f + result.amountoffacesadded * 1.f >
             std::abs((int)temp.ResultFacesA.size() -
                      (int)temp.ResultFacesB.size()) +
                 temp.cnt * 3.f + temp.amountoffacesadded * 1.f)))) {
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
    MapBSP::BSPnode node;

    node.Faces.insert(node.Faces.end(), result.currentfaces.begin(),
                      result.currentfaces.end());

    node.plane = result.plane;
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

  fprintf(file, "B\n");
  for (int i = 0; i < BSPResult.BSPtree.size(); i++) {
    MapBSP::BSPnode& bspthing = BSPResult.BSPtree[i];

    fprintf(file, "D%d ", bspthing.type);

    if (bspthing.type == MapBSP::BSPnode::Nodetype::INTERNAL) {
      fprintf(file, "%d %d %f %f %f %f ", bspthing.A, bspthing.B,
              bspthing.plane[0], bspthing.plane[1], bspthing.plane[2],
              bspthing.plane[3]);
      for (auto& face : bspthing.Faces) {
        fprintf(file, "D%u %u %u", face[0], face[1], face[2]);
      }
      fprintf(file, "E");
    }
    fprintf(file, "\n");
  }
  fprintf(file, "E");

  fclose(file);

  return true;
}