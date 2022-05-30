// **********************************************************************
// PUCRS/Escola Polit�cnica
// COMPUTA��O GR�FICA
//
// Programa basico para criar aplicacoes 2D em OpenGL
//
// Marcio Sarroglia Pinho
// pinho@pucrs.br
// **********************************************************************

// Para uso no Xcode:
// Abra o menu Product -> Scheme -> Edit Scheme -> Use custom working directory
// Selecione a pasta onde voce descompactou o ZIP que continha este arquivo.
//

#include <iostream>
#include <cmath>
#include <ctime>
#include <fstream>


using namespace std;

#ifdef WIN32
#include <windows.h>
#include <glut.h>
#else
#include <sys/time.h>
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#endif

#ifdef __linux__
#include <glut.h>
#endif

#include <typeinfo>


#define _USE_MATH_DEFINES
#define PI 3.14159265

#include "Ponto.h"
#include "Modelo.h"
#include "Instancia.h"
#include "Bezier.h"

#include "Temporizador.h"

#define NAVESCOUNT 5

#define ENEMYSPEED 30
#define PLAYERSPEED 0.85
#define SHOTSPEED 1.8
#define ROTACACAO_SPEED 5
#define TURNSIZE 10

#define SIZE_Y_MAX 50
#define SIZE_Y_MIN -50
#define SIZE_X_MAX 50
#define SIZE_X_MIN -50

#define HURT_ANIMATION_TIME 0.15;

Temporizador T;
double AccumDeltaT=0;
double ft;

Modelo MeiaSeta;
Modelo Mastro;
Modelo Disparador;
Modelo Enemy1;
Modelo Enemy2;
Modelo Enemy3;
Modelo Tiro;
Modelo TiroInimigo;
Modelo Vida;

Modelo hurtPlayer;
Modelo hurtEnemy1;
Modelo hurtEnemy2;
Modelo hurtEnemy3;
Modelo tiroInimigoBoom;

Modelo GameOverModel;
Modelo winModel;

Ponto dir = Ponto (0,1,0);
int quantEnemyModels;
vector <Modelo> modelos;
vector <Modelo> hurtModelos;
Instancia Universo[5000];

Ponto Estrelas[200];

// Limites l�gicos da �rea de desenho
Ponto Min, Max;

int playerHealth = 3;
int positionHealth = 0;
int positionShoot = 0;

bool movePlayer = true;

bool DesenhaBezier = false;
bool DesenhaEnvelope = false;
bool desenha = false;

float angulo=0.0;
double AngleToRad = PI / 180;
double RadToAngle = 180 / PI;
double lastAlfa = 0.0;

int nInstancias=0;

int limiteTiro = 10;
double damageTime = 0;
bool animacaoDeDano = false;

bool gameOver = false;
double gameOverTime = 1.2;

int navesDestruidas = 0;


// **********************************************************************
//    Calcula o produto escalar entre os vetores V1 e V2
// **********************************************************************
double ProdEscalar(Ponto v1, Ponto v2)
{
    return v1.x*v2.x + v1.y*v2.y+ v1.z*v2.z;
}
// **********************************************************************
//    Calcula o produto vetorial entre os vetores V1 e V2
// **********************************************************************
void ProdVetorial (Ponto v1, Ponto v2, Ponto &vresult)
{
    vresult.x = v1.y * v2.z - (v1.z * v2.y);
    vresult.y = v1.z * v2.x - (v1.x * v2.z);
    vresult.z = v1.x * v2.y - (v1.y * v2.x);
}
/* ********************************************************************** */
/*                                                                        */
/*  Calcula a interseccao entre 2 retas (no plano "XY" Z = 0)             */
/*                                                                        */
/* k : ponto inicial da reta 1                                            */
/* l : ponto final da reta 1                                              */
/* m : ponto inicial da reta 2                                            */
/* n : ponto final da reta 2                                              */
/*                                                                        */
/* s: valor do par�metro no ponto de interse��o (sobre a reta KL)         */
/* t: valor do par�metro no ponto de interse��o (sobre a reta MN)         */
/*                                                                        */
/* ********************************************************************** */
int intersec2d(Ponto k, Ponto l, Ponto m, Ponto n, double &s, double &t)
{
    double det;

    det = (n.x - m.x) * (l.y - k.y)  -  (n.y - m.y) * (l.x - k.x);

    if (det == 0.0)
        return 0 ; // n�o h� intersec��o

    s = ((n.x - m.x) * (m.y - k.y) - (n.y - m.y) * (m.x - k.x))/ det ;
    t = ((l.x - k.x) * (m.y - k.y) - (l.y - k.y) * (m.x - k.x))/ det ;

    return 1; // h� intersec��o
}

