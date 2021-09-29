#ifndef __P1_h
#define __P1_h

#include "Primitive.h"
#include "Scene.h"
#include "graphics/Application.h"

using namespace cg;

class P1: public GLWindow
{
public:
  P1(int width, int height):
    GLWindow{"cg2021 - P1", width, height},
    _program{"P1"}
  {
    // do nothing
  }

  /// Initialize the app.
  void initialize() override;

  /// Update the GUI.
  void gui() override;

  /// Render the scene.
  void render() override;

  bool keyInputEvent(int key, int action, int mods) override;

  void deleteCurrentSceneObject();

private:
  int boxCount = 4;
  int objectCount = 2;
  GLSL::Program _program;
  Reference<Scene> _scene;
  std::list<Reference<SceneObject>>::iterator it;
  Reference<SceneObject> _newObject;
  SceneNode* _current{};
  Color selectedWireframeColor{255, 102, 0};


  void buildScene();

  void hierarchyWindow();
  void inspectorWindow();
  void sceneGui();
  void sceneObjectGui();
  void objectGui();

}; // P1

#endif // __P1_h
