#include "geometry/MeshSweeper.h"
#include "P2.h"

MeshMap P2::_defaultMeshes;

inline void
P2::buildDefaultMeshes()
{
  _defaultMeshes["None"] = nullptr;
  _defaultMeshes["Box"] = GLGraphics3::box();
  _defaultMeshes["Sphere"] = GLGraphics3::sphere();
}

inline Primitive*
makePrimitive(MeshMapIterator mit)
{
  return new Primitive{mit->second, mit->first};
}

inline void
P2::buildScene()
{
  _current = _scene = new Scene{"Scene 1"};
  _editor = new SceneEditor{*_scene};
  _editor->setDefaultView((float)width() / (float)height());
  // **Begin initialization of temporary attributes
  // It should be replaced by your scene initialization
  {
    
    auto camera = new Camera;
    SceneObject *obj = new SceneObject{ "Camera 1", *_scene };
    obj->addComponent(camera);
    obj->setParent(_scene->root());
    _scene->append(obj);

    obj = new SceneObject{ "Box 1", *_scene };
    obj->addComponent(makePrimitive(_defaultMeshes.find("Box")));
    obj->setParent(_scene->root());
    _scene->append(obj);

    obj = new SceneObject{ "Sphere 1", *_scene };
    obj->addComponent(makePrimitive(_defaultMeshes.find("Sphere")));
    obj->setParent(_scene->root());
    _scene->append(obj);

    obj = new SceneObject{ "Object 1", *_scene };
    obj->setParent(_scene->root());
    _scene->append(obj);

    Camera::setCurrent(camera);
  }
  // **End initialization of temporary attributes
}

void
P2::initialize()
{
  Application::loadShaders(_program, "shaders/p2.vs", "shaders/p2.fs");
  Assets::initialize();
  buildDefaultMeshes();
  buildScene();
  _renderer = new GLRenderer{*_scene};
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(1.0f, 1.0f);
  glEnable(GL_LINE_SMOOTH);
  _program.use();
}

namespace ImGui
{
  void ShowDemoWindow(bool*);
}

