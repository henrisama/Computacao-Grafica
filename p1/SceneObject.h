//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018 Orthrus Group.                               |
//|                                                                 |
//| This software is provided 'as-is', without any express or       |
//| implied warranty. In no event will the authors be held liable   |
//| for any damages arising from the use of this software.          |
//|                                                                 |
//| Permission is granted to anyone to use this software for any    |
//| purpose, including commercial applications, and to alter it and |
//| redistribute it freely, subject to the following restrictions:  |
//|                                                                 |
//| 1. The origin of this software must not be misrepresented; you  |
//| must not claim that you wrote the original software. If you use |
//| this software in a product, an acknowledgment in the product    |
//| documentation would be appreciated but is not required.         |
//|                                                                 |
//| 2. Altered source versions must be plainly marked as such, and  |
//| must not be misrepresented as being the original software.      |
//|                                                                 |
//| 3. This notice may not be removed or altered from any source    |
//| distribution.                                                   |
//|                                                                 |
//[]---------------------------------------------------------------[]
//
// OVERVIEW: SceneObject.h
// ========
// Class definition for scene object.
//
// Author(s): Paulo Pagliosa (and your name)
// Last revision: 25/08/2018

#ifndef __SceneObject_h
#define __SceneObject_h

#include "SceneNode.h"
#include "Transform.h"
#include "Primitive.h"
#include "imgui.h"
#include <list>

namespace cg
{ // begin namespace cg

// Forward definition
class Scene;


/////////////////////////////////////////////////////////////////////
//
// SceneObject: scene object class
// ===========
class SceneObject: public SceneNode
{
public:
  bool visible{true};

  /// Constructs an empty scene object.
  SceneObject(const char* name, Scene* scene):
    SceneNode{name},
    _scene{scene},
    _parent{}
  {

    _components.push_back(new Transform);
    _cit = _components.begin();
    makeUse((Transform*) (*_cit)->get());
    //makeUse(&_transform);
  }

  SceneObject(const char* name, Scene* scene, Primitive* primitive):
    SceneNode{ name },
    _scene{ scene },
    _parent{}
  {
      _components.push_back(new Transform);
      _cit = _components.begin();
      makeUse((Transform*)(*_cit)->get());

      _components.push_back(primitive);
      _cit = --_components.end();

  }

  /// Returns the scene which this scene object belong to.
  auto scene() const
  {
    return _scene;
  }

  /// Returns the parent of this scene object.
  auto parent() const
  {
    return _parent;
  }

  /// Sets the parent of this scene object.
  void setParent(SceneObject* parent);

  SceneObject* getParent();

  void appendToChildren(Reference<SceneObject> ob);

  void removeOfChildren(Reference<SceneObject> ob);

  int getChildrenSize();

  void deleteIt();

  SceneNode* display(ImGuiTreeNodeFlags flag, SceneNode* current);

  void render(GLSL::Program* program);


  /// Returns the transform of this scene object.
  auto transform()
  {
    return (Transform*) (*_components.begin())->get();
    //return this transform;
  }

  Primitive* primitive()
  {
    if (_components.size() == 1)
        return nullptr;
    return (Primitive*) (*_cit)->get();
    //return this primitive
  }

  

private:
  Scene* _scene;
  SceneObject* _parent;
  // Transform _transform;
  
  std::list <Reference<SceneObject>> _children;
  std::list <Reference<SceneObject>>::iterator _sit;

  std::list <Reference<Component>> _components;
  std::list <Reference<Component>>::iterator _cit;

  friend class Scene;

}; // SceneObject

/// Returns the transform of a component.
inline Transform*
Component::transform() // declared in Component.h
{
  return sceneObject()->transform();
}

/// Returns the parent of a transform.
inline Transform*
Transform::parent() const // declared in Transform.h
{
  if (auto p = sceneObject()->parent())
    return p->transform();
   return nullptr;
}

} // end namespace cg

#endif // __SceneObject_h
