#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>
#include <QFileDialog>
#include <QProgressBar>
#include <QFile>

#include <QPixmap>
#include <QPainter>
#include <QPoint>
#include <QtMath>
#include <QTimer>
#include <QElapsedTimer>
#include <ctime>

#include <QThread>
//#include <QtCore>
//#include <QFuture>
#include <QtConcurrent/QtConcurrent>

//QFuture<void> future;

QImage myImage;
unsigned long frameBufferAlignment;
//uchar frameBuffer[800*600*4];
//unsigned long frameBuffer[800*600];

unsigned long frameBuffer1[1600*1200];
unsigned long frameBuffer2[1600*1200];

//unsigned long (*frontBuffer)[800*600];//=&frameBuffer1;
//unsigned long (*backBuffer)[800*600];//=&frameBuffer2;
//unsigned long (*tempBuffer)[800*600];


int after;

int frameBufferSize;
int frameBufferScanLine;
int frameBufferOffset;
int bufferCenterX,bufferCenterY;
long penColor;
//int px,py;
int px1,py1,px2,py2,px3,py3;
int px[3],py[3];
float fpx[3],fpy[3],fpz[3];
int temp,pxm;
float dx;

short int a[200000];
short int c[65536][2000];

bool doRotate=false;//true;
bool doVertices=true;
bool doWireframe=true;
bool doTriangleFill=true;
bool doZOrdering=true;
bool doRemoveBackfacing=true;
bool doShading=true;

class TInput
{
    QString semantic;
    QString source;
    int offset;
};

class Param
{
public:
    QString name;
    QString type;
};

class Input
{
public:
    QString semantic;
    QString source;
    QString offset;
    QString set;
};

class P
{
public:
    unsigned int count;
    //QList<unsigned short> indices;
    QList<unsigned int> indices;
};

class Accessor
{
public:
    QString source;
    unsigned int count;
    unsigned int string;
    QList<Param> params;
};

class Float_Array
{
public:
    unsigned int count;
    QString id;
    QList<float> floats;
};

class Technique_Common
{
public:
    Accessor accessor;
};

class Source
{
public:
    QString id;
    QString name;
    Float_Array floatArray;
    //Name_Array nameArray;
    Technique_Common techniqueCommon;
};

class Vertices
{
public:
    QString id;
    QString name;
    QList<Input> inputs;
};

class Triangles
{
public:
    //unsigned int count;
    unsigned int count;
    QString name;
    QString material;
    QList<Input> inputs;
    P p;
};

class Mesh
{
public:
    QList<Source> sources;
    Vertices vertices;
    QList<Triangles> triangles;
};

class Geometry
{
public:
    QString id;
    QString name;
    Mesh mesh;

};

class GeometryLibrary
{
public:
    QString macska="mici";
    QList<Geometry> geometries;
};

class Collada
{
public:
    GeometryLibrary geometryLibrary;
};


static const int DOM_ELEMENT = 1;
static const int DOM_ATTRIBUTE = 2;
static const int DOM_TEXT = 3;

class node
{
public:
    int type;
    QString name;
    QString value;
    QString text;
    node* parent;
    node* firstChild;
    node* lastChild;
    node* nextSibling;
    node* previousSibling;
};


class DOMObject
{


public:
    node* root;
    node* current;

    DOMObject()
    {

    }

    node* newElement(node* insertInto, QString name)
    {
        node* insertHere;

        node* item=new node;
        item->type=DOM_ELEMENT;
        item->name=name;
        item->value="";
        item->text="";
        item->parent=NULL;
        item->firstChild=NULL;
        item->lastChild=NULL;
        item->nextSibling=NULL;
        item->previousSibling=NULL;

        item->parent=insertInto;

        //Is it the first child?

        insertHere = insertInto->firstChild;
        if (insertHere==NULL)
        {
            insertHere=item;
            insertInto->firstChild=item;
            //item->parent->lastChild=item;
        }
        else
        {
            //There is already an existing item under it
            //-->> find the last sibling
            while (insertHere->nextSibling!=NULL)
            {
                insertHere=insertHere->nextSibling;
            }
            insertHere->nextSibling=item;
            item->previousSibling=insertHere;

            //item->parent->lastChild=item;

        }

        return item;


    }

    node* newText(node* insertInto, QString text)
    {
        node* insertHere;

        node* item=new node;
        item->type=DOM_TEXT;
        item->name="";
        item->value="";
        item->text=text;
        item->parent=NULL;
        item->firstChild=NULL;
        item->lastChild=NULL;
        item->nextSibling=NULL;
        item->previousSibling=NULL;

        item->parent=insertInto;

        //Is it the first child?

        insertHere = insertInto->firstChild;
        if (insertHere==NULL)
        {
            insertHere=item;
            insertInto->firstChild=item;
            //item->parent->lastChild=item;
        }
        else
        {
            //There is already an existing item under it
            //-->> find the last sibling
            while (insertHere->nextSibling!=NULL)
            {
                insertHere=insertHere->nextSibling;
            }
            insertHere->nextSibling=item;
            item->previousSibling=insertHere;

            //item->parent->lastChild=item;

        }

        return item;


    }

    node* newAttribute(node* insertInto, QString name, QString value)
    {
        node* insertHere;

        node* item=new node;
        item->type=DOM_ATTRIBUTE;
        item->name=name;
        item->value=value;
        item->text="";
        item->parent=NULL;
        item->firstChild=NULL;
        item->lastChild=NULL;
        item->nextSibling=NULL;
        item->previousSibling=NULL;

        item->parent=insertInto;

        //Is it the first child?

        insertHere = insertInto->firstChild;
        if (insertHere==NULL)
        {
            insertHere=item;
            insertInto->firstChild=item;
            //item->parent->lastChild=item;
        }
        else
        {
            //There is already an existing item under it
            //-->> find the last sibling
            while (insertHere->nextSibling!=NULL)
            {
                insertHere=insertHere->nextSibling;
            }
            insertHere->nextSibling=item;
            item->previousSibling=insertHere;

            //item->parent->lastChild=item;

        }

        return item;


    }

    //getFirstChild
    //getLastChild
    //getNextSibling
    //getPreviousSibling
    //getParent
    //getTopMostParent


    node* getFirstChildElement(node* thisNode)
    {
        node* current=thisNode;

        if (current->firstChild!=NULL)
        {
            current=current->firstChild;

            while (current->type!=DOM_ELEMENT)
            {
                //printf(thisNode->name.toUtf8().constData());
                //printf("  --  ");
                //printf(thisNode->text.toUtf8().constData());
                //printf("\r");
                current=current->nextSibling;
            }
            //printf("ELEMENT: ");
            //printf(thisNode->name.toUtf8().constData());
            //printf("  --  ");
            //printf(thisNode->text.toUtf8().constData());

            return current;
        }

        return NULL;



    }


    node* getNextSiblingElement(node* thisNode)
    {
        node* current=thisNode;

        current=current->nextSibling;



        /*
        while ((current->type!=DOM_ELEMENT) && (current!=NULL))
        {
            current=current->nextSibling;
        }
        */



        return current;
    }

    node* getParentElement(node* thisNode)
    {

        node* current=thisNode;

        current=current->parent;

        return current;
    }


    node* getElementByPath(QString path)
    {
        //?xml/COLLADA/asset/library_geometries/geometry/mesh/source/float

        node* current;
        QStringList elements;

        elements = path.split("/");



        current=this->root;

        int i=0;
        while (i!=elements.size() && current!=NULL)
        {
            //printf(" ++ ");
            //printf(elements[i].toUtf8().constData());
            //printf("\r");

            current=this->getFirstChildElement(current);
            if (current->name!=elements[i])
            {
                while ((current->name!=elements[i] && current->nextSibling!=NULL)) //&&????
                {
                    //current=current->nextSibling;
                    current=this->getNextSiblingElement(current);
                }

            }

            if (current->name!=elements[i]) current=NULL;

            i++;
        }

        if (i==elements.size()) return current;
        else return NULL;
    }

    node* getFirstChildElementByName(node* thisNode, QString searchedName)
    {

        node* current;

        current=thisNode;

        current=current->firstChild;

        while ((current!=NULL)) //(current->type!=DOM_ELEMENT) &&
        {
             if ((current->type==DOM_ELEMENT) && (current->name==searchedName)) return current;
             current=current->nextSibling;

        }

        return NULL;
    }

    node* getNextSiblingElementByName(node* thisNode, QString searchedName)
    {

        node* current;

        current=thisNode;

        current=current->nextSibling;

        while ((current!=NULL)) //(current->type!=DOM_ELEMENT) &&
        {
             if ((current->type==DOM_ELEMENT) && (current->name==searchedName)) return current;
             current=current->nextSibling;

        }

        return NULL;
    }


    QString getElementName(node* thisNode)
    {
        node* current=thisNode;

        if (current->type==DOM_ELEMENT)
        {
            return current->name;
        }
        return "";
    }

    node* getFirstAttribute(node* thisNode)
    {

        node* current=thisNode;

        if (current->firstChild!=NULL)
        {
            current=current->firstChild;

            while (current->type!=DOM_ATTRIBUTE)
            {
                //printf(thisNode->name.toUtf8().constData());
                //printf("  --  ");
                //printf(thisNode->text.toUtf8().constData());
                //printf("\r");
                current=current->nextSibling;
            }
            //printf("ELEMENT: ");
            //printf(thisNode->name.toUtf8().constData());
            //printf("  --  ");
            //printf(thisNode->text.toUtf8().constData());

            return current;
        }

        return NULL;

    }

    node* getNextAttribute(node* thisNode)
    {
        node* current=thisNode;

        current=current->nextSibling;

        if (current->type!=DOM_ATTRIBUTE) current=NULL;

        /*
        while ((current->type!=DOM_ELEMENT) && (current!=NULL))
        {
            current=current->nextSibling;
        }
        */



        return current;

    }

    node* getAttributeByName(node* thisNode, QString searchedName)
    {

        node* current;

        current=thisNode;

        current=current->firstChild;

        while ((current!=NULL))  //(current->type=DOM_ATTRIBUTE) &&
        {
             if ((current->type==DOM_ATTRIBUTE) && (current->name==searchedName)) return current;
             current=current->nextSibling;

        }

        return NULL;
    }


    QString getAttributeName(node* thisNode)
    {

        node* current=thisNode;

        if (current->type==DOM_ATTRIBUTE)
        {
            return current->name;
        }
        return "";
    }

    QString getAttributeValue(node* thisNode)
    {
        node* current=thisNode;

        if (current->type==DOM_ATTRIBUTE)
        {
            return current->value;
        }
        return "";
    }

    node* getFirstText(node* thisNode)
    {
        node* current=thisNode;

        if (current->firstChild!=NULL)
        {
            current=current->firstChild;

            while (current->type!=DOM_TEXT)
            {
                //printf(thisNode->name.toUtf8().constData());
                //printf("  --  ");
                //printf(thisNode->text.toUtf8().constData());
                //printf("\r");
                current=current->nextSibling;
            }
            //printf("ELEMENT: ");
            //printf(thisNode->name.toUtf8().constData());
            //printf("  --  ");
            //printf(thisNode->text.toUtf8().constData());

            return current;
        }

        return NULL;

    }

    QString getTextText(node* thisNode)
    {

        node* current=thisNode;

        if (current->type==DOM_TEXT)
        {
            return current->text;
        }
        return "";
    }



