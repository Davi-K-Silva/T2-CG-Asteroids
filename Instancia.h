//
//  Instancia.hpp
//  OpenGLTest
//
//  Created by Márcio Sarroglia Pinho on 22/09/20.
//  Copyright © 2020 Márcio Sarroglia Pinho. All rights reserved.
//


#ifndef Instancia_hpp
#define Instancia_hpp
#include <iostream>
using namespace std;

#include "Poligono.h"
#include "Modelo.h"
#include "Bezier.h"

class Instancia{
    //Poligono *modelo;

public:
    bool ativo = true;
    bool temColisao = true;
    double speed;
    int nCurva;
    bool turnSet;
    Bezier bezier;
    Bezier bezierTurn;
    double tAtual;
    Ponto envelope[4];
    Ponto posicao, escala;
    float rotacao;
    Modelo modelo;
    Modelo damageModel;
    bool ally = false;
    
    double damageTime = 0;
    bool deathAnimation = false;
    double defaultBulletTime = 0;
    double bulletTime = 5;

    void desenha();
    void ObtemPontos();
    void imprimeEnv();
    void DefineEnvelope();
    void DesenhaEnvelope();
};


#endif /* Instancia_hpp */