inline void
P2::hierarchyWindow()
{
  ImGui::Begin("Hierarchy");
  if (ImGui::Button("Create###object"))
    ImGui::OpenPopup("CreateObjectPopup");
  if (ImGui::BeginPopup("CreateObjectPopup"))
  {
    if (ImGui::MenuItem("Empty Object"))
    {
      // TODO: create an empty object.
        std::string name = "";
        name
            .append("Object ")
            .append(std::to_string(objectCount++));

        if (_current != _scene)
        {
            SceneObject* ob = (SceneObject*)_current;
            _newObject = new SceneObject{ name.c_str(), *_scene };
            _newObject->setParent(ob);
            ob->append(_newObject);
        }
        else 
        {
            Scene* ob = (Scene*)_current;
            _newObject = new SceneObject{ name.c_str(), *_scene };
            _newObject->setParent(ob->root());
            ob->append(_newObject);
        }
    }
    if (ImGui::BeginMenu("3D Object"))
    {
      if (ImGui::MenuItem("Box"))
      {
        // TODO: create a new box.
          std::string name = "";

          if (_current != _scene) 
          {
              SceneObject* ob = (SceneObject*)_current;

              auto component = ob->component();
              if (!dynamic_cast<Camera*>(component))
              {
                  name
                      .append("Box ")
                      .append(std::to_string(boxCount++));

                  _newObject = new SceneObject{ name.c_str(), *_scene };
                  _newObject->setParent(ob);
                  _newObject->addComponent(makePrimitive(_defaultMeshes.find("Box")));
                  ob->append(_newObject);
              }
          }
          else 
          {
              name
                  .append("Box ")
                  .append(std::to_string(boxCount++));

              Scene* ob = (Scene*)_current;
              _newObject = new SceneObject{ name.c_str(), *_scene };
              _newObject->setParent(ob->root());
              _newObject->addComponent(makePrimitive(_defaultMeshes.find("Box")));
              ob->append(_newObject);
          }
      }
      if (ImGui::MenuItem("Sphere"))
      {
        // TODO: create a new sphere.
          std::string name = "";

          if (_current != _scene) {
              SceneObject* ob = (SceneObject*)_current;

              auto component = ob->component();
              if (!dynamic_cast<Camera*>(component)) 
              {
                  name
                      .append("Sphere ")
                      .append(std::to_string(sphereCount++));

                  _newObject = new SceneObject{ name.c_str(), *_scene };
                  _newObject->setParent(ob);
                  _newObject->addComponent(makePrimitive(_defaultMeshes.find("Sphere")));
                  ob->append(_newObject);
              }
          }
          else 
          {
              name
                  .append("Box ")
                  .append(std::to_string(boxCount++));
              Scene* ob = (Scene*)_current;
              _newObject = new SceneObject{ name.c_str(), *_scene };
              _newObject->setParent(ob->root());
              _newObject->addComponent(makePrimitive(_defaultMeshes.find("Sphere")));
              ob->append(_newObject);
          }
      }
      ImGui::EndMenu();
    }
    if (ImGui::MenuItem("Camera"))
    {
      // TODO: create a new camera.
        std::string name = "";

        if (_current != _scene) {
            SceneObject* ob = (SceneObject*)_current;

            auto component = ob->component();
            if (!dynamic_cast<Camera*>(component))
            {
                name
                    .append("Camera ")
                    .append(std::to_string(cameraCount++));

                auto camera = new Camera;
                _newObject = new SceneObject{ name.c_str(), *_scene };
                _newObject->setParent(ob);
                _newObject->addComponent(camera);
                ob->append(_newObject);
            }
        }
        else 
        {
            name
                .append("Camera ")
                .append(std::to_string(cameraCount++));

            Scene* ob = (Scene*)_current;
            auto camera = new Camera;
            _newObject = new SceneObject{ name.c_str(), *_scene };
            _newObject->addComponent(camera);
            _newObject->setParent(_scene->root());
            ob->append(_newObject);
        }
    }
    ImGui::EndPopup();
  }

  if (ImGui::Button("Delete")) {
      if (_current != _scene) {
          SceneObject* ob = (SceneObject*)_current;
          if (ob->parent() != _scene->root()) {
              _current = ob->parent();
              ob->deleteIt();
          }
          else {
              _current = _scene;
              _scene->remove(ob);
          }
      }
  }

  ImGui::Separator();

  // **Begin hierarchy of temporary scene objects
  // It should be replaced by your hierarchy
  ImGuiTreeNodeFlags flag{ ImGuiTreeNodeFlags_OpenOnArrow };
  auto open = ImGui::TreeNodeEx(_scene,
    _current == _scene ? flag | ImGuiTreeNodeFlags_Selected : flag,
    _scene->name());

  if (ImGui::IsItemClicked())
    _current = _scene;
  if (open)
  {
    std::list <Reference<SceneObject>> aux = _scene->getroot();
    for (const auto&obj : aux)
    {
        _current = obj->display(flag, _current);
    }
    ImGui::TreePop();
  }
  // **End hierarchy of temporary scene objects
  ImGui::End();
}

namespace ImGui
{ // begin namespace ImGui

void
ObjectNameInput(NameableObject* object)
{
  const int bufferSize{128};
  static NameableObject* current;
  static char buffer[bufferSize];

  if (object != current)
  {
    strcpy_s(buffer, bufferSize, object->name());
    current = object;
  }
  if (ImGui::InputText("Name", buffer, bufferSize))
    object->setName(buffer);
}

inline bool
ColorEdit3(const char* label, Color& color)
{
  return ImGui::ColorEdit3(label, (float*)&color);
}

inline bool
DragVec3(const char* label, vec3f& v)
{
  return DragFloat3(label, (float*)&v, 0.1f, 0.0f, 0.0f, "%.2g");
}

void
TransformEdit(Transform* transform)
{
  vec3f temp;

  temp = transform->localPosition();
  if (ImGui::DragVec3("Position", temp))
    transform->setLocalPosition(temp);
  temp = transform->localEulerAngles();
  if (ImGui::DragVec3("Rotation", temp))
    transform->setLocalEulerAngles(temp);
  temp = transform->localScale();
  if (ImGui::DragVec3("Scale", temp))
    transform->setLocalScale(temp);
}

} // end namespace ImGui