    int loadXML(QString input)
    {

        //QProgressBar* openColladaProgess =


        /*

        printf("|");
        printf(input.toUtf8().constData());
        printf("|");
        printf("\r");

        */


        root=new node;

        root->type=DOM_ELEMENT;
        root->name="/";
        root->value="";
        root->text="";
        root->parent=NULL;
        root->firstChild=NULL;
        root->lastChild=NULL;
        root->nextSibling=NULL;
        root->previousSibling=NULL;

        current=root;

        QString parseable;
        int pos1;
        int pos2;
        int pos3;
        int pos4;
        QString elementName;
        QString text;
        QString attributes;
        QStringList attributelist;
        QString attributeName;
        QString attributeValue;

        int i=0;
        while (i<input.length())
        {
            pos1 = input.indexOf("<",i);
            pos2 = input.indexOf("<",pos1+1);

            if (pos2 == -1) pos2 = input.length();

            parseable = input.mid(pos1,pos2-pos1);

            i=pos2;

            //parseable will contain: <element attribute1="value1" attribute2="value2">text<

            pos1 = 1;
            pos2 = parseable.indexOf(" ");
            pos3 = parseable.indexOf("/>");
            pos4 = parseable.indexOf(">");

            if (pos2==-1) pos2=parseable.length();
            if (pos3==-1) pos3=parseable.length();
            if (pos4==-1) pos4=parseable.length();

            if (pos2>pos3) pos2=pos3;
            if (pos2>pos4) pos2=pos4;

            elementName=parseable.mid(pos1,pos2-pos1);

            pos1 = parseable.indexOf(">")+1;
            pos2 = parseable.length();

            text=parseable.mid(pos1, pos2-pos1);



            pos1 = parseable.indexOf(" ",1);
            pos2 = parseable.indexOf("/>",1);
            pos3 = parseable.indexOf(">",1);

            attributes="";
            if (pos1!=-1)
            {
              if (pos2==-1) pos2=parseable.length();
              if (pos3==-1) pos3=parseable.length();

              if (pos2>pos3) pos2=pos3;

              if (pos1<pos2)
              {
                attributes=parseable.mid(pos1+1, pos2-pos1-1);
              }
            }



            if (parseable.left(2)!="</")
            {
                current=this->newElement(current,elementName);
                if (attributes!="")
                {
                    attributelist=attributes.split(" ");
                    for (int j=0;j<attributelist.size();j++)
                    {
                        if (attributelist[j]!="")
                        {
                            pos1=0;
                            pos2=attributelist[j].indexOf("=");
                            attributeName=attributelist[j].mid(pos1,pos2-pos1);
                            pos1=attributelist[j].indexOf("\"",attributelist[j].indexOf("="))+1;
                            pos2=attributelist[j].indexOf("\"",pos1);
                            attributeValue=attributelist[j].mid(pos1,pos2-pos1);
                            this->newAttribute(current,attributeName,attributeValue); //current=
                        }
                    }

                }
                if (text!="") this->newText(current,text); //current=


            }
            else current=current->parent; //qobject_cast<node*>


            //if (parseable.right(2)=="/>") current=current->parent;
            if (parseable.indexOf("/>")!=-1) current=current->parent;

/*
            printf("|");
            printf(parseable.toUtf8().constData());
            printf("|");
            printf(attributes.toUtf8().constData());
            printf("|");
            printf("\r");
            */


        }




        return 0; //No error



    }

    int loadXMLFromFile(QString fileName)
    {
        QString fileContent;

        QFile xmlFile(fileName);

        if (xmlFile.open(QIODevice::ReadOnly | QIODevice::Text))
        {

            fileContent=xmlFile.readAll();

            xmlFile.close();
        }

        if (this->loadXML(fileContent)==0) return 0; //No error
        else return -1; //Error



    }

};



QString colladaFileName;

DOMObject* colladaXML;

bool checkElementByName(node* currentNode, QString elementName)
{
    if (currentNode!=NULL)
    {
        if (currentNode->type==DOM_ELEMENT)
        {
            if (colladaXML->getElementName(currentNode)==elementName)
            {
                printf("Valid <");
                printf(elementName.toUtf8().constData());
                printf("> element.");
                printf("\r");

                return true; //Parse successful
            }
        }
    }
    printf("Parsing error: Inalid <");
    printf(elementName.toUtf8().constData());
    printf("> element.");
    printf("\r");

    return false; //Parse failure
}

bool checkAttributeByName(node* currentNode, QString attributeName)
{
    if (currentNode!=NULL)
    {
        if (currentNode->type==DOM_ATTRIBUTE)
        {
            if (colladaXML->getAttributeName(currentNode)==attributeName)
            {
                printf("Valid <");
                printf(attributeName.toUtf8().constData());
                printf("> attribute.");
                printf("\r");

                return true; //Parse successful
            }
        }
    }
    printf("Parsing error: Invalid <");
    printf(attributeName.toUtf8().constData());
    printf("> attribute.");
    printf("\r");

    return false; //Parse failure
}

QList<unsigned int> getTrianglePositionsFromTriangleData(Mesh thisMesh, Triangles thisTriangle)
{
    int verticesOffset=0;
    int positionOffset=0;
    int normalOffset=0;
    int texcoordOffset=0;
    int triangleOffset=0;

    for (int j=0;j<thisTriangle.inputs.size();j++)
    {

        if (thisTriangle.inputs.at(j).semantic=="VERTEX")
        {
            verticesOffset=thisTriangle.inputs.at(j).offset.toInt();
        }
    }


    for (int j=0;j<thisTriangle.inputs.size();j++)
    {
        if (thisTriangle.inputs.at(j).semantic=="POSITION")
        {
            positionOffset=thisTriangle.inputs.at(j).offset.toInt();
        }
        if (thisTriangle.inputs.at(j).semantic=="NORMAL")
        {
            normalOffset=thisTriangle.inputs.at(j).offset.toInt();
        }
        if (thisTriangle.inputs.at(j).semantic=="TEXCOORD")
        {
            texcoordOffset=thisTriangle.inputs.at(j).offset.toInt();
        }

    }

    //IF POSITIONOFFSET STILL 0 IT IS NOT REFERENCED UNDER TRIANLGES -> STILL GOOD FOR US -> IT SHOULD BE IN THE VERTICES SECTION

    //if (isEmpty(collada.geometryLibrary.geometries.at(i).mesh.vertices))
    {
        for (int j=0;j<thisMesh.vertices.inputs.size();j++)
        {

            if (thisMesh.vertices.inputs.at(j).semantic=="POSITION")
            {
                positionOffset=j;
            }
            if (thisMesh.vertices.inputs.at(j).semantic=="NORMAL")
            {
                normalOffset=j;
            }
            if (thisMesh.vertices.inputs.at(j).semantic=="TEXCOORD")
            {
                texcoordOffset=j;
            }

        }
        positionOffset=positionOffset+verticesOffset;
        normalOffset=normalOffset+verticesOffset;
        texcoordOffset=texcoordOffset+verticesOffset;
    }


    if (positionOffset>triangleOffset) triangleOffset=positionOffset;
    if (normalOffset>triangleOffset) triangleOffset=normalOffset;
    if (texcoordOffset>triangleOffset) triangleOffset=texcoordOffset;
    triangleOffset++;

    triangleOffset=1; //TEAPOT=1, BOX=3, IRONMAN=1
    positionOffset=0;


    printf("POSITION offset: ");
    printf(QString::number(positionOffset).toUtf8().constData());
    printf("\r");
    printf("NORMAL offset: ");
    printf(QString::number(normalOffset).toUtf8().constData());
    printf("\r");
    printf("TEXCOORD offset: ");
    printf(QString::number(texcoordOffset).toUtf8().constData());
    printf("\r");
    printf("TRIANGLE offset: ");
    printf(QString::number(triangleOffset).toUtf8().constData());
    printf("\r");



    //-----------------++++++++++++++++-----------------

    //INQUIRE PRIMITIVES DATASET
    //QList<short unsigned int> primitives;
    QList<unsigned int> primitives;
    QList<unsigned int> trianglepositionsData;

    primitives=thisTriangle.p.indices;

    for (int j=0;j<primitives.size();j++)
    {
        printf("(");
        printf(QString::number(j).toUtf8().constData());
        printf(") ");
        printf(QString::number(primitives.at(j)).toUtf8().constData());
        printf(" ");
        //printf()
    }

    printf("\r");

    for (int j=0;j<primitives.size()/triangleOffset;j++)
    {
        trianglepositionsData.append(primitives.at(j*triangleOffset)+positionOffset);
    }



    return trianglepositionsData;


}


//RENDERING
QPainter* painter;

QList<float> vertexX;
QList<float> vertexY;
QList<float> vertexZ;

QList<float> vX;
QList<float> vY;
QList<float> vZ;


QList<short unsigned int> faceA;
QList<short unsigned int> faceB;
QList<short unsigned int> faceC;

QList<short unsigned int> faceA2;
QList<short unsigned int> faceB2;
QList<short unsigned int> faceC2;


QList<long> faceColor;
QList<short unsigned int> faceFront;

int frame;
int fps;

QTimer timer;

bool rotate;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setAttribute(Qt::WA_QuitOnClose);
}

MainWindow::~MainWindow()
{
    exit(0);
    rotate=false;
    delete ui;
}

void MainWindow::update()
{
    this->ui->label_2->setText(QString::number(fps));
    fps=0;
}

void MainWindow::on_actionQuit_triggered()
{
    rotate=false;
    exit(0);
    qApp->quit();

}




void closeEvent(QCloseEvent *event)
{

    rotate=false;
    exit(0);
    qApp->quit();
}





void magnify(float magLevel)
{
    for (int i=0;i<vertexX.size();i++)
    {
        vertexX.replace(i,vertexX.at(i)*magLevel);
        vertexY.replace(i,vertexY.at(i)*magLevel);
        vertexZ.replace(i,vertexZ.at(i)*magLevel);
    }
}

float radians=M_PI/180;
float delta;
float deltaInRadians=1*radians;
float cosdeltarad;
float sindeltarad;
float countX,countZ;

void rotating()//(float alphaInRadians)
{

    for (int i=0;i<vertexX.size();i++)
    {

        countX=cosdeltarad*vertexX.at(i) + sindeltarad*vertexZ.at(i);
        countZ=cosdeltarad*vertexZ.at(i) - sindeltarad*vertexX.at(i);
        //countY=allMesh1.vertY[allMeshI];

        //countX=cosalpharad*vertexX[i] + sinalpharad*vertexZ[i];
        //countZ=cosalpharad*vertexZ[i] - sinalpharad*vertexX[i];


        vertexX.replace(i,countX);
        //vertexY.replace(i,countY);
        vertexZ.replace(i,countZ);

        //vertexX[i]=countX;
        //vertexZ[i]=countZ;

    }

}



void rotateY(float alpha)//(float alphaInRadians)
{
    float alphaInRadians=alpha*M_PI/180;
    float cosalpharad=cos(alphaInRadians);
    float sinalpharad=sin(alphaInRadians);

    for (int i=0;i<vertexX.size();i++)
    {

        countX=cosalpharad*vertexX.at(i) + sinalpharad*vertexZ.at(i);
        countZ=cosalpharad*vertexZ.at(i) - sinalpharad*vertexX.at(i);
        //countY=allMesh1.vertY[allMeshI];

        //countX=cosalpharad*vertexX[i] + sinalpharad*vertexZ[i];
        //countZ=cosalpharad*vertexZ[i] - sinalpharad*vertexX[i];


        vertexX.replace(i,countX);
        //vertexY.replace(i,countY);
        vertexZ.replace(i,countZ);

        //vertexX[i]=countX;
        //vertexZ[i]=countZ;

    }
}



void rotateX(float alpha)
{
    float alphaInRadians=alpha*M_PI/180;
    float cosalpharad=cos(alphaInRadians);
    float sinalpharad=sin(alphaInRadians);

    float countY,countZ;

    for (int i=0;i<vertexY.size();i++)
    {

        countY=cosalpharad*vertexY.at(i) - sinalpharad*vertexZ.at(i);
        countZ=cosalpharad*vertexZ.at(i) + sinalpharad*vertexY.at(i);
        //countY=allMesh1.vertY[allMeshI];

        vertexY.replace(i,countY);
        //vertexY.replace(i,countY);
        vertexZ.replace(i,countZ);

    }
}

void rotateZ(float alpha)
{
    float alphaInRadians=alpha*M_PI/180;
    float cosalpharad=cos(alphaInRadians);
    float sinalpharad=sin(alphaInRadians);

    float countX,countY;

    for (int i=0;i<vertexX.size();i++)
    {

        countX=cosalpharad*vertexX.at(i) + sinalpharad*vertexY.at(i);
        countY=cosalpharad*vertexY.at(i) - sinalpharad*vertexX.at(i);
        //countY=allMesh1.vertY[allMeshI];

        vertexX.replace(i,countX);
        //vertexY.replace(i,countY);
        vertexY.replace(i,countY);

    }
}


int penPosX;
int penPosY;

void moveTo(int x, int y)
{
    penPosX=x;
    penPosY=y;
}


