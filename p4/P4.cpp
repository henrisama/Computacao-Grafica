#include "geometry/MeshSweeper.h"
#include "P4.h"

MeshMap P4::_defaultMeshes;

inline auto
normalize(const vec4f& p)
{
  return vec3f{p} * math::inverse(p.w);
}

inline auto
viewportToNDC(int x, int y)
{
  GLint v[4];

  glGetIntegerv(GL_VIEWPORT, v);

  const auto xn = float(x - v[0]) * 2.0f / float(v[2]) - 1.0f;
  const auto yn = float(y - v[1]) * 2.0f / float(v[3]) - 1.0f;

  return vec4f{xn, yn, -1, 1};
}

inline Ray
P4::makeRay(int x, int y) const
{
  auto c = _editor->camera();
  mat4f m{vpMatrix(c)};

  m.invert();

  auto p = normalize(m * viewportToNDC(x, height() - y));
  auto t = c->transform();
  Ray r{t->position(), -t->forward()};

  if (c->projectionType() == Camera::Perspective)
    r.direction = (p - r.origin).versor();
  else
    r.origin = p - r.direction * c->nearPlane();
  return r;
}

inline void
P4::buildDefaultMeshes()
{
  _defaultMeshes["None"] = nullptr;
  _defaultMeshes["Box"] = GLGraphics3::box();
  _defaultMeshes["Sphere"] = GLGraphics3::sphere();
}

inline Primitive*
makePrimitive(MeshMapIterator mit)
{
  return new Primitive(mit->second, mit->first);
}

inline void
P4::buildScene()
{
  _current = _scene = new Scene{"Scene 1"};
  _editor = new SceneEditor{*_scene};
  _editor->setDefaultView((float)width() / (float)height());
  // **Begin initialization of temporary attributes
  // It should be replaced by your scene initialization
  {
    auto o = new SceneObject{"Main Camera", *_scene};
    auto camera = new Camera;

    o->addComponent(camera);
    o->setParent(_scene->root());
    _objects.push_back(o);
    o = new SceneObject{"Directional Light", *_scene};
    o->addComponent(new Light);
    o->setParent(_scene->root());
    _objects.push_back(o);
    o = new SceneObject{"Box 1", *_scene};
    o->addComponent(makePrimitive(_defaultMeshes.find("Box")));
    o->setParent(_scene->root());
    _objects.push_back(o);
    Camera::setCurrent(camera);
  }
  // **End initialization of temporary attributes
}

void
P4::initialize()
{
  Application::loadShaders(_program, "shaders/P4.vs", "shaders/P4.fs");
  Assets::initialize();
  buildDefaultMeshes();
  buildScene();
  _renderer = new GLRenderer{*_scene};
  _rayTracer = new RayTracer{*_scene};
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(1.0f, 1.0f);
  glEnable(GL_LINE_SMOOTH);
  _program.use();
}