inline void
P2::sceneGui()
{
  auto scene = (Scene*)_current;

  ImGui::ObjectNameInput(_current);
  ImGui::Separator();
  if (ImGui::CollapsingHeader("Colors"))
  {
    ImGui::ColorEdit3("Background", scene->backgroundColor);
    ImGui::ColorEdit3("Ambient Light", scene->ambientLight);
  }
}

inline void
P2::inspectPrimitive(Primitive& primitive)
{
  char buffer[16];

  snprintf(buffer, 16, "%s", primitive.meshName());
  ImGui::InputText("Mesh", buffer, 16, ImGuiInputTextFlags_ReadOnly);
  if (ImGui::BeginDragDropTarget())
  {
    if (auto* payload = ImGui::AcceptDragDropPayload("PrimitiveMesh"))
    {
      auto mit = *(MeshMapIterator*)payload->Data;
      primitive.setMesh(mit->second, mit->first);
    }
    ImGui::EndDragDropTarget();
  }
  ImGui::SameLine();
  if (ImGui::Button("...###PrimitiveMesh"))
    ImGui::OpenPopup("PrimitiveMeshPopup");
  if (ImGui::BeginPopup("PrimitiveMeshPopup"))
  {
    auto& meshes = Assets::meshes();

    if (!meshes.empty())
    {
      for (auto mit = meshes.begin(); mit != meshes.end(); ++mit)
        if (ImGui::Selectable(mit->first.c_str()))
          primitive.setMesh(Assets::loadMesh(mit), mit->first);
      ImGui::Separator();
    }
    for (auto mit = _defaultMeshes.begin(); mit != _defaultMeshes.end(); ++mit)
      if (ImGui::Selectable(mit->first.c_str()))
        primitive.setMesh(mit->second, mit->first);
    ImGui::EndPopup();
  }
  ImGui::ColorEdit3("Mesh Color", (float*)&primitive.color);
}

void
P2::inspectCamera(Camera& camera)
{
  static const char* projectionNames[]{"Perspective", "Orthographic"};
  auto cp = camera.projectionType();

  if (ImGui::BeginCombo("Projection", projectionNames[cp]))
  {
    for (auto i = 0; i < IM_ARRAYSIZE(projectionNames); ++i)
    {
      auto selected = cp == i;

      if (ImGui::Selectable(projectionNames[i], selected))
        cp = (Camera::ProjectionType)i;
      if (selected)
        ImGui::SetItemDefaultFocus();
    }
    ImGui::EndCombo();
  }
  camera.setProjectionType(cp);
  if (cp == View3::Perspective)
  {
    auto fov = camera.viewAngle();

    if (ImGui::SliderFloat("View Angle",
      &fov,
      MIN_ANGLE,
      MAX_ANGLE,
      "%.0f deg",
      1.0f))
      camera.setViewAngle(fov <= MIN_ANGLE ? MIN_ANGLE : fov);
  }
  else
  {
    auto h = camera.height();

    if (ImGui::DragFloat("Height",
      &h,
      MIN_HEIGHT * 10.0f,
      MIN_HEIGHT,
      math::Limits<float>::inf()))
      camera.setHeight(h <= 0 ? MIN_HEIGHT : h);
  }

  float n;
  float f;

  camera.clippingPlanes(n, f);

  if (ImGui::DragFloatRange2("Clipping Planes",
    &n,
    &f,
    MIN_DEPTH,
    MIN_DEPTH,
    math::Limits<float>::inf(),
    "Near: %.2f",
    "Far: %.2f"))
  {
    if (n <= 0)
      n = MIN_DEPTH;
    if (f - n < MIN_DEPTH)
      f = n + MIN_DEPTH;
    camera.setClippingPlanes(n, f);
  }
}