int deltax;
int deltay;
int numpixels;
int d;
int dinc1,dinc2;
int xinc1,yinc1,xinc2,yinc2;

//int x,y;
//int index;
//int dyinc1;
//int dyinc2;

void lineTo(int bX, int bY)
{
    //Calculate deltax and deltay for initialisation
    /*
    int deltax=fabs(bX - penPosX);
    int deltay=fabs(bY - penPosY);
    int numpixels;
    int d;
    int dinc1,dinc2;
    int xinc1,yinc1,xinc2,yinc2;
    */


    deltax=fabs(bX - penPosX);
    deltay=fabs(bY - penPosY);

   //Initialize all vars based on which is the independent variable
   if (deltax >= deltay)
   {
       //x is independent variable
       //numpixels=deltax + 1;
       numpixels=deltax;
       d=(deltay *2) - deltax;//(2 * deltay) - deltax;
       dinc1=deltay *2;//deltay * 2;//deltay Shl 1;
       dinc2=(deltay - deltax) *2;//(deltay - deltax) shl 1;
       xinc1= 1;
       xinc2= 1;
       yinc1= 0;
       yinc2= 1;
   }
   else
   {
       //y is independent variable
       //numpixels=deltay + 1;
       numpixels=deltay;
       d=(deltax *2) - deltay;//(2 * deltax) - deltay;
       dinc1=deltax *2;//deltax*2;// deltax Shl 1;
       dinc2=(deltax - deltay) *2; //(deltax - deltay) shl 1;
       xinc1=0;
       xinc2=1;
       yinc1=1;
       yinc2=1;
   }

   //Make sure x and y move in the right directions
   if (penPosX > bX)
   {
       xinc1=-xinc1;
       xinc2=-xinc2;
   }

   if (penPosY>bY)
   {
       yinc1=-yinc1;
       yinc2=-yinc2;
   }

   //Start drawing at <x1, y1>
   int x=penPosX;
   int y=penPosY;

   //Draw the pixels

   //for (int i=1;i<numpixels;i++)

   int index=y*frameBufferScanLine+x;
   int dyinc1=frameBufferScanLine*yinc1;
   int dyinc2=frameBufferScanLine*yinc2;


   for (int i=0;i<numpixels;i++)
   {
       //if ((y>=0) && (y<=itmapBufferHeight))
       //and (x>=0) and (x<=BitmapBufferWidth) then
       //begin
       //  BitmapBufferBits[(y)*(BitmapBufferWidthByte)+(x)]:=BitmapBufferColor;
       //end;
       //painter->drawPoint(x,y);
       //myImage.setPixel(x,y,qRgb(255,0,255));

       //int index=y*frameBufferScanLine+x*frameBufferOffset;
       //int index=y*frameBufferScanLine+x;

       //(*backBuffer)[index]=penColor;

       frameBuffer2[index]=penColor;

       //frameBuffer[index]=penColor;//0xFF00FF00; //ARGB
       //frameBuffer[y*frameBufferScanLine+x]=0xFF00FF00; //ARGB

       //frameBuffer[index+1]=0;
       //frameBuffer[index+2]=0;

       //frameBuffer[y*frameBufferScanLine+x*4]=0;
       //frameBuffer[y*frameBufferScanLine+x*4+1]=0;
       //frameBuffer[y*frameBufferScanLine+x*4+2]=0;

       if (d < 0)
       {
         d= d + dinc1;
         x= x + xinc1;
         y= y + yinc1;
         //index=index+dyinc1+xinc1;

       }
       else
       {
         d= d + dinc2;
         x= x + xinc2;
         y= y + yinc2;
         //index=index+dyinc2+xinc2;
       }
       index=y*frameBufferScanLine+x;
    }

   penPosX=x;
   penPosY=y;


}


class RenderThread : public QThread//QObject //QObject
{
//Q_OBJECT


public:
    RenderThread(QObject* parent=0); //CONSTRUCTOR
    ~RenderThread();                 //DESTRUCTOR

    void render();
    void abortThread();

protected:
    void run();

//signals:
//    void emitUpdate(QString s);

private:
    bool abort;
    QWaitCondition condition;


};

RenderThread::RenderThread(QObject* parent)
{
    //Initialize variables
    abort=false;
}

RenderThread::~RenderThread()
{
    //Destroy objects
    abort=true;

    //Wait until run stops
    wait();

}

void RenderThread::render()
{
    //If thread is not running start it
    if (!isRunning())
    {
        start(LowPriority);
    }
    else
    {
        //If thread is sleeping wake it up
        condition.wakeOne();
    }

}

void RenderThread::abortThread()
{
    abort=true;

    this->~RenderThread();
}

/*
int dx1;
int dx2;
int dx3;
int dy;
int x0;
int x1;
int x2;
int y;
int offset;
*/

void fillFlatBottom(int px0,int py0,int px1,int py1,int px2,int py2)
{

    //int dx1=px1-px0;
    //int dx2=px2-px0;
    int dy=py2-py0; //py2 = py1 --> does not matter

    if (dy!=0)
    {
        float dx1=(float)((px1-px0))/(float)dy;
        float dx2=(float)((px2-px0))/(float)dy;

        for (int j=0;j<dy;j++)
        {
            //Be careful, everything is INT! --> division by dy will be zero!
            //int x1=px0+(dx1*j)/dy;
            //int x2=px0+(dx2*j)/dy;

            int x1=px0+(dx1*j);
            int x2=px0+(dx2*j);

            int y=py0+j;
            int offset=y*frameBufferScanLine;

            for (int i=x1;i<=x2;i++)
            {
                //(*backBuffer)[offset+i]=0xFF808080;

                frameBuffer2[offset+i]=penColor;//0xFF808080;//0xFF0000FF;
            }
        }
    }
}

/*
void fillFlatTop(int px0,int py0,int px1,int py1,int px2,int py2)
{
    int dx0=px2-px0;
    int dx1=px2-px1;
    int dy=py0-py2; //py0 = py1 --> does not matter

    if (dy!=0)
    {
        for (int j=0;j<dy;j++)
        {
            int x0=px2+(dx0*j)/dy;
            int x1=px2+(dx1*j)/dy;

            //int y=py2-j;
            int y=py0+j;
            int offset=y*frameBufferScanLine;

            for (int i=x0;i<x1;i++)
            {
                frameBuffer[offset+i]=0xFFFF00FF;
            }
        }
    }
}
*/






void fillFlatTop(int px0,int py0,int px1,int py1,int px2,int py2)
{
    //int dx2=px2-px0;
    //int dx3=px2-px1;
    int dy=py2-py0; //py0 = py1 --> does not matter

    if (dy!=0)
    {
        float dx2=float(px2-px0)/dy;
        float dx3=float(px2-px1)/dy;


        for (int j=0;j<(dy);j++)
        {
            //int x0=px0+(dx2*j)/dy;
            //int x1=px1+(dx3*j)/dy;
            int x0=px0+(dx2*j);
            int x1=px1+(dx3*j);

            int y=py0+j;
            int offset=y*frameBufferScanLine;

            for (int i=x0;i<=x1;i++)
            {
                //(*backBuffer)[offset+i]=0xFF808080;

                frameBuffer2[offset+i]=penColor;//0xFF808080;//0xFFFF00FF;
            }
        }
    }
}




