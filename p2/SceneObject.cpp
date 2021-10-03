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
#include "P2.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// SceneObject implementation
// ===========
void
SceneObject::setParent(SceneObject* parent)
{
	if (_parent == nullptr) {
		// cena atual precisa remover objeto da lista de objetos do própio
		_scene->removeOfRoot(this);
	}
	else {
		// pai atual precisa remover objeto da lista de objetos do próprio
		_parent->removeOfChildren(this);
	}
	_parent = parent;
}

void
SceneObject::appendToChildren(Reference<SceneObject> ob) {
	_children.push_back(ob);
}

void
SceneObject::removeOfChildren(Reference<SceneObject> ob) {
	_children.remove(ob);
}

int
SceneObject::getChildrenSize() {
	return _children.size();
}

SceneObject*
SceneObject::getParent() {
	return this->_parent;
}

void
SceneObject::deleteIt() {
	_parent->removeOfChildren(this);
}

SceneNode*
SceneObject::display(ImGuiTreeNodeFlags flag, SceneNode* current) {
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

void
SceneObject::render(GLSL::Program* program, SceneNode* _current) {
	if (!this->visible)
		return;
	if (this->primitive() == nullptr)
		return;

	/*program->setUniformMat4("transform", this->transform()->localToWorldMatrix());

	auto m = this->primitive()->mesh();
	m->bind();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawElements(GL_TRIANGLES, m->vertexCount(), GL_UNSIGNED_INT, 0);*/

	auto component = this->primitive();

	if (auto p = dynamic_cast<Primitive*>(component))
		P2::drawPrimitive(*p);
	else if (auto c = dynamic_cast<Camera*>(component))
		P2::drawCamera(*c);
		

	if (this == _current){
		auto t = this->transform();
		_editor->drawAxes(t->position(), mat3f{ t->rotation() });
	}

	if (_children.size() > 0) {
		// tem filhos para renderizar
		std::list<Reference<SceneObject>>::iterator it;
		for (it = _children.begin(); it != _children.end(); ++it) {
			it->get()->render(program);
		}
	}
}

} // end namespace cg