inline void
P2::addComponentButton(SceneObject& object)
{
  if (ImGui::Button("Add Component"))
    ImGui::OpenPopup("AddComponentPopup");
  if (ImGui::BeginPopup("AddComponentPopup"))
  {
    if (ImGui::MenuItem("Primitive"))
    {
        // TODO
        auto component = object.component();
        if (dynamic_cast<Transform*>(component))
        {
            object.addComponent(makePrimitive(_defaultMeshes.find("Box")));
        }
    }
    if (ImGui::MenuItem("Camera"))
    {
        // TODO
        auto component = object.component();
        if (dynamic_cast<Transform*>(component))
        {
            object.addComponent(new Camera);
        }
    }
    ImGui::EndPopup();
  }
}

inline void
P2::sceneObjectGui()
{
  auto object = (SceneObject*)_current;

  addComponentButton(*object);
  ImGui::Separator();
  ImGui::ObjectNameInput(object);
  ImGui::SameLine();
  ImGui::Checkbox("###visible", &object->visible);
  ImGui::Separator();
  if (ImGui::CollapsingHeader(object->transform()->typeName()))
    ImGui::TransformEdit(object->transform());

  // **Begin inspection of temporary components
  // It should be replaced by your component inspection
  auto component = object->component();

  if (auto p = dynamic_cast<Primitive*>(component))
  {
    auto notDelete{true};
    auto open = ImGui::CollapsingHeader(p->typeName(), &notDelete);

    if (!notDelete)
    {
      // TODO: delete primitive
    }
    else if (open)
      inspectPrimitive(*p);
  }
  else if (auto c = dynamic_cast<Camera*>(component))
  {
    auto notDelete{true};
    auto open = ImGui::CollapsingHeader(c->typeName(), &notDelete);

    if (!notDelete)
    {
      // TODO: delete camera
    }
    else if (open)
    {
      auto isCurrent = c == Camera::current();

      ImGui::Checkbox("Current", &isCurrent);
      Camera::setCurrent(isCurrent ? c : nullptr);
      inspectCamera(*c);
    }
  }
  // **End inspection of temporary components
}

inline void
P2::objectGui()
{
  if (_current == nullptr)
    return;
  if (dynamic_cast<SceneObject*>(_current))
  {
    sceneObjectGui();
    return;
  }
  if (dynamic_cast<Scene*>(_current))
    sceneGui();
}

inline void
P2::inspectorWindow()
{
  ImGui::Begin("Inspector");
  objectGui();
  ImGui::End();
}

inline void
P2::editorViewGui()
{
  if (ImGui::Button("Set Default View"))
    _editor->setDefaultView(float(width()) / float(height()));
  ImGui::Separator();

  auto t = _editor->camera()->transform();
  vec3f temp;

  temp = t->localPosition();
  if (ImGui::DragVec3("Position", temp))
    t->setLocalPosition(temp);
  temp = t->localEulerAngles();
  if (ImGui::DragVec3("Rotation", temp))
    t->setLocalEulerAngles(temp);
  inspectCamera(*_editor->camera());
  ImGui::Separator();
  ImGui::Checkbox("Show Ground", &_editor->showGround);

  if (ImGui::Button("Focus")) {
      auto cam = _editor->camera()->current();
      SceneObject* ob = (SceneObject*)_current;
      cam->transform()->setLocalPosition(ob->transform()->localPosition());
      cam->transform()->setLocalRotation(ob->transform()->localRotation());
  }
}

inline void
P2::assetsWindow()
{
  if (!_showAssets)
    return;
  ImGui::Begin("Assets");
  if (ImGui::CollapsingHeader("Meshes"))
  {
    auto& meshes = Assets::meshes();

    for (auto mit = meshes.begin(); mit != meshes.end(); ++mit)
    {
      auto meshName = mit->first.c_str();
      auto selected = false;

      ImGui::Selectable(meshName, &selected);
      if (ImGui::BeginDragDropSource())
      {
        Assets::loadMesh(mit);
        ImGui::Text(meshName);
        ImGui::SetDragDropPayload("PrimitiveMesh", &mit, sizeof(mit));
        ImGui::EndDragDropSource();
      }
    }
  }
  ImGui::Separator();
  if (ImGui::CollapsingHeader("Textures"))
  {
    // p3
  }
  ImGui::End();
}

