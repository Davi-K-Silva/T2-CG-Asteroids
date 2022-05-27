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

#define PI 3.14159265

#include "Ponto.h"
#include "Modelo.h"
#include "Instancia.h"
#include "Bezier.h"

#include "Temporizador.h"

#define NAVESCOUNT 4
#define ENEMYSPEED 30
#define PLAYERSPEED 0.85
#define SHOTSPEED 20
#define ROTACACAOSPEED 5
#define TURNSIZE 10

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

Ponto dir = Ponto (0,1,0);
int quantEnemyModels;
vector <Modelo> modelos;
Instancia Universo[5000];

// Limites l�gicos da �rea de desenho
Ponto Min, Max;

bool movePlayer = true;

bool DesenhaBezier = true;
bool desenha = false;

float angulo=0.0;
double AngleToRad = PI / 180;
double lastAlfa = 0.0;

int nInstancias=0;

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
// void DentroConvex(Poligono objeto, Poligono objetoColisao, Poligono &pontosOut, Poligono &pontosIn)
//  Verifica se um conjunto de pontos está dentro de um poligon convexo usando o produto vetorial
// **********************************************************************
bool DentroConvex(Ponto objeto[], Ponto objetoColisao[]) {

        Poligono vetores;
       // cout << "tmn 1:"  << sizeof(objeto)/ sizeof(Ponto) << endl;
       // cout << "tmn 2:"  << sizeof(objetoColisao)/ sizeof(objetoColisao[0]) << endl;
        //Coloca os vetores do convexHull base no poligono vetores(A->B, B->C, C->D, D->A)
        for(int i = 0; i < 4; i++) {
            if(i == 4 - 1) {


                Ponto lastVet = objeto[0] - objeto[i];
                vetores.insereVertice(lastVet);
                break;
            }

            Ponto vetor =   objeto[i+1] -  objeto[i];
            vetores.insereVertice(vetor);

           // vetor.imprime(); cout <<  "--" << endl;
        }

        //Insere nos poligonos os pontos que estão fora ou dentro do convexHull base
        for(int i = 0; i < 4; i++) {

            bool isInside = true;

            for(int k = 0; k < vetores.getNVertices(); k++ ) {
                Ponto vetor = objetoColisao[i] - objeto[k];
                Ponto result;
                ProdVetorial(vetor, vetores.getVertice(k), result);
                //cout << "Z: " << result.z << endl;

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

bool VerificaColisao(){
    int n = 0;
    for(int i=0; i< nInstancias;i++)
    {
        for(int j=n; j< nInstancias;j++)
        {
            if(j!=i){
                //cout<< "Verifica " << i << " com " << j << endl;
               // Universo[i].imprimeEnv();
               // Universo[j].imprimeEnv();
                if (DentroConvex(Universo[i].envelope, Universo[j].envelope)){
                    return true;
                }
            }
        }
        n++;
    }
    return false;
}

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

void LeObjeto(const char *nome, Poligono &p)
{
    ifstream input;
    input.open(nome, ios::in);
    if (!input)
    {
        cout << "Erro ao abrir " << nome << ". " << endl;
        exit(0);
    }
    cout << "Lendo arquivo " << nome << "...";
    string S;
    int nLinha = 0;
    unsigned int qtdVertices;

    input >> qtdVertices;
    double x,y;

    // Le a primeira linha apenas para facilitar o calculo do limites
    input >> x >> y;

    Min = Ponto(x,y);
    Max = Ponto(x,y);
    p.insereVertice(Ponto(x,y));

     cout << "Passou" << endl;
    for (int i=0; i< qtdVertices; i++)
    {
        // Le cada elemento da linha
        input >> x >> y;
        // atualiza os limites
        if (x<Min.x) Min.x = x;
        if (y<Min.y) Min.y = y;

        if (x>Max.x) Max.x = x;
        if (y>Max.y) Max.y = y;

        if(!input)
            break;
        nLinha++;
        //cout << "Arquivo: " << x << " " << y << endl;
        p.insereVertice(Ponto(x,y));
    }
    cout << "leitura concluida." << endl;
    //cout << "Linhas lidas: " << nLinha << endl;
    //cout << "Limites:" << endl;
    cout << "Minimo:"; Min.imprime();
    cout << "\tMaximo:"; Max.imprime();
    cout << endl;

}

void CarregaModelos()
{
    Disparador.LeModelo("Modelos/Player.txt");

    Enemy1.LeModelo("Modelos/Enemy1.txt");
    modelos.push_back(Enemy1);
    Enemy2.LeModelo("Modelos/Enemy2.txt");
    modelos.push_back(Enemy2);
    Enemy3.LeModelo("Modelos/Enemy3.txt");
    modelos.push_back(Enemy3);

    quantEnemyModels = modelos.size();

    Tiro.LeModelo("Modelos/PlayerShot.txt");
    TiroInimigo.LeModelo("Modelos/EnemyShot.txt");
    Vida.LeModelo("Modelos/PlayerHealth.txt");
}

// No trabalho, esta fun��o dever instanciar todos os
// personagens do cen�rio
void CarregaInstancias()
{
    srand(time(NULL));

    Universo[0].posicao = Ponto (1,1);
    Universo[0].rotacao = 0;
    Universo[0].modelo = Disparador;
    Universo[0].speed = PLAYERSPEED;
    Universo[0].DefineEnvelope();

    nInstancias = 1;
    for(int i = 0; i < NAVESCOUNT; i++){
        int modeloRandom = rand() % quantEnemyModels;

        int xRand = -50 + ( rand() % 101 );
        int yRand = -50 + ( rand() % 101 );
        Universo[nInstancias].posicao = Ponto (xRand,yRand);
        Universo[nInstancias].rotacao = 0;
        Universo[nInstancias].modelo = modelos[modeloRandom];
        Universo[nInstancias].speed = ENEMYSPEED;
        Universo[nInstancias].DefineEnvelope();

        while(VerificaColisaoInstancia(nInstancias)){
            int xRand = -50 + ( rand() % 101 );
            int yRand = -50 + ( rand() % 101 );
            Universo[nInstancias].posicao = Ponto (xRand,yRand);
            Universo[nInstancias].DefineEnvelope();
            cout << "Colisao" << endl;
        }


        nInstancias ++;
    }

    for(int i = 0; i < NAVESCOUNT; i++){
        int xRand = -50 + ( rand() % 101 );
        int yRand = -50 + ( rand() % 101 );
        Universo[i+1].bezier = Bezier(Universo[i+1].posicao,Universo[0].posicao,Ponto(xRand,yRand));
        Universo[i+1].turnSet = false;
        Universo[i+1].nCurva = 1;
    }


}


void DesenhaCenario()
{
    for(int i=0; i< nInstancias;i++)
    {
        Universo[i].desenha();
    }
}


Ponto obtemVetorUnitario(Ponto p1, Ponto p2){
    Ponto vetContinuidade = p2 - p1;
    double moduloVet = Universo[1].bezier.calculaDistancia(p1,p2);
    return vetContinuidade / moduloVet;
}

void AtualizarPersonagens(){

    if(movePlayer==true){
        Ponto novaPosicao = movimentaPersonagem(Universo[0].posicao, Universo[0].rotacao);
        if(novaPosicao.x > 50) novaPosicao.x = 50;
        if(novaPosicao.x < -50) novaPosicao.x = -50;
        if(novaPosicao.y > 50) novaPosicao.y = 50;
        if(novaPosicao.y <- 50) novaPosicao.y = -50;

        Universo[0].posicao = novaPosicao;
        Universo[0].DefineEnvelope();
    }


    Ponto P;
    for(int i = 0; i < NAVESCOUNT; i++){

        Universo[i+1].tAtual += Universo[i+1].bezier.CalculaT(Universo[i+1].speed*ft);

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
        Universo[i+1].posicao = P;
        Universo[i+1].DefineEnvelope();
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
    cout << "Vertices no Vetor: " << MeiaSeta.getNVertices() << endl;
    Min = Ponto(-50,-50);
    Max = Ponto(50,50);

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

void DesenhaSeta()
{
    glPushMatrix();
        //glScaled(2, 1, 1);
        MeiaSeta.desenhaLinhas();
        glScaled(1,-1, 1);
        MeiaSeta.desenhaLinhas();
    glPopMatrix();
}
void DesenhaApontador()
{
    glPushMatrix();
        glTranslated(-4, 0, 0);
        //glScalef(1.2,1,1);
        DesenhaSeta();
    glPopMatrix();
}
void DesenhaHelice()
{
    glPushMatrix();
    for(int i=0;i < 4; i++)
    {
        glRotatef(90, 0, 0, 1);
        DesenhaApontador();
    }
    glPopMatrix();
}
void DesenhaHelicesGirando()
{
    glPushMatrix();
        glRotatef(angulo, 0, 0, 1);
        DesenhaHelice();
   glPopMatrix();
}
void DesenhaMastro()
{
    Mastro.desenhaPoligono();
}
void DesenhaCatavento()
{
    glPushMatrix();
        glPushMatrix();
            glColor3f(1,0,0); // R, G, B  [0..1]
            glTranslated(0,3,0);
            glScaled(0.2, 0.2, 1);
            DesenhaHelicesGirando();
        glPopMatrix();
    glPopMatrix();
}
#define LARG
void DesenhaRetangulo()
{
    glBegin(GL_QUADS);
        glVertex2d(-1, -1);
        glVertex2d(-1, 1);
        glVertex2d(1, 1);
        glVertex2d(1, -1);
    glEnd();
}
void DesenhaPersonagem()  // modelo - veio de arquivo
{
    glPushMatrix();
        glColor3f(1, 1, 1);
        glTranslatef(-1, -1, 0);
        DesenhaRetangulo();

        glTranslatef(2, 0, 0);
        glColor3f(1, 0, 0);
        DesenhaRetangulo();

        glTranslatef(0, 2, 0);
        glColor3f(0, 0, 1);
        DesenhaRetangulo();

        glTranslatef(-2, 0, 0);
        glColor3f(1, 1, 0);
        DesenhaRetangulo();
    glPopMatrix();
}

void DesenhaCurvas(){
    for(int i=0; i < NAVESCOUNT; i++){
        Universo[i+1].bezier.Traca();
        if(Universo[i+1].turnSet == true){
            glColor3f(0, 1, 0);
            Universo[i+1].bezierTurn.Traca();
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
    DesenhaEixos();

    glLineWidth(2);

    //DesenhaCatavento();
    AtualizarPersonagens();
    if(DesenhaBezier){
        DesenhaCurvas();
    }

    DesenhaCenario();

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
        case 'f':
            glutFullScreen ( ); // Vai para Full Screen
			break;
        case ' ':
            desenha = !desenha;
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
            Universo[0].rotacao += ROTACACAOSPEED;
            break;
        case GLUT_KEY_RIGHT:
            Universo[0].rotacao -= ROTACACAOSPEED;
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