// **********************************************************************
bool HaInterseccao(Ponto k, Ponto l, Ponto m, Ponto n)
{
    int ret;
    double s,t;
    ret = intersec2d( k,  l,  m,  n, s, t);
    if (!ret) return false;
    if (s>=0.0 && s <=1.0 && t>=0.0 && t<=1.0)
        return true;
    else return false;
}
// **********************************************************************
// **********************************************************************

// **********************************************************************
// void DentroConvex(Ponto objeto[], Ponto objetoColisao)
//  Verifica se um conjunto de pontos está dentro de um poligono convexo usando o produto vetorial
// **********************************************************************
bool DentroConvex(Ponto objeto[], Ponto objetoColisao[]) {

        Poligono vetores;
        for(int i = 0; i < 4; i++) {
            if(i == 4 - 1) {


                Ponto lastVet = objeto[0] - objeto[i];
                vetores.insereVertice(lastVet);
                break;
            }

            Ponto vetor =   objeto[i+1] -  objeto[i];
            vetores.insereVertice(vetor);
        }

        //Insere nos poligonos os pontos que estão fora ou dentro do convexHull base
        for(int i = 0; i < 4; i++) {

            bool isInside = true;

            for(int k = 0; k < vetores.getNVertices(); k++ ) {
                Ponto vetor = objetoColisao[i] - objeto[k];
                Ponto result;
                ProdVetorial(vetor, vetores.getVertice(k), result);
                if(result.z > 0){
                    isInside = false;
                    break;
                }
            }

            if(isInside == true) {
                return true;
            }

        }

        return false;
}

// **********************************************************************
// bool VerificaColisaoInstancia(int instance)
//  Verifica se uma instancia especifica colide com as outras no Universo 
// **********************************************************************

bool VerificaColisaoInstancia(int instance){
    for(int i=0; i< nInstancias;i++){
        if (instance != i){
            //cout<< "Verifica " << i << " com " << instance << endl;
            //Universo[i].imprimeEnv();
            //Universo[instance].imprimeEnv();
            if (DentroConvex(Universo[instance].envelope, Universo[i].envelope)){
                    return true;
            }
        }
    }
    return false;
}

// **********************************************************************
// Ponto movimentaPersonagem(Ponto p, double alfa)
//  Retorna o próximo ponto do personagem de acordo com sua inclinação
// **********************************************************************
Ponto movimentaPersonagem(Ponto p, double alfa){

    double radAlfa = alfa * AngleToRad;

    //dir.imprime();
    if(alfa != lastAlfa){
      dir = Ponto (0,1);
      double xr = dir.x * cos(radAlfa) - dir.y * sin(radAlfa);
      double yr = dir.x * sin(radAlfa) + dir.y * cos(radAlfa);
      dir = Ponto (xr,yr);
    }
    //dir.imprime();

    lastAlfa = alfa;

    return p + dir * PLAYERSPEED;
}

// **********************************************************************
//  Ponto movimentaTiro(Ponto p, double alfa)
//  Retorna o próximo ponto do Tiro de acordo com sua inclinação
// **********************************************************************
Ponto movimentaTiro(Ponto p, double alfa){

    double radAlfa = alfa * AngleToRad;

    Ponto dirT = Ponto (0,1);
    double xr = dirT.x * cos(radAlfa) - dirT.y * sin(radAlfa);
    double yr = dirT.x * sin(radAlfa) + dirT.y * cos(radAlfa);
    dirT = Ponto (xr,yr);
    lastAlfa = alfa;

    return p + dirT * SHOTSPEED;
}


// **********************************************************************
//  void carregaModelos()
//  Carrega o modelo dos personagens para utilizar nas instancias
// **********************************************************************
void CarregaModelos()
{
    Disparador.LeModelo("Modelos/Player.txt");

    Enemy1.LeModelo("Modelos/Enemy1.txt");
    modelos.push_back(Enemy1);
    Enemy2.LeModelo("Modelos/Enemy2.txt");
    modelos.push_back(Enemy2);
    Enemy3.LeModelo("Modelos/Enemy3.txt");
    modelos.push_back(Enemy3);

    hurtPlayer.LeModelo("Modelos/PlayerHurt.txt");
    hurtEnemy1.LeModelo("Modelos/EnemyHurt1.txt");
    hurtModelos.push_back(hurtEnemy1);
    hurtEnemy2.LeModelo("Modelos/EnemyHurt2.txt");
    hurtModelos.push_back(hurtEnemy2);
    hurtEnemy3.LeModelo("Modelos/EnemyHurt3.txt");
    hurtModelos.push_back(hurtEnemy3);

    tiroInimigoBoom.LeModelo("Modelos/EnemyShotBOOM.txt");

    quantEnemyModels = modelos.size();

    Tiro.LeModelo("Modelos/PlayerShot.txt");
    TiroInimigo.LeModelo("Modelos/EnemyShot.txt");
    Vida.LeModelo("Modelos/PlayerHealth.txt");

    GameOverModel.LeModelo("Modelos/GameOver.txt");
    winModel.LeModelo("Modelos/Win.txt");
}

