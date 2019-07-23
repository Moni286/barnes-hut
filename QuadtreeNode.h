#pragma once

template<class T>
class QuadtreeNode
{

public:
	QuadtreeNode(const T & value);

	T & get_value();
	void set_value(const T & value);

	QuadtreeNode<T> **get_children();

	void set_children(QuadtreeNode *children[]);
	size_t count_nodes() const;

	void clearChildren();
	~QuadtreeNode();

protected:
	QuadtreeNode();
	T value;

private:
	QuadtreeNode *children[4];

	void validate_children(QuadtreeNode *children[]);

};

