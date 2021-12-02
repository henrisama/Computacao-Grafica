#include "geometry/MeshSweeper.h"
#include "P3.h"

MeshMap P3::_defaultMeshes;

inline void
P3::buildDefaultMeshes()
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
P3::buildScene()
{
    _current = _scene = new Scene{ "Scene 1" };
    _editor = new SceneEditor{ *_scene };
    _editor->setDefaultView((float)width() / (float)height());
    {
        auto camera = new Camera;
        SceneObject* obj = new SceneObject{ "Camera 1", *_scene };
        obj->addComponent(camera);
        obj->setParent(_scene->root());
        obj->transform()->setLocalPosition(vec3f(0,0,20));
        _scene->root()->append(obj);

        obj = new SceneObject{ "Box 1", *_scene };
        obj->addComponent(makePrimitive(_defaultMeshes.find("Box")));
        obj->setParent(_scene->root());
        obj->transform()->setLocalPosition(vec3f(0, 5, 0));
        _scene->root()->append(obj);

        obj = new SceneObject{ "Sphere 1", *_scene };
        obj->addComponent(makePrimitive(_defaultMeshes.find("Sphere")));
        obj->setParent(_scene->root());
        obj->transform()->setLocalPosition(vec3f(0, -5, 0));
        _scene->root()->append(obj);

        obj = new SceneObject{ "Object 1", *_scene };
        obj->setParent(_scene->root());
        _scene->root()->append(obj);

        obj = new SceneObject{ "Light 1", *_scene };
        obj->addComponent(new Light);
        obj->setParent(_scene->root());
        _scene->root()->append(obj);

        Camera::setCurrent(camera);
    }
}

void
P3::initialize()
{
    Application::loadShaders(_program, "shaders/p3.vs", "shaders/p3.fs");
    Application::loadShaders(_phongProgram, "shaders/phong.vs", "shaders/phong.fs");
    Application::loadShaders(_gouraudProgram, "shaders/gouraud.vs", "shaders/gouraud.fs");

    Assets::initialize();
    buildDefaultMeshes();
    buildScene();
    _renderer = new GLRenderer{ *_scene };
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0f, 1.0f);
    glEnable(GL_LINE_SMOOTH);
    _program.use();
}

