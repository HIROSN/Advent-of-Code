#include <main.h>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    std::string line;
    std::getline(file, line);
    int position = 0, disk_left = 0, disk_right = line.size() - 1;
    uint64_t checksum = 0;

    while (disk_left <= disk_right)
    {
        int id_left = disk_left / 2, id_right = disk_right / 2;

        if (!(disk_left % 2))
        {
            int file_size = line[disk_left] - '0';

            for (int i = 0; i < file_size; i++)
                checksum += position++ * id_left;

            disk_left++;
        }
        else
        {
            int space_size = line[disk_left] - '0';
            int move_size = line[disk_right] - '0';

            if (space_size == move_size)
            {
                for (int i = 0; i < move_size; i++)
                    checksum += position++ * id_right;

                disk_left++;
                disk_right -= 2;
            }

            if (space_size > move_size)
            {
                for (int i = 0; i < move_size; i++)
                    checksum += position++ * id_right;

                line[disk_left] = space_size - move_size + '0';
                disk_right -= 2;
            }

            if (space_size < move_size)
            {
                for (int i = 0; i < space_size; i++)
                    checksum += position++ * id_right;

                disk_left++;
                line[disk_right] = move_size - space_size + '0';
            }
        }
    }

    return checksum;
}