// **********************************************************************
//  double fRand(double fMin, double fMax)
//  Carrega o modelo dos personagens para utilizar nas instancias
// **********************************************************************
double fRand(double fMin, double fMax)
{
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

// **********************************************************************
//  float pegaAnguloInicial(Bezier curva, Ponto P)
//     retorna o angulo inicial dos personagens de acordo com sua curva de bezier
// **********************************************************************

float pegaAnguloInicial(Bezier curva, Ponto P){
    Ponto Pd = curva.Calcula(0.1);
    Ponto v1 = Ponto(0,1,0);
    Ponto v2 = Pd - P;

    double prodEscalar = ProdEscalar(v1,v2);
    double tV1 = 1;
    double tV2 = curva.calculaDistancia(Pd,P);

    Ponto prodVet;
    ProdVetorial(v1,v2,prodVet);
    if( prodVet.z > 0 ){
        return acos(prodEscalar/(tV1*tV2))* RadToAngle;
    }

    return -acos(prodEscalar/(tV1*tV2)) * RadToAngle;
}

// **********************************************************************
//  CarregaInstancias()
//     Carrega as instancias com os respectivos modelos no Universo
// **********************************************************************
void CarregaInstancias()
{
    srand(time(NULL));
    //Player
    Universo[0].posicao = Ponto (1,1);
    Universo[0].rotacao = 0.0;
    Universo[0].modelo = Disparador;
    Universo[0].speed = PLAYERSPEED;
    Universo[0].ativo = true;
    Universo[0].DefineEnvelope();

    nInstancias = 1;
    //Inimigos
    for(int i = 0; i < NAVESCOUNT; i++){
        int modeloRandom = rand() % quantEnemyModels;

        int xRand = SIZE_X_MIN + ( rand() % (SIZE_X_MAX - SIZE_X_MIN + 1) );
        int yRand = SIZE_Y_MIN + ( rand() % (SIZE_Y_MAX - SIZE_Y_MIN + 1) );

        while(xRand < 15 && xRand > -15){
                cout << "reposicionando X - Muito perto" << endl;
                xRand = SIZE_X_MIN + ( rand() % SIZE_X_MAX - SIZE_X_MIN + 1 );
            }
            while(yRand < 15 && yRand > -15){
                 cout << "reposicionando Y - Muito perto" << endl;
                yRand = SIZE_Y_MIN + ( rand() % SIZE_Y_MAX - SIZE_Y_MIN + 1 );
            }

        Universo[nInstancias].posicao = Ponto (xRand,yRand);
        Universo[nInstancias].rotacao = 0;
        Universo[nInstancias].modelo = modelos[modeloRandom];
        double bulletTime = fRand(2.0, 5);
        Universo[nInstancias].defaultBulletTime = bulletTime;
        Universo[nInstancias].bulletTime = bulletTime;
        Universo[nInstancias].damageModel = hurtModelos[modeloRandom];
        Universo[nInstancias].speed = ENEMYSPEED;
        Universo[nInstancias].DefineEnvelope();


        while(VerificaColisaoInstancia(nInstancias)){
            int xRand = SIZE_X_MIN + ( rand() % SIZE_X_MAX - SIZE_X_MIN + 1 );
            int yRand = SIZE_Y_MIN + ( rand() % SIZE_Y_MAX - SIZE_Y_MIN + 1 );
           while(xRand < 40 && xRand > -40){
                cout << "reposicionando X - Muito perto" << endl;
                xRand = SIZE_X_MIN + ( rand() % SIZE_X_MAX - SIZE_X_MIN + 1 );
            }
            while(yRand < 40 && yRand > -40){
                 cout << "reposicionando Y - Muito perto" << endl;
                yRand = SIZE_Y_MIN + ( rand() % SIZE_Y_MAX - SIZE_Y_MIN + 1 );
            }
            Universo[nInstancias].posicao = Ponto (xRand,yRand);
            Universo[nInstancias].DefineEnvelope();
            cout << "Colisao" << endl;
        }

        nInstancias ++;
    }

    for(int i = 0; i < NAVESCOUNT; i++){
        int xRand = SIZE_X_MIN + ( rand() % SIZE_X_MAX - SIZE_X_MIN + 1 );
        int yRand = SIZE_Y_MIN + ( rand() % SIZE_Y_MAX - SIZE_Y_MIN + 1 );
        Universo[i+1].bezier = Bezier(Universo[i+1].posicao,Universo[0].posicao,Ponto(xRand,yRand));
        Universo[i+1].turnSet = false;
        Universo[i+1].nCurva = 1;

        Universo[i+1].rotacao = pegaAnguloInicial(Universo[i+1].bezier ,Universo[i+1].posicao);

    }

    int px = SIZE_X_MIN + 3;
    int py = SIZE_Y_MAX - 3;
    int quantVidas = nInstancias + playerHealth;
    positionHealth = nInstancias;

    for(int i = nInstancias; i < quantVidas; i++){
        Universo[i].posicao = Ponto(px,py);
        px += 5;
        Universo[i].modelo = Vida;
        Universo[i].temColisao = false;
        nInstancias ++;

    }

    positionShoot = nInstancias;
}


// **********************************************************************
//  void instanciaTiro(int instancia, Modelo model, Modelo death, bool ally)
//     Instancia um tiro na frente de outra instancia com a inclinação correta
// **********************************************************************
void instanciaTiro(int instancia, Modelo model, Modelo death, bool ally){

    double alfa = Universo[instancia].rotacao;

    double radAlfa = alfa * AngleToRad;

    //dir.imprime();

    Ponto dirT = Ponto (0,1);
    double xr = dirT.x * cos(radAlfa) - dirT.y * sin(radAlfa);
    double yr = dirT.x * sin(radAlfa) + dirT.y * cos(radAlfa);
    dirT = Ponto (xr,yr);

    //dir.imprime();

    lastAlfa = alfa;

    Universo[nInstancias].posicao =  Universo[instancia].posicao + dirT * (Universo[instancia].modelo.h/2 + 0.1);
    Universo[nInstancias].modelo = model;
    Universo[nInstancias].damageModel = death;
    Universo[nInstancias].ally = ally;
    Universo[nInstancias].rotacao = alfa;
    Universo[nInstancias].DefineEnvelope();

    nInstancias++;
}

// **********************************************************************
//  void EnemyBulletTime(int instancia)
//    inicia o tempo de tiro dos inimigos
// **********************************************************************
void EnemyBulletTime(int instancia){
    if(Universo[instancia].ativo){
        Universo[instancia].bulletTime = Universo[instancia].defaultBulletTime;
        instanciaTiro(instancia, TiroInimigo, tiroInimigoBoom, false);
    }
}

// **********************************************************************
//  void DecreaseBulletTime()
//    Atualiza o tempo dos tiros
// **********************************************************************
void DecreaseBulletTime(){
    for(int i = 1; i < NAVESCOUNT + 1; i++){
        Universo[i].bulletTime -= ft;
        if(Universo[i].bulletTime <= 0){
            EnemyBulletTime(i);
        }
    }
}

// **********************************************************************
//  void DesenhaCenario()
//    desenha todas as instancias do universo
// **********************************************************************
void DesenhaCenario()
{
    for(int i=0; i< nInstancias;i++)
    {
        if(Universo[i].ativo){
            Universo[i].desenha();
        }
    }

}

// **********************************************************************
//  void EnemyDeath(int instancia)
//     Inicia a animação de morte de uma instancia e o tira do jogo
// **********************************************************************
void EnemyDeath(int instancia){
    Universo[instancia].modelo = Universo[instancia].damageModel;
    Universo[instancia].deathAnimation = true;
    Universo[instancia].damageTime = HURT_ANIMATION_TIME;
    Universo[instancia].temColisao = false;
    if(Universo[instancia].ally ){
        limiteTiro += 1;
    }

}

// **********************************************************************
//  void DestroyEnemy(int instancia)
//    Desativo os desenhos de uma instancia
// **********************************************************************
void DestroyEnemy(int instancia){
    Universo[instancia].ativo = false;
}

// **********************************************************************
//  void DecreaseDamageTime(int instancia, double time)
//    Atualiza o tempo de animação de uma instancia
// **********************************************************************
void DecreaseDamageTime(int instancia, double time){
    Universo[instancia].damageTime -= time;

    if(Universo[instancia].damageTime <= 0){
        DestroyEnemy(instancia);
    }
}

// **********************************************************************
//  void verificaMorte()
//    Verifica se alguma instancia está na animacao de morte e a atualiza
// **********************************************************************
void verificaMorte(){
    for(int i = 1; i < nInstancias; i++){
        if(Universo[i].deathAnimation){
            DecreaseDamageTime(i, ft);
        }
    }
}

// **********************************************************************
//  void GameOver()
//    Faz as ações necessearias para o final do jogo em caso de derrota
// **********************************************************************
void GameOver(){
    Universo[0].ativo = false;
    Universo[0].temColisao = false;

    Universo[nInstancias].temColisao = false;
    Universo[nInstancias].posicao = Ponto(0,0);
    Universo[nInstancias].modelo = GameOverModel;
    Universo[nInstancias].temColisao = false;
    nInstancias++;
    cout << "Game Over!!!" << endl;
}

// **********************************************************************
// void Win()
//    Faz as ações necessearias para o final do jogo em caso de vitoria
// **********************************************************************
void Win(){
    Universo[0].ativo = false;
    Universo[0].temColisao = false;

    Universo[nInstancias].temColisao = false;
    Universo[nInstancias].posicao = Ponto(0,0);
    Universo[nInstancias].modelo = winModel;
    Universo[nInstancias].temColisao = false;
    nInstancias++;
}

// **********************************************************************
// void criaAnimacao()
//    troca o modelo para o modelo de dano do disparador
// **********************************************************************
void criaAnimacao(){
    Universo[0].modelo = hurtPlayer;
}

// **********************************************************************
// void animacaoReceberDano()
//    inicia as animações de dano
// **********************************************************************
void animacaoReceberDano(){
    if(damageTime <= 0){
        damageTime = HURT_ANIMATION_TIME;
        animacaoDeDano = true;
        criaAnimacao();
    }
}

// **********************************************************************
// void animacaoReceberDano()
//    volta o modelo do disparador ao normal após o dano
// **********************************************************************
void cancelaAnimacao(){
    animacaoDeDano = false;
    Universo[0].modelo = Disparador;
}

// **********************************************************************
// void receberDano()
//    diminui a vida e tira a instancia de vida da tela
// **********************************************************************
void receberDano(){
    playerHealth--;
    Universo[positionHealth+playerHealth].ativo = false;
}

// **********************************************************************
// void PerderVida()
//    Verifica se as vidas acabaram para encerrar o jogo
// **********************************************************************
void PerderVida(){
    if(playerHealth == 1){
        receberDano();
        GameOver();
        gameOver = true;
    }
    else{
        receberDano();
        animacaoReceberDano();
    }
}

// **********************************************************************
// Ponto obtemVetorUnitario(Ponto p1, Ponto p2)
//    retorna o vetor unitário de um vetor composto de dois pontos
// **********************************************************************
Ponto obtemVetorUnitario(Ponto p1, Ponto p2){
    Ponto vetContinuidade = p2 - p1;
    double moduloVet = Universo[1].bezier.calculaDistancia(p1,p2);
    return vetContinuidade / moduloVet;
}

// **********************************************************************
// loat pegaAngulo(Bezier curva, double t , double dt, Ponto P)
//    retorna o angulo em relacao a curva de bezier para rotacionar os personagens
// **********************************************************************
float pegaAngulo(Bezier curva, double t , double dt, Ponto P){
    if(dt == 0) dt = 0.1;

    Ponto Pd = curva.Calcula(t+dt);
    Ponto v1 = Ponto(0,1,0);
    Ponto v2 = Pd - P;

    double prodEscalar = ProdEscalar(v1,v2);
    double tV1 = 1;
    double tV2 = curva.calculaDistancia(Pd,P);

    Ponto prodVet;
    ProdVetorial(v1,v2,prodVet);
    if( prodVet.z > 0 ){
        return acos(prodEscalar/(tV1*tV2)) * RadToAngle;
    }

    return -acos(prodEscalar/(tV1*tV2)) *  RadToAngle;
}

// **********************************************************************
// void attInimigos()
//   atualiza a posicao dos inimigos imendando as curvas de bezier 
// **********************************************************************
void attInimigos(){

    Ponto P;
    for(int i = 0; i < NAVESCOUNT; i++){
        if(Universo[i+1].ativo){
            double dt =  Universo[i+1].bezier.CalculaT(Universo[i+1].speed*ft);
        Universo[i+1].tAtual += dt;

        if(Universo[i+1].tAtual > 0.7 && Universo[i+1].turnSet==false && Universo[i+1].nCurva==1){
            Ponto vetUnitario = obtemVetorUnitario(Universo[i+1].bezier.Coords[1],Universo[i+1].bezier.Coords[2]);
            Ponto continuidadeDerivada = Universo[i+1].bezier.Coords[2] + (vetUnitario * TURNSIZE);
            Ponto dirPlayer = continuidadeDerivada + obtemVetorUnitario(continuidadeDerivada, Universo[0].posicao) * TURNSIZE;

            Universo[i+1].bezierTurn = Bezier(Universo[i+1].bezier.Coords[2], continuidadeDerivada, dirPlayer);
            Universo[i+1].turnSet = true;
        }

        if(Universo[i+1].tAtual > 1 && Universo[i+1].nCurva==1){
            Universo[i+1].bezier = Universo[i+1].bezierTurn;
            Universo[i+1].tAtual -=1 ;
            Universo[i+1].turnSet = false;
            Universo[i+1].nCurva = 2;
        }


        if(Universo[i+1].tAtual > 0.7 && Universo[i+1].turnSet==false && Universo[i+1].nCurva==2){
            int Randsize = 1 + rand() % 5 ;
            Ponto vetUnitario = obtemVetorUnitario(Universo[i+1].bezier.Coords[1],Universo[i+1].bezier.Coords[2]);
            Ponto continuidadeDerivada = Universo[i+1].bezier.Coords[2] + (vetUnitario * TURNSIZE);
            Ponto dirPlayer = continuidadeDerivada + obtemVetorUnitario(continuidadeDerivada, Universo[0].posicao) * TURNSIZE * Randsize;

            Universo[i+1].bezierTurn = Bezier(Universo[i+1].bezier.Coords[2], continuidadeDerivada, dirPlayer);
            Universo[i+1].turnSet = true;
        }

        if(Universo[i+1].tAtual > 1 && Universo[i+1].nCurva==2){
            Universo[i+1].bezier = Universo[i+1].bezierTurn;
            Universo[i+1].tAtual -=1 ;
            Universo[i+1].turnSet = false;
            Universo[i+1].nCurva = 1;
        }

        P = Universo[i+1].bezier.Calcula(Universo[i+1].tAtual);
        //float angle = pegaAngulo(Universo[i+1].bezier,Universo[i+1].tAtual,dt,P);
        //Universo[i+1].rotacao += angle;
        Universo[i+1].rotacao = pegaAngulo(Universo[i+1].bezier,Universo[i+1].tAtual,dt,P);
        Universo[i+1].posicao = P;
        Universo[i+1].DefineEnvelope();
        }
    }
}

// **********************************************************************
// void AtualizarPersonagens()
//   atualiza a posicao de todos os personagens (Disparador, inimgos, tiros)
// **********************************************************************
void AtualizarPersonagens(){

    if(movePlayer==true){
        Ponto novaPosicao = movimentaPersonagem(Universo[0].posicao, Universo[0].rotacao);
        if(novaPosicao.x > SIZE_X_MAX) novaPosicao.x = SIZE_X_MAX;
        if(novaPosicao.x < SIZE_X_MIN) novaPosicao.x = SIZE_X_MIN;
        if(novaPosicao.y > SIZE_Y_MAX) novaPosicao.y = SIZE_Y_MAX;
        if(novaPosicao.y < SIZE_Y_MIN) novaPosicao.y = SIZE_Y_MIN;

        Universo[0].posicao = novaPosicao;

    }
    Universo[0].DefineEnvelope();

    attInimigos();

    for(int i = positionShoot; i < nInstancias; i++){

        if(Universo[i].temColisao){
            Universo[i].posicao = movimentaTiro(Universo[i].posicao, Universo[i].rotacao);
            Universo[i].DefineEnvelope();
            if(Universo[i].posicao.x > SIZE_X_MAX + 10) EnemyDeath(i);
            if(Universo[i].posicao.x < SIZE_X_MIN - 10) EnemyDeath(i);
            if(Universo[i].posicao.y > SIZE_Y_MAX + 10) EnemyDeath(i);
            if(Universo[i].posicao.y < SIZE_Y_MIN - 10) EnemyDeath(i);
        }
    }

}

// **********************************************************************
// void VerificaColisao()
//   Verifica a colisão de todas as instancias com colisão
// **********************************************************************
void VerificaColisao(){
    int n = 0;
    for(int i=0; i< nInstancias;i++)
    {
        for(int j=n; j< nInstancias;j++)
        {
            if(j!=i && Universo[i].temColisao && Universo[j].temColisao){
                //cout<< "Verifica " << i << " com " << j << endl;
                // Universo[i].imprimeEnv();
                // Universo[j].imprimeEnv();
                if (DentroConvex(Universo[i].envelope, Universo[j].envelope)){
                    if(i==0){
                        PerderVida();
                        EnemyDeath(j);
                        if(j>0 && j<=NAVESCOUNT) navesDestruidas++;
                    }
                    else if(i>0 && i<=NAVESCOUNT){
                        if(Universo[j].ally){
                            EnemyDeath(i);
                            EnemyDeath(j);
                            navesDestruidas++;
                        }
                    }
                }
            }
        }
        n++;
    }
}

// **********************************************************************
// void criaEstrelas()
//   Gera as estrelas pelo mapa (estetico)
// **********************************************************************
void criaEstrelas(){
    for(int i = 0; i<200;i++){
        int xRand = SIZE_X_MIN + ( rand() % (SIZE_X_MAX - SIZE_X_MIN + 1) );
        int yRand = SIZE_Y_MIN + ( rand() % (SIZE_Y_MAX - SIZE_Y_MIN + 1) );

        Estrelas[i] = Ponto(xRand, yRand);
    }
}

// **********************************************************************
// void desenhaEstrelas()
//   desenha as estrelas
// **********************************************************************
void desenhaEstrelas(){
    for(int i = 0; i<200;i++){

        glPushMatrix();
            glColor3f(1.0f,1.0f,1.0f);
            glPointSize(1.0f);
            glBegin(GL_POINTS);
                glVertex3f(Estrelas[i].x, Estrelas[i].y, 0.0f);
            glEnd();
        glPopMatrix();

    }

}
// **********************************************************************
//
// **********************************************************************
void init()
{
    // Define a cor do fundo da tela (AZUL)
    glClearColor(.0f, .0f, .0f, 1.0f);

    //LeObjeto("EstadoRS.txt");
    //LeObjeto("PoligonoDeTeste.txt", Seta);
    //LeObjeto("MeiaSeta.txt", MeiaSeta);
    //LeObjeto("Mastro.txt", Mastro);
    //Min.x--;Min.y--;
    //Max.x++;Max.y++;

    CarregaModelos();
    CarregaInstancias();
    Min = Ponto(-50,-50);
    Max = Ponto(50,50);
    criaEstrelas();
}

double nFrames=0;
double TempoTotal=0;
// **********************************************************************
//
// **********************************************************************
void animate()
{
    double dt;
    dt = T.getDeltaT();
    AccumDeltaT += dt;
    TempoTotal += dt;
    nFrames++;
    ft = dt;

    if(damageTime > 0 ){
        damageTime -= ft;
    }
    else if(animacaoDeDano && damageTime <=0){
        cancelaAnimacao();
    }

    verificaMorte();
    DecreaseBulletTime();
    if (AccumDeltaT > 1.0/30) // fixa a atualiza��o da tela em 30
    {
        AccumDeltaT = 0;
        angulo+=1;
        glutPostRedisplay();
    }
    if (TempoTotal > 5.0)
    {
        cout << "Tempo Acumulado: "  << TempoTotal << " segundos. " ;
        cout << "Nros de Frames sem desenho: " << nFrames << endl;
        cout << "FPS(sem desenho): " << nFrames/TempoTotal << endl;
        TempoTotal = 0;
        nFrames = 0;
    }
}
// **********************************************************************
//  void reshape( int w, int h )
//  trata o redimensionamento da janela OpenGL
//
// **********************************************************************
void reshape( int w, int h )
{
    // Reset the coordinate system before modifying
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Define a area a ser ocupada pela area OpenGL dentro da Janela
    glViewport(0, 0, w, h);
    // Define os limites logicos da area OpenGL dentro da Janela
    glOrtho(Min.x,Max.x,
            Min.y,Max.y,
            -10,+10);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
// **********************************************************************
//
// **********************************************************************
void DesenhaEixos()
{
    Ponto Meio;
    Meio.x = (Max.x+Min.x)/2;
    Meio.y = (Max.y+Min.y)/2;
    Meio.z = (Max.z+Min.z)/2;

    glBegin(GL_LINES);
    //  eixo horizontal
        glVertex2f(Min.x,Meio.y);
        glVertex2f(Max.x,Meio.y);
    //  eixo vertical
        glVertex2f(Meio.x,Min.y);
        glVertex2f(Meio.x,Max.y);
    glEnd();
}

void DesenhaCurvas(){
    for(int i=0; i < NAVESCOUNT; i++){
        if(Universo[i+1].temColisao){
            Universo[i+1].bezier.Traca();
            if(Universo[i+1].turnSet == true){
                Universo[i+1].bezierTurn.Traca();
            }
        }

    }
}

// **********************************************************************
//  void display( void )
//
// **********************************************************************
void display( void )
{
    //cout << __FUNCTION__ << endl;
	// Limpa a tela coma cor de fundo
	glClear(GL_COLOR_BUFFER_BIT);

    // Define os limites l�gicos da �rea OpenGL dentro da Janela
	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	// Coloque aqui as chamadas das rotinas que desenham os objetos
	// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	glLineWidth(1);
	glColor3f(1,1,1); // R, G, B  [0..1]
    //DesenhaEixos();

    glLineWidth(2);

    //DesenhaCatavento();
    AtualizarPersonagens();
    VerificaColisao();
    if(DesenhaBezier){

        DesenhaCurvas();
    }

    if(DesenhaEnvelope){
       for(int i=0; i < nInstancias; i++){
           if(Universo[i].temColisao)
           Universo[i].DesenhaEnvelope();
        }
    }

    DesenhaCenario();

    desenhaEstrelas();


    if(navesDestruidas == NAVESCOUNT && !gameOver){
        Win();
        navesDestruidas++;
    }
    
    if(gameOver){
        gameOverTime -= ft;
    }

    if(gameOverTime <= 0) exit(0);

	glutSwapBuffers();
}
// **********************************************************************
// ContaTempo(double tempo)
//      conta um certo n�mero de segundos e informa quanto frames
// se passaram neste per�odo.
// **********************************************************************
void ContaTempo(double tempo)
{
    Temporizador T;

    unsigned long cont = 0;
    cout << "Inicio contagem de " << tempo << "segundos ..." << flush;
    while(true)
    {
        tempo -= T.getDeltaT();
        cont++;
        if (tempo <= 0.0)
        {
            cout << "fim! - Passaram-se " << cont << " frames." << endl;
            break;
        }
    }

}
// **********************************************************************
//  void keyboard ( unsigned char key, int x, int y )
//
// **********************************************************************

void keyboard ( unsigned char key, int x, int y )
{

	switch ( key )
	{
		case 27:        // Termina o programa qdo
			exit ( 0 );   // a tecla ESC for pressionada
			break;
        case 't':
            ContaTempo(3);
            break;
        case 'b':
            DesenhaBezier = !DesenhaBezier;
            break;
        case 'v':
            DesenhaEnvelope = !DesenhaEnvelope;
            break;
        case 'f':
            glutFullScreen ( ); // Vai para Full Screen
			break;
        case ' ':
            if(limiteTiro > 0 && !gameOver){
                limiteTiro -= 1;
                instanciaTiro(0,Tiro,tiroInimigoBoom, true);
            }
            break;
		default:
			break;
	}
}
// **********************************************************************
//  void arrow_keys ( int a_keys, int x, int y )
//
//
// **********************************************************************
void arrow_keys ( int a_keys, int x, int y )
{
	switch ( a_keys )
	{
		case GLUT_KEY_UP:       // Se pressionar UP
            movePlayer = true;
			break;
	    case GLUT_KEY_DOWN:     // Se pressionar UP
			movePlayer = false;					// Reposiciona a janela
            //glutPositionWindow (50,50);
			//glutReshapeWindow ( 700, 700 );
			break;
        case GLUT_KEY_LEFT:
            Universo[0].rotacao += ROTACACAO_SPEED;
            break;
        case GLUT_KEY_RIGHT:
            Universo[0].rotacao -= ROTACACAO_SPEED;
            break;
		default:
			break;
	}
}

void Mouse(int button,int state,int x,int y)
{
    GLint viewport[4];
    GLdouble modelview[16],projection[16];
    GLfloat wx=x,wy,wz;
    GLdouble ox=0.0,oy=0.0,oz=0.0;

    if(state!=GLUT_DOWN)
      return;
    if(button!=GLUT_RIGHT_BUTTON)
     return;
    cout << "Botao da direita! ";

    glGetIntegerv(GL_VIEWPORT,viewport);
    y=viewport[3]-y;
    wy=y;
    glGetDoublev(GL_MODELVIEW_MATRIX,modelview);
    glGetDoublev(GL_PROJECTION_MATRIX,projection);
    glReadPixels(x,y,1,1,GL_DEPTH_COMPONENT,GL_FLOAT,&wz);
    gluUnProject(wx,wy,wz,modelview,projection,viewport,&ox,&oy,&oz);
    Ponto P(ox,oy, oz);
    P.imprime();
}


// **********************************************************************
//  void main ( int argc, char** argv )
//
// **********************************************************************
int  main ( int argc, char** argv )
{
    cout << "Programa OpenGL" << endl;

    glutInit            ( &argc, argv );
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB );
    glutInitWindowPosition (0,0);

    // Define o tamanho inicial da janela grafica do programa
    glutInitWindowSize  ( 650, 650);

    // Cria a janela na tela, definindo o nome da
    // que aparecera na barra de t�tulo da janela.
    glutCreateWindow    ( "Asteorids" );

    // executa algumas inicializa��es
    init ();

    // Define que o tratador de evento para
    // o redesenho da tela. A funcao "display"
    // ser� chamada automaticamente quando
    // for necess�rio redesenhar a janela
    glutDisplayFunc ( display );

    // Define que o tratador de evento para
    // o invalida��o da tela. A funcao "display"
    // ser� chamada automaticamente sempre que a
    // m�quina estiver ociosa (idle)
    glutIdleFunc(animate);

    // Define que o tratador de evento para
    // o redimensionamento da janela. A funcao "reshape"
    // ser� chamada automaticamente quando
    // o usu�rio alterar o tamanho da janela
    glutReshapeFunc ( reshape );

    // Define que o tratador de evento para
    // as teclas. A funcao "keyboard"
    // ser� chamada automaticamente sempre
    // o usu�rio pressionar uma tecla comum
    glutKeyboardFunc ( keyboard );

    // Define que o tratador de evento para
    // as teclas especiais(F1, F2,... ALT-A,
    // ALT-B, Teclas de Seta, ...).
    // A funcao "arrow_keys" ser� chamada
    // automaticamente sempre o usu�rio
    // pressionar uma tecla especial
    glutSpecialFunc ( arrow_keys );

    glutMouseFunc (Mouse);

    // inicia o tratamento dos eventos
    glutMainLoop ( );

    return 0;
}
