#include "dermatops.h"
#include "iostream"



using Eigen::Matrix;
using namespace DerOperations;

using namespace DerivableVectorMatrixes;
using namespace  std;

Matrix<Derivable, 1, 4> DerivableVectorMatrixes::SetDerive4DVector (QVector4D vec) {
    Matrix<Derivable, 1, 4> M = Matrix<Derivable, 1, 4>();
    for (int i = 0; i < 4; i++)
        M(0,i) = Derivable(vec[i]);
    return M;
}
Matrix<Derivable, 1, 4> DerivableVectorMatrixes::SetDerive4DVector (){
    return DerivableVectorMatrixes::SetDerive4DVector(QVector4D(0,0,0,0));
}
Matrix<Derivable, 1, 4> DerivableVectorMatrixes::SetDerive4DVector (float x, float y, float z, float w){
    return SetDerive4DVector(QVector4D(x,y,z,w));
}
Matrix<Derivable, 1, 4> DerivableVectorMatrixes::SetDerive4DVector (QVector3D qv3, float w){
    return SetDerive4DVector(QVector4D(qv3.x(),qv3.y(),qv3.z(),w));
}

void DerivableVectorMatrixes::TraceVector (Matrix<Derivable, 1, 4> Vec){
 QString lin = "Derivable4D (";
 for (int j = 0; j < 4; j++)
     lin += (QString::number( Vec(0, j).getValue()) + " (" + QString::number( Vec(0, j).getProiz()) + ")").rightJustified(19, ' ') + " ";
 qDebug() << lin + ")";
}

void DerivableVectorMatrixes::TraceMatrix (Matrix<Derivable, 4, 4> Mat){
    qDebug() << "Matrix4x4 Derivable (type:Custom";
    for (int i = 0; i < 4; i++){
        QString lin = "";
        for (int j = 0; j < 4; j++)
            lin += (QString::number( Mat(i,j).getValue()) + " (" + QString::number( Mat(i,j).getProiz()) + ")").rightJustified(19, ' ') + " ";
        qDebug() << lin;
    }
    qDebug() << ")";
}

Matrix<Derivable, 4, 4> DerivableVectorMatrixes::SetDeriveMatrix (){
    Matrix<Derivable, 4, 4> M = Matrix<Derivable, 4, 4>();
    for (int i = 0; i < 4; i++)
        M(i,i) = Derivable(1);
    return M;
}

Matrix<Derivable, 4, 4> DerivableVectorMatrixes::SetDeriveMatrix (const QVector<Derivable> values){
    Q_ASSERT(values.length() == 16);
    Matrix<Derivable, 4, 4> M = Matrix<Derivable, 4, 4>();
    M(0,0) = values[0];
    M(0,1) = values[1];
    M(0,2) = values[2];
    M(0,3) = values[3];
    M(1,0) = values[4];
    M(1,1) = values[5];
    M(1,2) = values[6];
    M(1,3) = values[7];
    M(2,0) = values[8];
    M(2,1) = values[9];
    M(2,2) = values[10];
    M(2,3) = values[11];
    M(3,0) = values[12];
    M(3,1) = values[13];
    M(3,2) = values[14];
    M(3,3) = values[15];
    return M;
}

Matrix<Derivable, 4, 4> DerivableVectorMatrixes::SetDeriveMatrix (const QMatrix4x4 original){
    Matrix<Derivable, 4, 4> M = Matrix<Derivable, 4, 4>();
    for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++)M(i,j) = Derivable(original(i,j));
    return M;
}

Matrix<Derivable, 4, 4> DerivableVectorMatrixes::MakeDeriveTranslationMatrix (const Matrix<Derivable,1,3> vec){
    Matrix<Derivable, 4, 4> M = SetDeriveMatrix();
//    M(0,3) = vec.x();
//    M(1,3) = vec.y();
//    M(2,3) = vec.z();
    M(3, 0) = vec.x();
    M(3, 1) = vec.y();
    M(3, 2) = vec.z();
    return M;
}

Matrix<Derivable, 4, 4> DerivableVectorMatrixes::MakeDeriveRotationXMatrix (const Derivable angle0){
    Derivable angle = angle0 / 180.0 * M_PI;
    Matrix<Derivable, 4, 4> M = SetDeriveMatrix();
    M(1,1) = DerOperations::cos(angle); M(1,2) = Derivable(-1)* DerOperations::sin(angle);
    M(2,1) = DerOperations::sin(angle); M(2,2) = DerOperations::cos(angle);
    return M;
}
Matrix<Derivable, 4, 4> DerivableVectorMatrixes::MakeDeriveRotationZMatrix (const Derivable angle0){
    Derivable angle = angle0 / 180.0 * M_PI;
    Matrix<Derivable, 4, 4> M = SetDeriveMatrix();
    M(0,0) = DerOperations::cos(angle); M(0,1) = Derivable(-1)*  DerOperations::sin(angle);
    M(1,0) = DerOperations::sin(angle); M(1,1) = DerOperations::cos(angle);
    return M;
}
Matrix<Derivable, 4, 4> DerivableVectorMatrixes::MakeDeriveRotationYMatrix (const Derivable angle0){
    Derivable angle = angle0 / 180.0 * M_PI;
    Matrix<Derivable, 4, 4> M = SetDeriveMatrix();
    M(0,0) = DerOperations::cos(angle); M(0,2) = DerOperations::sin(angle);
    M(2,0) =  Derivable(-1)*  DerOperations::sin(angle); M(2,2) = DerOperations::cos(angle);
    return M;
}
Matrix<Derivable, 4, 4> DerivableVectorMatrixes::MakeDeriveRotationMatrix (const Matrix<Derivable, 1,3> ang){
    return MakeDeriveRotationXMatrix(ang(0,0)) * MakeDeriveRotationYMatrix(ang(0,1)) * MakeDeriveRotationZMatrix(ang(0,2));
}