inline void
P3::hierarchyWindow()
{
    ImGui::Begin("Hierarchy");
    if (ImGui::Button("Create###object"))
        ImGui::OpenPopup("CreateObjectPopup");
    if (ImGui::BeginPopup("CreateObjectPopup"))
    {
        if (ImGui::MenuItem("Empty Object"))
        {
            _name = "";
            SceneObject* ob;

            if(_current != _scene)
                ob = (SceneObject*)_current;
            else
                ob = ((Scene*)_current)->root();

            _name
                .append("Object ")
                .append(std::to_string(objectCount++));

            _newObject = new SceneObject{ _name.c_str(), *_scene };
            _newObject->setParent(ob);
            ob->append(_newObject);

        }
        if (ImGui::BeginMenu("3D Object"))
        {
            if (ImGui::MenuItem("Box"))
            {
                _name = "";
                SceneObject* ob;

                if (_current != _scene)
                    ob = (SceneObject*)_current;
                else
                    ob = ((Scene*)_current)->root();

                /// condição para não criar box dentro de um objeto de cena com componente camera ou light
                if (!dynamic_cast<Camera*>(ob->component())
                    && !dynamic_cast<Light*>(ob->component()))
                {
                    _name
                        .append("Box ")
                        .append(std::to_string(boxCount++));

                    _newObject = new SceneObject{ _name.c_str(), *_scene };
                    _newObject->setParent(ob);
                    _newObject->addComponent(makePrimitive(_defaultMeshes.find("Box")));
                    ob->append(_newObject);
                }
            }
            if (ImGui::MenuItem("Sphere"))
            {
                _name = "";
                SceneObject* ob;

                if (_current != _scene)
                    ob = (SceneObject*)_current;
                else
                    ob = ((Scene*)_current)->root();

                /// condição para não criar sphere dentro de um objeto de cena com componente camera ou light
                if (!dynamic_cast<Camera*>(ob->component())
                    && !dynamic_cast<Light*>(ob->component()))
                {
                    _name
                        .append("Sphere  ")
                        .append(std::to_string(sphereCount++));

                    _newObject = new SceneObject{ _name.c_str(), *_scene };
                    _newObject->setParent(ob);
                    _newObject->addComponent(makePrimitive(_defaultMeshes.find("Sphere")));
                    ob->append(_newObject);
                }
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Light"))
        {
            _name = "";
            SceneObject* ob;
            auto light = new Light;

            if (ImGui::MenuItem("Directional Light"))
            {
                _name
                    .append("Light ")
                    .append(std::to_string(lightCount++));

                light->setType(Light::Directional);

                if (_current != _scene)
                    ob = (SceneObject*)_current;
                else
                    ob = ((Scene*)_current)->root();

                _newObject = new SceneObject{ _name.c_str(), *_scene };
                _newObject->setParent(ob);
                _newObject->addComponent(light);
                ob->append(_newObject);
            }
            if (ImGui::MenuItem("Point Light"))
            {
                _name
                    .append("Light ")
                    .append(std::to_string(lightCount++));

                light->setType(Light::Point);

                if (_current != _scene)
                    ob = (SceneObject*)_current;
                else
                    ob = ((Scene*)_current)->root();

                _newObject = new SceneObject{ _name.c_str(), *_scene };
                _newObject->setParent(ob);
                _newObject->addComponent(light);
                ob->append(_newObject);
            }
            if (ImGui::MenuItem("Spotlight"))
            {
                _name
                    .append("Light ")
                    .append(std::to_string(lightCount++));

                light->setType(Light::Spot);

                if (_current != _scene)
                    ob = (SceneObject*)_current;
                else
                    ob = ((Scene*)_current)->root();

                _newObject = new SceneObject{ _name.c_str(), *_scene };
                _newObject->setParent(ob);
                _newObject->addComponent(light);
                ob->append(_newObject);
            }
            ImGui::EndMenu();
        }
        if (ImGui::MenuItem("Camera"))
        {
            _name = "";
            SceneObject* ob;

            if (_current != _scene)
                ob = (SceneObject*)_current;
            else
                ob = ((Scene*)_current)->root();

            /// condição para não criar camera dentro de um objeto de cena com componente camera ou light
            if (!dynamic_cast<Camera*>(ob->component())
                && !dynamic_cast<Light*>(ob->component()))
            {
                _name
                    .append("Camera ")
                    .append(std::to_string(cameraCount++));

                auto camera = new Camera;
                _newObject = new SceneObject{ _name.c_str(), *_scene };
                _newObject->setParent(ob);
                _newObject->addComponent(camera);
                ob->append(_newObject);
            }
        }
        ImGui::EndPopup();
    }

    if (ImGui::Button("Delete"))
    {
        if (_current != _scene)
        {
            SceneObject* ob = (SceneObject*)_current;
            _current = ob->parent();
            ob->deleteIt();
        }
    }

    ImGui::Separator();

    auto flag{ ImGuiTreeNodeFlags_OpenOnArrow };
    auto open = ImGui::TreeNodeEx(_scene,
        _current == _scene ? flag | ImGuiTreeNodeFlags_Selected : flag,
        _scene->name());

    if (ImGui::IsItemClicked())
        _current = _scene;
    if (open)
    {
        auto aux = _scene->root()->children();
        for (const auto& obj : aux)
        {
            _current = obj->display(flag, _current);
        }

        ImGui::TreePop();
    }
    ImGui::End();
}

namespace ImGui
{ // begin namespace ImGui

    void
        ObjectNameInput(NameableObject* object)
    {
        const int bufferSize{ 128 };
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
P3::sceneGui()
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
P3::inspectShape(Primitive& primitive)
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
P3::inspectMaterial(Material& material)
{
    ImGui::ColorEdit3("Ambient", material.ambient);
    ImGui::ColorEdit3("Diffuse", material.diffuse);
    ImGui::ColorEdit3("Spot", material.spot);
    ImGui::DragFloat("Shine", &material.shine, 1, 0, 1000.0f);
}

inline void
P3::inspectPrimitive(Primitive& primitive)
{
    //const auto flag = ImGuiTreeNodeFlags_NoTreePushOnOpen;

    //if (ImGui::TreeNodeEx("Shape", flag))
    inspectShape(primitive);
    //if (ImGui::TreeNodeEx("Material", flag))
    inspectMaterial(primitive.material);
}

inline void
P3::inspectLight(Light& light)
{
    static const char* lightTypes[]{ "Directional", "Point", "Spot" };
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
P3::inspectCamera(Camera& camera)
{
    static const char* projectionNames[]{ "Perspective", "Orthographic" };
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
P3::addComponentButton(SceneObject& object)
{
    if (ImGui::Button("Add Component"))
        ImGui::OpenPopup("AddComponentPopup");
    if (ImGui::BeginPopup("AddComponentPopup"))
    {
        if (ImGui::MenuItem("Primitive"))
        {
            if (dynamic_cast<Transform*>(object.component()))
            {
                object.addComponent(makePrimitive(_defaultMeshes.find("Box")));
            }
        }
        if (ImGui::MenuItem("Light"))
        {
            if (dynamic_cast<Transform*>(object.component()))
            {
                object.addComponent(new Light);
            }
        }
        if (ImGui::MenuItem("Camera"))
        {
            if (dynamic_cast<Transform*>(object.component()))
            {
                object.addComponent(new Camera);
            }
        }
        ImGui::EndPopup();
    }
}

inline void
P3::sceneObjectGui()
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

    auto component = object->component();

    if (auto p = dynamic_cast<Primitive*>(component))
    {
        auto notDelete{ true };
        auto open = ImGui::CollapsingHeader(p->typeName(), &notDelete);

        if (!notDelete)
        {
            // delete primitive
            object->delComponent(component);
        }
        else if (open)
            inspectPrimitive(*p);
    }
    else if (auto l = dynamic_cast<Light*>(component))
    {
        auto notDelete{ true };
        auto open = ImGui::CollapsingHeader(l->typeName(), &notDelete);

        if (!notDelete)
        {
            // delete light
            object->delComponent(component);
        }
        else if (open)
            inspectLight(*l);
    }
    else if (auto c = dynamic_cast<Camera*>(component))
    {
        auto notDelete{ true };
        auto open = ImGui::CollapsingHeader(c->typeName(), &notDelete);

        if (!notDelete)
        {
            // delete camera
            object->delComponent(component);
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
P3::objectGui()
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
P3::inspectorWindow()
{
    ImGui::Begin("Inspector");
    objectGui();
    ImGui::End();
}

void
P3::focus()
{
    if (auto current = dynamic_cast<SceneObject*>(_current))
    {
        auto eCameraTransform = _editor->camera()->transform();
        auto curPosition = current->transform()->position();

        auto s = current->transform()->localScale();
        auto d = ((s[0] + s[1] + s[2]) / 3) * 5;

        auto ea = eCameraTransform->eulerAngles();

        auto x = d * (float)sin(math::toRadians(ea.y)) * (float)cos(math::toRadians(ea.x));
        auto y = d * (float)-sin(math::toRadians(ea.x));
        auto z = d * (float)cos(math::toRadians(ea.x)) * (float)cos(math::toRadians(ea.y));

        curPosition += vec3f{ x,y,z };
        eCameraTransform->setPosition(curPosition);
    }
}

inline void
P3::editorViewGui()
{
    if (ImGui::Button("Set Default View"))
        _editor->setDefaultView(float(width()) / float(height()));

    if (ImGui::Button("Focus"))
        focus();

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

        ImGui::Combo("Shading Mode", &sm, "None\0Flat\0Gouraud\0Phong\0");
        // TODO

        switch (sm)
        {
            case 0:
                _program.use();
                break;
            case 1:
                
                break;
            case 2:
                _gouraudProgram.use();
                break;
            case 3:
                _phongProgram.use();
                break;
            default:
                break;
        }

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
P3::assetsWindow()
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
P3::editorView()
{
    if (!_showEditorView)
        return;
    ImGui::Begin("Editor View Settings");
    editorViewGui();
    ImGui::End();
}

inline void
P3::fileMenu()
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
P3::showOptions()
{
    ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.6f);
    showStyleSelector("Color Theme##Selector");
    ImGui::ColorEdit3("Selected Wireframe", _selectedWireframeColor);
    ImGui::PopItemWidth();
}

inline void
P3::mainMenu()
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
                static const char* viewLabels[]{ "Editor", "Renderer" };

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
P3::gui()
{
    mainMenu();
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
P3::drawPrimitive(Primitive& primitive)
{
    auto program = GLSL::Program::current();
    auto m = glMesh(primitive.mesh());

    if (nullptr == m)
        return;

    auto t = primitive.transform();
    auto normalMatrix = mat3f{ t->worldToLocalMatrix() }.transposed();

    program->setUniformMat4("transform", t->localToWorldMatrix());
    program->setUniformMat3("normalMatrix", normalMatrix);
    program->setUniformVec4("color", primitive.material.diffuse);
    program->setUniform("flatMode", (int)0);
    m->bind();
    drawMesh(m, GL_FILL);
    if (primitive.sceneObject() != _current)
        return;
    program->setUniformVec4("color", _selectedWireframeColor);
    program->setUniform("flatMode", (int)1);
    drawMesh(m, GL_LINE);
}

inline void
P3::drawLight(Light& light)
{
    // TODO
    std::vector<vec3f> lightVertexes;

    if (light.type() == light.Directional) {
        lightVertexes = {
            // corpo da luz directional
            { 0,0.2,0.5 }, { 0,0.2,-0.5 }, { -0.2,0,0.5 }, { -0.2,0,-0.5 },
            { 0,0,0.5 }, { 0,0,-0.5 }, { 0.2,0,0.5 }, { 0.2,0,-0.5 },
            { 0,-0.2,0.5 }, { 0,-0.2,-0.5 },

            // setinhas das pontas
            { 0,0.2,-0.5 }, { -0.125,0.2,-0.25 }, { 0,0.2,-0.5 }, { 0.125,0.2,-0.25 },
            { -0.2,0,-0.5 }, { -0.325,0,-0.25 }, { -0.2,0,-0.5 }, { 0.325,0,-0.25 },
            { 0,0,-0.5 }, { -0.125,0,-0.25 }, { 0,0,-0.5 }, { 0.125,0,-0.25 },
            { 0.2,0,-0.5 }, { 0.075,0,-0.25 }, { 0.2,0,-0.5 }, { 0.375,0,-0.25 },
            { 0,-0.2,-0.5 }, { -0.125,-0.2,-0.25 }, { 0,-0.2,-0.5 }, { 0.125,-0.2,-0.25 }
        };
    }
    else if (light.type() == light.Point) {
        lightVertexes = {
            { 0,0.2,0 }, { 0,0,0.2 }, { 0,0,0.2 }, { 0.2,0,0 },
            { 0.2,0,0 }, { 0,0.2,0 }, { 0,-0.2,0 }, { 0,0,-0.2 },
            { 0,0,-0.2 }, { -0.2,0,0 }, { -0.2,0,0 }, { 0,-0.2,0 },
            { 0,0.2,0 }, { 0,0,0.2 }, { 0,0,0.2 }, { -0.2,0,0 },
            { -0.2,0,0 }, { 0,0.2,0 }, { 0,-0.2,0 }, { 0,0,0.2 },
            { 0,0,0.2 }, { 0.2,0,0 }, { 0.2,0,0 }, { 0,-0.2,0 },
            { 0,-0.2,0 }, { 0,0,0.2 }, { 0,0,0.2 }, { -0.1,0,0 },
            { -0.2,0,0 }, { 0,-0.2,0 }, { 0,0.2,0 }, { 0,0,-0.2 },
            { 0,0,-0.2 }, { 0.2,0,0 }, { 0.2,0,0 }, { 0,0.2,0 }
        };
    }
    else if (light.type() == light.Spot) {

        lightVertexes = {
            { 0,0,0 }, { -0.5,1.5,-0.5 }, { 0,0,0 }, { 0.5,1.5,-0.5 },
            { 0,0,0 }, { 0.5,1.5,0.5 }, { 0,0,0 }, { -0.5,1.5,0.5 },
            { -0.5,1.5,-0.5 }, { 0.5,1.5,-0.5 }, { 0.5,1.5,-0.5 },  { 0.5,1.5,0.5 },
            { 0.5,1.5,0.5 }, { -0.5,1.5,0.5 }, { -0.5,1.5,0.5 }, { -0.5,1.5,-0.5 }
        };
        // size of the draw
        float spotSize = 1.25;

        // calculates radius
        auto circleRadius = (float) tan(math::toRadians<float>(light.ghama())) * spotSize;

        // position of circle of spot light drawing
        vec3f circleCenter{ 0, 0, -spotSize };
        vec3f circleLeftEdge{ circleRadius, 0, -spotSize };
        vec3f circleRightEdge{ -circleRadius, 0, -spotSize };
        vec3f circleUpperEdge{ 0, circleRadius, -spotSize };
        vec3f circleBottomEdge{ 0, -circleRadius, -spotSize };

        circleCenter = light.transform()->transform(circleCenter);
        circleLeftEdge = light.transform()->transform(circleLeftEdge);
        circleRightEdge = light.transform()->transform(circleRightEdge);
        circleUpperEdge = light.transform()->transform(circleUpperEdge);
        circleBottomEdge = light.transform()->transform(circleBottomEdge);

        // normal of the circle
        auto circleNormal = light.transform()->rotation() * vec3f { 0, 0, -1 };

        auto spotEdge = light.transform()->transform(vec3f(0, 0, 0));

        // save scale
        auto temp = light.transform()->localScale();
        light.transform()->setLocalScale({ 1,1,1 });

        _editor->setLineColor(light.color);
        _editor->drawCircle(circleCenter, circleRadius, circleNormal);
        _editor->drawLine(spotEdge, circleLeftEdge);
        _editor->drawLine(spotEdge, circleRightEdge);
        _editor->drawLine(spotEdge, circleUpperEdge);
        _editor->drawLine(spotEdge, circleBottomEdge);


        light.transform()->setLocalScale(temp);
        _editor->setLineColor(Color::blue);
    }

    if (light.type() != Light::Spot)
    {
        auto lightTransform = light.transform();
        auto pos = lightTransform->position();

        // save scale
        auto temp = lightTransform->localScale();

        lightTransform->setLocalScale({ 1,1,1 });

        for (int i = 0; i < lightVertexes.size(); i++) {
            lightVertexes[i] = lightTransform->transform(lightVertexes[i]);
        }

        _editor->setLineColor(light.color);
        for (int i = 0; i < lightVertexes.size() / 2; i++) {
            _editor->drawLine(lightVertexes[i * 2], lightVertexes[(i * 2) + 1]);
        }

        // reset previous scale & line color
        lightTransform->setLocalScale(temp);
        _editor->setLineColor(Color::blue);
    }
}

inline void
P3::drawCamera(Camera& cam)
{
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
P3::renderScene()
{
    if (auto camera = Camera::current())
    {
        _renderer->setCamera(camera);
        _renderer->setImageSize(width(), height());
        _renderer->render();
    }
}

constexpr auto CAMERA_RES = 0.01f;
constexpr auto ZOOM_SCALE = 1.01f;

void
P3::preview(Camera& c) 
{
    _phongProgram.use();

    glEnable(GL_SCISSOR_TEST);
    glScissor(0, 0, width() / 4, height() / 4);
    glViewport(0, 0, width() / 4, height() / 4);

    _renderer->setCamera(&c);
    _renderer->setImageSize(width(), height());
    _renderer->render();

    glViewport(0, 0, width(), height());
    glDisable(GL_SCISSOR_TEST);
}

void
P3::renderObjects(SceneObject* obj)
{
    if (!obj->visible)
        return;

    auto component = obj->component();

    if (auto p = dynamic_cast<Primitive*>(component))
        drawPrimitive(*p);
    else if (auto c = dynamic_cast<Camera*>(component))
        drawCamera(*c);
    else if (auto c = dynamic_cast<Light*>(component))
        drawLight(*c);

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

void
P3::render()
{
    auto program = GLSL::Program::current();

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

    auto ec = _editor->camera();
    const auto& p = ec->transform()->position();
    auto vp = vpMatrix(ec);

    program->setUniformMat4("vpMatrix", vp);
    program->setUniformVec4("ambientLight", _scene->ambientLight);
    program->setUniformVec3("lightPosition", p);
    
    for (const auto& obj : _scene->root()->children())
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
}

bool
P3::windowResizeEvent(int width, int height)
{
    _editor->camera()->setAspectRatio(float(width) / float(height));
    return true;
}

bool
P3::keyInputEvent(int key, int action, int mods)
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
    case GLFW_KEY_F:
        focus();
        break;
    }
    return false;
}

bool
P3::scrollEvent(double, double yOffset)
{
    if (ImGui::GetIO().WantCaptureMouse)
        return false;
    _editor->zoom(yOffset < 0 ? 1.0f / ZOOM_SCALE : ZOOM_SCALE);
    return true;
}

bool
P3::mouseButtonInputEvent(int button, int actions, int mods)
{
    if (ImGui::GetIO().WantCaptureMouse)
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
P3::mouseMoveEvent(double xPos, double yPos)
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