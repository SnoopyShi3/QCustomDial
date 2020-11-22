#include "customdial.h"
#include <QDebug>
#include <QInputEvent>
#include <cmath>
#include <QMessageBox>
#include <QThread>


CustomDial::CustomDial(QWidget *_parent) : QWidget (_parent)
//  m_minBorder(0), m_maxBorder(100), m_singleStep(1),m_currentValue(0),m_defaultValue(0),m_startAngle(40), m_endAngle(320), m_lineAngle(30), m_pressed(false), m_currentAngle(m_startAngle)
{


  setStyle(TYPE_STYLE::Light);
  setFocusPolicy(Qt::StrongFocus);

  //Установка дефолтной размерности
  setGeometry(0,0,200,200);

  setStyleSheet("* {background-color: " + QString(m_colors.BackGroundColor.name()) + "}");
//  setStyleSheet("* {background-color: rgb(222, 222, 222)}");


//  //Настройка центрального Label для вывода значений
//  m_defaultValue = m_minBorder;
//  m_currentValue = m_defaultValue;
//  setm_Amount(6);


//  //Приведение углов к положительному диапазону
//  CorrectNegativeRange();
//  //Расчёт диапазона углов
//  if(fabs(static_cast<double>(m_startAngle-m_endAngle)) < 0.01)
//  {
//    m_startAngle = 0;
//    m_endAngle   = 180;
//  }
//  float range = m_startAngle < m_endAngle ? (m_endAngle-m_startAngle): ((360-m_startAngle)+m_endAngle);

//  //Расчёт диапазона значений
//  //Проверка на заданный диапазон углов
//  m_range = m_maxBorder - m_minBorder;
//  m_rangeAnlge =  static_cast<float>(range);
  setDefaultValues(0, -180, 180, 13, 50, 300, 240);
  //Расчёт начального положения относительно заданного значения
  ValueToAngle();
  //-------------------------------------------------
}

void CustomDial::setDefaultValues(float defaultValue, float minBorder,float MaxBorder,
                                 unsigned posAmount, float sStep,  float minAngle, float maxAngle)
{
  setm_Amount(posAmount);
  setminBorder(minBorder);
  setMaxBorder(MaxBorder);
//  // установка диапазона значений
  setRange(getmaxBorder() - getminBorder());
  setSingleStep(sStep);
  setstartAngle(fmod(minAngle,360));
  setendAngle(fmod(maxAngle,360));
//  //Приведение углов к положительному диапазону
  CorrectNegativeRange();

  setm_rangeAngle(m_startAngle < m_endAngle ? (m_endAngle-m_startAngle): ((360-m_startAngle)+m_endAngle));
  setdefaultValue(defaultValue);

  CustomDial::repaint();
}

void CustomDial::CorrectNegativeRange()
{
  // Кореектируем к нужному диапазону
  if(fmod(m_startAngle,360) < 0)
    m_startAngle += 360;
  if(fmod(m_endAngle,360) < 0)
    m_endAngle += 360;
}

void CustomDial::DrawSwitchIndicator()
{
  auto SwitchIndicatorRadius = (getm_sz()/2 + getm_sz()/3) / 2;
  auto BigTickSize = getm_sz()/20;
//  painter->setPen(QColor(197,201,202));
  painter->setPen(QColor(Light::Gray/*155,154,162*/));
  if(!getm_pressed())
    painter->setBrush(QColor(m_colors.SwithcerCircleleColorOff));
  else
  {
    painter->setBrush(QColor(m_colors.SwithcerCircleleColorOn));
  }

  painter->drawEllipse(QPointF(CenterX-std::cos((static_cast<double>(m_currentAngle))*M_PI/180)*static_cast<double>(SwitchIndicatorRadius),
                               CenterY-std::sin((static_cast<double>(m_currentAngle))*M_PI/180)*static_cast<double>(SwitchIndicatorRadius)),
                              static_cast<double>((getm_sz()/2 - getm_sz()/3-BigTickSize))/ 3,
                              static_cast<double>((getm_sz()/2 - getm_sz()/3-BigTickSize))/ 3);

}