inline void
P2::editorView()
{
  if (!_showEditorView)
    return;
  ImGui::Begin("Editor View Settings");
  editorViewGui();
  ImGui::End();
}

inline void
P2::fileMenu()
{
  if (ImGui::MenuItem("New"))
  {
    // TODO
  }
  if (ImGui::MenuItem("Open...", "Ctrl+O"))
  {
    // TODO
  }
  ImGui::Separator();
  if (ImGui::MenuItem("Save", "Ctrl+S"))
  {
    // TODO
  }
  if (ImGui::MenuItem("Save As..."))
  {
    // TODO
  }
  ImGui::Separator();
  if (ImGui::MenuItem("Exit", "Alt+F4"))
  {
    shutdown();
  }
}

inline bool
showStyleSelector(const char* label)
{
  static int style = 1;

  if (!ImGui::Combo(label, &style, "Classic\0Dark\0Light\0"))
    return false;
  switch (style)
  {
    case 0: ImGui::StyleColorsClassic();
      break;
    case 1: ImGui::StyleColorsDark();
      break;
    case 2: ImGui::StyleColorsLight();
      break;
  }
  return true;
}

inline void
P2::showOptions()
{
  ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.6f);
  showStyleSelector("Color Theme##Selector");
  ImGui::ColorEdit3("Selected Wireframe", _selectedWireframeColor);
  ImGui::PopItemWidth();
}

