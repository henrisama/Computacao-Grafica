#ifndef __P3_h
#define __P3_h

#include "Assets.h"
#include "GLRenderer.h"
#include "Light.h"
#include "Primitive.h"
#include "SceneEditor.h"
#include "core/Flags.h"
#include "graphics/Application.h"
#include <vector>

using namespace cg;

class P3 : public GLWindow
{
public:
    P3(int width, int height) :
        GLWindow{ "cg2021 - P3", width, height },
        _program{ "P3" }
    {
        // do nothing
    }

    /// Initialize the app.
    void initialize() override;

    /// Update the GUI.
    void gui() override;

    /// Render the scene.
    void render() override;

private:
    enum ViewMode
    {
        Editor = 0,
        Renderer = 1
    };

    enum class MoveBits
    {
        Left = 1,
        Right = 2,
        Forward = 4,
        Back = 8,
        Up = 16,
        Down = 32
    };

    enum class DragBits
    {
        Rotate = 1,
        Pan = 2
    };

    GLSL::Program _program;
    GLSL::Program _phongProgram{ "Phong" };
    GLSL::Program _gouraudProgram{ "Gouraud" };
    Reference<Scene> _scene;
    Reference<SceneEditor> _editor;
    Reference<GLRenderer> _renderer;

    std::vector<Reference<SceneObject>> _objects;
    Reference<SceneObject> _newObject;
    std::string _name;
    int boxCount = 2;
    int objectCount = 2;
    int sphereCount = 2;
    int cameraCount = 2;
    int lightCount = 2;

  SceneNode* _current{};
  Color _selectedWireframeColor{255, 102, 0};
  Flags<MoveBits> _moveFlags{};
  Flags<DragBits> _dragFlags{};
  int _pivotX;
  int _pivotY;
  int _mouseX;
  int _mouseY;
  bool _showAssets{true};
  bool _showEditorView{true};
  ViewMode _viewMode{ViewMode::Editor};

  static MeshMap _defaultMeshes;

  void buildScene();
  void renderScene();

  void mainMenu();
  void fileMenu();
  void showOptions();

  void hierarchyWindow();
  void inspectorWindow();
  void assetsWindow();
  void editorView();
  void sceneGui();
  void sceneObjectGui();
  void objectGui();
  void editorViewGui();
  void inspectPrimitive(Primitive&);
  void inspectShape(Primitive&);
  void inspectMaterial(Material&);
  void inspectLight(Light&);
  void inspectCamera(Camera&);
  void addComponentButton(SceneObject&);

  void drawPrimitive(Primitive&);
  void drawLight(Light&);
  void drawCamera(Camera&);

  bool windowResizeEvent(int, int) override;
  bool keyInputEvent(int, int, int) override;
  bool scrollEvent(double, double) override;
  bool mouseButtonInputEvent(int, int, int) override;
  bool mouseMoveEvent(double, double) override;

  static void buildDefaultMeshes();

  void renderObjects(SceneObject* obj);
  void preview(Camera& c);
  void focus();

}; // P3

#endif // __P3_h