void CustomDial::GradientCircle()
{
  QRadialGradient gradient (QPointF(CenterX, CenterY),getm_sz()/3);
  if(!getm_pressed())
  {
//    painter->setBrush(QColor(62, 70, 73));
    painter->setBrush(QColor(m_colors.CentralCircleColor));
    //painter->setPen(QColor(44, 71, 52));
    painter->setPen(QColor(m_colors.BorderColor));
  }
  else
  {
     painter->setBrush(QColor(m_colors.CentralCircleColor));
//     painter->setPen(QColor(222, 222, 222));
     painter->setPen(QColor(m_colors.BorderColor));
  }
//  painter->setPen(Qt::black);
}

void CustomDial::mouseDoubleClickEvent(QMouseEvent *event)
{
//  Q_UNUSED(event);
  setCurrentValue(m_defaultValue);
  ValueToAngle();
  repaint();

}

void CustomDial::mousePressEvent(QMouseEvent *event)
{
  //Установка флага на нажатие кнопки мыши
  setm_pressed(true);
  CustomDial::repaint();
}

void CustomDial::mouseReleaseEvent(QMouseEvent *event)
{
  Q_UNUSED(event);

  //Установка флага на отжатие кнопки мыши
  setm_pressed(false);
  CustomDial::repaint();
}

void CustomDial::mouseMoveEvent(QMouseEvent *event)
{
  //Если кнопка мыши была нажата, считаем угол отрисовки малого круга
  if(getm_pressed())
  {
    auto SwitchIndicatorRadius = getm_sz()/2;/*(getm_sz()/2 + getm_sz()/3) / 2*/;
    auto diffX = -(event->x()-CenterX);
    auto diffY = -(event->y()-CenterY);

    if(diffX >= -0.01 && diffX <= 0.01)
      diffX = 0.01;
    if(diffY >= -0.01 && diffY <= 0.01)
      diffY = 0.01;

    auto rad = std::atan(diffY/diffX);
    auto deg = rad*180/M_PI;

    // Корректирование углов в диапазоне от [0,360] - GRAD
    if(diffX < 0 && diffY  > 0)
      deg += 180;
    else if(diffX < 0 && diffY < 0)
      deg += 180;
    else if(diffX > 0 && diffY < 0)
      deg += 360;

    // Ограничение по радиус-векутору заданного размера в правой части неравенства
    if(sqrt(pow(diffX,2) + pow(diffY,2)) < SwitchIndicatorRadius)
    {
      if(m_startAngle >= m_endAngle)
      {
        //Расчёт угла относительно текущего значения
        AngleToValue(static_cast<float>(deg),TYPE_CALC::First);
      }
      else{
        //Расчёт угла относительно текущего значения
        AngleToValue(static_cast<float>(deg),TYPE_CALC::Second);
      }
    }
    CustomDial::repaint();
  }
}

void CustomDial::keyPressEvent(QKeyEvent *_event)
{
  //Если нажата стрелка направо
  if(_event->key() == Qt::Key::Key_Right)
  {
    setm_pressed(true);
    m_currentValue += m_singleStep;
    //Функция расчёта нового положения угла относительно измененного значения
    ValueToAngle();
    CustomDial::repaint();
  }
  //Если нажата стрелка налево
  else if (_event->key() == Qt::Key::Key_Left)
  {
    setm_pressed(true);
    m_currentValue -= m_singleStep;
    //Функция расчёта нового положения угла относительно измененного значения
    ValueToAngle();
    CustomDial::repaint();
  }
}

void CustomDial::wheelEvent(QWheelEvent *_event)
{
    setm_pressed(true);
    //Если колесо мыши движется вверх
    if(_event->delta() > 0)
    {
      m_currentValue += m_singleStep;
      ValueToAngle();
      CustomDial::repaint();
    }
    //Если колесо мыши движется вниз
    else
    {
      m_currentValue -= m_singleStep;
      ValueToAngle();
      CustomDial::repaint();
    }
    setm_pressed(false);
    //Ожидаем отрисовку для смены цвета круга
    QThread::msleep(50);
    CustomDial::repaint();
}

