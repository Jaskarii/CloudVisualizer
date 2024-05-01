#include <iostream>
#include <vector>
#include <netinet/in.h>
#include <sstream>  // For std::istringstream
#include <string>   // For std::string


namespace MessageParser 
{
    struct Point3D
    {
        float x, y, z;
    };

    struct Point2D
    {
        float x, y;
    };

    void AddCloudToArray(const char *message, size_t size, std::vector<float> &buffer)
    {
        // Interpret the message as a series of uint32_t
        const uint32_t *uintArray = reinterpret_cast<const uint32_t *>(message);
        int count = 0;

        // Convert the data from network byte order to host byte order and add the points to the buffer
        for (int i = 0; i < size / 4; i++)
        {
            uint32_t hostByteOrderData = ntohl(uintArray[i]);
            float hostByteOrderFloat = *reinterpret_cast<float *>(&hostByteOrderData);

            buffer.push_back(hostByteOrderFloat);
            count++;
        }
    }

    int ReadCloudBufferToPoints(std::vector<float> &buffer, Point3D *points)
    {
        int tempPointCount = 0;
        // If all packages have been received, update the points array and the vertex buffer
        for (int i = 0; i < buffer.size() / 3; i++)
        {
            points[i].x = buffer[i * 3];
            points[i].y = buffer[i * 3 + 1];
            points[i].z = buffer[i * 3 + 2];

            tempPointCount++;
        }
        // Update the vertex buffer
        return tempPointCount;
    }

    int AddTreesToBuffer(const char *message, size_t size, Point2D *tree_points)
    {
        // Check if the message starts with '!T'
        if (size < 3 || message[0] != '!' || message[1] != 'T') {
            // Invalid message format
            return -1;
        }

        // Initialize the number of trees added to the buffer
        int treeCount = 0;

        // Parse the message to extract tree positions
        std::istringstream iss(std::string(message, size));
        std::string line;

        // Skip the first line containing the indicator
        std::getline(iss, line);

        // Parse each line to extract tree positions
        while (std::getline(iss, line)) {
            std::istringstream lineStream(line);
            float x, y;
            // Extract x and y coordinates
            if (lineStream >> x >> y) {
                // Store the tree position in the buffer
                tree_points[treeCount].x = x;
                tree_points[treeCount].y = y;
                // Increment the tree count
                ++treeCount;
            }
        }

        return treeCount;
    }
}