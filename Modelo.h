//
//  Modelo.hpp
//  OpenGLTest
//
//  Created by Márcio Sarroglia Pinho on 22/09/20.
//  Copyright © 2020 Márcio Sarroglia Pinho. All rights reserved.
//

#ifdef WIN32
#include <windows.h>
#include <glut.h>
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#endif

#ifdef __linux__
#include <glut.h>
#endif

#ifndef Modelo_h
#define Modelo_h

#include "Poligono.h"
#include <vector>

class Modelo: public Poligono
{
public:
    int w;
    int h;
    vector <vector<int>> Cores;
    vector <vector<int>> Quad;
    void desenhaLinhas();
    void desenhaModelo();
    void desenhaPixel(float r, float g, float b);
    vector <int> splitString(string str, string delimiter = " ");
    void LeModelo(const char *nome);
};

#endif /* Modelo_hpp */
