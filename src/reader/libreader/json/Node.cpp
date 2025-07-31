#include "Node.hpp"

namespace MALTJson
{

JsonNode::JsonNode(JsonNode && orig)
{
	//copy all
	this->type = orig.type;
	this->view = orig.view;
	this->value = orig.value;

	//reset orig
	orig.type = JSON_NODE_NONE;
	orig.view.map = nullptr;
}

JsonNode::~JsonNode(void)
{
	if (this->type == JSON_NODE_ARRAY &&  this->view.vector != nullptr) {
		delete this->view.vector;
		this->view.vector = nullptr;
	} else if (this->type == JSON_NODE_OBJECT && this->view.vmap != nullptr) {
		delete this->view.vmap;
		this->view.vmap = nullptr;
	}
}

}