void RenderThread::run()
{
    int index;


    float check1,check2;
    unsigned short int temp;

    while(!abort)
    {


        fps++;

        memset(&frameBuffer2[0], 255, frameBufferSize*4); //307200

        //memset((unsigned long*)&(*backBuffer)[1], 255, frameBufferSize*4); //307200

        //memset(&(backBuffer)[0], 255, frameBufferSize*4); //307200

        //*backBuffer[

        //if (doRotate==true) rotating();
        //rotating();

        //Z-Ordering

        /*
        //BUBBLE SORT

        int j,i;
        for (j=0;j<faceA.size()-1;j++) //faceA.size();i++)
        {
            //int i=0;
            check1=vertexZ[faceA[j]]+vertexZ[faceB[j]]+vertexZ[faceC[j]];

            for (i=0;i<faceA.size()-1;i++) //faceA.size();i++)
            {

                check2=vertexZ[faceA[i+1]]+vertexZ[faceB[i+1]]+vertexZ[faceC[i+1]];

                //fpz[0]=vertexZ[faceA[i]];
                //fpz[1]=vertexZ[faceB[i]];
                //fpz[2]=vertexZ[faceC[i]];
                //check1=fpz[0]+fpz[1]+fpz[2];

                if (check1>check2)
                {


                    temp = faceA[faceA.size()-1];
                    faceA[faceA.size()-1] = faceA[i+1];
                    faceA[i+1] = temp;

                    temp = faceB[faceA.size()-1];
                    faceB[faceA.size()-1] = faceB[i+1];
                    faceB[i+1] = temp;

                    temp = faceC[faceA.size()-1];
                    faceC[faceA.size()-1] = faceC[i+1];
                    faceC[i+1] = temp;



                }

            }
            check1=check2;
        }

        */


        /*
        QList<unsigned short int> faceA2,faceB2,faceC2;

        faceA2.clear();
        faceB2.clear();
        faceC2.clear();

        faceA2.append(faceA[0]);
        faceB2.append(faceB[0]);
        faceC2.append(faceC[0]);
        for (int j=1;j<faceA.size();j++)
        {
            float check1=vertexZ[faceA[j]]+vertexZ[faceB[j]]+vertexZ[faceC[j]];

            int i=0;
            while (i<faceA2.size())
            {
                float check2=vertexZ[faceA2[i]]+vertexZ[faceB2[i]]+vertexZ[faceC2[i]];

                if (check2>check1)
                {

                    faceA2.insert(i-1,faceA[j]);
                    faceB2.insert(i-1,faceB[j]);
                    faceC2.insert(i-1,faceC[j]);
                    i=faceA2.size();
                }
                i++;
            }
        }

        for (int i=0;i<faceA.size();i++)
        {
            faceA[i]=faceA2[i];
            faceB[i]=faceB2[i];
            faceC[i]=faceC2[i];
        }
        */

        /*
        //MUXIK, DE LASSU
        faceA2.clear();
        faceB2.clear();
        faceC2.clear();

        for (int i=0;i<faceA.size();i++)
        {

            check1=(vertexZ[faceA[i]]+vertexZ[faceB[i]]+vertexZ[faceC[i]])/3;

            if (faceA2.size()>0)
            {
                int j=0;
                check2=(vertexZ[faceA2[j]]+vertexZ[faceB2[j]]+vertexZ[faceC2[j]])/3;
                while ((j<faceA2.size()) && (check1<=check2))
                {
                    check2=(vertexZ[faceA2[j]]+vertexZ[faceB2[j]]+vertexZ[faceC2[j]])/3;
                    j++;
                }
                faceA2.insert(j,faceA[i]);
                faceB2.insert(j,faceB[i]);
                faceC2.insert(j,faceC[i]);

            }
            else
            {

                faceA2.append(faceA[i]);
                faceB2.append(faceB[i]);
                faceC2.append(faceC[i]);
            }
        }
        for (int i=0;i<faceA.size();i++)
        {
            faceA[i]=faceA2[i];
            faceB[i]=faceB2[i];
            faceC[i]=faceC2[i];
        }
        */



        if (doZOrdering==true)
        {
            faceA2.clear();
            faceB2.clear();
            faceC2.clear();
            for (int i=0;i<faceA.size();i++)
            {
                faceA2.append(faceA.at(i));
                faceB2.append(faceB.at(i));
                faceC2.append(faceC.at(i));
            }

            for (int i=0;i<65536;i++)
            {
                c[i][0]=0;
            }
            /*
            int maxz=0;
            for (int i=0;i<faceA.size();i++)
            {
                int check1 = (vertexZ[faceA[i]]+vertexZ[faceB[i]]+vertexZ[faceC[i]])/3;

                if (maxz<fabs(check1)) maxz=fabs(check1);
            }
            //maxz=0.1;
            */


            for (int i=0;i<faceA.size();i++)
            {
                int check1 = (vertexZ[faceA[i]]+vertexZ[faceB[i]]+vertexZ[faceC[i]])/3;

                check1 = check1 + 32768;//(check1*32768/maxz)+32768; //check1 + 32768;

                c[check1][0]=c[check1][0]+1;

                c[check1] [c[check1][0]]=i;

            }

            int k=0;
            for (int i=0;i<65535;i++)
            {
                if (c[i][0]>0)
                {
                    for (int j=0;j<c[i][0];j++)
                    {
                        a[k]=c[i][j+1];
                        k=k+1;
                    }
                }
            }

            for (int i=0;i<faceA.size();i++)
            {
                faceA[i]=faceA2[a[i]];
                faceB[i]=faceB2[a[i]];
                faceC[i]=faceC2[a[i]];
            }
        }

        if (doRemoveBackfacing==true)
        {
            faceFront.clear();
            for (int i=0;i<faceA.size();i++) //faceA.size();i++)
            {
                fpx[0]=vertexX[faceA[i]];
                fpy[0]=vertexY[faceA[i]];
                //fpz[0]=vertexZ[faceA[i]];
                fpx[1]=vertexX[faceB[i]];
                fpy[1]=vertexY[faceB[i]];
                //fpz[1]=vertexZ[faceB[i]];
                fpx[2]=vertexX[faceC[i]];
                fpy[2]=vertexY[faceC[i]];
                //fpz[2]=vertexZ[faceC[i]];

                float check1=(fpx[1]-fpx[0])*(fpy[2]-fpy[1]);
                float check2=(fpy[1]-fpy[0])*(fpx[2]-fpx[1]);

                int frontFace=0;
                if (check1>=check2) frontFace=1;

                faceFront.append(frontFace);
            }
        }
        else
        {
            faceFront.clear();
            for (int i=0;i<faceA.size();i++) //faceA.size();i++)
            {
                faceFront.append(1);
            }
        }

        if (doShading==true)
        {
            faceColor.clear();
            for (int i=0;i<faceA.size();i++) //faceA.size();i++)
            {
                fpx[0]=vertexX[faceA[i]];
                fpy[0]=vertexY[faceA[i]];
                fpz[0]=vertexZ[faceA[i]];
                fpx[1]=vertexX[faceB[i]];
                fpy[1]=vertexY[faceB[i]];
                fpz[1]=vertexZ[faceB[i]];
                fpx[2]=vertexX[faceC[i]];
                fpy[2]=vertexY[faceC[i]];
                fpz[2]=vertexZ[faceC[i]];

                //U Vector = The vector goes from P2 to P1
                float uX=fpx[2]-fpx[1];
                float uY=fpy[2]-fpy[1];
                float uZ=fpz[2]-fpz[1];

                //V Vector = The vector goes from P2 to P0
                float vX=fpx[2]-fpx[0];
                float vY=fpy[2]-fpy[0];
                float vZ=fpz[2]-fpz[0];

                //Normal Vector - Cross Product
                float nX = uY * vZ - uZ * vY;
                float nY = uZ * vX -  uX * vZ;
                float nZ = uX * vY -  uY * vX;

                //Normalize - Make a unit long vector from N
                float n=sqrt(nX*nX + nY*nY + nZ*nZ);

                if (n!=0)
                {
                    nX=nX/n;
                    nY=nY/n;
                    nZ=nZ/n;
                }

                //Assuming light direction / vector - Light products
                float lX=0.90; //-0.56; //- right , + left
                float lY=-0.28; //- up , + down
                float lZ=0;

                //Determining the angle between vectors - Dot procuct
                float x=nX * lX + nY * lY + nZ * lZ;

                penColor=(sin(x)*180/M_PI);
                if (penColor<0) penColor=0;
                penColor=penColor+80;
                //penColor=(penColor * 0x00010101) | 0xFF000000;
                penColor=(penColor * 0x00010000) | 0xFF000000;

                faceColor.append(penColor);

                //penColor=0xFFFFFFFF;
            }
        }
        else
        {
            faceColor.clear();
            for (int i=0;i<faceA.size();i++)
            {
                faceColor.append(0xFF800000);
            }
        }


        if (doTriangleFill==true)
        {
            for (int i=0;i<faceA.size();i++) //faceA.size();i++)
            {
                px[0]=bufferCenterX+vertexX[faceA[i]];
                py[0]=bufferCenterY+vertexY[faceA[i]];//+200;
                px[1]=bufferCenterX+vertexX[faceB[i]];
                py[1]=bufferCenterY+vertexY[faceB[i]];//+200;
                px[2]=bufferCenterX+vertexX[faceC[i]];
                py[2]=bufferCenterY+vertexY[faceC[i]];//+200;

                for (int k=0;k<2;k++)
                {
                    for (int j=0;j<2;j++)
                    {
                        if (py[j+1]<py[j])
                        {
                            temp=py[j+1];
                            py[j+1]=py[j];
                            py[j]=temp;
                            temp=px[j+1];
                            px[j+1]=px[j];
                            px[j]=temp;
                       }
                    }
                }

                /*
                moveTo(px[0],py[0]);
                lineTo(px[1],py[1]);
                lineTo(px[2],py[2]);
                lineTo(px[0],py[0]);
                */

                //if (py[1]!=py[0])
                {
                    //if (py[2]!=py[0])
                    {
                        dx=(px[2]-px[0])/fabs(py[2]-py[0]);//fabs(py[2]-py[0]);
                        pxm=px[0]+dx*fabs(py[1]-py[0]);//*fabs(py[1]-py[0]);

                        if (px[1]<pxm)
                        {
                            temp=pxm;
                            pxm=px[1];
                            px[1]=temp;
                        }

                        //if (pxm!=px[1])
                        {
                            //moveTo(px[0],py[0]);
                            //lineTo(pxm,py[1]);
                            //lineTo(px[1],py[1]);
                            //lineTo(px[0],py[0]);
                            penColor=faceColor[i];

                            //if (faceFront[i]!=0)
                            {
                                fillFlatBottom(px[0],py[0],pxm,py[1],px[1],py[1]);
                                fillFlatTop(pxm,py[1],px[1],py[1],px[2],py[2]);
                                //moveTo(pxm,py[1]);
                                //lineTo(px[1],py[1]);
                                //lineTo(px[2],py[2]);
                                //lineTo(pxm,py[1]);
                            }
                        }
                    }
                }
            }
        }

        if (doWireframe==true)
        {
            penColor=0xFF000000;
            //Faces
            for (int i=0;i<faceA.size();i++) //faceA.size();i++)
            {
               //960-970 FPS
               if (faceFront[i]!=0)
               {
                   px1=bufferCenterX+vertexX[faceA[i]];//+vertexX.at(faceA.at(i));
                   py1=bufferCenterY+vertexY[faceA[i]];//+vertexY.at(faceA.at(i));
                   px2=bufferCenterX+vertexX[faceB[i]];//.at(faceB.at(i));
                   py2=bufferCenterY+vertexY[faceB[i]];//+vertexY.at(faceB.at(i));
                   px3=bufferCenterX+vertexX[faceC[i]];//+vertexX.at(faceC.at(i));
                   py3=bufferCenterY+vertexY[faceC[i]];//+vertexY.at(faceC.at(i));

                   moveTo(px1,py1);
                   lineTo(px2,py2);
                   lineTo(px3,py3);
                   lineTo(px1,py1);
               }
            }
        }

        if (doVertices==true)
        {
            //Vertices
            penColor=0xFF004080;
            for (int i=0;i<vertexX.size();i++)
            {
               index=(bufferCenterY+(int)vertexY.at(i))*frameBufferScanLine+(bufferCenterX+(int)vertexX.at(i))*frameBufferOffset;

               if (i>=after) penColor=0xFFFF8020;

               frameBuffer2[index]=penColor;
               frameBuffer2[index+1]=penColor;
               frameBuffer2[index-1]=penColor;
               frameBuffer2[index-frameBufferScanLine]=penColor;
               frameBuffer2[index+frameBufferScanLine]=penColor;

            }
        }




        //tempBuffer=frontBuffer;
        //frontBuffer=backBuffer;
        //backBuffer=tempBuffer;

/*
            if (frontBuffer==&frameBuffer2)
            {
                frontBuffer=&frameBuffer2;
                backBuffer=&frameBuffer1;
            }
            else
            {
                frontBuffer=&frameBuffer1;
                backBuffer=&frameBuffer2;
            }


            if ((*backBuffer)==&frameBuffer1[0])
            {
                backBuffer=&frameBuffer2;
                frontBuffer=&frameBuffer1;
            }
            else
            {
                backBuffer=&frameBuffer1;
                frontBuffer=&frameBuffer2;
            }
*/

        //memcpy((uchar*)*frameBuffer1,(uchar*)*frameBuffer2,frameBufferSize*4);
        //memset(&(backBuffer)[0], 255, frameBufferSize*4); //307200
        memcpy((unsigned long*)&frameBuffer1[0], (unsigned long*)&frameBuffer2[0], frameBufferSize*4); //307200

        //sleep(10);

    }

    /*
    //New Frame Ready
    //if ((*backBuffer)==&frameBuffer1[0])
    {
        *backBuffer=*frameBuffer2;
        *frontBuffer=*frameBuffer2;
    }
    //else
    {
        backBuffer=&frameBuffer1;
        frontBuffer=&frameBuffer2;
    }
    */



}



RenderThread renderThread1;




void sleep(int ms)
{
    QEventLoop loop;
    QTimer::singleShot(ms, &loop, SLOT(quit()) );
    loop.exec();
}

