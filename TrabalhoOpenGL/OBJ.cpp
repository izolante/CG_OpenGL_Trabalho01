#include "OBJ.h"
using namespace std;
using namespace Eigen;

#include <iostream>
#include <fstream>
#include <algorithm>

OBJ::OBJ(){

}


bool OBJ::load(string caminho, vector<Vector3f> *vert, vector<Vector2f> *text, vector<Vector3f> *norm)
{
    streambuf *cinbuf = std::cin.rdbuf(); //salvar o buffer atual!
    
    ifstream file(caminho);
    if (file.is_open())
		cin.rdbuf(file.rdbuf()); //redirecionar std::cin para o arquivo!
	else
        return false;
    
    vector<Vector3f> vecV;
    vector<Vector2f> vecT;
    vector<Vector3f> vecN;
    vector<string> vecF;
    string codigo;

    while(std::cin){

        cin >> codigo;


        if (!std::cin)
            break;

        //converter para minuscula!
        std::transform(codigo.begin(), codigo.end(), codigo.begin(), [](unsigned char c){ return std::tolower(c); });


        if (codigo.size() > 0 && codigo[0] == '#') {
            string aux; //jogar fora
            getline(cin,aux);
		}
        else if( codigo == "v"){
            Vector3f v;
            cin >> v[0];
            cin >> v[1];
            cin >> v[2];

            vecV.push_back(v);
        }
        else if( codigo == "vt"){
            Vector2f vt;
            cin >> vt[0];
            cin >> vt[1];

            vecT.push_back(vt);
        }
        else if( codigo == "vn"){
            Vector3f vn;
            cin >> vn[0];
            cin >> vn[1];
            cin >> vn[2];

            vecN.push_back(vn);
        }
        else if(codigo == "f"){
            string f;
            cin >> f;
            vecF.push_back(f);
            cin >> f;
            vecF.push_back(f);
            cin >> f;
            vecF.push_back(f);
        }
    }
    

    //terminou a leitura!
	file.close(); //fechar o arquivo!
	std::cin.rdbuf(cinbuf);   //restabelecer a entrada padrão novamente!


    if(vert)
        vert->clear();
    if(text)
        text->clear();
    if(norm)
        norm->clear();

    //processa as faces no formato "v/t/n"
    for(int k=0; k<vecF.size();k++){
        vector<string> d = OBJ::splitstring(vecF[k], '/');
        int nDados = d.size();
      

        int vIdx =-1, tIdx =-1, nIdx =-1;

        if(nDados>0 && d[0].size()>0)
            vIdx = std::stoi(d[0]);
        if(nDados>1 && d[1].size()>0)
            tIdx = std::stoi(d[1]);
        if(nDados>2 && d[2].size()>0)
            nIdx = std::stoi(d[2]);
        
        vIdx--;
        tIdx--;
        nIdx--;

        if(vert && vIdx>=0 && vIdx< vecV.size())
            vert->push_back( vecV[vIdx] );
        if(text && tIdx>=0 && tIdx< vecT.size())
            text->push_back( vecT[tIdx] );
        if(norm && nIdx>=0 && nIdx< vecN.size())
            norm->push_back( vecN[nIdx] );
    }

    //verifica erros na leitura dos dados
    if( (vert && vert->size()< 0) || (text && text->size()>0 && text->size() != vert->size()) || (norm && norm->size()>0 && norm->size() != vert->size()) )
    {
        cout << "Erro OBJ: Erro ao compilar as faces! O numero de coordenadas de vertices deve ser igual ao numero de coordendas de textura e de normais!\n";
        if(vert)
            vert->clear();
        if(text)
            text->clear();
        if(norm)
            norm->clear();
        return false;
    }

	return true;
}




std::vector<std::string> OBJ::splitstring(std::string s, char delimitador)
{
    std::vector<std::string> pedacos;
    std::string pedaco;
    std::istringstream strstream(s);
    while (std::getline(strstream, pedaco, delimitador))
    {
        pedacos.push_back(pedaco);
    }

    return pedacos;
}