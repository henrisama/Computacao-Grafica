//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018, 2019 Orthrus Group.                         |
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
// Last revision: 23/09/2019

#ifndef __SceneObject_h
#define __SceneObject_h

#include "SceneNode.h"
#include "Transform.h"
#include "imgui.h"
#include "Primitive.h"
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
  SceneObject(const char* name, Scene& scene):
    SceneNode{name},
    _scene{&scene},
    _parent{}
  {
    addComponent(makeUse(&_transform));
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

  /// Returns the transform of this scene object.
  auto transform() const
  {
    return (Transform*)(*_components.begin())->get();
    //return &_transform;
  }

  auto transform()
  {
    return (Transform*)(*_components.begin())->get();
    //return &_transform;
  }

  void addComponent(Component* component)
  {
    component->_sceneObject = this;
    // TODO
    _component = Component::makeUse(component); // temporary

    _components.push_back(Component::makeUse(component));
  }

  /* */
  // **Begin temporary methods
  // They should be replace by your child and component iterators
  Component* component()
  {
    return _component;
  }
  // **End temporary methods

  /*  */
  void append(Reference<SceneObject> ob)
  {
      _children.push_back(ob);
  }

  void remove(Reference<SceneObject> ob)
  {
      _children.remove(ob);
  }

  Primitive* primitive()
  {
      if (_components.size() == 1)
          return nullptr;
      _cit = --_components.end();
      return (Primitive*)(*_cit)->get();
      //return this primitive
  }

  bool hasChildren() 
  {
      if (_children.size() > 0)
          return true;
      return false;
  }

  std::list <Reference<SceneObject>> children() 
  {
      return _children;
  }

  /* Implementado no SceneObject.cpp */
  SceneNode* display(ImGuiTreeNodeFlags flag, SceneNode* current);

  void deleteIt();

private:
  Scene* _scene;
  SceneObject* _parent;
  Transform _transform;
  // **Begin temporary attributes
  // They should be replace by your child and component collections
  Reference<Component> _component;
  // **End temporary attributes
  std::list <Reference<SceneObject>> _children;

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
