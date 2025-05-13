#include "Scene.hpp"


Scene::Scene()
	: parserMap({
		{ "H", [this](istr_t& is) -> istr_t& { return H(is); } },
		{ "A", [this](istr_t& is) -> istr_t& { return A(is); } }
	}),
	  header("Default"),
	  ambient(0xFFFFFF)
{}

istr_t& Scene::H(istr_t& is) {
	is >> header;
	return is;
}

istr_t& Scene::A(istr_t& is) {
	is >> ambient;
	return is;
}