void MainWindow::on_actionOpen_triggered()
{

    colladaFileName = QFileDialog::getOpenFileName(this, "Open Collada File...","E:/COLLADA/Models/*.dae","DAE Files (*.dae)");

    QWidget* openColladaWindow = new QWidget;

    int width=400;
    int height=200;

    openColladaWindow->setGeometry(QRect(this->x()+this->width()/2-width/2,this->y()+this->height()/2-height/2,width,height));
    openColladaWindow->setWindowTitle("Opening COLLADA file...");

    QLabel* openColladaLabel = new QLabel;
    openColladaLabel->setParent(openColladaWindow);
    openColladaLabel->setGeometry(10,10,200,30);
    openColladaLabel->setText("");
    openColladaLabel->show();




    QProgressBar* openColladaProgress = new QProgressBar;
    openColladaProgress->setParent(openColladaWindow);
    openColladaProgress->setGeometry(10,40,200,30);
    openColladaProgress->setMinimum(0);
    openColladaProgress->setMaximum(100);
    openColladaProgress->setValue(50);
    openColladaProgress->show();

    openColladaWindow->show();

    //Reading file
    openColladaLabel->setText("Opening file...");
    openColladaProgress->setValue(0);

    DOMObject* colladaXML = new DOMObject;

    //colladaXML->loadXML("<a><a1>1</a1><a2 attribute=\"value\" attribute2=\"value2\">2</a2></a>");
    //colladaXML->loadXML("<note><to>Tove</to><from>Jani</from><heading>Reminder</heading><MAGIC/><body>Don't forget me this weekend!</body></note><note2><to2></to2></note2>");
    //colladaXML->loadXML("<CATALOG><CD><TITLE>Empire Burlesque</TITLE><ARTIST>Bob Dylan</ARTIST><COUNTRY>USA</COUNTRY><COMPANY>Columbia</COMPANY><PRICE>10.90</PRICE><YEAR>1985</YEAR></CD><CD><TITLE>Hide your heart</TITLE><ARTIST>Bonnie Tyler</ARTIST><COUNTRY>UK</COUNTRY><COMPANY>CBS Records</COMPANY><PRICE>9.90</PRICE><YEAR>1988</YEAR></CD><CD><TITLE>Greatest Hits</TITLE><ARTIST>Dolly Parton</ARTIST><COUNTRY>USA</COUNTRY><COMPANY>RCA</COMPANY><PRICE>9.90</PRICE><YEAR>1982</YEAR></CD><CD><TITLE>Still got the blues</TITLE><ARTIST>Gary Moore</ARTIST><COUNTRY>UK</COUNTRY><COMPANY>Virgin records</COMPANY><PRICE>10.20</PRICE><YEAR>1990</YEAR></CD><CD><TITLE>Eros</TITLE><ARTIST>Eros Ramazzotti</ARTIST><COUNTRY>EU</COUNTRY><COMPANY>BMG</COMPANY><PRICE>9.90</PRICE><YEAR>1997</YEAR></CD><CD><TITLE>One night only</TITLE><ARTIST>Bee Gees</ARTIST><COUNTRY>UK</COUNTRY><COMPANY>Polydor</COMPANY><PRICE>10.90</PRICE><YEAR>1998</YEAR></CD><CD><TITLE>Sylvias Mother</TITLE><ARTIST>Dr.Hook</ARTIST><COUNTRY>UK</COUNTRY><COMPANY>CBS</COMPANY><PRICE>8.10</PRICE><YEAR>1973</YEAR></CD><CD><TITLE>Maggie May</TITLE><ARTIST>Rod Stewart</ARTIST><COUNTRY>UK</COUNTRY><COMPANY>Pickwick</COMPANY><PRICE>8.50</PRICE><YEAR>1990</YEAR></CD><CD><TITLE>Romanza</TITLE><ARTIST>Andrea Bocelli</ARTIST><COUNTRY>EU</COUNTRY><COMPANY>Polydor</COMPANY><PRICE>10.80</PRICE><YEAR>1996</YEAR></CD><CD><TITLE>When a man loves a woman</TITLE><ARTIST>Percy Sledge</ARTIST><COUNTRY>USA</COUNTRY><COMPANY>Atlantic</COMPANY><PRICE>8.70</PRICE><YEAR>1987</YEAR></CD><CD><TITLE>Black angel</TITLE><ARTIST>Savage Rose</ARTIST><COUNTRY>EU</COUNTRY><COMPANY>Mega</COMPANY><PRICE>10.90</PRICE><YEAR>1995</YEAR></CD><CD><TITLE>1999 Grammy Nominees</TITLE><ARTIST>Many</ARTIST><COUNTRY>USA</COUNTRY><COMPANY>Grammy</COMPANY><PRICE>10.20</PRICE><YEAR>1999</YEAR></CD><CD><TITLE>For the good times</TITLE><ARTIST>Kenny Rogers</ARTIST><COUNTRY>UK</COUNTRY><COMPANY>Mucik Master</COMPANY><PRICE>8.70</PRICE><YEAR>1995</YEAR></CD><CD><TITLE>Big Willie style</TITLE><ARTIST>Will Smith</ARTIST><COUNTRY>USA</COUNTRY><COMPANY>Columbia</COMPANY><PRICE>9.90</PRICE><YEAR>1997</YEAR></CD><CD><TITLE>Tupelo Honey</TITLE><ARTIST>Van Morrison</ARTIST><COUNTRY>UK</COUNTRY><COMPANY>Polydor</COMPANY><PRICE>8.20</PRICE><YEAR>1971</YEAR></CD><CD><TITLE>Soulsville</TITLE><ARTIST>Jorn Hoel</ARTIST><COUNTRY>Norway</COUNTRY><COMPANY>WEA</COMPANY><PRICE>7.90</PRICE><YEAR>1996</YEAR></CD><CD><TITLE>The very best of</TITLE><ARTIST>Cat Stevens</ARTIST><COUNTRY>UK</COUNTRY><COMPANY>Island</COMPANY><PRICE>8.90</PRICE><YEAR>1990</YEAR></CD><CD><TITLE>Stop</TITLE><ARTIST>Sam Brown</ARTIST><COUNTRY>UK</COUNTRY><COMPANY>A and M</COMPANY><PRICE>8.90</PRICE><YEAR>1988</YEAR></CD><CD><TITLE>Bridge of Spies</TITLE><ARTIST>T'Pau</ARTIST><COUNTRY>UK</COUNTRY><COMPANY>Siren</COMPANY><PRICE>7.90</PRICE><YEAR>1987</YEAR></CD><CD><TITLE>Private Dancer</TITLE><ARTIST>Tina Turner</ARTIST><COUNTRY>UK</COUNTRY><COMPANY>Capitol</COMPANY><PRICE>8.90</PRICE><YEAR>1983</YEAR></CD><CD><TITLE>Midt om natten</TITLE><ARTIST>Kim Larsen</ARTIST><COUNTRY>EU</COUNTRY><COMPANY>Medley</COMPANY><PRICE>7.80</PRICE><YEAR>1983</YEAR></CD><CD><TITLE>Pavarotti Gala Concert</TITLE><ARTIST>Luciano Pavarotti</ARTIST><COUNTRY>UK</COUNTRY><COMPANY>DECCA</COMPANY><PRICE>9.90</PRICE><YEAR>1991</YEAR></CD><CD><TITLE>The dock of the bay</TITLE><ARTIST>Otis Redding</ARTIST><COUNTRY>USA</COUNTRY><COMPANY>Atlantic</COMPANY><PRICE>7.90</PRICE><YEAR>1987</YEAR></CD><CD><TITLE>Picture book</TITLE><ARTIST>Simply Red</ARTIST><COUNTRY>EU</COUNTRY><COMPANY>Elektra</COMPANY><PRICE>7.20</PRICE><YEAR>1985</YEAR></CD><CD><TITLE>Red</TITLE><ARTIST>The Communards</ARTIST><COUNTRY>UK</COUNTRY><COMPANY>London</COMPANY><PRICE>7.80</PRICE><YEAR>1987</YEAR></CD><CD><TITLE>Unchain my heart</TITLE><ARTIST>Joe Cocker</ARTIST><COUNTRY>USA</COUNTRY><COMPANY>EMI</COMPANY><PRICE>8.20</PRICE><YEAR>1987</YEAR></CD></CATALOG>");
    colladaXML->loadXMLFromFile(colladaFileName);


    openColladaProgress->setValue(25);


    //Reading Values off the Collada file

    printf("\r");
    printf("\r");
    printf("\r");

    node* currentNode = colladaXML->root;

    //CHECKING COLLADA DOCUMENT

    currentNode=colladaXML->getElementByPath("?xml/COLLADA");
    if (currentNode==NULL)
    {
        printf("Invalid COLLADA document.");
        printf("\r");
        return;
    }
    if (colladaXML->getElementName(currentNode)!="COLLADA")
    {
        printf("Invalid COLLADA document.");
        printf("\r");
        return;
    }
    printf("Valid COLLADA document.");
    printf("\r");

    Collada collada;

    openColladaLabel->setText("Reading geometries...");

    //CREATING SCENE

    //CHECKING LIBRARY GEOMETRIES ELEMENT
    currentNode=colladaXML->getElementByPath("?xml/COLLADA/library_geometries");
    if (checkElementByName(currentNode,"library_geometries")!=true) return;

    GeometryLibrary geometryLibrary;

    //currentNode=colladaXML->getFirstChildElement(currentNode);
    currentNode=colladaXML->getFirstChildElementByName(currentNode,"geometry");

    while (currentNode!=NULL)
    {

        if (checkElementByName(currentNode,"geometry")!=true) return;

        //CHECKING GEOMETRY ELEMENT(S)

        Geometry geometry;

        node* savedCurrentNode = currentNode;

        //ATTRIBUTE ID - OPTIONAL
        if (colladaXML->getAttributeByName(currentNode,"id")!=NULL)
        {
            geometry.id=colladaXML->getAttributeValue(colladaXML->getAttributeByName(currentNode,"id"));
        }
        //ATTRIBUTE NAME - OPTIONAL
        if (colladaXML->getAttributeByName(currentNode,"name")!=NULL)
        {
            geometry.name=colladaXML->getAttributeValue(colladaXML->getAttributeByName(currentNode,"name"));
        }

        //CHECKING MESH ELEMENT
        currentNode = colladaXML->getFirstChildElementByName(currentNode,"mesh");
        if (checkElementByName(currentNode,"mesh")!=true) return;

        Mesh mesh;

        node* savedCurrentNode2 = currentNode;

        //SOURCE
        currentNode = colladaXML->getFirstChildElementByName(currentNode,"source");
        while (currentNode!=NULL)
        {
            //SOURCE ELEMENT
            if (checkElementByName(currentNode,"source")!=true) return;

            Source source;

            //ATTRIBUTE ID - REQUIRED
            if (colladaXML->getAttributeByName(currentNode,"id")==NULL) return;
            source.id=colladaXML->getAttributeValue(colladaXML->getAttributeByName(currentNode,"id"));

            //ATTRIBUTE NAME - OPTIONAL
            if (colladaXML->getAttributeByName(currentNode,"name")!=NULL)
            {
                source.name=colladaXML->getAttributeValue(colladaXML->getAttributeByName(currentNode,"name"));
            }

            //CHECKING FLOAT_ARRAY ELEMENT
            currentNode = colladaXML->getFirstChildElementByName(currentNode,"float_array");
            if (checkElementByName(currentNode,"float_array")!=true) return;

            Float_Array floatarray;

            //ATTRIBUTE COUNT - REQUIRED
            if (colladaXML->getAttributeByName(currentNode,"count")==NULL) return;
            floatarray.count=colladaXML->getAttributeValue(colladaXML->getAttributeByName(currentNode,"count")).toInt();

            //ATTRIBUTE ID - OPTIONAL
            if (colladaXML->getAttributeByName(currentNode,"id")!=NULL)
            {
                floatarray.id=colladaXML->getAttributeValue(colladaXML->getAttributeByName(currentNode,"id"));
            }

            node* currentText=colladaXML->getFirstText(currentNode);
            QString values;
            QStringList splitValues;

            values=colladaXML->getTextText(currentText);
            if (values.at(0)==' ') values=values.mid(1,values.length()-1);
            if (values.at(values.length()-1)==' ') values=values.mid(0,values.length()-1);
            splitValues=values.split(" ");

            for (int i=0;i<floatarray.count;i++) //newGeometry->noOfVertices
            //for (int i=0;i<splitValues.size();i++) //newGeometry->noOfVertices
            {
                floatarray.floats.append(splitValues.at(i).toFloat());
            }

            source.floatArray=floatarray;
            currentNode = colladaXML->getParentElement(currentNode);

            mesh.sources.append(source);
            currentNode=colladaXML->getNextSiblingElementByName(currentNode,"source");
        }

        currentNode=savedCurrentNode2;
        //VERTICES
        currentNode = colladaXML->getFirstChildElementByName(currentNode,"vertices");
        //while (currentNode!=NULL)
        if (currentNode!=NULL)
        {
            //VERTICES ELEMENT
            if (checkElementByName(currentNode,"vertices")!=true) return;

            Vertices vertices;

            //ATTRIBUTE ID - REQUIRED
            if (colladaXML->getAttributeByName(currentNode,"id")==NULL) return;
            vertices.id=colladaXML->getAttributeValue(colladaXML->getAttributeByName(currentNode,"id"));

            //ATTRIBUTE NAME - OPTIONAL
            if (colladaXML->getAttributeByName(currentNode,"name")!=NULL)
            {
                vertices.name=colladaXML->getAttributeValue(colladaXML->getAttributeByName(currentNode,"name"));
            }

            //node* savedCurrentNode3 = currentNode;

            //CHECKING INPUT ELEMENT
            currentNode = colladaXML->getFirstChildElementByName(currentNode,"input");
            while (currentNode!=NULL)
            {
                //INPUT ELEMENT
                if (checkElementByName(currentNode,"input")!=true) return;

                Input input;

                //ATTRIBUTE SEMANTIC - REQUIRED
                if (colladaXML->getAttributeByName(currentNode,"semantic")==NULL) return;
                input.semantic=colladaXML->getAttributeValue(colladaXML->getAttributeByName(currentNode,"semantic"));

                //ATTRIBUTE SOURCE - REQUIRED
                if (colladaXML->getAttributeByName(currentNode,"source")==NULL) return;
                input.source=colladaXML->getAttributeValue(colladaXML->getAttributeByName(currentNode,"source"));

                vertices.inputs.append(input);
                currentNode=colladaXML->getNextSiblingElementByName(currentNode,"input");
            }

            mesh.vertices=vertices;
            //currentNode = colladaXML->getParentElement(currentNode);
            //currentNode=savedCurrentNode3;

        }

        currentNode=savedCurrentNode2;
        //TRIANGLES
        currentNode = colladaXML->getFirstChildElementByName(currentNode,"triangles");
        //while (currentNode!=NULL)
        if (currentNode!=NULL)
        {
            //TRIANGLES ELEMENT
            if (checkElementByName(currentNode,"triangles")!=true) return;

            Triangles triangles;

            //ATTRIBUTE COUNT - REQUIRED
            if (colladaXML->getAttributeByName(currentNode,"count")==NULL) return;
            triangles.count=colladaXML->getAttributeValue(colladaXML->getAttributeByName(currentNode,"count")).toInt();

            //ATTRIBUTE NAME - OPTIONAL
            if (colladaXML->getAttributeByName(currentNode,"name")!=NULL)
            {
                triangles.name=colladaXML->getAttributeValue(colladaXML->getAttributeByName(currentNode,"name"));
            }

            //ATTRIBUTE MATERIAL - OPTIONAL
            if (colladaXML->getAttributeByName(currentNode,"material")!=NULL)
            {
                triangles.material=colladaXML->getAttributeValue(colladaXML->getAttributeByName(currentNode,"material"));
            }

            //CHECKING INPUT ELEMENT
            node* savedCurrentNode3=currentNode;
            currentNode = colladaXML->getFirstChildElementByName(currentNode,"input");
            while (currentNode!=NULL)
            {
                //INPUT ELEMENT
                if (checkElementByName(currentNode,"input")!=true) return;

                Input input;

                //ATTRIBUTE SEMANTIC - REQUIRED
                if (colladaXML->getAttributeByName(currentNode,"semantic")==NULL) return;
                input.semantic=colladaXML->getAttributeValue(colladaXML->getAttributeByName(currentNode,"semantic"));

                //ATTRIBUTE SOURCE - REQUIRED
                if (colladaXML->getAttributeByName(currentNode,"source")==NULL) return;
                input.source=colladaXML->getAttributeValue(colladaXML->getAttributeByName(currentNode,"source"));

                //ATTRIBUTE OFFSET - REQUIRED
                if (colladaXML->getAttributeByName(currentNode,"offset")==NULL) return;
                input.offset=colladaXML->getAttributeValue(colladaXML->getAttributeByName(currentNode,"offset"));

                triangles.inputs.append(input);
                currentNode=colladaXML->getNextSiblingElementByName(currentNode,"input");
            }

            currentNode=savedCurrentNode3;
            currentNode = colladaXML->getFirstChildElementByName(currentNode,"p");
            if (currentNode!=NULL)
            {
                //P ELEMENT
                if (checkElementByName(currentNode,"p")!=true) return;

                P p;

                node* currentText=colladaXML->getFirstText(currentNode);
                QString values;
                QStringList splitValues;

                values=colladaXML->getTextText(currentText);
                if (values.at(0)==' ') values=values.mid(1,values.length()-1);
                if (values.at(values.length()-1)==' ') values=values.mid(0,values.length()-1);
                splitValues=values.split(" ");

                p.count=splitValues.size();

                //for (int i=0;i<triangles.count;i++) //newGeometry->noOfVertices
                for (int i=0;i<splitValues.size();i++) //newGeometry->noOfVertices
                {
                    p.indices.append(splitValues.at(i).toInt());
                }

                triangles.p=p;

                //currentNode = colladaXML->getParentElement(currentNode);
            }



            mesh.triangles.append(triangles);
        }







        //FINALIZING MESH AND GEOMETRIES

        geometry.mesh=mesh;
        geometryLibrary.geometries.append(geometry);

        currentNode=savedCurrentNode;
        //currentNode=colladaXML->getParentElement(currentNode); //mesh
        //currentNode=colladaXML->getParentElement(currentNode); //geometry


        //currentNode=colladaXML->getNextSiblingElement(currentNode); //next geometry
        currentNode=colladaXML->getNextSiblingElementByName(currentNode,"geometry"); //next geometry






    }

    collada.geometryLibrary=geometryLibrary;








    printf("\r");

    //printf(collada.geometryLibrary.macska.toUtf8().constData());

    printf(collada.geometryLibrary.geometries.at(0).id.toUtf8().constData());
    printf("\r");
    printf(collada.geometryLibrary.geometries.at(0).mesh.sources.at(0).id.toUtf8().constData());
    printf("\r");
    printf(collada.geometryLibrary.geometries.at(0).mesh.sources.at(0).floatArray.id.toUtf8().constData());
    printf("\r");
    printf(QString::number(collada.geometryLibrary.geometries.at(0).mesh.sources.at(0).floatArray.count).toUtf8().constData());
    printf("\r");
    printf(QString::number(collada.geometryLibrary.geometries.at(0).mesh.sources.at(0).floatArray.floats.at(0)).toUtf8().constData());
    printf("\r");



    int j;
    //int i=0;
    for (int i=0;i<collada.geometryLibrary.geometries.size();i++)

    {

        j=0;

        while (collada.geometryLibrary.geometries.at(i).mesh.vertices.inputs.at(j).semantic!="POSITION")
        {
            QString vertexID=collada.geometryLibrary.geometries.at(i).mesh.vertices.inputs.at(j).semantic;
            printf(vertexID.toUtf8().constData());
            printf("\r");
            j++;
        }

        QString vertexID=collada.geometryLibrary.geometries.at(i).mesh.vertices.inputs.at(j).source;
        vertexID=vertexID.mid(1,vertexID.length()-1);
        printf(vertexID.toUtf8().constData());
        printf("\r");


        j=0;
        while (collada.geometryLibrary.geometries.at(i).mesh.sources.at(j).id!=vertexID)
        {
            j++;
        }

        printf(QString::number(collada.geometryLibrary.geometries.at(i).mesh.sources.at(j).floatArray.count).toUtf8().constData());
        printf("\r");

        for (int a=0; a<collada.geometryLibrary.geometries.at(i).mesh.sources.at(j).floatArray.count/3;a++)
        {
            /*
            VertexData vertexData;
            vertexData.x=collada.geometryLibrary.geometries.at(i).mesh.sources.at(j).floatArray.floats.at(3*a);

            vertexData.y=collada.geometryLibrary.geometries.at(i).mesh.sources.at(j).floatArray.floats.at(a*3+1);
            vertexData.z=collada.geometryLibrary.geometries.at(i).mesh.sources.at(j).floatArray.floats.at(a*3+2);
            vertices.append(vertexData);
            */
/*
            printf(QString::number(a).toUtf8().constData());
            printf("[");
            printf(QString::number(vertexData.x).toUtf8().constData());
            printf(" , ");
            printf(QString::number(vertexData.y).toUtf8().constData());
            printf(" , ");
            printf(QString::number(vertexData.z).toUtf8().constData());
            printf("]");
            printf("\r");
*/


        }
        printf("\r");

    }











    int noOfGeometries=collada.geometryLibrary.geometries.size();
    int currentGeometry=0;
    int verticesSoFar=0;

    vertexX.clear();
    vertexY.clear();
    vertexZ.clear();

    faceA.clear();
    faceB.clear();
    faceC.clear();


//for (currentGeometry=0;currentGeometry<3;currentGeometry++) //collada.geometryLibrary.geometries.size();currentGeometry++)
for (currentGeometry=0;currentGeometry<collada.geometryLibrary.geometries.size();currentGeometry++) //collada.geometryLibrary.geometries.size();currentGeometry++)
{

    //EXTRACT POSITIONS
    //EXTRACT NORMALS
    //EXTRACT TEXCOORDS
    //EXTRACT POSITONTRIANGLES
    //EXTRACT NORMALSTRIANGLES
    //EXTRACT TEXCOORDSTRIANGLES


    //EXTRACT POSITIONS
    QList<float> positionsData;

    QString positionsSource;
    //LOOK UP POSITIONS SOURCE FROM VERTICES

    for (int j=0;j<collada.geometryLibrary.geometries.at(currentGeometry).mesh.vertices.inputs.size();j++)
    {
        if (collada.geometryLibrary.geometries.at(currentGeometry).mesh.vertices.inputs.at(j).semantic=="POSITION")
        {
            positionsSource=collada.geometryLibrary.geometries.at(currentGeometry).mesh.vertices.inputs.at(j).source;
        }
    }

    //IF STILL EMPTY POSITION SOURCE IS UNDER TRIANGLES
    if (positionsSource=="")
    {
        for (int j=0;j<collada.geometryLibrary.geometries.at(currentGeometry).mesh.triangles.at(0).inputs.size();j++)
        {
            if (collada.geometryLibrary.geometries.at(currentGeometry).mesh.triangles.at(0).inputs.at(j).semantic=="POSITION")
            {
                positionsSource=collada.geometryLibrary.geometries.at(currentGeometry).mesh.triangles.at(0).inputs.at(j).source;
            }
        }
    }

    positionsSource=positionsSource.mid(1,positionsSource.length()-1);

    printf("POSITION SOURCE: ");
    printf(positionsSource.toUtf8().constData());
    printf("\r");

    //GET SOURCE DATA

    for (int j=0;j<collada.geometryLibrary.geometries.at(currentGeometry).mesh.sources.size();j++)
    {
        if (collada.geometryLibrary.geometries.at(currentGeometry).mesh.sources.at(j).id==positionsSource)
        {
            positionsData=collada.geometryLibrary.geometries.at(currentGeometry).mesh.sources.at(j).floatArray.floats;
        }
    }

    for (int j=0;j<positionsData.size();j++)
    {
        printf("(");
        printf(QString::number(j).toUtf8().constData());
        printf(") ");
        printf(QString::number(positionsData.at(j)).toUtf8().constData());
        printf(" ");
    }
    printf("\r");


    //EXTRACT FACES
    //QList<short unsigned int> trianglePositionsData;
    QList<unsigned int> trianglePositionsData;
    trianglePositionsData.clear();

    trianglePositionsData=getTrianglePositionsFromTriangleData(collada.geometryLibrary.geometries.at(currentGeometry).mesh,collada.geometryLibrary.geometries.at(currentGeometry).mesh.triangles.at(0));

    for (int j=0;j<trianglePositionsData.size();j++)
    {
        printf("[");
        printf(QString::number(j).toUtf8().constData());
        printf("] ");
        printf(QString::number(trianglePositionsData.at(j)).toUtf8().constData());
        printf(" ");


    }
    printf("\r");


    //CONVERTING TO DIGESTABLE FORMAT



    for (int j=0;j<positionsData.size()/3;j++)
    {
        vertexX.append(positionsData.at(j*3));
        vertexY.append(positionsData.at(j*3+1));
        vertexZ.append(positionsData.at(j*3+2));
    }


    for (int j=0;j<trianglePositionsData.size()/3;j++)
    {
        faceA.append(trianglePositionsData.at(j*3)+verticesSoFar);
        faceB.append(trianglePositionsData.at(j*3+1)+verticesSoFar);
        faceC.append(trianglePositionsData.at(j*3+2)+verticesSoFar);
    }

    verticesSoFar=verticesSoFar+positionsData.size()/3;


}

/*

    //PRINT ALL VERTEXES, FACES
    for (int j=0;j<vertexX.size();j++)
    {
        printf("<");
        printf(QString::number(j).toUtf8().constData());
        printf("> ");
        printf(QString::number(vertexX.at(j)).toUtf8().constData());
        printf(", ");
        printf(QString::number(vertexY.at(j)).toUtf8().constData());
        printf(", ");
        printf(QString::number(vertexZ.at(j)).toUtf8().constData());
        printf("\r");
    }
    for (int j=0;j<faceA.size();j++)
    {
        printf("<<");
        printf(QString::number(j).toUtf8().constData());
        printf(">> ");
        printf(QString::number(faceA.at(j)).toUtf8().constData());
        printf(", ");
        printf(QString::number(faceB.at(j)).toUtf8().constData());
        printf(", ");
        printf(QString::number(faceC.at(j)).toUtf8().constData());
        printf("\r");
    }
*/


//return;




    //currentNode=colladaXML->getRootElement();
    //currentNode=colladaXML->getElementByPath("?xml/COLLADA/asset/library_geometries/geometry/mesh/source/float");
    //currentNode=colladaXML->getFirstChildElement(currentNode);
    //currentNode=colladaXML->getLastChildElement(currentNode);
    //currentNode=colladaXML->getNextSiblingElement(currentNode);
    //currentNode=colladaXML->getPreviousSiblingElement(currentNode);
    //getNumberOfAttributes(element*);
    //getFirstAttribute(element*)
    //getNextAttribute(element*, node* <-attribute);
    //getAttributeName(node*);
    //getAttributeValue(node*);
    //getNumberOfTexts(element*);
    //getFirstText(element*)
    //getNextText(element*, node* <-text);
    //getText(node*);



    openColladaWindow->close();

    int lastBackslashIndex;
    int k=0;
    while (k!=-1)
    {
        k=colladaFileName.indexOf("/",k);
        if (k!=-1)
        {
            lastBackslashIndex=k;
            k++;
        }
    }

    QString onlyFileName=colladaFileName.mid(lastBackslashIndex+1,colladaFileName.length()-lastBackslashIndex);

    this->ui->label_4->setText("Filename : "+onlyFileName);
    this->ui->label_8->setText("Vertices : "+QString::number(vertexX.size()));
    this->ui->label_9->setText("Faces : "+QString::number(faceA.size()));




    //RENDERING

    //ROTATION AND MAGNIFICATION
    //magnify(0.01);
    //magnify(0.2); //TIE
    //magnify(80); //BOX
    //magnify(8); //TEAPOT
    //magnify(5); //TORUS-HI
    //magnify(275); //IRONMAN //275
    //magnify(140); //LARA

    magnify(this->ui->lineEdit_5->text().toFloat());


    rotateX(90);
    rotateY(this->ui->lineEdit->text().toFloat());
    rotateX(this->ui->lineEdit_2->text().toFloat());
    rotateZ(this->ui->lineEdit_3->text().toFloat());


    delta=this->ui->lineEdit_4->text().toFloat();
    deltaInRadians=delta*M_PI/180;
    cosdeltarad=cos(deltaInRadians);
    sindeltarad=sin(deltaInRadians);



    //VARIABLES
    bufferCenterX=this->ui->label->width()/2;
    bufferCenterY=this->ui->label->height()/2;//+500;//+500;//+240;

    int imwidth=ui->label->width();
    int imheight=ui->label->height();

    //myImage=QImage((uchar*)frameBuffer2, imwidth, imheight, QImage::Format_RGB32); //<<EZ EDDIG A LEGGYORSABB

    frameBufferSize = imwidth*imheight;//*4;
    frameBufferScanLine = imwidth;//*4;
    frameBufferOffset=1;//4;



    //int (*j)[2] = &g;
    //frontBuffer=&frameBuffer1;
    //backBuffer=&frameBuffer1;

    //renderThread1.render();

    //FPS TIMER
    timer.setInterval(1000);
    connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
    timer.start();
    fps=0;

//    sleep(1000);

/*
    for (int b=0;b<45;b++)
    {
    rotateY();
    }
    */



    //myImage=QImage((uchar*)&backBuffer, imwidth, imheight, QImage::Format_RGB32); //<<EZ EDDIG A LEGGYORSABB
    //myImage=QImage((uchar*)&backBuffer[0], imwidth, imheight, QImage::Format_RGB32); //<<EZ EDDIG A LEGGYORSABB


    myImage=QImage((uchar*)frameBuffer1, imwidth, imheight, QImage::Format_RGB32); //<<EZ EDDIG A LEGGYORSABB

    while (true)
    {
        //950 FPS

/*
        if (fps % 2==0)
        {
            myImage=QImage((uchar*)&backBuffer[0], imwidth, imheight, QImage::Format_RGB32); //<<EZ EDDIG A LEGGYORSABB
        }
        else
        {
            myImage=QImage((uchar*)&frontBuffer[0], imwidth, imheight, QImage::Format_RGB32); //<<EZ EDDIG A LEGGYORSABB
        }
        */


        //myImage=QImage((uchar*)&frontBuffer[0], imwidth, imheight, QImage::Format_RGB32); //<<EZ EDDIG A LEGGYORSABB

        ui->label->setPixmap(QPixmap::fromImage(myImage));


        QApplication::processEvents();
        sleep(40);


    }






    return;

}



