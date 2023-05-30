#include <SketchUpAPI/common.h>
#include <SketchUpAPI/geometry.h>
#include <SketchUpAPI/initialize.h>
#include <SketchUpAPI/unicodestring.h>
#include <SketchUpAPI/model/model.h>
#include <SketchUpAPI/model/entities.h>
#include <SketchUpAPI/model/face.h>
#include <SketchUpAPI/model/edge.h>
#include <SketchUpAPI/model/vertex.h>
#include <vector>
#include <iostream>

int main() {
  // Always initialize the API before using it
  SUInitialize();

  // Load the model from a file
  SUModelRef model = SU_INVALID;
  SUModelLoadStatus status;
  SUResult res = SUModelCreateFromFileWithStatus(&model, "D:\\Projects\\sketchup\\model.skp", &status);

  // It's best to always check the return code from each SU function call.
  // Only showing this check once to keep this example short.
  if (res != SU_ERROR_NONE) {
    std::cout << "Failed creating model from a file" << std::endl;
    return 1;
  }

  if (status == SUModelLoadStatus_Success_MoreRecent) {
    std::cout
        << "This model was created in a more recent SketchUp version than that of the SDK. "
           "It contains data which will not be read. Saving the model over the original file may "
           "lead to permanent data loss."
        << std::endl;
  }

  // Get the entity container of the model.
  SUEntitiesRef entities = SU_INVALID;
  SUModelGetEntities(model, &entities);

  // Get all the faces from the entities object
  size_t faceCount = 0;
  SUEntitiesGetNumFaces(entities, &faceCount);
  std::cout << "Number of faces: " << faceCount << std::endl;
  if (faceCount > 0) {
    std::vector<SUFaceRef> faces(faceCount);
    SUEntitiesGetFaces(entities, faceCount, &faces[0], &faceCount);

    // Get all the edges in this face
    for (size_t i = 0; i < faceCount; i++) {
      size_t edgeCount = 0;
      SUFaceGetNumEdges(faces[i], &edgeCount);
      if (edgeCount > 0) {
        std::vector<SUEdgeRef> edges(edgeCount);
        SUFaceGetEdges(faces[i], edgeCount, &edges[0], &edgeCount);

        // Get the vertex positions for each edge
        for (size_t j = 0; j < edgeCount; j++) {
          SUVertexRef startVertex = SU_INVALID;
          SUVertexRef endVertex = SU_INVALID;
          SUEdgeGetStartVertex(edges[j], &startVertex);
          SUEdgeGetEndVertex(edges[j], &endVertex);
          SUPoint3D start;
          SUPoint3D end;
          SUVertexGetPosition(startVertex, &start);
          SUVertexGetPosition(endVertex, &end);
          // Now do something with the point data
        }
      }
    }
  }

  // Get model name
  SUStringRef name = SU_INVALID;
  SUStringCreate(&name);
  SUModelGetName(model, &name);
  size_t name_length = 0;
  SUStringGetUTF8Length(name, &name_length);
  char* name_utf8 = new char[name_length + 1];
  SUStringGetUTF8(name, name_length + 1, name_utf8, &name_length);
  // Now we have the name in a form we can use
  SUStringRelease(&name);
  delete[] name_utf8;

  // Must release the model or there will be memory leaks
  SUResult result = SUModelRelease(&model);

  if (result != SU_ERROR_NONE) {
    std::cout << "Reading SketchUp model: Failed" << std::endl;
  } else {
    std::cout << "Reading SketchUp model: Success" << std::endl;
  }
  // Always terminate the API when done using it
  SUTerminate();

  return 0;
}