void CustomDial::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event);

  QPainter paint(this);
  painter = &paint;

  // Расчёт центра окружности для дальнейших действий в полярных координатах
  painter->setRenderHint(QPainter::Antialiasing, true);

  QColor color(m_colors.MainCircleColor);
  QBrush brush(color,Qt::SolidPattern);
  painter->setPen(color);
  painter->setBrush(brush);

  painter->drawEllipse(QPointF(CenterX, CenterY),static_cast<double>(getm_sz()/2), static_cast<double>(getm_sz()/2));
  auto RadiusBig = getm_sz()/2;
  auto BigTickSize = RadiusBig/10;
  auto LittleTickSize = RadiusBig/20;
  Q_UNUSED(LittleTickSize);

  //Описание параметров квадрата
  QRect rect;        //Квадрат для вывода значений на больших рисках
  float tempWidth;   //Ширина квадрата
  float tempHeight;  //Высота квадрата
  float RectX;       //Координата X
  float RectY;       //Координата Y
  //----------------------------

  //Отрисовка градиента
  GradientCircle();

  painter->drawEllipse(QPointF(CenterX, CenterY),static_cast<double>(getm_sz()/3), static_cast<double>(getm_sz()/3));

  //Проверка на заданный диапазон углов
  if(fabs(static_cast<double>(m_startAngle-m_endAngle)) < 0.01)
  {
    QMessageBox::warning(this,tr("Внимание!!!"),"Неправильно задан диапазон углов\n (по умолчанию выставлено от 0 до 180)");
    m_startAngle = 0;
    m_endAngle   = 180;
    return;
  }

  //Проверка на заданный диапазон значений
  if(m_maxBorder < m_minBorder)
  {
    QMessageBox::warning(this,tr("Внимание!!!"),"Неправильно задан диапазон значений\n (по умолчанию выставлено от 0 до 9)");
    m_minBorder = 0;
    m_maxBorder = 9;
  }

  //Расчёт кол-ва жирных рисок