void MainWindow::on_actionExit_triggered()
{
    rotate=false;
    this->close();
}

void MainWindow::on_actionClose_triggered()
{

    renderThread1.abortThread();

    rotate=false;
    this->close();
}


void MainWindow::on_checkBox_toggled(bool checked)
{
    if (this->ui->checkBox->checkState()==true) rotate=true;
    else rotate=false;
}

void MainWindow::on_MainWindow_destroyed()
{

}

void MainWindow::on_pushButton_clicked()
{
    rotate=false;

    exit(0);
}



void MainWindow::on_checkBox_clicked()
{
    if (doRotate==true) doRotate=false;
    else doRotate=true;
}

void MainWindow::on_checkBox_3_clicked()
{
    if (doWireframe==true) doWireframe=false;
    else doWireframe=true;
}

void MainWindow::on_checkBox_4_clicked()
{
    if (doTriangleFill==true) doTriangleFill=false;
    else doTriangleFill=true;
}

void MainWindow::on_checkBox_5_clicked()
{
    if (doRemoveBackfacing==true) doRemoveBackfacing=false;
    else doRemoveBackfacing=true;
}

void MainWindow::on_checkBox_6_clicked()
{
    if (doZOrdering==true) doZOrdering=false;
    else doZOrdering=true;
}