inline void
P4::hierarchyWindow()
{
  ImGui::Begin("Hierarchy");
  if (ImGui::Button("Create###object"))
    ImGui::OpenPopup("CreateObjectPopup");
  if (ImGui::BeginPopup("CreateObjectPopup"))
  {
    if (ImGui::MenuItem("Empty Object"))
    {
      // TODO: create an empty object.
    }
    if (ImGui::BeginMenu("3D Object"))
    {
      if (ImGui::MenuItem("Box"))
      {
        // TODO: create a new box.
      }
      if (ImGui::MenuItem("Sphere"))
      {
        // TODO: create a new sphere.
      }
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Light"))
    {
      if (ImGui::MenuItem("Directional Light"))
      {
        // TODO: create a new directional light.
      }
      if (ImGui::MenuItem("Point Light"))
      {
        // TODO: create a new pontual light.
      }
      if (ImGui::MenuItem("Spotlight"))
      {
        // TODO: create a new spotlight.
      }
      ImGui::EndMenu();
    }
    if (ImGui::MenuItem("Camera"))
    {
      // TODO: create a new camera.
    }
    ImGui::EndPopup();
  }
  ImGui::Separator();

  // **Begin hierarchy of temporary scene objects
  // It should be replaced by your hierarchy
  auto f = ImGuiTreeNodeFlags_OpenOnArrow;
  auto open = ImGui::TreeNodeEx(_scene,
    _current == _scene ? f | ImGuiTreeNodeFlags_Selected : f,
    _scene->name());

  if (ImGui::IsItemClicked())
    _current = _scene;
  if (open)
  {
    for (const auto& o : _objects)
    {
      auto f = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

      ImGui::TreeNodeEx(o,
        _current == o ? f | ImGuiTreeNodeFlags_Selected : f,
        o->name());
      if (ImGui::IsItemClicked())
        _current = o;
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
P4::sceneGui()
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
P4::inspectShape(Primitive& primitive)
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
}

inline void
P4::inspectMaterial(Material& material)
{
  ImGui::ColorEdit3("Ambient", material.ambient);
  ImGui::ColorEdit3("Diffuse", material.diffuse);
  ImGui::ColorEdit3("Spot", material.spot);
  ImGui::DragFloat("Shine", &material.shine, 1, 0, 1000.0f);
  ImGui::ColorEdit3("Specular", material.specular);
}

inline void
P4::inspectPrimitive(Primitive& primitive)
{
  //const auto flag = ImGuiTreeNodeFlags_NoTreePushOnOpen;

  //if (ImGui::TreeNodeEx("Shape", flag))
    inspectShape(primitive);
  //if (ImGui::TreeNodeEx("Material", flag))
    inspectMaterial(primitive.material);
}

inline void
P4::inspectLight(Light& light)
{
  static const char* lightTypes[]{"Directional", "Point", "Spot"};
  auto lt = light.type();

  if (ImGui::BeginCombo("Type", lightTypes[lt]))
  {
    for (auto i = 0; i < IM_ARRAYSIZE(lightTypes); ++i)
    {
      bool selected = lt == i;

      if (ImGui::Selectable(lightTypes[i], selected))
        lt = (Light::Type)i;
      if (selected)
        ImGui::SetItemDefaultFocus();
    }
    ImGui::EndCombo();
  }
  light.setType(lt);
  ImGui::ColorEdit3("Color", light.color);
}

void
P4::inspectCamera(Camera& camera)
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
P4::addComponentButton(SceneObject& object)
{
  if (ImGui::Button("Add Component"))
    ImGui::OpenPopup("AddComponentPopup");
  if (ImGui::BeginPopup("AddComponentPopup"))
  {
    if (ImGui::MenuItem("Primitive"))
    {
      // TODO
    }
    if (ImGui::MenuItem("Light"))
    {
      // TODO
    }
    if (ImGui::MenuItem("Camera"))
    {
      // TODO
    }
    ImGui::EndPopup();
  }
}

inline void
P4::sceneObjectGui()
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
  else if (auto l = dynamic_cast<Light*>(component))
  {
    auto notDelete{true};
    auto open = ImGui::CollapsingHeader(l->typeName(), &notDelete);

    if (!notDelete)
    {
      // TODO: delete light
    }
    else if (open)
      inspectLight(*l);
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
P4::objectGui()
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
P4::inspectorWindow()
{
  ImGui::Begin("Inspector");
  objectGui();
  ImGui::End();
}

inline void
P4::editorViewGui()
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
  {
    static int sm;

    ImGui::Combo("Shading Mode", &sm, "None\0Flat\0Gouraud\0\0");
    // TODO

    static Color edgeColor;
    static bool showEdges;

    ImGui::ColorEdit3("Edges", edgeColor);
    ImGui::SameLine();
    ImGui::Checkbox("###showEdges", &showEdges);
  }
  ImGui::Separator();
  ImGui::Checkbox("Show Ground", &_editor->showGround);
}

inline void
P4::assetsWindow()
{
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
    // next semester
  }
  ImGui::End();
}

inline void
P4::editorView()
{
  if (!_showEditorView)
    return;
  ImGui::Begin("Editor View Settings");
  editorViewGui();
  ImGui::End();
}

inline void
P4::fileMenu()
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
P4::showOptions()
{
  ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.6f);
  showStyleSelector("Color Theme##Selector");
  ImGui::ColorEdit3("Selected Wireframe", _selectedWireframeColor);
  ImGui::PopItemWidth();
}

inline void
P4::mainMenu()
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
        static const char* viewLabels[]{"Editor", "OpenGL", "Ray Tracer"};

        if (ImGui::BeginCombo("View", viewLabels[_viewMode]))
        {
          for (auto i = 0; i < IM_ARRAYSIZE(viewLabels); ++i)
          {
            if (ImGui::Selectable(viewLabels[i], _viewMode == i))
              _viewMode = (ViewMode)i;
          }
          ImGui::EndCombo();
          // TODO: change mode only if scene has changed
          if (_viewMode == ViewMode::Editor)
            _image = nullptr;
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
P4::gui()
{
  mainMenu();
  if (_viewMode == ViewMode::RT_Renderer)
    return;
  hierarchyWindow();
  inspectorWindow();
  assetsWindow();
  editorView();
}

inline void
drawMesh(GLMesh* mesh, GLuint mode)
{
  glPolygonMode(GL_FRONT_AND_BACK, mode);
  glDrawElements(GL_TRIANGLES, mesh->vertexCount(), GL_UNSIGNED_INT, 0);
}

inline void
P4::drawPrimitive(Primitive& primitive)
{
  auto mesh = primitive.mesh();
  auto m = glMesh(mesh);

  if (nullptr == m)
    return;

  auto t = primitive.transform();
  auto normalMatrix = mat3f{t->worldToLocalMatrix()}.transposed();

  _program.setUniformMat4("transform", t->localToWorldMatrix());
  _program.setUniformMat3("normalMatrix", normalMatrix);
  _program.setUniformVec4("color", primitive.material.diffuse);
  _program.setUniform("flatMode", (int)0);
  m->bind();
  drawMesh(m, GL_FILL);
  // **Begin BVH test
  auto bvh = bvhMap[mesh];

  if (bvh == nullptr)
    bvhMap[mesh] = bvh = new BVH{*mesh, 16};
  // **End BVH test
  if (primitive.sceneObject() != _current)
    return;
  //_program.setUniformVec4("color", _selectedWireframeColor);
  //_program.setUniform("flatMode", (int)1);
  //drawMesh(m, GL_LINE);
  //_editor->setVectorColor(Color::white);
  //_editor->drawNormals(*mesh, t->localToWorldMatrix(), normalMatrix);
  //_editor->setLineColor(_selectedWireframeColor);
  //_editor->drawBounds(mesh->bounds(), t->localToWorldMatrix());
  bvh->iterate([this, t] (const BVHNodeInfo& node)
  {
    _editor->setLineColor(node.isLeaf ? Color::yellow : Color::magenta);
    _editor->drawBounds(node.bounds, t->localToWorldMatrix());
  });
}

inline void
P4::drawLight(Light& light)
{
  // TODO
}

inline void
P4::drawCamera(Camera& camera)
{
  // TODO
}

inline void
P4::renderScene()
{
  if (auto camera = Camera::current())
  {
    if (_viewMode == ViewMode::GL_Renderer)
    {
      _renderer->setCamera(camera);
      _renderer->setImageSize(width(), height());
      _renderer->render();
      _program.use();
      return;
    }
    if (_image == nullptr)
    {
      const auto w = width(), h = height();

      _image = new GLImage{w, h};
      _rayTracer->setImageSize(w, h);
      _rayTracer->setCamera(camera);
      _rayTracer->renderImage(*_image);
    }
    _image->draw(0, 0);
  }
}

constexpr auto CAMERA_RES = 0.01f;
constexpr auto ZOOM_SCALE = 1.01f;

void
P4::render()
{
  if (_viewMode != ViewMode::Editor)
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
  for (const auto& o : _objects)
  {
    if (!o->visible)
      continue;

    auto component = o->component();

    if (auto p = dynamic_cast<Primitive*>(component))
      drawPrimitive(*p);
    if (o == _current)
    {
      auto t = o->transform();
      _editor->drawAxes(t->position(), mat3f{t->rotation()});
    }
  }
  // **End rendering of temporary scene objects
}

bool
P4::windowResizeEvent(int width, int height)
{
  _editor->camera()->setAspectRatio(float(width) / float(height));
  _viewMode = ViewMode::Editor;
  _image = nullptr;
  return true;
}

bool
P4::keyInputEvent(int key, int action, int mods)
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
    case GLFW_KEY_Z:
      _moveFlags.enable(MoveBits::Down, active);
      break;
  }
  return false;
}