inline void
P2::mainMenu()
{
  if (ImGui::BeginMainMenuBar())
  {
    if (ImGui::BeginMenu("File"))
    {
      fileMenu();
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("View"))
    {
      if (Camera::current() == 0)
        ImGui::MenuItem("Edit View", nullptr, true, false);
      else
      {
        static const char* viewLabels[]{"Editor", "Renderer"};

        if (ImGui::BeginCombo("View", viewLabels[_viewMode]))
        {
          for (auto i = 0; i < IM_ARRAYSIZE(viewLabels); ++i)
          {
            if (ImGui::Selectable(viewLabels[i], _viewMode == i))
              _viewMode = (ViewMode)i;
          }
          ImGui::EndCombo();
        }
      }
      ImGui::Separator();
      ImGui::MenuItem("Assets Window", nullptr, &_showAssets);
      ImGui::MenuItem("Editor View Settings", nullptr, &_showEditorView);
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Tools"))
    {
      if (ImGui::BeginMenu("Options"))
      {
        showOptions();
        ImGui::EndMenu();
      }
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }
}

void
P2::gui()
{
  mainMenu();
  hierarchyWindow();
  inspectorWindow();
  assetsWindow();
  editorView();

  /*
  static bool demo = true;
  ImGui::ShowDemoWindow(&demo);
  */
}

inline void
drawMesh(GLMesh* mesh, GLuint mode)
{
  glPolygonMode(GL_FRONT_AND_BACK, mode);
  glDrawElements(GL_TRIANGLES, mesh->vertexCount(), GL_UNSIGNED_INT, 0);
}

inline void
P2::drawPrimitive(Primitive& primitive)
{
  auto m = glMesh(primitive.mesh());

  if (nullptr == m)
    return;

  auto t = primitive.transform();
  auto normalMatrix = mat3f{t->worldToLocalMatrix()}.transposed();

  _program.setUniformMat4("transform", t->localToWorldMatrix());
  _program.setUniformMat3("normalMatrix", normalMatrix);
  _program.setUniformVec4("color", primitive.color);
  _program.setUniform("flatMode", (int)0);
  m->bind();
  drawMesh(m, GL_FILL);
  if (primitive.sceneObject() != _current)
    return;
  _program.setUniformVec4("color", _selectedWireframeColor);
  _program.setUniform("flatMode", (int)1);
  drawMesh(m, GL_LINE);
}

inline void
P2::drawCamera(Camera& cam)
{
  // TODO
    constexpr auto scale = .6f;

    auto height_on = [&](float z) -> float
    {
        return 2.f * z * tan(math::toRadians(cam.viewAngle() / 2.f));
    };

    auto distance = (
        _editor->camera()->transform()->position()
        - cam.transform()->position()
        ).length();

    auto z_near = cam.z_near();
    auto z_far = std::min(
        cam.z_far(),
        std::max(0.f,
            scale * _editor->camera()->z_far() - distance));

    float h_near = 0;
    float h_far = 0;

    switch (cam.projectionType())
    {
    case Camera::Parallel:
        h_far = h_near = cam.height();
        break;
    case Camera::Perspective:
        h_near = height_on(z_near);
        h_far = height_on(z_far);
        break;
    }

    auto j = .5f;
    auto i = j * cam.aspectRatio();

    auto t = j;    // top
    auto b = -j;    // bottom
    auto l = -i;    // left
    auto r = i;    // right

    auto to_world = cam.cameraToWorldMatrix();

    vec3f p[8] =
    {
        vec3f(vec2f(l, b) * h_near, -z_near),
        vec3f(vec2f(r, b) * h_near, -z_near),
        vec3f(vec2f(l, t) * h_near, -z_near),
        vec3f(vec2f(r, t) * h_near, -z_near),
        vec3f(vec2f(l, b) * h_far, -z_far),
        vec3f(vec2f(r, b) * h_far, -z_far),
        vec3f(vec2f(l, t) * h_far, -z_far),
        vec3f(vec2f(r, t) * h_far, -z_far),
    };

    for (auto& e : p)
        e = to_world.transform3x4(e);

    _editor->drawLine(p[0], p[1]);
    _editor->drawLine(p[1], p[3]);
    _editor->drawLine(p[3], p[2]);
    _editor->drawLine(p[2], p[0]);

    _editor->drawLine(p[4], p[5]);
    _editor->drawLine(p[5], p[7]);
    _editor->drawLine(p[7], p[6]);
    _editor->drawLine(p[6], p[4]);

    _editor->drawLine(p[0], p[4]);
    _editor->drawLine(p[1], p[5]);
    _editor->drawLine(p[2], p[6]);
    _editor->drawLine(p[3], p[7]);
}

inline void
P2::renderScene()
{
  if (auto camera = Camera::current())
  {
    _renderer->setCamera(camera);
    _renderer->setImageSize(width(), height());
    _renderer->render();
    _program.use();
  }
}

static void
mantain_aspect(ImGuiSizeCallbackData* data)
{
    auto c = static_cast<Camera*>(data->UserData);
    data->DesiredSize.x = round(c->aspectRatio() * data->DesiredSize.y);
}

void
P2::preview(Camera& c) {

    glEnable(GL_SCISSOR_TEST);
    glScissor(0, 0, width() / 4, height() / 4);
    glViewport(0, 0, width() / 4, height() / 4);

    _renderer->setCamera(&c);
    _renderer->setImageSize(width(), height());
    _renderer->render();
    _program.use();

    glViewport(0, 0, width(), height());
    glDisable(GL_SCISSOR_TEST);
}

void
P2::renderObjects(SceneObject* obj)
{
    if (!obj->visible)
        return;

    auto component = obj->component();

    if (auto p = dynamic_cast<Primitive*>(component))
        drawPrimitive(*p);
    else if (auto c = dynamic_cast<Camera*>(component))
        drawCamera(*c);

    if (obj == _current)
    {
        auto t = obj->transform();
        _editor->drawAxes(t->position(), mat3f{ t->rotation() });
    }

    if (obj->hasChildren())
    {
        auto children = obj->children();

        for (const auto& child : children)
        {
            renderObjects(child);
        }
    }
}

constexpr auto CAMERA_RES = 0.01f;
constexpr auto ZOOM_SCALE = 1.01f;

void
P2::render()
{
  if (_viewMode == ViewMode::Renderer)
  {
    renderScene();
    return;
  }
  if (_moveFlags)
  {
    const auto delta = _editor->orbitDistance() * CAMERA_RES;
    auto d = vec3f::null();

    if (_moveFlags.isSet(MoveBits::Forward))
      d.z -= delta;
    if (_moveFlags.isSet(MoveBits::Back))
      d.z += delta;
    if (_moveFlags.isSet(MoveBits::Left))
      d.x -= delta;
    if (_moveFlags.isSet(MoveBits::Right))
      d.x += delta;
    if (_moveFlags.isSet(MoveBits::Up))
      d.y += delta;
    if (_moveFlags.isSet(MoveBits::Down))
      d.y -= delta;
    _editor->pan(d);
  }
  _editor->newFrame();

  // **Begin rendering of temporary scene objects
  // It should be replaced by your rendering code (and moved to scene editor?)
  auto ec = _editor->camera();
  const auto& p = ec->transform()->position();
  auto vp = vpMatrix(ec);

  _program.setUniformMat4("vpMatrix", vp);
  _program.setUniformVec4("ambientLight", _scene->ambientLight);
  _program.setUniformVec3("lightPosition", p);

  for (const auto& obj : _scene->getroot())
  {
      renderObjects(obj);
  }

  if (auto obj = dynamic_cast<SceneObject*>(_current))
  {
      Camera* camera = nullptr;
      auto component = obj->component();

      if (auto c = dynamic_cast<Camera*>(component))
          camera = c;

      if (camera)
          preview(*camera);
  }

  // **End rendering of temporary scene objects
}

bool
P2::windowResizeEvent(int width, int height)
{
  _editor->camera()->setAspectRatio(float(width) / float(height));
  return true;
}

bool
P2::keyInputEvent(int key, int action, int mods)
{
  auto active = action != GLFW_RELEASE && mods == GLFW_MOD_ALT;

  switch (key)
  {
    case GLFW_KEY_W:
      _moveFlags.enable(MoveBits::Forward, active);
      break;
    case GLFW_KEY_S:
      _moveFlags.enable(MoveBits::Back, active);
      break;
    case GLFW_KEY_A:
      _moveFlags.enable(MoveBits::Left, active);
      break;
    case GLFW_KEY_D:
      _moveFlags.enable(MoveBits::Right, active);
      break;
    case GLFW_KEY_Q:
      _moveFlags.enable(MoveBits::Up, active);
      break;
    case GLFW_KEY_E:
      _moveFlags.enable(MoveBits::Down, active);
      break;
  }
  return false;
}

bool
P2::scrollEvent(double, double yOffset)
{
  if (ImGui::GetIO().WantCaptureMouse || _viewMode == ViewMode::Renderer)
    return false;
  _editor->zoom(yOffset < 0 ? 1.0f / ZOOM_SCALE : ZOOM_SCALE);
  return true;
}

bool
P2::mouseButtonInputEvent(int button, int actions, int mods)
{
  if (ImGui::GetIO().WantCaptureMouse || _viewMode == ViewMode::Renderer)
    return false;
  (void)mods;

  auto active = actions == GLFW_PRESS;

  if (button == GLFW_MOUSE_BUTTON_RIGHT)
    _dragFlags.enable(DragBits::Rotate, active);
  else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
    _dragFlags.enable(DragBits::Pan, active);
  if (_dragFlags)
    cursorPosition(_pivotX, _pivotY);
  return true;
}

bool
P2::mouseMoveEvent(double xPos, double yPos)
{
  if (!_dragFlags)
    return false;
  _mouseX = (int)xPos;
  _mouseY = (int)yPos;

  const auto dx = (_pivotX - _mouseX);
  const auto dy = (_pivotY - _mouseY);

  _pivotX = _mouseX;
  _pivotY = _mouseY;
  if (dx != 0 || dy != 0)
  {
    if (_dragFlags.isSet(DragBits::Rotate))
    {
      const auto da = -_editor->camera()->viewAngle() * CAMERA_RES;
      isKeyPressed(GLFW_KEY_LEFT_ALT) ?
        _editor->orbit(dy * da, dx * da) :
        _editor->rotateView(dy * da, dx * da);
    }
    if (_dragFlags.isSet(DragBits::Pan))
    {
      const auto dt = -_editor->orbitDistance() * CAMERA_RES;
      _editor->pan(-dt * math::sign(dx), dt * math::sign(dy), 0);
    }
  }
  return true;
}