void MainWindow::on_checkBox_7_clicked()
{
    if (doShading==true) doShading=false;
    else doShading=true;
}

void MainWindow::on_checkBox_2_clicked()
{
    if (doVertices==true) doVertices=false;
    else doVertices=true;
}


/*
void MainWindow::on_pushButton_2_clicked()
{
    float ftemp;
    QString fvalue;

    float deltaX,deltaY,deltaZ;
    float x1,x2,z1,z2;
    float e,f;
    float fpxm;
    float fpxmin,fpxmax;


    QString saveMe;



    after=vertexX.size();


    for (int i=0;i<faceA.size();i++) //faceA.size();i++)
    {
        fpx[0]=vertexX[faceA[i]];
        fpy[0]=vertexY[faceA[i]];
        fpz[0]=vertexZ[faceA[i]];
        fpx[1]=vertexX[faceB[i]];
        fpy[1]=vertexY[faceB[i]];
        fpz[1]=vertexZ[faceB[i]];
        fpx[2]=vertexX[faceC[i]];
        fpy[2]=vertexY[faceC[i]];
        fpz[2]=vertexZ[faceC[i]];


        for (int a=0;a<2;a++)
        {
            for (int b=0;b<2;b++)
            {
                if (fpy[b+1]<fpy[b])
                {
                    ftemp=fpy[b+1];
                    fpy[b+1]=fpy[b];
                    fpy[b]=ftemp;
                    ftemp=fpx[b+1];
                    fpx[b+1]=fpx[b];
                    fpx[b]=ftemp;
                    ftemp=fpz[b+1];
                    fpz[b+1]=fpz[b];
                    fpz[b]=ftemp;
                }
            }
        }


        //fvalue="(0) "+QString::number(fpx[0])+", "+QString::number(fpy[0])+", "+QString::number(fpz[0]);
        //printf("%s\r",fvalue.toUtf8().constData());

        //fvalue="(1) "+QString::number(fpx[1])+", "+QString::number(fpy[1])+", "+QString::number(fpz[1]);
        //printf("%s\r",fvalue.toUtf8().constData());

        //fvalue="(2) "+QString::number(fpx[2])+", "+QString::number(fpy[2])+", "+QString::number(fpz[2]);
        //printf("%s\r",fvalue.toUtf8().constData());



        //Dx = (fpx[0]+fpx[1]+fpx[2])/3;
        //Dy = (fpy[0]+fpy[1]+fpy[2])/3;
        //Dz = fpz[0];





        //ITT KEZDODIK:
        float fpxm = fpx[0] + (fpx[2] - fpx[0]) / fabs(fpy[2] - fpy[0]) * fabs(fpy[1]-fpy[0]); //fabs
        float fpzm = fpz[0] + (fpz[2] - fpz[0]) / fabs(fpy[2] - fpy[0]) * fabs(fpy[1]-fpy[0]); //fabs



        if (fpx[1]<fpxm)
        {
            ftemp=fpx[1];
            fpx[1]=fpxm;
            fpxm=ftemp;

            //ftemp=fpz[1];
            //fpz[1]=fpzm;
            //fpzm=ftemp;


        }


        //FLAT BOTTOM (fpx[0],fpy[0],fpxm,fpy[1],fpx[1],fpy[1]);


        //float dy = fpy[1] - fpy[0];

        if (fpy[1] - fpy[0]!=0)  //Otherwise it is a line at best not a triangle
        {

            float f=0;
            while (f<(fpy[1] - fpy[0]))
            {

                x2 = fpx[0] + (fpx[1]-fpx[0]) / (fpy[1]-fpy[0]) * f;
                x1 = fpx[0] + (fpxm-fpx[0]) / (fpy[1]-fpy[0]) * f;

                z1 = fpz[0] + (fpz[1]-fpz[0]) / (fpy[1]-fpy[0]) * f;
                z2 = fpz[0] + (fpzm-fpz[0]) / (fpy[1]-fpy[0]) * f;

                //EZ EDDIG NEM VOLT BENT!!!
                //A MASIKNAL IS EZ VOLT A PROBLEMA
                //if (x1>x2)
                //{
                //     ftemp=x1;
                //     x1=x2;
                //     x2=ftemp;
                //
                //     ftemp=z1;
                //     z1=z2;
                //     z2=ftemp;
                //}


                float e=0;
                while (e<=(x2-x1))
                {

                    deltaZ = z1 + (z2-z1)/(x2-x1)*e;

                    //deltaZ=0;

                    vX.append(x1+e); //fpx[0]+e
                    vY.append(fpy[0]+f);
                    vZ.append(deltaZ); //Dz


                    e=e+1;
                }
                f=f+1;

            }
        }

        if (fpy[2] - fpy[1]!=0)  //Otherwise it is a line at best not a triangle
        {

            float f=0;
            while (f<(fpy[2] - fpy[1]))
            {

                x2 = fpx[2] + (fpx[2]-fpx[1]) / (fpy[2]-fpy[1]) * f;
                x1 = fpx[2] + (fpx[2]-fpxm) / (fpy[2]-fpy[1]) * f;

                z1 = fpz[2] + (fpz[2]-fpz[1]) / (fpy[2]-fpy[1]) * f;
                z2 = fpz[2] + (fpz[2]-fpzm) / (fpy[2]-fpy[1]) * f;


                float e=0;
                while (e<=(x2-x1))
                {

                    deltaZ = z1 + (z2-z1)/(x2-x1)*e;

                    //deltaZ=0;

                    vX.append(x1+e); //fpx[0]+e
                    vY.append(fpy[1]+f);
                    vZ.append(deltaZ); //Dz


                    e=e+1;
                }
                f=f+1;

            }
        }


        this->ui->label_12->setText("Current face : "+QString::number(i));
        QApplication::processEvents();
    }

    saveMe="";

    QFile saveFile("F:/PointCloud4.txt");
    if (saveFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        for (int i=0; i<vX.size();i++)
        {
            saveMe=QString::number(vX.at(i))+",";
            saveMe=saveMe+QString::number(vY.at(i))+",";
            saveMe=saveMe+QString::number(vZ.at(i))+",";

            saveFile.write(saveMe.toUtf8().constData());//,result.length());

            //vertexX.append(vX.at(i));
            //vertexY.append(vY.at(i));
            //vertexZ.append(vZ.at(i));

        }
        saveFile.close();
    }


}
*/



