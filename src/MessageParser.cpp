    #include "MessageParser.h"

    namespace MessageParser
    {

        void AddCloudToArray(const char *message, size_t size, std::vector<float> &buffer)
        {
            // Interpret the message as a series of uint32_t
            const uint32_t *uintArray = reinterpret_cast<const uint32_t *>(message);
            int count = 0;

            // Convert the data from network byte order to host byte order and add the points to the buffer
            for (int i = 0; i < size / 4; i++)
            {
                uint32_t hostByteOrderData = UDPSocket::ntohl_wrapper(uintArray[i]);
                float hostByteOrderFloat = *reinterpret_cast<float *>(&hostByteOrderData);

                buffer.push_back(hostByteOrderFloat);
                count++;
            }
        }

        MinMaxValues ReadCloudBufferToPoints(std::vector<float> &buffer, Point3D *points)
        {
            int tempPointCount = 0;

            MinMaxValues minMaxValues;

            // If all packages have been received, update the points array and the vertex buffer
            for (int i = 0; i < buffer.size() / 3; i++)
            {
                points[i].x = buffer[i * 3];
                points[i].y = buffer[i * 3 + 1];
                points[i].z = buffer[i * 3 + 2];
                points[i].density = 0.0f;
                points[i].isVegetation = 0.0f;
                minMaxValues.pointCount++;

                if (points[i].x < minMaxValues.minX) minMaxValues.minX = points[i].x;
                if (points[i].x > minMaxValues.maxX) minMaxValues.maxX = points[i].x;
                if (points[i].y < minMaxValues.minY) minMaxValues.minY = points[i].y;
                if (points[i].y > minMaxValues.maxY) minMaxValues.maxY = points[i].y;
                if (points[i].z < minMaxValues.minZ) minMaxValues.minZ = points[i].z;
                if (points[i].z > minMaxValues.maxZ) minMaxValues.maxZ = points[i].z;
            }
            // Update the vertex buffer
            return minMaxValues;
        }

        int AddTreesToBuffer(const char *message, int Fromindex, size_t size, Point2D *tree_points)
        {
            // Check if the message starts with '!T'
            if (size < 3 || message[0] != '!' || message[1] != 'T') {
                // Invalid message format
                return -1;
            }

            // Initialize the number of trees added to the buffer
            int treeCount = Fromindex;

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
    }  // namespace MessageParser