void DerivableVectorMatrixes::TranslateDeriveMatrix (Matrix<Derivable, 4, 4>& originalMatrix, const Matrix<Derivable, 1,3> vec){
    originalMatrix = originalMatrix * MakeDeriveTranslationMatrix(vec);
}
void DerivableVectorMatrixes::RotateDeriveMatrix (Matrix<Derivable, 4, 4>& originalMatrix, const Matrix<Derivable, 1,3> rot){
    originalMatrix = originalMatrix * MakeDeriveRotationMatrix(rot);
}
void DerivableVectorMatrixes::TestTrace()
{
    Matrix<Derivable,4,4> m = SetDeriveMatrix();
    Matrix<Derivable,1,4> v = SetDerive4DVector();
//    Matrix<int,1,3> v1(3,4,5);
//    Matrix<int,1,4> v2 (v1(0,0),v1(0,1),v1(0,2),8);
    v = v * m * m;
    //cout << v1 << endl;
    //cout << v2 << endl;
    return;
    //Matrix<Derivable,4,4>();
    //cout << m(0,0).getValue() << m(1,1).getValue()  << m(2,2).getValue() <<m(3,3).getValue() << endl;
//    TraceMatrix(m);
//    Matrix<Derivable,1,3> v = Matrix<Derivable,1,3>(1,2,3);
//    //v << 1,2,3;
//    cout << "wow" << endl;
//    cout << v(0,1).getValue() << endl;
//    return;

//    QMatrix4x4 Qmat = QMatrix4x4(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16), translateMatrix = Qmat, rotateMatrix = Qmat;

//    QVector<Derivable> a = QVector<Derivable>();
//    for (int i = 1; i <= 16; i++)
//        a << Derivable(i);
//    Matrix<Derivable, 4, 4> M = SetDeriveMatrix(Qmat), M2 = M, M3 = M;

//    M(0,0) = 1.0;
//    qDebug() << Qmat; qDebug() << "*"; qDebug() << Qmat; qDebug() << "="; qDebug() << Qmat * Qmat;  qDebug() << " ";


//    TraceMatrix(M);
//    qDebug() << "*";
//    TraceMatrix(M2);
//    qDebug() << "=";
//    TraceMatrix(M * M2);


//    translateMatrix.translate(3,4,5);
//    qDebug() << "";qDebug() << "Translations:";qDebug() << "";
//    qDebug() << translateMatrix;

//    TranslateDeriveMatrix(M2, QVector3D(3,4,5));
//    TraceMatrix(M2);

//    QVector3D rotat = QVector3D(12,22,32);//22,32);
//    qDebug() << "";qDebug() << "Rotations:";qDebug() << "";

//    rotateMatrix.rotate(rotat.x(), 1, 0, 0);
//    rotateMatrix.rotate(rotat.y(), 0, 1, 0);
//    rotateMatrix.rotate(rotat.z(), 0, 0, 1);
//    qDebug() << rotateMatrix;

//    //M3 = MakeDeriveRotationMatrix(rotat);
//    RotateDeriveMatrix(M3, rotat);
//    TraceMatrix(M3);

//    qDebug() << "";qDebug() << "Vector4D:";qDebug() << "";
//    QVector4D q4d= QVector4D(15,16,13,1);
//    qDebug() << q4d;
//    Matrix<Derivable,1,4> d4d = SetDerive4DVector(15,16,13,1);
//    TraceVector(d4d);

//    qDebug() << "Vector4D * Matrix4x4";
//    qDebug() << q4d * rotateMatrix;
//    TraceVector(d4d * M3);



}



Matrix<Derivable, 1, 4> DerivableVectorMatrixes::MakeVector4From3(const Matrix<Derivable, 1, 3> vec, const Derivable add)
{
    return Matrix<Derivable, 1, 4>(vec(0,0), vec(0,1), vec(0,2), add);
}

Matrix<Derivable, 1, 3> DerivableVectorMatrixes::Summ3and4(const Matrix<Derivable, 1, 3> v3, const Matrix<Derivable, 1, 4> v4)
{
    return Matrix<Derivable, 1, 3>(v3(0,0) + v4(0,0), v3(0,1) + v4(0,1), v3(0,2) + v4(0,2));
}

Derivable DerivableVectorMatrixes::SquaredLength(const Matrix<Derivable, 1, 3> vect)
{
    return  DerOperations::pow(vect(0,0),2) + DerOperations::pow(vect(0,1),2) + DerOperations::pow(vect(0,2),2);
}

QVector3D DerivableVectorMatrixes::QfromDer3(const Matrix<Derivable, 1, 3> orig)
{
    return QVector3D(orig(0,0).getValue(), orig(0,1).getValue(), orig(0,2).getValue());
}