void MainWindow::on_pushButton_2_clicked()
{
    float ftemp;
    QString fvalue;

    float deltaX,deltaY,deltaZ;
    float x1,x2,z1,z2;
    float e,f;
    float fpxm;
    float fpxmin,fpxmax;


    QString saveMe;



    after=vertexX.size();


    deltaY = - 600.0;
    while (deltaY<600.0)
    {

        deltaX = - 600.0;
        while (deltaX<600.0)
        {

            for (int i=0;i<faceA.size();i++) //faceA.size();i++)
            {
                fpx[0]=vertexX[faceA[i]];
                fpy[0]=vertexY[faceA[i]];
                fpz[0]=vertexZ[faceA[i]];
                fpx[1]=vertexX[faceB[i]];
                fpy[1]=vertexY[faceB[i]];
                fpz[1]=vertexZ[faceB[i]];
                fpx[2]=vertexX[faceC[i]];
                fpy[2]=vertexY[faceC[i]];
                fpz[2]=vertexZ[faceC[i]];


                for (int a=0;a<2;a++)
                {
                    for (int b=0;b<2;b++)
                    {
                        if (fpy[b+1]<fpy[b])
                        {
                            ftemp=fpy[b+1];
                            fpy[b+1]=fpy[b];
                            fpy[b]=ftemp;
                            ftemp=fpx[b+1];
                            fpx[b+1]=fpx[b];
                            fpx[b]=ftemp;
                            ftemp=fpz[b+1];
                            fpz[b+1]=fpz[b];
                            fpz[b]=ftemp;
                        }
                    }
                }


                //fvalue="(0) "+QString::number(fpx[0])+", "+QString::number(fpy[0])+", "+QString::number(fpz[0]);
                //printf("%s\r",fvalue.toUtf8().constData());

                //fvalue="(1) "+QString::number(fpx[1])+", "+QString::number(fpy[1])+", "+QString::number(fpz[1]);
                //printf("%s\r",fvalue.toUtf8().constData());

                //fvalue="(2) "+QString::number(fpx[2])+", "+QString::number(fpy[2])+", "+QString::number(fpz[2]);
                //printf("%s\r",fvalue.toUtf8().constData());



                //Dx = (fpx[0]+fpx[1]+fpx[2])/3;
                //Dy = (fpy[0]+fpy[1]+fpy[2])/3;
                //Dz = fpz[0];





                //ITT KEZDODIK:
                float fpxm = fpx[0] + (fpx[2] - fpx[0]) / fabs(fpy[2] - fpy[0]) * fabs(fpy[1]-fpy[0]); //fabs
                float fpzm = fpz[0] + (fpz[2] - fpz[0]) / fabs(fpy[2] - fpy[0]) * fabs(fpy[1]-fpy[0]); //fabs



                //if (fpx[1]>fpxm)
                //{
                //    ftemp=fpx[1];
                //    fpx[1]=fpxm;
                //    fpxm=ftemp;
                //}



                fpxmin=fpx[0];
                if (fpxmin>fpx[1]) fpxmin=fpx[1];
                if (fpxmin>fpx[2]) fpxmin=fpx[2];
                fpxmax=fpx[0];
                if (fpxmax<fpx[1]) fpxmax=fpx[1];
                if (fpxmax<fpx[2]) fpxmax=fpx[2];


                //FLAT BOTTOM (fpx[0],fpy[0],fpxm,fpy[1],fpx[1],fpy[1]);

                if ((deltaY>=fpy[0]) && (deltaY<fpy[1]))
                {
                    e = deltaY - fpy[0];

                    if (fpy[1]-fpy[0]!=0)  //Otherwise it is a line at best not a triangle
                    {
                        x1 = fpx[0] + (fpx[1]-fpx[0]) / (fpy[1]-fpy[0]) * e;
                        x2 = fpx[0] + (fpxm-fpx[0]) / (fpy[1]-fpy[0]) * e;

                        z1 = fpz[0] + (fpz[1]-fpz[0]) / (fpy[1]-fpy[0]) * e;
                        z2 = fpz[0] + (fpzm-fpz[0]) / (fpy[1]-fpy[0]) * e;

                        if (x1>x2)
                        {
                            ftemp=x1;
                            x1=x2;
                            x2=ftemp;

                            ftemp=z1;
                            z1=z2;
                            z2=ftemp;
                        }
                        if (x1!=x2)
                        {
                            if ((deltaX>=x1) && (deltaX<x2))
                            {
                                f = deltaX - x1;

                                deltaZ = z1 + (z2-z1)/(x2-x1)*f;

                                vX.append(deltaX);
                                vY.append(deltaY);
                                vZ.append(deltaZ); //Dz
                            }
                        }
                    }
                }

                if ((deltaY>=fpy[1]) && (deltaY<fpy[2]))
                {
                    e = deltaY - fpy[2];

                    if (fpy[2]-fpy[1]!=0)  //Otherwise it is a line at best not a triangle
                    {
                        x1 = fpx[2] + (fpx[2]-fpx[1]) / (fpy[2]-fpy[1]) * e;
                        x2 = fpx[2] + (fpx[2]-fpxm) / (fpy[2]-fpy[1]) * e;

                        z1 = fpz[2] + (fpz[2]-fpz[1]) / (fpy[2]-fpy[1]) * e;
                        z2 = fpz[2] + (fpz[2]-fpzm) / (fpy[2]-fpy[1]) * e;

                        if (x1>x2)
                        {
                            ftemp=x1;
                            x1=x2;
                            x2=ftemp;

                            ftemp=z1;
                            z1=z2;
                            z2=ftemp;


                        }
                        if (x1!=x2)
                        {
                            if ((deltaX>=x1) && (deltaX<x2))
                            {
                                f = deltaX - x1;

                                deltaZ = z1 + (z2-z1)/(x2-x1)*f;

                                vX.append(deltaX);
                                vY.append(deltaY);
                                vZ.append(deltaZ); //Dz
                            }
                        }
                    }
                }
/*
                if ((deltaY>=fpy[1]) && (deltaY<fpy[2]))
                {
                    e = deltaY - fpy[2];

                    if (fpy[1]-fpy[2]!=0)  //Otherwise it is a line at best not a triangle
                    {
                        x1 = fpx[2] + (fpx[1]-fpx[2]) / (fpy[1]-fpy[2]) * e;
                        x2 = fpx[2] + (fpxm-fpx[2]) / (fpy[1]-fpy[2]) * e;

                        z1 = fpz[2] + (fpz[1]-fpz[2]) / (fpy[1]-fpy[2]) * e;
                        z2 = fpz[2] + (fpzm-fpz[2]) / (fpy[1]-fpy[2]) * e;

                        if (x1>x2)
                        {
                            ftemp=x1;
                            x1=x2;
                            x2=ftemp;
                        }
                        if (x1!=x2)
                        {
                            if ((deltaX>=x1) && (deltaX<x2))
                            {
                                f = deltaX - x1;

                                deltaZ = z1 + (z2-z1)/(x2-x1)*f;

                                vX.append(deltaX);
                                vY.append(deltaY);
                                vZ.append(deltaZ); //Dz
                            }
                        }
                    }
                }
*/







/*


                //if ((Dy>=fpy[0]) && (Dy<fpy[1]))
                if ((Dy>=fpy[0]) && (Dy<fpy[1]))
                {
                    //e=Dy-fpy[0];

                    //x1 = fpx[0] + (fpx[1]-fpx [0])/(fpy[1]-fpy[0])*e;
                    //x2 = fpx[0] + (fpx[2]-fpx [0])/(fpy[2]-fpy[0])*e;

                    e = Dy - fpy[0];


                    //EZ NEM A PROBLEMA:
                    //if ((fpz[0]==fpz[1])
                    //|| (fpz[0]==fpz[2])
                    //|| (fpz[1]==fpz[2])) e=0;


                    //EZ SEM A PROBLEMA:
                    //if ((e!=0) && (fpy[1]-fpy[0]!=0) && (fpy[2]-fpy[0]!=0))
                    if (e!=0)
                    {

                    if (fpy[1]-fpy[0]!=0) x1 = fpx[0] + (fpx[1]-fpx[0]) / (fpy[1]-fpy[0]) * e;
                    else x1=fpx[0];
                    if (fpy[2]-fpy[0]!=0) x2 = fpx[0] + (fpx[2]-fpx[0]) / (fpy[2]-fpy[0]) * e;
                    else x2=fpx[0];

                    if (fpy[1]-fpy[0]!=0) z1 = fpz[0] + (fpz[1]-fpz[0]) / (fpy[1]-fpy[0]) * e;
                    else z1=fpz[0];
                    if (fpy[2]-fpy[0]!=0) z2 = fpz[0] + (fpz[2]-fpz[0]) / (fpy[2]-fpy[0]) * e;
                    else z2=fpz[0];

                    if ((Dx>=x1) && (Dx<x2))
                    {

                    f = Dx - x1;


                    //EZ SEM A PROBLEMA:
                    if (x2-x1!=0)
                    {
                        Dz = z1 + (z2-z1)/(x2-x1)*f;
                    }
                    else Dz=z1;

                    Dz=0;

                    vX.append(Dx);
                    vY.append(Dy);
                    vZ.append(Dz); //Dz
                    }
                    }

                }

                else
                if ((Dy>=fpy[1]) && (Dy<fpy[2]))
                {
                    //e=Dy-fpy[0];

                    //x1 = fpx[0] + (fpx[1]-fpx [0])/(fpy[1]-fpy[0])*e;
                    //x2 = fpx[0] + (fpx[2]-fpx [0])/(fpy[2]-fpy[0])*e;

                    e = Dy - fpy[2];

                    if (fpy[1]-fpy[2]!=0) x1 = fpx[2] + (fpx[1]-fpx[2]) / (fpy[1]-fpy[2]) * e;
                    else x1=fpx[2];
                    if (fpy[0]-fpy[2]!=0) x2 = fpx[2] + (fpx[0]-fpx[2]) / (fpy[0]-fpy[2]) * e;
                    else x2=fpx[0];

                    if (fpy[1]-fpy[2]!=0) z1 = fpz[0] + (fpz[1]-fpz[2]) / (fpy[1]-fpy[2]) * e;
                    else z1=fpz[2];
                    if (fpy[0]-fpy[2]!=0) z2 = fpz[2] + (fpz[0]-fpz[2]) / (fpy[0]-fpy[2]) * e;
                    else z2=fpz[2];

                    if ((Dx>=x1) && (Dx<x2))
                    {

                    f = Dx - x1;

                    if (x2-x1!=0) Dz = z1 + (z2-z1)/(x2-x1)*f;
                    else Dz=z1;

                    vX.append(Dx);
                    vY.append(Dy);
                    vZ.append(Dz); //Dz
                    }

                }





                //saveMe=saveMe+QString::number(Dx)+","+QString::number(Dy)+","+QString::number(Dz)+",";




                //Dx = (fpx[0]+fpx[1]+fpx[2])/3;
                //Dy = (fpy[0]+fpy[1]+fpy[2])/3;

                e = Dy - fpy[0];

                if (fpy[1]-fpy[0]!=0) x1 = fpx[0] + (fpx[1]-fpx[0]) / (fpy[1]-fpy[0]) * e;
                else x1=fpx[0];
                if (fpy[2]-fpy[0]!=0) x2 = fpx[0] + (fpx[2]-fpx[0]) / (fpy[2]-fpy[0]) * e;
                else x2=fpx[0];

                if (fpy[1]-fpy[0]!=0) z1 = fpz[0] + (fpz[1]-fpz[0]) / (fpy[1]-fpy[0]) * e;
                else z1=fpz[0];
                if (fpy[2]-fpy[0]!=0) z2 = fpz[0] + (fpz[2]-fpz[0]) / (fpy[2]-fpy[0]) * e;
                else z2=fpz[0];

                f = Dx - x1;

                if (x2-x1!=0) Dz = z1 + (z2-z1)/(x2-x1)*f;
                else Dz=z1;


                //fvalue="x1 = "+QString::number(x1);
                //printf("%s\r",fvalue.toUtf8().constData());
                //fvalue="x2 = "+QString::number(x2);
                //printf("%s\r",fvalue.toUtf8().constData());

                //fvalue="z1 = "+QString::number(z1);
                //printf("%s\r",fvalue.toUtf8().constData());
                //fvalue="z2 = "+QString::number(z2);
                //printf("%s\r",fvalue.toUtf8().constData());

                //fvalue="D "+QString::number(Dx)+", "+QString::number(Dy)+", "+QString::number(Dz);
                //printf("%s\r",fvalue.toUtf8().constData());

                //saveMe=saveMe+QString::number(Dx)+","+QString::number(Dy)+","+QString::number(Dz)+",";


*/



            }
            deltaX=deltaX+0.25;


        }

        deltaY=deltaY+0.25;
        this->ui->label_12->setText("Dy = "+QString::number(deltaY)+"; Dx = "+QString::number(deltaX));
        QApplication::processEvents();

    }


    saveMe="";

    QFile saveFile("F:/PointCloud10.txt");
    if (saveFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        for (int i=0; i<vX.size();i++)
        {
            saveMe=QString::number(vX.at(i))+",";
            saveMe=saveMe+QString::number(vY.at(i))+",";
            saveMe=saveMe+QString::number(vZ.at(i))+",";

            saveFile.write(saveMe.toUtf8().constData());//,result.length());

            vertexX.append(vX.at(i));
            vertexY.append(vY.at(i));
            vertexZ.append(vZ.at(i));

        }
        saveFile.close();
    }
}

