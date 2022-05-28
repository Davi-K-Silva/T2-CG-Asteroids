//
//  Instancia.cpp
//  OpenGLTest
//
//  Created by Márcio Sarroglia Pinho on 22/09/20.
//  Copyright © 2020 Márcio Sarroglia Pinho. All rights reserved.
//

#include "Instancia.h"


void DesenhaPersonagem();
void DesenhaRetangulo();
// ***********************************************************
//  void InstanciaPonto(Ponto3D *p, Ponto3D *out)
//  Esta funcao calcula as coordenadas de um ponto no
//  sistema de referencia do universo (SRU), ou seja,
//  aplica as rotacoes, escalas e translacoes a um
//  ponto no sistema de referencia do objeto (SRO).
// ***********************************************************
void InstanciaPonto(Ponto &p, Ponto &out)
{
    GLfloat ponto_novo[4];
    GLfloat matriz_gl[4][4];
    int  i;

    glGetFloatv(GL_MODELVIEW_MATRIX,&matriz_gl[0][0]);

    for(i=0;i<4;i++)
    {
        ponto_novo[i]= matriz_gl[0][i] * p.x+
        matriz_gl[1][i] * p.y+
        matriz_gl[2][i] * p.z+
        matriz_gl[3][i];
    }
    out.x=ponto_novo[0];
    out.y=ponto_novo[1];
    out.z=ponto_novo[2];
}

Ponto InstanciaPonto(Ponto P)
{
    Ponto temp;
    InstanciaPonto(P, temp);
    return temp;
}

void Instancia::desenha()
{
    // aplica as transformacoes geometricas no modelo
    // desenha a geometria do objeto

    glPushMatrix();
    glTranslatef(posicao.x, posicao.y, 0);
    //posicao.imprime();
    glRotatef(rotacao, 0, 0, 1);

    Ponto posicaoPersonagem;
    Ponto Origem = Ponto(0,0,0);

    glTranslated(-modelo.w/4, modelo.h/4, 0);
    InstanciaPonto(Origem, posicaoPersonagem);

    //ObtemPontos();

    //posicaoPersonagem.imprime();

    //glTranslated(0, 0, 0);
    modelo.desenhaModelo();

    glPopMatrix();

    
}

void Instancia :: ObtemPontos(){

    Ponto pontoXminYmin;
    Ponto pontoXmaxYmin;
    Ponto pontoXminYmax;
    Ponto pontoXmaxYmax;

    Ponto origemPontoXminYmin = Ponto (0,-modelo.h/2,0);
    Ponto origemPontoXmaxYmin = Ponto (modelo.w/2,-modelo.h/2,0);
    Ponto origemPontoXminYmax = Ponto (modelo.w/2,0,0);
    Ponto origemPontoXmaxYmax = Ponto (0,0,0);

    InstanciaPonto(origemPontoXminYmin, pontoXminYmin);
    InstanciaPonto(origemPontoXmaxYmin, pontoXmaxYmin);
    InstanciaPonto(origemPontoXminYmax, pontoXminYmax);
    InstanciaPonto(origemPontoXmaxYmax, pontoXmaxYmax);

    envelope[0] = pontoXminYmin;
    envelope[1] = pontoXmaxYmin;
    envelope[2] = pontoXminYmax;
    envelope[3] = pontoXmaxYmax;
}

void Instancia :: DefineEnvelope(){
    glPushMatrix();
    glTranslatef(posicao.x, posicao.y, 0);
    glRotatef(rotacao, 0, 0, 1);
    glTranslated(-modelo.w/4, modelo.h/4, 0);


    Ponto pontoXminYmin;
    Ponto pontoXmaxYmin;
    Ponto pontoXminYmax;
    Ponto pontoXmaxYmax;

    Ponto origemPontoXminYmin = Ponto (0,-modelo.h/2,0);
    Ponto origemPontoXmaxYmin = Ponto (modelo.w/2,-modelo.h/2,0);
    Ponto origemPontoXminYmax = Ponto (modelo.w/2,0,0);
    Ponto origemPontoXmaxYmax = Ponto (0,0,0);

    InstanciaPonto(origemPontoXminYmin, pontoXminYmin);
    InstanciaPonto(origemPontoXmaxYmin, pontoXmaxYmin);
    InstanciaPonto(origemPontoXminYmax, pontoXminYmax);
    InstanciaPonto(origemPontoXmaxYmax, pontoXmaxYmax);

    envelope[0] = pontoXminYmin;
    envelope[1] = pontoXmaxYmin;
    envelope[2] = pontoXminYmax;
    envelope[3] = pontoXmaxYmax;

    glPopMatrix();
}

void Instancia :: imprimeEnv(){
    cout << "---" << endl;
    envelope[0].imprime();  cout << " <> " ;
    envelope[1].imprime();  cout << endl;
    envelope[2].imprime();  cout << " <> " ;
    envelope[3].imprime();  cout << endl;
}

void Instancia :: DesenhaEnvelope(){
    glPushMatrix();
    glColor3f(1,0.65,0);
    glBegin(GL_LINES);
        glVertex2f(envelope[0].x,envelope[0].y);
        glVertex2f(envelope[1].x,envelope[1].y);
        glVertex2f(envelope[0].x,envelope[0].y);
        glVertex2f(envelope[3].x,envelope[3].y);
        glVertex2f(envelope[1].x,envelope[1].y);
        glVertex2f(envelope[2].x,envelope[2].y);
        glVertex2f(envelope[2].x,envelope[2].y);
        glVertex2f(envelope[3].x,envelope[3].y);
    glEnd();
    glPopMatrix();
}

