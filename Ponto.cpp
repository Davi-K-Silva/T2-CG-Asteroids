//
//  Ponto.cpp
//  OpenGLTest
//
//  Created by Márcio Sarroglia Pinho on 18/08/20.
//  Copyright © 2020 Márcio Sarroglia Pinho. All rights reserved.
//

#include "Ponto.h"
Ponto::Ponto ()
{
    x=y=z=0;
}
Ponto::Ponto(float x, float y, float z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}
void Ponto::set(float x, float y, float z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}
void Ponto::imprime() {
    cout << "(" << x << ", " << y << ")" << flush;
}

bool operator==(Ponto P1, Ponto P2)
{
    if (P1.x != P2.x) return false;
    if (P1.y != P2.y) return false;
    if (P1.z != P2.z) return false;
    return true;

}
Ponto operator+(Ponto P1, Ponto P2)
{
    Ponto temp;
    temp = P1;
    temp.x += P2.x;
    temp.y += P2.y;
    temp.z += P2.z;
    return temp;
}

Ponto operator- (Ponto P1, Ponto P2)
{
    Ponto temp;
    temp = P1;
    temp.x -= P2.x;
    temp.y -= P2.y;
    temp.z -= P2.z;
    return temp;
}
Ponto operator* (Ponto P1, float k)
{
    Ponto temp;
    temp.x = P1.x * k;
    temp.y = P1.y * k;
    temp.z = P1.z * k;
    return temp;
}

Ponto operator/ (Ponto P1, double k)
{
    Ponto temp;
    temp.x = P1.x / k;
    temp.y = P1.y / k;
    temp.z = P1.z / k;
    return temp;
}

Ponto operator-(Ponto P1)
{
    return P1 * -1;
}
