#include <iostream>

namespace cqrs {

	class Application {
	public:
		void run(int argc, const char* argv[])
		{
			std::cout << "Hello CQRS" << std::endl;
		}
	};

}

int main(int argc, const char* argv[]) {
	
	cqrs::Application theApp;
	theApp.run(argc, argv);

	std::cin.get();

	return 0;
}