bool
P4::scrollEvent(double, double yOffset)
{
  if (ImGui::GetIO().WantCaptureMouse)
    return false;
  _editor->zoom(yOffset < 0 ? 1.0f / ZOOM_SCALE : ZOOM_SCALE);
  return true;
}

bool
P4::mouseButtonInputEvent(int button, int actions, int mods)
{
  if (ImGui::GetIO().WantCaptureMouse)
    return false;
  (void)mods;

  auto active = actions == GLFW_PRESS;

  if (button == GLFW_MOUSE_BUTTON_LEFT)
  {
    if (active)
    {
      cursorPosition(_pivotX, _pivotY);

      const auto ray = makeRay(_pivotX, _pivotY);
      auto minDistance = math::Limits<float>::inf();

      // **Begin picking of temporary scene objects
      // It should be replaced by your picking code
      for (const auto& o : _objects)
      {
        if (!o->visible)
          continue;

        auto component = o->component();
        float distance;

        if (auto p = dynamic_cast<Primitive*>(component))
          if (p->intersect(ray, distance) && distance < minDistance)
          {
            minDistance = distance;
            _current = o;
          }
      }
      // **End picking of temporary scene objects
    }
    return true;
  }
  if (button == GLFW_MOUSE_BUTTON_RIGHT)
    _dragFlags.enable(DragBits::Rotate, active);
  else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
    _dragFlags.enable(DragBits::Pan, active);
  if (_dragFlags)
    cursorPosition(_pivotX, _pivotY);
  return true;
}

bool
P4::mouseMoveEvent(double xPos, double yPos)
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
