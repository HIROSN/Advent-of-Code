#pragma once

template <typename T>
bool skip(std::istream &stream, int many = 1)
{
    T ignore;

    for (int i = 0; i < many; i++)
    {
        if (!(stream >> ignore))
        {
            return false;
        }
    }

    return true;
}
