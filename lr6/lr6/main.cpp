#include <iostream>


#include "figure.h"
#include "renderer.h"

int main() {

	std::shared_ptr<Icosaedr> model = std::make_shared<Icosaedr>();
	
	size_t approximation = 4;
	//std::cin >> approximation;
	model->increaseApproximation(approximation);
	//MeshExporter::toStl(test, "test");

	try {
		Renderer scene(800, 600, model);

		scene.run();
	}
	catch (std::exception ex) {
		std::cout << "\t\t[EXCEPTION] " << ex.what() << std::endl;
	}

	return 0;
}