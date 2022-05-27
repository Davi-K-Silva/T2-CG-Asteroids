//
//  Modelo.cpp
//  OpenGLTest
//
//  Created by Márcio Sarroglia Pinho on 22/09/20.
//  Copyright © 2020 Márcio Sarroglia Pinho. All rights reserved.
//

#include "Modelo.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

void Modelo::desenhaLinhas()
{

}


void Modelo:: desenhaModelo(){
    glTranslated(0.25, 0, 0);
    for(int i=0; i<Quad.size(); i++){
        for(int j=0; j<Quad[i].size();j++){
            int corQ =  Quad[i][j];
            if(corQ != 1) desenhaPixel((float)Cores[corQ-1][0]/255,(float)Cores[corQ-1][1]/255,(float)Cores[corQ-1][2]/255);
            glTranslated(0.5, 0, 0);
        }
        float size = (float) Quad[0].size();
        glTranslated(-size/2, -0.5, 0);
    }
}

void Modelo :: desenhaPixel(float r,float g,float b){
    glColor3f(r,g,b);
    glBegin(GL_QUADS);
        glVertex2d(-0.25, -0.25);
        glVertex2d(-0.25, 0.25);
        glVertex2d(0.25, 0.25);
        glVertex2d(0.25, -0.25);
    glEnd();
}

vector <int> Modelo:: splitString(string str, string delimiter)
{   
    vector <int> rt;
    int start = 0;
    int end = str.find(delimiter);
    while (end != -1) {
        //cout << str.substr(start, end - start) << endl;
        rt.push_back(stoi(str.substr(start, end - start)));
        start = end + delimiter.size();
        end = str.find(delimiter, start);
    }

    if(str.substr(start, end - start).length() > 0){
        rt.push_back(stoi(str.substr(start, end - start)));
    }
    return rt;
}

void Modelo::LeModelo(const char *nome)
{
    
    ifstream input;            // ofstream arq;
    input.open(nome, ios::in); //arq.open(nome, ios::out);
    
    if(input.is_open()) {
        string line;
        string comp = "#CORES";
        
        bool initLine = true;
        bool coresAux = true;
        bool initPontos = false;

        int quantCoresAux = 0;
        int quantObjetosLinesAux = 0;

        int quantCores;
        int quantObjetosLines;
        while (getline(input, line)) {
            if(line == "#CORES" || line == ""){
                continue;
            }
            
            //Verifica a quantidade de cores que há
            if(coresAux){
                quantCores = stoi(line);
                coresAux = false;
                continue;
            }

            //Verifica para cada linha as cores
            if(quantCoresAux < quantCores){
                vector <int> split = splitString(line, " ");
                Cores.push_back({split[1],split[2],split[3]});
                quantCoresAux++;
                continue;
            }
           
            if(line == "#OBJETO"){
                initPontos = true; 
                continue;
            }
            
            //Para cada linha de pontos 
            if(initPontos == true){
                vector <int> split = splitString(line, " ");
                quantObjetosLines = split[0];
                initPontos = false;
                continue;
            }

            if(quantObjetosLinesAux <= quantObjetosLines){
                Quad.push_back(splitString(line, " "));
                quantObjetosLinesAux++;
                continue;
            }
            
        }

        for (int i=0; i<Cores.size(); i++){
            for(int j=0; j< Cores[i].size(); j++){
                cout << Cores[i][j] << " ";
            }
            cout << endl;
        }
        
        for (int i=0; i<Quad.size(); i++){
            for(int j=0; j< Quad[i].size(); j++){
                cout << Quad[i][j] << " ";
            }
            cout << endl;
        }
        
        h = Quad.size();
        w = Quad[0].size();
        input.close();
    }   

    cout << "Modelo lido com sucesso!" << endl;

}
