#include "QuadtreeNode.h"
#include "Cell.h"

#include <algorithm>
#include <stdexcept>
#include <iostream>


template<class T>
QuadtreeNode<T>::QuadtreeNode() {
	for (int i = 0; i < 4; i++)
		this->children[i] = nullptr;
}

template<class T>
QuadtreeNode<T>::QuadtreeNode(const T & value) {
	QuadtreeNode();
	this->value = value;
}

template<class T>
T & QuadtreeNode<T>::get_value() {
	return this->value;
}

template<class T>
void QuadtreeNode<T>::set_value(const T & value) {
	this->value = value;
}

template<class T>
QuadtreeNode<T> **QuadtreeNode<T>::get_children() {
	return this->children;
}

template<class T>
void QuadtreeNode<T>::validate_children(QuadtreeNode *children[]) {
	if (children == nullptr)
		throw std::invalid_argument("QuadtreeNode **children is a nullptr");

	bool allEmpty = true, allFull = true, isNullptr = true;
	for (uint8_t i = 0; i < 4; i++) {
		isNullptr = (children[i] == nullptr);
		allEmpty = allEmpty && isNullptr;
		allFull = allFull && !isNullptr;

		if (!(allEmpty || allFull))
			throw std::invalid_argument("QuadtreeNode **children must have 0 or 4 elements");

		if (allEmpty && allFull)
			throw std::invalid_argument("What. The. Fuck.");
	}
}

template<class T>
void QuadtreeNode<T>::set_children(QuadtreeNode *srcChildren[]) {
	validate_children(srcChildren);
	std::copy(srcChildren, srcChildren + 4, this->children);
}

template<class T>
size_t QuadtreeNode<T>::count_nodes() const
{
	size_t n = 1;

	if (children[0] != nullptr){
		for (int i = 0; i < 4; i++)
			n += children[i]->count_nodes();
	}

	return n;
}

template<class T>
void QuadtreeNode<T>::clearChildren()
{
	for (int i = 0; i < 4; i++) {
		delete this->children[i];
		this->children[i] = nullptr;
	}
}

template<class T>
QuadtreeNode<T>::~QuadtreeNode() {
	clearChildren();
}

template class QuadtreeNode<CellProperties>;