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
// OVERVIEW: SceneObject.cpp
// ========
// Source file for scene object.
//
// Author(s): Paulo Pagliosa (and your name)
// Last revision: 07/09/2019

#include "SceneObject.h"
#include "Scene.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// SceneObject implementation
// ===========
void
SceneObject::setParent(SceneObject* parent)
{
  // TODO
	if (_parent == nullptr) {
	// cena atual precisa remover objeto da lista de objetos do própio
		//_scene->remove(this);
	}
	else {
	// pai atual precisa remover objeto da lista de objetos do próprio
		_parent->remove(this);
	}
	_parent = parent;
}

SceneNode*
SceneObject::display(ImGuiTreeNodeFlags flag, SceneNode* current) 
{
	if (_children.size() > 0) {
		// com filhos para mostrar
		auto open = ImGui::TreeNodeEx(
			this,
			current == this ? flag | ImGuiTreeNodeFlags_Selected : flag,
			this->name()
		);

		if (ImGui::IsItemClicked())
			current = this;

		if (open) {
			std::list<Reference<SceneObject>>::iterator it;
			for (it = _children.begin(); it != _children.end(); ++it) {
				current = (*it)->display(flag, current);
			}

			ImGui::TreePop();
		}
	}
	else {
		// sem filhos para mostrar
		flag |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
		ImGui::TreeNodeEx(
			this,
			current == this ? flag | ImGuiTreeNodeFlags_Selected : flag,
			this->name()
		);

		if (ImGui::IsItemClicked())
			current = this;
	}

	return current;
}

} // end namespace cg
