#ifndef OBJ_H_INCLUDED
#define OBJ_H_INCLUDED

#include <string>
#include <vector>
#include <Eigen/Dense>


class OBJ
{
public:
    OBJ();

    static bool load(std::string caminho, std::vector<Eigen::Vector3f> *vert, std::vector<Eigen::Vector2f> *text=nullptr, std::vector<Eigen::Vector3f> *norm=nullptr);
private:


    static std::vector<std::string> splitstring(std::string s, char delimitador);
	
};

#endif