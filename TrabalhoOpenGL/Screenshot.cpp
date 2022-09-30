#include "Screenshot.h"
#include <iostream>
#include <filesystem>


#include <sstream>
#include <iomanip>
#include <algorithm>
#include <regex>

#include <stb/stb_image_write.h>

#include <GL/glew.h>  //GLEW

using namespace std;


namespace fs = std::filesystem;

Screenshot::Screenshot(){

}


bool Screenshot::saveScreenshot(int winWidth, int winHeight)
{

    if( (winWidth < 1) || (winHeight < 1) )
	{
		return false;
	}

    fs::path screenshotPath = fs::current_path().parent_path();
    screenshotPath.append("screenshot");

    //cout << screenshotPath << endl;
    

    if(!fs::exists(screenshotPath))
        fs::create_directory(screenshotPath);

    int count = 0;
    for (const auto & entry : fs::directory_iterator(screenshotPath)){
        if(entry.path().has_filename())
        {
            //string input =  entry.path().filename().string();
            string input =  entry.path().stem().string(); //sem a extenção
            //cout << input << endl;

            if(input.rfind("screenshot_", 0) == 0)
            {
                std::string output;
                std::smatch match;
                std::regex_search(input, match, std::regex("([1-9])([0-9]*)"));
                
                if(match.size()>0){
                    int n = std::stoi( match.str(0) );
                    //cout << n << endl;
                    if(n> count)
                        count = n;
                }

            }
            
        }
    }
    count++; //incrementa o contador de screenshot


    std::ostringstream ss;
    ss << std::setw(3) << std::setfill('0') << count;

    screenshotPath.append("screenshot_" + ss.str() + ".png");
    
    //cout << screenshotPath.string() << endl;



	//le os dados da imagem do OpenGL
	unsigned char * pixel_data = new unsigned char[3 * winWidth * winHeight];
	glReadPixels (0, 0, winWidth, winHeight, GL_RGB, GL_UNSIGNED_BYTE, pixel_data);

	//inverte a imagem
	for(int j = 0; j*2 < winHeight; ++j )
	{
		int index1 = j * winWidth * 3;
		int index2 = (winHeight - 1 - j) * winWidth * 3;
		for(int  i = winWidth * 3; i > 0; --i )
		{
			unsigned char temp = pixel_data[index1];
			pixel_data[index1] = pixel_data[index2];
			pixel_data[index2] = temp;
			++index1;
			++index2;
		}
	}


    stbi_write_png(screenshotPath.string().c_str(), winWidth, winHeight, 3, pixel_data, winWidth * 3);

    delete pixel_data;
	
    cout << "\nScreenshot: " << screenshotPath << endl;



    return true;
}
