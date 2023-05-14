#include "ccl_charts.h"

using namespace ccl;

RealtimeHistogram::RealtimeHistogram(QQuickItem* parent)
    : QQuickPaintedItem{parent}
{
    this->setRenderTarget(QQuickPaintedItem::FramebufferObject);
    this->setPerformanceHint(QQuickPaintedItem::FastFBOResizing, true);

//    connect(this, &RealtimeHistogram::axisColorChanged, this, &RealtimeHistogram::update);
//    connect(this, &RealtimeHistogram::axisFontChanged, this, &RealtimeHistogram::update);
//    connect(this, &RealtimeHistogram::histogramColorChanged, this, &RealtimeHistogram::update);
//    connect(this, &RealtimeHistogram::backgroundColorChanged, this, &RealtimeHistogram::update);
//    connect(this, &RealtimeHistogram::intervalChanged, this, &RealtimeHistogram::update); // add timer restart
//    connect(this, &RealtimeHistogram::horizontalAxisMaxValue, this, &RealtimeHistogram::update);
    //    connect(this, &RealtimeHistogram::verticalAxisMaxValue, this, &RealtimeHistogram::update);
}

void RealtimeHistogram::append(float value)
{

}
