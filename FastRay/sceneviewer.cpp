#include "sceneviewer.h"
#include "qdebug.h"
#include "qmath.h"

SceneViewer::SceneViewer()
{
    cameraRays = QVector<Ray>();
}

void SceneViewer::setCameraSize(const int width, const int height, const QVector3D camCenter, const float step, const float angle, const float perspectiveDistance)
{
    Width = width;
    Height = height;
    Step = step;

    makeRaysCamera(camCenter, angle, perspectiveDistance);
}

void SceneViewer::makeRaysCamera(const QVector3D camCenter, const float angle, const float perspectiveDistance)
{
    cameraRays.clear();
    const QVector3D stepRigth = QVector3D(cos((double)angle),0,-sin((double)angle));
    const QVector3D stepTop = QVector3D(0,-1,0);
    const QVector3D perspective = QVector3D(perspectiveDistance * sin((double)angle), 0, perspectiveDistance * cos((double)angle));

    for (int i = 0; i < Height; i++)
        for (int j = 0; j < Width; j++)
            cameraRays << Ray(camCenter + Step * (stepRigth * (j - Width / 2) + stepTop * ( i - Height / 2)),
                              camCenter + perspective);
}

bool DrawOn2DRay(QPainter *qp, const QVector2D offset, const float scale, const QVector3D from, const QVector3D to, const QColor clr)
{
    //return false;
    qp->setPen(QPen(clr, 1.0));
    if (to.y() == 0 && from.y() == 0){
        qp->drawLine(QPoint((from.x() + offset.x()) * scale,(from.z() + offset.y()) * scale), QPoint((to.x() + offset.x()) * scale,(to.z() + offset.y()) * scale));
        qp->drawEllipse( QPoint((to.x() + offset.x()) * scale,(to.z() + offset.y()) * scale), 2,2);
    }
    return true;
}

Ray Reflect (Ray& currentRay, Ray& normal){
    QVector3D cur = currentRay.ray(), nor = normal.ray();
    QVector3D temp = nor * (cur.dotProduct(cur, nor)) * (-2),
              res = temp + cur;
    res.normalize();
    return Ray(normal.From() /*+ res * 1e-10*/, normal.From() + res * 100);
}

QColor SceneViewer::renderPixel(const Ray ray, QPainter *qp, const QVector2D offset, const float scale, int level) const
{
    QVector<QVector3D> intersectPoints;
    QVector<int> intersectIndexes;

    for (int i = 0; i < objects.length(); i++){
        QVector3D res;
        bool intersected = objects[i]->IntersectWithRay(&ray, res);
        if (intersected)
        {
            intersectPoints << res;
            intersectIndexes << i;
        }
    }

    DrawOn2DRay(qp, offset, scale, ray.From(), ray.To(), Qt::red);

    // SORT ON DIST and decide which one is intersected
    if (intersectIndexes.length() > 0){
        float minDist = (ray.From() - ray.To()).length() + .001;
        int bestInd = -1, bestI = -1;
        for (int i = 0; i < intersectIndexes.length(); i++)
        {
            float dist = intersectPoints[i].distanceToPoint(ray.From());
            if (dist < minDist)
            {bestInd = intersectIndexes[i];bestI = i; minDist = dist;}
        }
        if (bestInd >= 0){
            //qp->setPen(QPen(Qt::red, level* 10 + 1));
            if (level > 0){
                DrawOn2DRay(qp, offset, scale, ray.From(), intersectPoints[bestI], (level)? Qt::red : Qt::green);
            }
            QColor summ = Qt::black;
            for (int i = 0; i < light.length(); i++){
                QVector3D lightBlock; bool isVisible = true;
                // check if light is visible from a contact point

                Ray R = Ray(light[i]->center, intersectPoints[bestI]);
                for (int j = 0; j < objects.length(); j++){
                    bool intersected = objects[j]->IntersectWithRay(&R, lightBlock);
                    //if (intersected && lightBlock.distanceToPoint(R.To()) > .01){isVisible = false; break;}
                    if (intersected && R.From().distanceToPoint(R.To()) - 0.01 > R.From().distanceToPoint((lightBlock)))
                    {isVisible = false; break;}
                }
                //
                if (!isVisible){
                    //DrawOn2DRay(qp, offset, scale, lightBlock, intersectPoints[bestI], Qt::darkRed);
                    continue;
                }

                float distToLight = intersectPoints[bestI].distanceToPoint(light[i]->center);

                float normalKof = objects[bestInd]->GetNormalRay(intersectPoints[bestI])
                        .GetAngleBetween(R) * (-1);
                if (!(normalKof >= 0)) normalKof = 0;

                summ = LightSourse::ColorAdd(summ, light[i]->AddColorOnDist(distToLight), light[i]->maxLight * normalKof);
            }

            /// trace a ray on 2D canvas
            QColor retColor = LightSourse::ColorMask(objects[bestInd]->clr, summ);
            //qp->setPen(QPen(retColor));

            Ray normalRay = objects[bestInd]->GetNormalRay(intersectPoints[bestI]);
            //DrawOn2DRay(qp, offset, scale, normalRay.From(), normalRay.To(), Qt::blue);
            Ray castedRay = Ray(ray.From(), normalRay.From());
            //DrawOn2DRay(qp, offset, scale, reflectedRay.From(), reflectedRay.To(), Qt::red);

            if (objects[bestInd]->mirrority > 0 && level < maxLevel){
                Ray reflectedRay = Reflect(castedRay, normalRay);//.inverse();
                QColor reflectedColor = renderPixel(reflectedRay, qp, offset, scale, level + 1);

                return LightSourse::ColorAdd(retColor, reflectedColor, objects[bestInd]->mirrority * (maxLevel - level + 0.0)/maxLevel);
            }
            return retColor;
        }
    }
    return QColor(5, 5, 200);
}

bool SceneViewer::renderAndDraw(QPainter *qp, const int resol) const
{
    qp->setPen(QPen(QColor(0,0,0))); QVector2D offset = QVector2D(470,220); float scale = 3.0;

    int wid = resol;
    QPen pen = QPen();
    pen.setWidth(wid);
    for (int i = 0; i < Height; i++)
        for (int j = 0; j < Width; j++){
            int rayInd = i + j * Width;
            if (rayInd >= cameraRays.length())
                return false;
            //qDebug() << rayInd << i << j;
            pen.setColor(renderPixel(cameraRays[rayInd], qp, offset, scale, 0));
            qp->setPen(pen);
            qp->drawPoint(20 + i * wid, 20 + j * wid);
        }

    qp->setPen(QPen(Qt::darkGray));
    for (int i = 0; i < objects.length(); i++)
        objects[i]->DrawOn2D(qp, offset, scale);
    for (int i = 0; i < light.length(); i++)
        light[i]->DrawOn2D(qp, offset, scale);

    return true;
}
