    #include "MessageParser.h"
    #include <algorithm>
    #include <cctype>
    #include <fstream>
    #include <pcl/io/pcd_io.h>
    #include <pcl/point_types.h>

    namespace MessageParser
    {
        namespace
        {
            std::string ToLower(std::string value)
            {
                std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
                    return static_cast<char>(std::tolower(c));
                });
                return value;
            }

            bool ExtractAttributeValue(const std::string &line, const std::string &attributeName, std::string &value)
            {
                const std::string token = attributeName + "=\"";
                const size_t tokenStart = line.find(token);
                if (tokenStart == std::string::npos)
                {
                    return false;
                }

                const size_t valueStart = tokenStart + token.size();
                const size_t valueEnd = line.find('"', valueStart);
                if (valueEnd == std::string::npos)
                {
                    return false;
                }

                value = line.substr(valueStart, valueEnd - valueStart);
                return true;
            }

            bool ExtractAttributeFloat(const std::string &line, const std::string &attributeName, float &value)
            {
                std::string valueString;
                if (!ExtractAttributeValue(line, attributeName, valueString))
                {
                    return false;
                }

                try
                {
                    value = std::stof(valueString);
                    return true;
                }
                catch (...)
                {
                    return false;
                }
            }

            bool IsLikelyTreeObjectName(const std::string &objectName)
            {
                const std::string lowered = ToLower(objectName);
                return lowered.find("tree") != std::string::npos ||
                       lowered.find("pine") != std::string::npos ||
                       lowered.find("spruce") != std::string::npos ||
                       lowered.find("birch") != std::string::npos ||
                       lowered.find("fir") != std::string::npos ||
                       lowered.find("stump") != std::string::npos;
            }

            bool ParseTreePointLines(std::istream &input, Point2D *tree_points, int maxTreePoints, int &parsedTrees)
            {
                parsedTrees = 0;
                std::string line;

                while (std::getline(input, line))
                {
                    if (line.empty())
                    {
                        continue;
                    }

                    if (line[0] == '#')
                    {
                        continue;
                    }

                    for (char &c : line)
                    {
                        if (c == ',' || c == ';' || c == '\t')
                        {
                            c = ' ';
                        }
                    }

                    std::istringstream lineStream(line);
                    float x = 0.0f;
                    float y = 0.0f;
                    if (!(lineStream >> x >> y))
                    {
                        continue;
                    }

                    if (parsedTrees >= maxTreePoints)
                    {
                        return false;
                    }

                    tree_points[parsedTrees].x = x;
                    tree_points[parsedTrees].y = y;
                    parsedTrees++;
                }

                return true;
            }
        }


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

        bool ReadCloudFileToBuffer(const std::string &filePath, std::vector<float> &buffer, std::string &errorMessage)
        {
            const bool isPcd = filePath.size() >= 4 && filePath.substr(filePath.size() - 4) == ".pcd";
            if (isPcd)
            {
                pcl::PointCloud<pcl::PointXYZ> cloud;
                if (pcl::io::loadPCDFile<pcl::PointXYZ>(filePath, cloud) < 0)
                {
                    errorMessage = "Failed to load PCD file using PCL: " + filePath;
                    return false;
                }

                if (cloud.empty())
                {
                    errorMessage = "PCD file contains no points: " + filePath;
                    return false;
                }

                buffer.clear();
                buffer.reserve(cloud.size() * 3);
                for (const auto &point : cloud.points)
                {
                    buffer.push_back(point.x);
                    buffer.push_back(point.y);
                    buffer.push_back(point.z);
                }

                errorMessage.clear();
                return true;
            }

            std::ifstream input(filePath);
            if (!input.is_open())
            {
                errorMessage = "Could not open file: " + filePath;
                return false;
            }

            buffer.clear();
            std::string line;
            int parsedPoints = 0;

            while (std::getline(input, line))
            {
                if (line.empty())
                {
                    continue;
                }

                if (line[0] == '#')
                {
                    continue;
                }

                for (char &c : line)
                {
                    if (c == ',' || c == ';' || c == '\t')
                    {
                        c = ' ';
                    }
                }

                std::istringstream lineStream(line);
                float x, y, z;
                if (!(lineStream >> x >> y >> z))
                {
                    continue;
                }

                buffer.push_back(x);
                buffer.push_back(y);
                buffer.push_back(z);
                parsedPoints++;
            }

            if (parsedPoints == 0)
            {
                errorMessage = "No valid points found in file: " + filePath;
                return false;
            }

            errorMessage.clear();
            return true;
        }

        bool ReadTreeXmlFileToBuffer(const std::string &filePath, Point2D *tree_points, int maxTreePoints, int &treeCount, std::string &errorMessage)
        {
            std::string loweredPath = ToLower(filePath);
            const bool isXml = loweredPath.size() >= 4 && loweredPath.substr(loweredPath.size() - 4) == ".xml";
            std::ifstream input(filePath);
            if (!input.is_open())
            {
                errorMessage = "Could not open tree file: " + filePath;
                treeCount = 0;
                return false;
            }

            if (!isXml)
            {
                int parsedTrees = 0;
                if (!ParseTreePointLines(input, tree_points, maxTreePoints, parsedTrees))
                {
                    errorMessage = "Tree file contains more points than buffer size supports.";
                    treeCount = parsedTrees;
                    return false;
                }

                if (parsedTrees == 0)
                {
                    errorMessage = "No tree positions found in tree file: " + filePath;
                    treeCount = 0;
                    return false;
                }

                treeCount = parsedTrees;
                errorMessage.clear();
                return true;
            }

            bool inObject = false;
            bool inTree = false;
            bool currentObjectIsForestType = false;
            bool currentObjectIsTreeLikeName = false;
            bool objectPositionRead = false;
            int parsedTrees = 0;
            std::string line;

            while (std::getline(input, line))
            {
                if (line.find("<object") != std::string::npos)
                {
                    inObject = true;
                    inTree = false;
                    objectPositionRead = false;

                    currentObjectIsForestType = false;
                    currentObjectIsTreeLikeName = false;

                    std::string objectType;
                    if (ExtractAttributeValue(line, "type", objectType))
                    {
                        currentObjectIsForestType = ToLower(objectType) == "forest";
                    }

                    std::string objectName;
                    if (ExtractAttributeValue(line, "name", objectName))
                    {
                        currentObjectIsTreeLikeName = IsLikelyTreeObjectName(objectName);
                    }
                }

                if (inObject && currentObjectIsForestType && line.find("<tree") != std::string::npos)
                {
                    inTree = true;
                }

                if (inObject && currentObjectIsForestType && line.find("</tree>") != std::string::npos)
                {
                    inTree = false;
                }

                const bool parseForestTreePosition = inObject && currentObjectIsForestType && inTree;
                const bool parseTreeObjectPosition = inObject && currentObjectIsTreeLikeName && !objectPositionRead;

                if ((parseForestTreePosition || parseTreeObjectPosition) && line.find("<position") != std::string::npos)
                {
                    float x = 0.0f;
                    float y = 0.0f;
                    const bool hasX = ExtractAttributeFloat(line, "x", x);
                    const bool hasY = ExtractAttributeFloat(line, "y", y);

                    if (hasX && hasY)
                    {
                        if (parsedTrees >= maxTreePoints)
                        {
                            errorMessage = "Tree XML contains more points than buffer size supports.";
                            treeCount = parsedTrees;
                            return false;
                        }

                        tree_points[parsedTrees].x = x;
                        tree_points[parsedTrees].y = y;
                        parsedTrees++;

                        if (parseTreeObjectPosition)
                        {
                            objectPositionRead = true;
                        }
                    }
                }

                if (line.find("</object>") != std::string::npos)
                {
                    inObject = false;
                    inTree = false;
                    currentObjectIsForestType = false;
                    currentObjectIsTreeLikeName = false;
                    objectPositionRead = false;
                }
            }

            if (parsedTrees == 0)
            {
                errorMessage = "No tree positions found in XML file: " + filePath;
                treeCount = 0;
                return false;
            }

            treeCount = parsedTrees;
            errorMessage.clear();
            return true;
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