//  float range = m_startAngle < m_endAngle ? (m_endAngle-m_startAngle): ((360-m_startAngle)+m_endAngle);
//  m_range = m_maxBorder - m_minBorder;
//  m_rangeAnlge =  static_cast<float>(range);

  float tempAngle = getm_rangeAngle()/getm_Amount();
  m_lineAngle = tempAngle;

  QVector<float> posAngles;
  posAngles.push_back(getstartAngle());
  float temp = getm_rangeAngle()/(getm_Amount()-1);

  //Если задана не единственная позиция на компоненте
  if(getm_Amount()-1  != 1)
  {
      for(int i = 1; i < getm_Amount()-1;i++)
      {
        posAngles.push_back(getstartAngle()+i*temp);
      }
      posAngles.push_back(getendAngle());
  }

  painter->setPen(QColor(m_colors.TextColor));
  painter->setBrush(Qt::transparent);

  //Расчёт шага по большой риске
  auto TextStep =(m_maxBorder - m_minBorder)/(getm_Amount()-1);
  //Создание метрики для дальнейшего расчёта ширины текста
  QFont font("Curier",static_cast<int>(BigTickSize*1.25f),20);

  QFontMetrics metrics(font);
  //Установка текущего шрифта для значений (painter)
  painter->setFont(font);
  //Установка текущего шрифта для вывода значений
 // painter->drawText(QPointF(CenterX,CenterY),QString::number(getcurrentValue()));

  for(int i = 0; i < posAngles.size(); i++)
  {
    painter->drawLine(QPointF(CenterX-std::cos(posAngles[i]*M_PI/180)*getm_sz()/2, CenterY-std::sin(posAngles[i]*M_PI/180)*getm_sz()/2),
                      QPointF(CenterX-std::cos(posAngles[i]*M_PI/180)*(getm_sz()/2-BigTickSize), CenterY-std::sin(posAngles[i]*M_PI/180)*(getm_sz()/2-BigTickSize)));
    tempWidth = metrics.width(QString::number(static_cast<int>(this->m_minBorder+i*TextStep)));//BigTickSize*3;
    tempHeight = BigTickSize*3;

    // temp Values
    int PositionNumber = static_cast<int>(this->m_minBorder+i*TextStep);
    int Alignment;

    //Отрисовка текста в диапазоне от [0,80) - GRAD
    if(fmod(posAngles[i],360) >= 0 && fmod(posAngles[i], 360) < 80)
    {
      //Установка параметров квадрата для текста
      RectX      = CenterX-std::cos(posAngles[i]*M_PI/180)*getm_sz()/1.9 - tempWidth;
      RectY      = CenterY-std::sin(posAngles[i]*M_PI/180)*getm_sz()/1.9 - tempHeight/2;
      Alignment = Qt::AlignRight;

    }
    //Отрисовка текста в диапазоне от [80,110) - GRAD
    else if(fmod(posAngles[i],360) >= 80 && fmod(posAngles[i], 360) < 100)
    {
      //Установка параметров квадрата для текста
      RectX      = CenterX-std::cos(posAngles[i]*M_PI/180)*getm_sz()/1.9 - tempWidth/2;
      RectY      = CenterY-std::sin(posAngles[i]*M_PI/180)*getm_sz()/1.9 - tempHeight/1.5;
      Alignment = Qt::AlignCenter;
    }
    //Отрисовка текста в диапазоне от [110,180) - GRAD
    else if(fmod(posAngles[i],360) >= 100 && fmod(posAngles[i], 360) < 180)
    {
      //Установка параметров квадрата для текста
      RectX      = CenterX-std::cos(posAngles[i]*M_PI/180)*getm_sz()/1.9;
      RectY      = CenterY-std::sin(posAngles[i]*M_PI/180)*getm_sz()/1.9 - tempHeight/2;
      Alignment = Qt::AlignLeft;
    }
    //Отрисовка текста в диапазоне от [180,260) - GRAD
    else if(fmod(posAngles[i],360) >= 180 && fmod(posAngles[i], 360) < 260)
    {
      //Установка параметров квадрата для текста
      RectX      = CenterX-std::cos(posAngles[i]*M_PI/180)*getm_sz()/1.9/* + tempWidth;*/;
      RectY      = CenterY-std::sin(posAngles[i]*M_PI/180)*getm_sz()/1.9 - tempHeight/2.2;
      Alignment = Qt::AlignLeft;
    }
    //Отрисовка текста в диапазоне от [260,280) - GRAD
    else if(fmod(posAngles[i],360) >= 260 && fmod(posAngles[i], 360) < 280)
    {
      //Установка параметров квадрата для текста
      RectX      = CenterX-std::cos(posAngles[i]*M_PI/180)*getm_sz()/1.9 - tempWidth/2;
      RectY      = CenterY-std::sin(posAngles[i]*M_PI/180)*getm_sz()/1.9 - tempHeight/2;
      Alignment = Qt::AlignCenter;
    }
    //Отрисовка текста в диапазоне от [270,360) - GRAD
    else if(fmod(posAngles[i],360) >= 280 && fmod(posAngles[i], 360) < 360)
    {
      //Установка параметров квадрата для текста
      RectX      = CenterX-std::cos(posAngles[i]*M_PI/180)*getm_sz()/1.9 - tempWidth;
      RectY      = CenterY-std::sin(posAngles[i]*M_PI/180)*getm_sz()/1.9 - tempHeight/2.2;
      Alignment = Qt::AlignRight;
    }

    rect.setRect(RectX,RectY, tempWidth, tempHeight);
    painter->setPen(Qt::transparent);
    painter->drawRect(rect);
    painter->setPen(QColor(m_colors.TextColor));

    painter->drawText(rect,Alignment,QString::number(PositionNumber));
  }
  DrawSwitchIndicator();
