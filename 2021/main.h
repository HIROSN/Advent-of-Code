#include <fstream>
#include <iostream>

int Answer(std::ifstream &file);

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        std::cout << "Usage: " << argv[0] << " input" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file.is_open())
    {
        return 2;
    }

    std::cout << Answer(file) << std::endl;
    file.close();
    return 0;
}