//  painter->setPen(QColor(35,169,110)/*Qt::darkGreen*/);
  painter->setPen(QColor(m_colors.CentralTextColor));

  RectX      = CenterX;
  RectY      = CenterY;
  font.setBold(true);
  QFontMetrics centLabelMetrics(font);
  tempWidth = centLabelMetrics.width(QString::number(getcurrentValue(),'f',2));
  tempHeight = centLabelMetrics.height();
  painter->setFont(font);
  rect.setRect(RectX-tempWidth/2,RectY-tempHeight/2, tempWidth, tempHeight);
  painter->drawText(rect,Qt::AlignCenter, QString::number(getcurrentValue(),'f',2));
}

void CustomDial::resizeEvent(QResizeEvent *event)
{
  Q_UNUSED(event)
  // Расчет центра в зависимости от размера виджета
  CenterX = this->width()/2;
  CenterY = this->height()/2;

  // sz - размер радиуса окружности помещенной в виджет
  // минимальный размер от ширины и высоты виджета
  setm_sz(std::min(this->width(), this->height())*0.7);
  //Установка минимальной ширины и высоты компонента
  setMinimumSize(100,100);
}

void CustomDial::ValueToAngle()
{
  float Angle;  //Выходной угол без учёта начальной границы

  //Если текущее значение попадает в диапазон
  if(m_currentValue >= m_minBorder && m_currentValue <= m_maxBorder)
  {
    Angle = ((m_range - (m_maxBorder - m_currentValue)) * m_rangeAnlge) / m_range;
    //Выходной угол с учётом начальной границы
    setm_currentAngle(m_startAngle + Angle);
  }
  //Если текущее значение больше максимального
  else if(m_currentValue > m_maxBorder)
  {
    setCurrentValue(m_maxBorder);
  }
  //Если текущее значение меньше минимального
  else if (m_currentValue < m_minBorder)
  {
    setCurrentValue(m_minBorder);
  }
}

void CustomDial::AngleToValue(float Angle, unsigned int TypeCalc)
{
  float DeltaAngles  = 0;

  // Расчет в случае если начальный угол >= конечного угла
  if(TypeCalc == TYPE_CALC::First)
  {
    if(m_startAngle <= Angle || Angle <= m_endAngle)
    {
      setm_currentAngle(Angle);
      // Если значение угла лежит в диапазоне от startAngle до 360
      if(Angle >= m_startAngle && Angle <= 360)
        DeltaAngles = Angle-m_startAngle;
      // Если значение угла лежит в диапазоне от 0 до 360
      else if(Angle >= 0)
      {
        DeltaAngles = (360 - m_startAngle) + Angle;
      }
      setCurrentValue(m_minBorder + (DeltaAngles)/m_rangeAnlge * m_range);
    }
    // Проверка граничных значений
    if(fabs(Angle-m_startAngle) <= 0.3)
      setCurrentValue(m_minBorder);
    else if(fabs(Angle-m_endAngle) <= 0.3)
      setCurrentValue(m_maxBorder);
  }
  // Расчет в случае если начальный угол <= конечного
  else if(TypeCalc == TYPE_CALC::Second)
  {
    if(m_startAngle <= Angle && Angle <= m_endAngle)
    {
      setm_currentAngle(Angle);
      DeltaAngles = Angle - m_startAngle;
      setCurrentValue(m_minBorder + (DeltaAngles)/m_rangeAnlge * m_range);
    }
    // Проверка граничных значений
    if(fabs(Angle-m_startAngle) <= 0.3)
      setCurrentValue(m_minBorder);
    else if(fabs(Angle-m_endAngle) <= 0.3)
      setCurrentValue(m_maxBorder);
  }
}

void CustomDial::keyReleaseEvent(QKeyEvent *_event)
{
  if(getm_pressed())
    setm_pressed(false);
  CustomDial::repaint();
}
